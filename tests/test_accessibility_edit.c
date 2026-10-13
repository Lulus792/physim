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
 ps_a11y_destroy(m);free(m);puts("Editable accessibility: Unicode value, immutable snapshot, delivery once, no truncation, disabled/stale targets and capacity changes passed");return 0;
}
