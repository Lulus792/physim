#include "accessibility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"a11y edit %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
 ps_a11y_model *m=malloc(sizeof *m);CHECK(m);ps_a11y_init(m);
 char text[16]="θ";float bounds[]={1,2,100,24};ps_a11y_begin(m);
 CHECK(!ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text));CHECK(ps_a11y_publish(m));
 uint64_t id=m->nodes[0].id;CHECK(m->nodes[0].role==PS_A11Y_EDIT && m->nodes[0].focusable && !strcmp(m->nodes[0].text_value,"θ"));
 CHECK(!ps_a11y_press(m,id));CHECK(ps_a11y_set_text(m,id,"Größe 🌍"));CHECK(!strcmp(text,"θ"));
 CHECK(!ps_a11y_set_text(m,id,"other"));CHECK(ps_a11y_focus(m,id));
 ps_a11y_begin(m);CHECK(ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text));CHECK(ps_a11y_publish(m));
 CHECK(m->nodes[0].id==id && !strcmp(text,"Größe 🌍") && !m->pending_text_id && m->focused_id==id);
 uint64_t version=m->nodes[0].text_version;CHECK(version);
 CHECK(!ps_a11y_edit_select(m,id,version,1,3));CHECK(!ps_a11y_edit_select(m,id,version+1,0,5));
 CHECK(ps_a11y_edit_select(m,id,version,8,12));CHECK(!ps_a11y_set_text(m,id,"conflict"));
 ps_a11y_begin(m);ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text);
 size_t start=0,end=0;CHECK(ps_a11y_edit_take_selection(m,0,&start,&end) && start==8 && end==12);
 CHECK(!ps_a11y_edit_take_selection(m,0,&start,&end));CHECK(ps_a11y_edit_update(m,0,text,start,end));
 float glyph[]={4,4,10,16};CHECK(ps_a11y_edit_box(m,0,8,12,glyph));CHECK(!ps_a11y_edit_box(m,0,9,12,glyph));
 CHECK(ps_a11y_publish(m));CHECK(m->nodes[0].text_version==version && m->nodes[0].text_start==8 && m->nodes[0].text_end==12 && m->nodes[0].box_count==1);
 ps_a11y_blur(m,id);CHECK(!ps_a11y_edit_scroll(m,id,version,9,12,true));CHECK(!ps_a11y_edit_scroll(m,id,version+1,8,12,true));
 CHECK(!ps_a11y_edit_scroll(m,id,version,12,8,true));CHECK(ps_a11y_edit_scroll(m,id,version,8,12,false));
 CHECK(!m->focused_id && !m->pending_focus && m->nodes[0].text_start==8 && m->nodes[0].text_end==12);
 CHECK(!ps_a11y_edit_scroll(m,id,version,0,0,true));ps_a11y_begin(m);ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text);
 bool top=true;CHECK(ps_a11y_edit_take_scroll(m,0,&start,&end,&top) && start==8 && end==12 && !top);
 CHECK(!ps_a11y_edit_take_scroll(m,0,&start,&end,&top));ps_a11y_publish(m);
 CHECK(ps_a11y_edit_scroll(m,id,version,8,12,true));strcpy(text,"new");ps_a11y_begin(m);
 ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text);CHECK(!ps_a11y_edit_take_scroll(m,0,&start,&end,&top));ps_a11y_publish(m);
 strcpy(text,"Größe 🌍");ps_a11y_begin(m);ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text);ps_a11y_publish(m);version=m->nodes[0].text_version;
 CHECK(ps_a11y_edit_scroll(m,id,version,8,12,true));ps_a11y_begin(m);
 ps_a11y_record_edit(m,"help","Suche",bounds,false,text,sizeof text);ps_a11y_publish(m);CHECK(!m->pending_edit_scroll_id && !ps_a11y_edit_scroll(m,id,version,8,12,true));
 ps_a11y_begin(m);ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text);ps_a11y_publish(m);
 ps_a11y_begin(m);ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text);
 CHECK(ps_a11y_edit_scroll(m,id,version,8,12,true));ps_a11y_publish(m);CHECK(m->pending_edit_scroll_id==id);
 ps_a11y_begin(m);ps_a11y_publish(m);CHECK(!m->pending_edit_scroll_id && !ps_a11y_edit_scroll(m,id,version,8,12,true));
 ps_a11y_begin(m);ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text);ps_a11y_publish(m);id=m->nodes[0].id;version=m->nodes[0].text_version;
 CHECK(ps_a11y_edit_select(m,id,version,8,12));strcpy(text,"changed");
 ps_a11y_begin(m);ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text);
 CHECK(!ps_a11y_edit_take_selection(m,0,&start,&end));ps_a11y_publish(m);CHECK(m->nodes[0].text_version!=version);
 CHECK(!ps_a11y_set_text(m,id,"1234567890123456"));CHECK(!ps_a11y_set_text(m,id,"\xc0\xaf"));
 CHECK(!ps_a11y_set_text(m,id,"line\nline"));CHECK(!ps_a11y_set_text(m,id,"tab\there"));
 CHECK(ps_a11y_set_text(m,id,""));ps_a11y_begin(m);CHECK(ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text));CHECK(ps_a11y_publish(m));CHECK(!text[0]);
 ps_a11y_begin(m);CHECK(!ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text));
 CHECK(ps_a11y_set_text(m,id,"later"));CHECK(!ps_a11y_publish(m) && m->pending_text_id==id);
 ps_a11y_begin(m);CHECK(ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text));CHECK(ps_a11y_publish(m));CHECK(!strcmp(text,"later"));
 CHECK(ps_a11y_set_text(m,id,"ignored"));ps_a11y_begin(m);CHECK(!ps_a11y_record_edit(m,"help","Suche",bounds,false,text,sizeof text));ps_a11y_publish(m);
 CHECK(!strcmp(text,"later") && !m->pending_text_id && !ps_a11y_set_text(m,id,"new"));
 ps_a11y_begin(m);ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text);ps_a11y_publish(m);CHECK(ps_a11y_set_text(m,id,"longer"));
 char small[4]="old";ps_a11y_begin(m);CHECK(!ps_a11y_record_edit(m,"help","Suche",bounds,true,small,sizeof small));ps_a11y_publish(m);CHECK(!strcmp(small,"old") && !m->pending_text_id);
 ps_a11y_begin(m);ps_a11y_publish(m);CHECK(!ps_a11y_set_text(m,id,"stale"));
 ps_a11y_begin(m);CHECK(!ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text));ps_a11y_publish(m);CHECK(m->nodes[0].id!=id);
 id=m->nodes[0].id;strcpy(text,"A θ🌍 end");ps_a11y_begin(m);
 ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text);CHECK(ps_a11y_edit_update(m,0,text,2,8));ps_a11y_publish(m);
 version=m->nodes[0].text_version;CHECK(!ps_a11y_edit_replace(m,id,version+1,"β"));
 CHECK(ps_a11y_edit_replace(m,id,version,"β"));CHECK(!strcmp(text,"A θ🌍 end"));CHECK(!ps_a11y_set_text(m,id,"conflict"));
 ps_a11y_begin(m);CHECK(ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text));ps_a11y_publish(m);
 CHECK(!strcmp(text,"A β end") && m->nodes[0].text_start==4 && m->nodes[0].text_end==4 && m->nodes[0].text_caret==4 && !m->pending_text_has_caret);
 ps_a11y_begin(m);ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text);CHECK(ps_a11y_edit_update(m,0,text,2,4));ps_a11y_publish(m);
 version=m->nodes[0].text_version;CHECK(ps_a11y_edit_replace(m,id,version,"β"));
 ps_a11y_begin(m);CHECK(ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text));ps_a11y_publish(m);
 CHECK(m->nodes[0].text_version==version && m->nodes[0].text_start==4 && m->nodes[0].text_end==4);
 CHECK(ps_a11y_edit_replace(m,id,version,""));strcpy(text,"user change");ps_a11y_begin(m);
 CHECK(!ps_a11y_record_edit(m,"help","Suche",bounds,true,text,sizeof text));ps_a11y_publish(m);CHECK(!strcmp(text,"user change") && !m->pending_text_id);
 version=m->nodes[0].text_version;CHECK(ps_a11y_edit_replace(m,id,version,"x"));ps_a11y_suspend(m);
 CHECK(!m->pending_text_has_caret && !m->pending_text_id && !ps_a11y_edit_replace(m,id,version,"x"));
 /* Real path fields have a4096-byte capacity even when their current value
  * is short. They must be published and accept a complete long native value,
  * without weakening the independent1024-byte control-name bound. */
 char path[4096]="D:/",long_value[4096],too_long[4097];
 memset(long_value,'p',sizeof long_value-1);memcpy(long_value,"D:/λ/",6);long_value[sizeof long_value-1]=0;
 ps_a11y_begin(m);CHECK(!ps_a11y_record_edit(m,"picker","Ort",bounds,true,path,sizeof path));CHECK(ps_a11y_publish(m));
 CHECK(m->count==1 && m->nodes[0].role==PS_A11Y_EDIT && m->nodes[0].text_capacity==sizeof path);
 id=m->nodes[0].id;CHECK(ps_a11y_set_text(m,id,long_value) && !strcmp(path,"D:/"));
 ps_a11y_begin(m);CHECK(ps_a11y_record_edit(m,"picker","Ort",bounds,true,path,sizeof path));CHECK(ps_a11y_publish(m));
 CHECK(!strcmp(path,long_value) && !strcmp(m->nodes[0].text_value,long_value) && !m->pending_text_id);
 version=m->nodes[0].text_version;ps_a11y_begin(m);ps_a11y_record_edit(m,"picker","Ort",bounds,true,path,sizeof path);
 CHECK(ps_a11y_edit_update(m,0,path,1020,1024));CHECK(ps_a11y_publish(m));
 CHECK(ps_a11y_edit_replace(m,id,version,"Ω"));ps_a11y_begin(m);
 CHECK(ps_a11y_record_edit(m,"picker","Ort",bounds,true,path,sizeof path));CHECK(ps_a11y_publish(m));
 CHECK(strlen(path)==4093 && !memcmp(path,long_value,1020) && !memcmp(path+1020,"Ω",2) &&
       !strcmp(path+1022,long_value+1024) && m->nodes[0].text_caret==1022);
 memset(too_long,'q',sizeof too_long-1);too_long[sizeof too_long-1]=0;
 CHECK(!ps_a11y_set_text(m,id,too_long) && !m->pending_text_id);
 char maximum[PS_A11Y_EDIT_BYTES];memset(maximum,'x',sizeof maximum-1);maximum[sizeof maximum-1]=0;
 ps_a11y_begin(m);CHECK(!ps_a11y_record_edit(m,"picker","Maximum",bounds,true,maximum,sizeof maximum));CHECK(ps_a11y_publish(m));
 CHECK(m->count==1 && strlen(m->nodes[0].text_value)==sizeof maximum-1);
 id=m->nodes[0].id;CHECK(ps_a11y_set_text(m,id,maximum));ps_a11y_begin(m);
 CHECK(!ps_a11y_record_edit(m,"picker","Maximum",bounds,true,maximum,sizeof maximum));CHECK(!ps_a11y_publish(m) && !m->pending_text_id);
 char beyond[PS_A11Y_EDIT_BYTES+1]="";ps_a11y_begin(m);
 CHECK(!ps_a11y_record_edit(m,"picker","Too large",bounds,true,beyond,sizeof beyond));CHECK(ps_a11y_publish(m) && !m->count);
 char original[PS_A11Y_EDIT_BYTES];memset(original,'x',sizeof original-1);original[sizeof original-1]=0;
 memcpy(original,"α\r\n\t",5);
 ps_a11y_begin(m);CHECK(!ps_a11y_record_edit_mode(m,"log","Originalmeldung",bounds,true,true,true,original,sizeof original));CHECK(ps_a11y_publish(m));
 CHECK(m->count==1 && m->nodes[0].text_read_only && m->nodes[0].text_multiline && m->nodes[0].enabled && m->nodes[0].focusable);
 CHECK(!strcmp(m->nodes[0].text_value,original));id=m->nodes[0].id;version=m->nodes[0].text_version;
 CHECK(!ps_a11y_set_text(m,id,"replace") && !ps_a11y_edit_replace(m,id,version,"replace"));
 CHECK(ps_a11y_edit_select(m,id,version,2,strlen(original)));
 ps_a11y_begin(m);CHECK(!ps_a11y_record_edit_mode(m,"log","Originalmeldung",bounds,true,true,true,original,sizeof original));
 size_t first=0,last=0;CHECK(ps_a11y_edit_take_selection(m,0,&first,&last) && first==2 && last==strlen(original));
 CHECK(ps_a11y_edit_update(m,0,original,first,last));CHECK(ps_a11y_publish(m));
 CHECK(m->nodes[0].text_start==2 && m->nodes[0].text_end==strlen(original));
 /* A natural-height readonly page must not lose its last character geometry
  * to the former 2048-box budget. Use an actual bounded UTF-8 snapshot. */
 memset(original,'x',sizeof original-1);original[sizeof original-2]='!';
 float page_bounds[]={0,0,900,2000};ps_a11y_begin(m);
 CHECK(!ps_a11y_record_edit_mode(m,"log","Originalmeldung",page_bounds,true,true,true,original,sizeof original));
 for(size_t i=0;i<sizeof original-1;i++) {
  float box[]={(float)(i%128)*7,(float)(i/128)*20,7,20};
  CHECK(ps_a11y_edit_box(m,0,i,i+1,box));
 }
 CHECK(ps_a11y_publish(m));CHECK(!m->dropped && m->nodes[0].box_count==sizeof original-1);
 size_t final=m->nodes[0].box_offset+m->nodes[0].box_count-1;
 CHECK(m->edit_boxes[final].begin==sizeof original-2 && m->edit_boxes[final].end==sizeof original-1);
 ps_a11y_destroy(m);free(m);puts("Editable accessibility and readonly 12KiB original: Unicode value, exact multiline snapshot, selection, rejected mutation, delivery once and capacity changes passed");return 0;
}
