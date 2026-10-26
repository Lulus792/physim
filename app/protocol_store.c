#include "protocol_store.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct ps_protocol_entry { ps_protocol_record record; size_t bytes; char data[]; };
static uint64_t add_count(uint64_t a,uint64_t b) {return UINT64_MAX-a<b?UINT64_MAX:a+b;}
static const char *optional(const char *text) {return text?text:"";}
static bool outcome_valid(ps_protocol_outcome outcome) {return outcome>=PS_PROTOCOL_NO_OUTCOME && outcome<=PS_PROTOCOL_CANCELLED;}
static bool valid(const ps_protocol_message *m) {
    return m && m->level>=PS_LOG_DEBUG && m->level<=PS_LOG_ERROR &&
        m->origin>=PS_PROTOCOL_APP && m->origin<PS_PROTOCOL_ORIGIN_COUNT && outcome_valid(m->outcome) &&
        (!m->has_time || isfinite(m->time_s)) && (!m->raw_size || m->raw) &&
        (!m->has_code || (m->code>=PS_OK && m->code<=PS_NUMERIC)) &&
        (!m->column || m->line) && (!m->line || (m->source && *m->source));
}
static bool allocation_size(const ps_protocol_message *m,size_t *bytes) {
    size_t total=sizeof(ps_protocol_entry);
    const char *strings[]={optional(m->title),optional(m->context),optional(m->source),optional(m->operation),optional(m->argument),optional(m->project)};
    for(unsigned i=0;i<6;i++) {
        size_t n=strlen(strings[i]);
        if(n>=PS_PROTOCOL_BYTE_BUDGET || total>PS_PROTOCOL_BYTE_BUDGET-n-1)return false;
        total+=n+1;
    }
    if(m->raw_size>=PS_PROTOCOL_BYTE_BUDGET || total>PS_PROTOCOL_BYTE_BUDGET-m->raw_size-1)return false;
    *bytes=total+m->raw_size+1;return true;
}
static ps_protocol_entry *copy_entry(const ps_protocol_message *m,size_t bytes,const void *extra,size_t extra_size) {
    ps_protocol_entry *entry=malloc(bytes);if(!entry)return NULL;
    memset(entry,0,sizeof *entry);entry->bytes=bytes;entry->record.message=*m;
    const char *strings[]={optional(m->title),optional(m->context),optional(m->source),optional(m->operation),optional(m->argument),optional(m->project)};
    const char **dest[]={&entry->record.message.title,&entry->record.message.context,&entry->record.message.source,&entry->record.message.operation,&entry->record.message.argument,&entry->record.message.project};
    char *cursor=entry->data;
    for(unsigned i=0;i<6;i++){size_t n=strlen(strings[i])+1;memcpy(cursor,strings[i],n);*dest[i]=cursor;cursor+=n;}
    entry->record.message.raw=cursor;
    if(m->raw_size)memcpy(cursor,m->raw,m->raw_size);
    if(extra_size)memcpy(cursor+m->raw_size,extra,extra_size);
    entry->record.message.raw_size=m->raw_size+extra_size;
    cursor[entry->record.message.raw_size]=0;return entry;
}
static void omitted(ps_protocol *store,uint64_t messages,size_t bytes) {
    store->_omitted_messages=add_count(store->_omitted_messages,messages);
    store->_omitted_bytes=add_count(store->_omitted_bytes,(uint64_t)bytes);
}
static size_t find_slot(const ps_protocol *store,uint64_t id) {
    if(store && id)for(size_t i=0;i<store->_count;i++) {
        size_t slot=(store->_head+i)%PS_PROTOCOL_CAPACITY;
        if(store->_entries[slot]->record.id==id)return slot;
    }
    return PS_PROTOCOL_CAPACITY;
}
ps_result ps_protocol_push(ps_protocol *store,const ps_protocol_message *m,uint64_t *id) {
    if(!store || !valid(m))return PS_INVALID;
    size_t bytes;
    if(store->_next_id==UINT64_MAX || !allocation_size(m,&bytes)){omitted(store,1,m->raw_size);return PS_LIMIT;}
    ps_protocol_entry *entry=copy_entry(m,bytes,NULL,0);
    if(!entry){omitted(store,1,m->raw_size);return PS_MEMORY;}
    /* Copy before evicting: input may borrow a record in the same store. */
    while(store->_count && (store->_count==PS_PROTOCOL_CAPACITY || store->_bytes>PS_PROTOCOL_BYTE_BUDGET-bytes)) {
        ps_protocol_entry *old=store->_entries[store->_head];
        omitted(store,1,old->record.message.raw_size);store->_bytes-=old->bytes;
        free(old);store->_entries[store->_head]=NULL;store->_head=(store->_head+1)%PS_PROTOCOL_CAPACITY;store->_count--;
    }
    entry->record.id=++store->_next_id;
    store->_entries[(store->_head+store->_count)%PS_PROTOCOL_CAPACITY]=entry;store->_count++;store->_bytes+=bytes;
    if(id)*id=entry->record.id;
    return PS_OK;
}
ps_result ps_protocol_append(ps_protocol *store,uint64_t id,const void *raw,size_t size) {
    size_t slot=find_slot(store,id);
    if(slot==PS_PROTOCOL_CAPACITY || (size && !raw))return PS_INVALID;
    if(!size)return PS_OK;
    ps_protocol_entry *old=store->_entries[slot];
    ps_result result=PS_OK;ps_protocol_entry *entry=NULL;
    size_t prefix=(size_t)((const char*)old->record.message.raw-(const char*)old);
    size_t used=prefix+old->record.message.raw_size+1;
    size_t available=PS_PROTOCOL_BYTE_BUDGET-(store->_bytes-old->bytes);
    if(size>available || used>available-size)result=PS_LIMIT;
    else if(used+size<=old->bytes) {
        char *dest=(char*)old->record.message.raw;
        memmove(dest+old->record.message.raw_size,raw,size);old->record.message.raw_size+=size;
        dest[old->record.message.raw_size]=0;return PS_OK;
    } else {
        size_t growth=old->bytes*2,needed=used+size;
        if(growth<needed)growth=needed;
        if(growth>available)growth=available;
        entry=copy_entry(&old->record.message,growth,raw,size);
        if(!entry)result=PS_MEMORY;
    }
    if(result!=PS_OK){old->record.omitted_bytes=add_count(old->record.omitted_bytes,(uint64_t)size);omitted(store,0,size);return result;}
    entry->record.id=old->record.id;entry->record.omitted_bytes=old->record.omitted_bytes;
    store->_entries[slot]=entry;store->_bytes+=entry->bytes-old->bytes;free(old);return PS_OK;
}
bool ps_protocol_set_outcome(ps_protocol *store,uint64_t id,ps_protocol_outcome outcome) {
    size_t slot=find_slot(store,id);if(slot==PS_PROTOCOL_CAPACITY || !outcome_valid(outcome))return false;
    store->_entries[slot]->record.message.outcome=outcome;return true;
}
bool ps_protocol_remove(ps_protocol *store,uint64_t id) {
    size_t slot=find_slot(store,id);if(slot==PS_PROTOCOL_CAPACITY)return false;
    ps_protocol_entry *entry=store->_entries[slot];store->_bytes-=entry->bytes;free(entry);
    size_t tail=(store->_head+store->_count-1)%PS_PROTOCOL_CAPACITY;
    while(slot!=tail){size_t next=(slot+1)%PS_PROTOCOL_CAPACITY;store->_entries[slot]=store->_entries[next];slot=next;}
    store->_entries[tail]=NULL;store->_count--;return true;
}
const ps_protocol_record *ps_protocol_at(const ps_protocol *store,size_t index) {
    return store && index<store->_count?&store->_entries[(store->_head+index)%PS_PROTOCOL_CAPACITY]->record:NULL;
}
const ps_protocol_record *ps_protocol_find(const ps_protocol *store,uint64_t id) {
    size_t slot=find_slot(store,id);return slot==PS_PROTOCOL_CAPACITY?NULL:&store->_entries[slot]->record;
}
ps_protocol_summary ps_protocol_describe(const ps_protocol *store) {
    ps_protocol_summary summary={0};if(!store)return summary;
    summary.count=store->_count;summary.bytes=store->_bytes;
    summary.omitted_messages=store->_omitted_messages;summary.omitted_bytes=store->_omitted_bytes;
    for(size_t i=0;i<store->_count;i++){const ps_protocol_message *m=&ps_protocol_at(store,i)->message;summary.levels[m->level]++;summary.origins[m->origin]++;}
    return summary;
}
void ps_protocol_clear(ps_protocol *store) {
    if(!store)return;
    uint64_t next=store->_next_id;
    for(size_t i=0;i<store->_count;i++)free(store->_entries[(store->_head+i)%PS_PROTOCOL_CAPACITY]);
    memset(store,0,sizeof *store);store->_next_id=next;
}
