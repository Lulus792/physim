#include "accessibility.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"a11y focus %d: %s\n",__LINE__,#x);return 1;}}while(0)
static void draw(ps_a11y_model *m,bool enabled){float bounds[]={0,0,80,24};ps_a11y_begin(m);
 ps_a11y_record(m,"main","One",PS_A11Y_BUTTON,bounds,enabled);
 ps_a11y_record(m,"main","Text",PS_A11Y_TEXT,bounds,true);
 ps_a11y_record_state(m,"main","Check",PS_A11Y_CHECKBOX,bounds,true,false);
 ps_a11y_record_option(m,"main","Group","A",bounds,true,true,false);
 ps_a11y_record_option(m,"main","Group","B",bounds,true,false,false);ps_a11y_publish(m);}
int main(void){ps_a11y_model *m=malloc(sizeof *m);CHECK(m);ps_a11y_init(m);draw(m,true);
 uint64_t one=m->nodes[0].id,check=m->nodes[2].id,a=m->nodes[4].id,b=m->nodes[5].id;
 CHECK(!ps_a11y_focus(m,m->nodes[1].id) && !ps_a11y_focus(m,m->nodes[3].id));
 CHECK(ps_a11y_focus(m,one) && !ps_a11y_find(m,one)->focused && !ps_a11y_focus(m,check));draw(m,true);
 CHECK(ps_a11y_find(m,one)->focused && m->focused_id==one && !m->pending_focus);
 CHECK(ps_a11y_focus_move(m,1,false));draw(m,true);CHECK(m->focused_id==check);
 CHECK(ps_a11y_focus_move(m,1,false));draw(m,true);CHECK(m->focused_id==a);
 CHECK(ps_a11y_focus_move(m,-1,false));draw(m,true);CHECK(m->focused_id==check);
 CHECK(ps_a11y_focus(m,b));draw(m,true);CHECK(m->focused_id==b);
 CHECK(ps_a11y_focus_move(m,-1,true));draw(m,true);CHECK(m->focused_id==a);
 m->keyboard_focus=false;draw(m,true);CHECK(!ps_a11y_find(m,a)->focused && m->focused_id==a);
 m->keyboard_focus=true;draw(m,true);CHECK(ps_a11y_find(m,a)->focused);
 ps_a11y_blur(m,a);draw(m,true);CHECK(!m->focused_id);
 CHECK(ps_a11y_focus(m,one));draw(m,false);CHECK(!m->focused_id && !ps_a11y_focus(m,one));
 draw(m,true);CHECK(ps_a11y_focus(m,one));ps_a11y_begin(m);ps_a11y_publish(m);CHECK(!m->focused_id && !m->pending_focus && !ps_a11y_focus(m,one));
 free(m);puts("Accessibility focus: deferred delivery, unique focus, Tab/radio traversal, keyboard ownership and stale/disabled rejection passed");return 0;}
