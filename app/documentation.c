#include "documentation.h"
#include "text_validation.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "documentation_math_catalog.inc"

const ps_doc_formula *ps_document_formula(const char *tex,size_t length) {
    if(!tex)return NULL;
    for(size_t i=0;i<sizeof formula_catalog/sizeof formula_catalog[0];i++)
        if(strlen(formula_catalog[i].tex)==length && !memcmp(tex,formula_catalog[i].tex,length))return &formula_catalog[i];
    return NULL;
}

static bool history_valid(const ps_doc_history *h) {
    return h && h->count<=PS_DOC_HISTORY_LIMIT && (!h->count || h->index<h->count);
}
static bool location_valid(const ps_doc_location *p) {
    return p && p->topic>=0 && p->track>=0 && p->track<=2 && p->match>=0 &&
           memchr(p->query,0,sizeof p->query) && memchr(p->filter,0,sizeof p->filter);
}
bool ps_doc_history_visit(ps_doc_history *h,const ps_doc_location *p) {
    if(!history_valid(h) || !location_valid(p))return false;
    ps_doc_location copy=*p;
    if(h->count)h->count=h->index+1;
    if(h->count==PS_DOC_HISTORY_LIMIT) {
        memmove(h->entries,h->entries+1,(PS_DOC_HISTORY_LIMIT-1)*sizeof h->entries[0]);
        --h->count;
    }
    h->entries[h->count++]=copy;h->index=h->count-1;return true;
}
bool ps_doc_history_store(ps_doc_history *h,const ps_doc_location *p) {
    if(!history_valid(h) || !h->count || !location_valid(p))return false;
    h->entries[h->index]=*p;return true;
}
const ps_doc_location *ps_doc_history_peek(const ps_doc_history *h,int direction) {
    if(!history_valid(h) || !h->count)return NULL;
    if(direction==-1 && h->index)return &h->entries[h->index-1];
    if(direction==1 && h->index+1<h->count)return &h->entries[h->index+1];
    return NULL;
}
bool ps_doc_history_move(ps_doc_history *h,int direction) {
    if(!ps_doc_history_peek(h,direction))return false;
    h->index=direction<0?h->index-1:h->index+1;return true;
}

static uint32_t folded_char(const char **text) {
    const unsigned char *p = (const unsigned char *)*text;
    uint32_t c = *p++;
    unsigned extra = 0;
    if (c >= 0xc2 && c <= 0xdf) {
        c &= 31;
        extra = 1;
    } else if (c >= 0xe0 && c <= 0xef) {
        c &= 15;
        extra = 2;
    } else if (c >= 0xf0 && c <= 0xf4) {
        c &= 7;
        extra = 3;
    }
    for (unsigned i = 0; i < extra; i++) {
        if ((p[i] & 0xc0) != 0x80) {
            uint32_t original = (unsigned char)**text;
            (*text)++;
            return original;
        }
        c = (c << 6) | (p[i] & 63);
    }
    *text = (const char *)(p + extra);
    if ((c >= 'A' && c <= 'Z') || (c >= 0xc0 && c <= 0xde && c != 0xd7))
        c += 32;
    if (c == 0x1e9e)
        c = 0xdf;
    return c;
}
const char *ps_document_find(const char *text, const char *query) {
    if (!text || !query || !*query)
        return NULL;
    for (const char *p = text; *p;) {
        const char *a = p, *b = query;
        bool same = true;
        while (*a && *b) {
            uint32_t ca = folded_char(&a), cb = folded_char(&b);
            if (ca != cb) {
                same = false;
                break;
            }
        }
        if (same && !*b)
            return p;
        folded_char(&p);
    }
    return NULL;
}
static bool append(ps_document *d, ps_doc_kind kind, unsigned level, const char *s, size_t n,
                   const char *target, size_t tn) {
    if (d->count == PS_DOC_MAX_BLOCKS || n + tn + 2 > sizeof d->text - d->used)
        return false;
    ps_doc_block *b = &d->blocks[d->count++];
    b->kind = kind;
    b->level = level;
    b->text = d->used;
    b->length = n;
    b->inline_parent=SIZE_MAX;
    b->reading_begin=SIZE_MAX;
    b->table_first=SIZE_MAX;b->table_row=SIZE_MAX;
    memcpy(d->text + d->used, s, n);
    d->used += n;
    d->text[d->used++] = 0;
    b->target = d->used;
    if (tn)
        memcpy(d->text + d->used, target, tn);
    d->used += tn;
    d->text[d->used++] = 0;
    return true;
}
typedef struct {const char *target;size_t target_length,text,length;bool image;} inline_link;
typedef struct {
    ps_document *document;char *plain;size_t used,base,first_span,parent;
    inline_link *links;size_t link_count,link_capacity;
    bool table;
} inline_parser;
static bool inline_emit(inline_parser *p,const char *s,size_t n,unsigned style,size_t link) {
    if(!n)return true;
    ps_document *d=p->document;
    ps_doc_span *span=d->span_count>p->first_span?&d->spans[d->span_count-1]:NULL;
    if(!span || span->style!=style || span->link!=link || span->text+span->length!=p->base+p->used) {
        if(d->span_count==PS_DOC_MAX_SPANS)return false;
        span=&d->spans[d->span_count++];
        *span=(ps_doc_span){.text=p->base+p->used,.link=link,.style=style};
    }
    span->length+=n;memcpy(p->plain+p->used,s,n);p->used+=n;return true;
}
/* Unmatched decoration stays literal. Backticks protect code from other markup. */
static size_t inline_close(const char *s,size_t start,size_t n,const char *token,size_t length) {
    for(size_t i=start;i+length<=n;i++) {
        if(s[i]=='\\' && i+1<n){++i;continue;}
        if(length==2 && s[i]=='`') {
            const char *end=memchr(s+i+1,'`',n-i-1);
            if(end){i=(size_t)(end-s);continue;}
        }
        if(!memcmp(s+i,token,length))return i;
    }
    return SIZE_MAX;
}
static bool inline_parse(inline_parser *p,const char *s,size_t n,unsigned style,size_t link,unsigned depth) {
    if(depth>8)return inline_emit(p,s,n,style,link);
    for(size_t i=0;i<n;) {
        if(s[i]=='\\' && i+1<n && strchr("\\`*[]()!|",s[i+1])) {
            if(!inline_emit(p,s+i+1,1,style,link))return false;
            i+=2;continue;
        }
        if(s[i]=='`') {
            size_t end=inline_close(s,i+1,n,"`",1);
            if(end!=SIZE_MAX) {
                if(!inline_emit(p,s+i+1,end-i-1,style|PS_DOC_INLINE_CODE,link))return false;
                i=end+1;continue;
            }
        }
        if(s[i]=='*' && i+1<n && s[i+1]=='*') {
            size_t end=inline_close(s,i+2,n,"**",2);
            if(end!=SIZE_MAX) {
                if(!inline_parse(p,s+i+2,end-i-2,style|PS_DOC_BOLD,link,depth+1))return false;
                i=end+2;continue;
            }
        }
        bool image=s[i]=='!' && i+1<n && s[i+1]=='[';
        size_t begin=i+(image?1:0);
        if(s[begin]=='[' && link==SIZE_MAX) {
            size_t close=inline_close(s,begin+1,n,"]",1);
            if(close!=SIZE_MAX && close+1<n && s[close+1]=='(') {
                size_t end=close+2;unsigned parentheses=1;
                for(;end<n;end++) {
                    if(s[end]=='\\' && end+1<n){++end;continue;}
                    if(s[end]=='(')++parentheses;
                    if(s[end]==')' && !--parentheses)break;
                }
                if(end<n) {
                    if(p->link_count==p->link_capacity)return false;
                    size_t ordinal=p->link_count++,text=p->used;
                    if(!inline_parse(p,s+begin+1,close-begin-1,style|(image?0:PS_DOC_INLINE_LINK),
                        image?SIZE_MAX:p->parent+1+ordinal,depth+1))return false;
                    p->links[ordinal]=(inline_link){s+close+2,end-close-2,text,p->used-text,image};
                    i=end+1;continue;
                }
            }
        }
        unsigned emitted_style=style;
        if(p->table && !depth && s[i]=='|')emitted_style|=PS_DOC_TABLE_SEPARATOR;
        if(!inline_emit(p,s+i,1,emitted_style,link))return false;
        ++i;
    }
    return true;
}
static bool table_cell(ps_document *d,ps_doc_block *row,size_t start,size_t end) {
    while(start<end && (d->text[start]==' ' || d->text[start]=='\t'))++start;
    while(end>start && (d->text[end-1]==' ' || d->text[end-1]=='\t'))--end;
    if(d->cell_count==PS_DOC_MAX_CELLS)return false;
    d->cells[d->cell_count++]=(ps_doc_cell){start,end-start};
    ++row->cell_count;
    return true;
}
static bool table_cells(ps_document *d,ps_doc_block *row) {
    row->first_cell=d->cell_count;
    size_t start=row->text+1,end=row->text+row->length;
    for(size_t i=0;i<row->span_count;i++) {
        const ps_doc_span *span=&d->spans[row->first_span+i];
        if(!(span->style&PS_DOC_TABLE_SEPARATOR))continue;
        for(size_t j=0;j<span->length;j++) {
            size_t at=span->text+j;
            if(at<start)continue;
            if(!table_cell(d,row,start,at))return false;
            start=at+1;
        }
    }
    size_t tail=start;
    while(tail<end && (d->text[tail]==' ' || d->text[tail]=='\t'))++tail;
    return tail==end || table_cell(d,row,start,end);
}
static bool paragraph(ps_document *d,const char *s,size_t n,ps_doc_kind kind,unsigned level) {
    size_t capacity=n/4+1;if(capacity>PS_DOC_MAX_BLOCKS)capacity=PS_DOC_MAX_BLOCKS;
    char *plain=malloc(n+1);inline_link *links=malloc(capacity*sizeof *links);
    if(!plain || !links){free(plain);free(links);return false;}
    inline_parser p={.document=d,.plain=plain,.base=d->used,.first_span=d->span_count,
        .parent=d->count,.links=links,.link_capacity=capacity,.table=kind==PS_DOC_TABLE_ROW};
    bool ok=inline_parse(&p,s,n,0,SIZE_MAX,0);
    if(ok)ok=append(d,kind,level,plain,p.used,NULL,0);
    if(ok) {
        d->blocks[p.parent].first_span=p.first_span;
        d->blocks[p.parent].span_count=d->span_count-p.first_span;
        if(kind==PS_DOC_TABLE_ROW)ok=table_cells(d,&d->blocks[p.parent]);
        for(size_t i=0;ok && i<p.link_count;i++) {
            inline_link *l=&links[i];
            ok=append(d,l->image?PS_DOC_IMAGE:PS_DOC_LINK,0,plain+l->text,l->length,l->target,l->target_length);
            if(ok && !l->image)d->blocks[d->count-1].inline_parent=p.parent;
        }
    }
    free(plain);free(links);return ok;
}
static bool formula_block(ps_document *d,const char *text,size_t length) {
    while(length && isspace((unsigned char)*text)){++text;--length;}
    while(length && isspace((unsigned char)text[length-1]))--length;
    const ps_doc_formula *formula=ps_document_formula(text,length);
    return formula?append(d,PS_DOC_MATH,0,formula->reading,strlen(formula->reading),text,length):
                   append(d,PS_DOC_CODE,0,text,length,NULL,0);
}
ps_result ps_document_parse(const char *source, size_t size, bool code, ps_document **out) {
    if (!source || !out || size > (code?PS_DOC_MAX_CODE_BYTES:PS_DOC_MAX_BYTES) || memchr(source, 0, size))
        return PS_INVALID;
    ps_document *d = calloc(1, sizeof *d);
    char *pending = malloc(size + 1);
    if (!d || !pending) {
        free(d);
        free(pending);
        return PS_MEMORY;
    }
    if (code) {
        if (!append(d, PS_DOC_CODE, 0, source, size, NULL, 0))
            goto limit;
    } else {
        size_t at = 0, length = 0;
        size_t table_first=SIZE_MAX,table_row=0,last_table_row=SIZE_MAX;
        bool fenced = false,math=false;
        while (at < size) {
            size_t begin = at;
            while (at < size && source[at] != '\n')
                at++;
            size_t end = at;
            if (at < size)
                at++;
            if (end > begin && source[end - 1] == '\r')
                end--;
            const char *line = source + begin;
            size_t n = end - begin;
            if(!n || line[0]!='|')table_first=last_table_row=SIZE_MAX;
            if(!fenced && n==2 && !memcmp(line,"$$",2)) {
                if(math) {if(!formula_block(d,pending,length))goto limit;}
                else if(length && !paragraph(d,pending,length,PS_DOC_TEXT,0))goto limit;
                length=0;math=!math;continue;
            }
            if(math) {memcpy(pending+length,line,n);length+=n;pending[length++]='\n';continue;}
            bool fence = n >= 3 && !memcmp(line, "```", 3);
            if (fence) {
                if (length && !(fenced ? append(d, PS_DOC_CODE, 0, pending, length, NULL, 0)
                                       : paragraph(d, pending, length, PS_DOC_TEXT, 0)))
                    goto limit;
                length = 0;
                fenced = !fenced;
                continue;
            }
            if (fenced) {
                memcpy(pending + length, line, n);
                length += n;
                pending[length++] = '\n';
                continue;
            }
            unsigned heading = 0;
            while (heading < n && line[heading] == '#')
                heading++;
            bool list =
                n >= 2 && ((line[0] == '-' && line[1] == ' ') ||
                           (isdigit((unsigned char)line[0]) && memchr(line, '.', n < 4 ? n : 4)));
            bool special =
                !n || heading || list || line[0] == '|' || (n == 3 && !memcmp(line, "---", 3));
            if (special && length) {
                if (!paragraph(d, pending, length, PS_DOC_TEXT, 0))
                    goto limit;
                length = 0;
            }
            if (!n)
                continue;
            if (heading && heading < n && line[heading] == ' ') {
                if (!paragraph(d, line + heading + 1, n - heading - 1, PS_DOC_HEADING, heading))
                    goto limit;
            } else if (n == 3 && !memcmp(line, "---", 3)) {
                if (!append(d, PS_DOC_RULE, 0, "", 0, NULL, 0))
                    goto limit;
            } else if (list || line[0] == '|') {
                bool separator = line[0] == '|' && memchr(line, '-', n);
                for (size_t i = 0; separator && i < n; i++)
                    if (line[i] != '|' && line[i] != '-' && line[i] != ':' && line[i] != ' ' &&
                        line[i] != '\t')
                        separator = false;
                if (separator) {
                    if(last_table_row!=SIZE_MAX && d->blocks[last_table_row].table_row==0)
                        d->blocks[last_table_row].level=1;
                    continue;
                }
                size_t row_block=d->count;
                if (!paragraph(d, line, n, line[0] == '|' ? PS_DOC_TABLE_ROW : PS_DOC_TEXT, 0))
                    goto limit;
                if(line[0]=='|') {
                    if(table_first==SIZE_MAX){table_first=row_block;table_row=0;}
                    d->blocks[row_block].table_first=table_first;
                    d->blocks[row_block].table_row=table_row++;
                    ++d->blocks[table_first].table_rows;
                    if(d->blocks[row_block].cell_count>d->blocks[table_first].table_columns)
                        d->blocks[table_first].table_columns=d->blocks[row_block].cell_count;
                    last_table_row=row_block;
                }
            } else {
                if (length)
                    pending[length++] = ' ';
                memcpy(pending + length, line, n);
                length += n;
            }
        }
        if(math){free(pending);free(d);return PS_INVALID;}
        if (length && !(fenced ? append(d, PS_DOC_CODE, 0, pending, length, NULL, 0)
                               : paragraph(d, pending, length, PS_DOC_TEXT, 0)))
            goto limit;
    }
    /* One continuous reading order, without repeated link-action labels or
     * image captions. Preserve code bytes; block separators are outside code. */
    for(size_t i=0;i<d->count;i++) {
        ps_doc_block *b=&d->blocks[i];
        if(b->inline_parent!=SIZE_MAX || b->kind==PS_DOC_IMAGE || b->kind==PS_DOC_RULE)continue;
        if(b->length+2>sizeof d->reading-1-d->reading_length)goto limit;
        b->reading_begin=d->reading_length;b->reading_length=b->length;
        memcpy(d->reading+d->reading_length,d->text+b->text,b->length);d->reading_length+=b->length;
        d->reading[d->reading_length++]='\n';d->reading[d->reading_length++]='\n';
    }
    d->reading[d->reading_length]=0;
    /* Embedded actions share their parent text; never append duplicate labels. */
    for(size_t i=0;i<d->count;i++) {
        ps_doc_block *link=&d->blocks[i];if(link->kind!=PS_DOC_LINK || link->inline_parent==SIZE_MAX)continue;
        const ps_doc_block *parent=&d->blocks[link->inline_parent];size_t first=SIZE_MAX,last=0;
        for(size_t j=0;j<parent->span_count;j++) {
            const ps_doc_span *span=&d->spans[parent->first_span+j];if(span->link!=i)continue;
            if(span->text<first)first=span->text;if(span->text+span->length>last)last=span->text+span->length;
        }
        if(first!=SIZE_MAX && parent->reading_begin!=SIZE_MAX) {
            link->reading_begin=parent->reading_begin+first-parent->text;link->reading_length=last-first;
        }
    }
    free(pending);
    *out = d;
    return PS_OK;
limit:
    free(pending);
    free(d);
    return PS_LIMIT;
}
typedef struct {char *name;size_t next;} doc_anchor_slot;
static doc_anchor_slot *anchor_slot(doc_anchor_slot *slots,const char *name) {
    uint64_t hash=UINT64_C(14695981039346656037);
    for(const unsigned char *p=(const unsigned char *)name;*p;p++){hash^=*p;hash*=UINT64_C(1099511628211);}
    size_t at=(size_t)hash%(2*PS_DOC_MAX_BLOCKS);
    while(slots[at].name && strcmp(slots[at].name,name))at=(at+1)%(2*PS_DOC_MAX_BLOCKS);
    return &slots[at];
}
static size_t anchor_utf8(char *out,uint32_t c) {
    if(c<128){out[0]=(char)c;return 1;}
    if(c<2048){out[0]=(char)(192|(c>>6));out[1]=(char)(128|(c&63));return 2;}
    if(c<65536){out[0]=(char)(224|(c>>12));out[1]=(char)(128|((c>>6)&63));out[2]=(char)(128|(c&63));return 3;}
    out[0]=(char)(240|(c>>18));out[1]=(char)(128|((c>>12)&63));out[2]=(char)(128|((c>>6)&63));out[3]=(char)(128|(c&63));return 4;
}
static int anchor_hex(unsigned char c) {
    return c>='0' && c<='9'?c-'0':c>='a' && c<='f'?c-'a'+10:c>='A' && c<='F'?c-'A'+10:-1;
}
ps_result ps_document_anchor(const ps_document *d,const char *fragment,size_t *block) {
    if(!d || !fragment || !block || d->count>PS_DOC_MAX_BLOCKS)return PS_INVALID;
    if(*fragment=='#')fragment++;
    size_t n=strlen(fragment);if(!n || n>4095)return PS_INVALID;
    char query[4096];size_t used=0;
    for(size_t i=0;i<n;i++) {
        if(fragment[i]=='%') {
            if(i+2>=n)return PS_INVALID;
            int hi=anchor_hex((unsigned char)fragment[i+1]),lo=anchor_hex((unsigned char)fragment[i+2]);
            if(hi<0 || lo<0 || !(hi*16+lo))return PS_INVALID;
            query[used++]=(char)(hi*16+lo);i+=2;
        } else query[used++]=fragment[i];
    }
    query[used]=0;if(!ps_text_valid(query,sizeof query,false))return PS_INVALID;
    doc_anchor_slot *slots=calloc(2*PS_DOC_MAX_BLOCKS,sizeof *slots);if(!slots)return PS_MEMORY;
    ps_result result=PS_INVALID;size_t found=0;
    for(size_t i=0;i<d->count;i++) {
        if(d->blocks[i].kind!=PS_DOC_HEADING)continue;
        const char *text=d->text+d->blocks[i].text;size_t bytes=strlen(text);
        char *base=malloc(bytes+1);if(!base){result=PS_MEMORY;break;}
        size_t length=0;
        while(*text) {
            uint32_t c=folded_char(&text);
            if(c==' ' || c=='\t')base[length++]='-';
            else if((c<128 && (isalnum((unsigned char)c) || c=='-' || c=='_')) ||
                    (c>=128 && c!=0xa7 && !(c>=0x2000 && c<=0x206f) && !(c>=0x2190 && c<=0x2bff) && c<0x1f000))length+=anchor_utf8(base+length,c);
        }
        base[length]=0;doc_anchor_slot *owner=anchor_slot(slots,base);
        char *name=base;
        if(owner->name) {
            size_t suffix=owner->next;name=malloc(length+32);
            if(!name){free(base);result=PS_MEMORY;break;}
            do {snprintf(name,length+32,"%s-%zu",base,suffix++);}while(anchor_slot(slots,name)->name);
            owner->next=suffix;free(base);
        }
        doc_anchor_slot *assigned=anchor_slot(slots,name);assigned->name=name;assigned->next=1;
        if(!strcmp(name,query)){found=i;result=PS_OK;break;}
    }
    for(size_t i=0;i<2*PS_DOC_MAX_BLOCKS;i++)free(slots[i].name);
    free(slots);if(result==PS_OK)*block=found;return result;
}
static bool api_heading(const char *heading,const char *name,bool language,bool members) {
    size_t n=strlen(name);
    if(strncmp(heading,name,n))return false;
    return !heading[n] || (language && (heading[n]=='(' || (members && heading[n]=='.')));
}
static bool api_word(unsigned char c){return isalnum(c) || c=='_';}
ps_result ps_document_api_section(const ps_document *d,const char *name,bool language,size_t *first,size_t *end) {
    if(!d || !name || !*name || !first || !end)return PS_INVALID;
    size_t begin=d->count;bool members=false;
    for(unsigned pass=0;pass<(language?2u:1u) && begin==d->count;pass++) {
        members=pass!=0;
        for(size_t i=0;i<d->count;i++)if(d->blocks[i].kind==PS_DOC_HEADING &&
            api_heading(d->text+d->blocks[i].text,name,language,members)){begin=i;break;}
    }
    if(begin==d->count && !language)for(unsigned pass=0;pass<2 && begin==d->count;pass++)for(size_t i=0;i<d->count;i++) {
        if(d->blocks[i].kind!=(pass?PS_DOC_TEXT:PS_DOC_CODE))continue;
        const char *text=d->text+d->blocks[i].text,*at=text;size_t n=strlen(name);
        while((at=strstr(at,name))!=NULL) {
            if((at==text || !api_word((unsigned char)at[-1])) && !api_word((unsigned char)at[n])){begin=i;break;}
            at++;
        }
        if(begin<d->count)break;
    }
    if(begin==d->count)return PS_INVALID;
    size_t stop=begin+1;
    while(stop<d->count) {
        const ps_doc_block *block=&d->blocks[stop];
        if(block->kind==PS_DOC_HEADING && !(language && api_heading(d->text+block->text,name,true,members)))break;
        stop++;
    }
    *first=begin;*end=stop;return PS_OK;
}
