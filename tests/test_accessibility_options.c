#include "accessibility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"a11y option %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
 ps_a11y_model *m=malloc(sizeof *m);CHECK(m);ps_a11y_init(m);float box[]={10,20,80,24};
 ps_a11y_begin(m);
 CHECK(!ps_a11y_record_option(m,"main","Oberfläche","16 px",box,true,true,false));
 box[0]=90;CHECK(!ps_a11y_record_option(m,"main","Oberfläche","22 px",box,true,false,false));
 box[0]=10;box[1]=60;CHECK(!ps_a11y_record_option(m,"main","Code","16 px",box,true,true,false));
 box[0]=90;CHECK(!ps_a11y_record_option(m,"main","Code","22 px",box,true,false,false));
 CHECK(ps_a11y_publish(m) && m->count==6 && ps_a11y_child_count(m,0)==2);
 uint64_t surface=m->nodes[0].id,first=m->nodes[1].id,last=m->nodes[2].id,code=m->nodes[3].id;
 CHECK(m->nodes[1].parent==surface && m->nodes[4].parent==code && m->nodes[1].id!=m->nodes[4].id);
 CHECK(ps_a11y_child_count(m,surface)==2 && ps_a11y_child_at(m,surface,1)->id==last && !ps_a11y_child_at(m,surface,2));
 CHECK(ps_a11y_index(m,first)==0 && ps_a11y_index(m,last)==1 && ps_a11y_index(m,code)==1);
 CHECK(!ps_a11y_press(m,surface) && ps_a11y_press(m,last));
 /* Native selection wins even if a later peer is supplied the old selection. */
 box[1]=20;ps_a11y_begin(m);CHECK(ps_a11y_record_option(m,"main","Oberfläche","22 px",box,true,false,false));
 box[0]=10;CHECK(!ps_a11y_record_option(m,"main","Oberfläche","16 px",box,true,true,false));
 box[1]=60;CHECK(!ps_a11y_record_option(m,"main","Code","16 px",box,true,true,false));
 box[0]=90;CHECK(!ps_a11y_record_option(m,"main","Code","22 px",box,true,false,false));ps_a11y_publish(m);
 CHECK(ps_a11y_find(m,last)->checked && !ps_a11y_find(m,first)->checked && ps_a11y_find(m,code));
 CHECK(ps_a11y_child_at(m,code,0)->checked && !m->pending_press);
 /* A later explicit pointer selection wins over an earlier native selection. */
 CHECK(ps_a11y_press(m,last));ps_a11y_begin(m);box[1]=20;
 CHECK(ps_a11y_record_option(m,"main","Oberfläche","22 px",box,true,true,false));
 CHECK(!ps_a11y_record_option(m,"main","Oberfläche","16 px",box,true,true,true));ps_a11y_publish(m);
 CHECK(ps_a11y_find(m,first)->checked && !ps_a11y_find(m,last)->checked);
 CHECK(ps_a11y_press(m,last));ps_a11y_begin(m);box[1]=20;
 CHECK(ps_a11y_record_option(m,"main","Oberfläche","22 px",box,true,true,false));ps_a11y_publish(m);
 CHECK(ps_a11y_find(m,last)->checked); /* Idempotent selection, never toggle off. */
 ps_a11y_begin(m);CHECK(!ps_a11y_record_option(m,"main","Oberfläche","22 px",box,true,true,false));
 CHECK(ps_a11y_press(m,last));ps_a11y_publish(m);CHECK(m->pending_press==last);
 ps_a11y_begin(m);CHECK(!ps_a11y_record_option(m,"main","Oberfläche","22 px",box,false,true,false));ps_a11y_publish(m);
 CHECK(!m->pending_press && !ps_a11y_press(m,last) && ps_a11y_find(m,last)->checked);
 ps_a11y_begin(m);CHECK(!ps_a11y_record_option(m,"main","","16 px",box,true,true,false));
 CHECK(!ps_a11y_record_option(m,"main","bad","\xc0\xaf",box,true,true,false));ps_a11y_publish(m);CHECK(!m->count && !ps_a11y_press(m,last));
 ps_a11y_begin(m);for(unsigned i=0;i<PS_A11Y_MAX_NODES-1;i++)ps_a11y_record(m,"main","text",PS_A11Y_TEXT,box,true);
 CHECK(!ps_a11y_record_option(m,"main","Too many","16 px",box,true,true,false));ps_a11y_publish(m);
 CHECK(m->count==PS_A11Y_MAX_NODES-1 && m->dropped==1);
 ps_a11y_begin(m);m->next_id=UINT64_MAX;CHECK(!ps_a11y_record_option(m,"main","Last","16 px",box,true,true,false));ps_a11y_publish(m);CHECK(!m->count);
 free(m);puts("Accessibility radio groups: distinct parents, sibling order, selection/idempotence, stale/disabled/capacity and UTF-8 passed");return 0;
}
