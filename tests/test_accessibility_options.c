#include "accessibility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"a11y option %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int clipped_options(ps_a11y_model *m) {
 float viewport[]={0,0,100,100},box[]={10,20,80,24};
 ps_a11y_begin(m);
 for(unsigned i=0;i<8;i++) {
  char name[32];snprintf(name,sizeof name,"Karte %u",i);box[1]=20+i*30.f;
  CHECK(!ps_a11y_record_option(m,"Dialog","Vorlage",name,box,true,i==0,false));
  CHECK(ps_a11y_clip_node(m,m->draft_count-1,viewport));
 }
 CHECK(ps_a11y_publish(m) && m->count==9);
 uint64_t group=m->nodes[0].id,last=ps_a11y_child_at(m,group,7)->id;
 CHECK(ps_a11y_child_count(m,group)==8 && ps_a11y_child_at(m,group,0)->checked);
 const ps_a11y_node *hidden=ps_a11y_find(m,last);
 CHECK(hidden && hidden->offscreen && hidden->focusable && hidden->enabled && hidden->bounds[2]==0 && hidden->bounds[3]==0);
 CHECK(m->nodes[0].bounds[1]==20 && m->nodes[0].bounds[3]==80 && !m->nodes[0].offscreen);
 CHECK(!ps_a11y_clip_node(m,0,viewport));
 CHECK(ps_a11y_focus(m,last) && ps_a11y_press(m,last));
 ps_a11y_begin(m);
 for(unsigned i=0;i<8;i++) {
  char name[32];snprintf(name,sizeof name,"Karte %u",i);box[1]=20+i*30.f-180;
  CHECK(ps_a11y_record_option(m,"Dialog","Vorlage",name,box,true,i==0,false)==(i==7));
  size_t index=m->draft_count-1;float original[4];memcpy(original,m->draft[index].bounds,sizeof original);
  float invalid[]={0,0,-1,100};CHECK(!ps_a11y_clip_node(m,index,invalid));
  CHECK(!memcmp(original,m->draft[index].bounds,sizeof original));
  CHECK(ps_a11y_clip_node(m,index,viewport));
 }
 CHECK(!ps_a11y_clip_node(m,m->draft_count,viewport));
 CHECK(ps_a11y_publish(m) && m->count==9 && ps_a11y_child_count(m,group)==8);
 CHECK(m->focused_id==last && !m->pending_focus && !m->pending_press);
 CHECK(ps_a11y_find(m,last)->checked && !ps_a11y_find(m,last)->offscreen && ps_a11y_find(m,last)->bounds[1]==50);
 CHECK(ps_a11y_child_at(m,group,0)->offscreen && !ps_a11y_child_at(m,group,0)->checked);
 CHECK(m->nodes[0].bounds[1]==0 && m->nodes[0].bounds[3]==74);
 return 0;
}
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
 ps_a11y_destroy(m);ps_a11y_init(m);CHECK(clipped_options(m)==0);ps_a11y_destroy(m);
 free(m);puts("Accessibility radio groups: parents, order, selection, stale/disabled/capacity, UTF-8, clipped membership and focus/reveal passed");return 0;
}
