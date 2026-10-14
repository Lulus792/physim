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
 ps_a11y_destroy(m);free(m);puts("Editable accessibility: Unicode value, selection replacement caret, immutable snapshot, delivery once, no truncation, disabled/stale targets and capacity changes passed");return 0;
}
