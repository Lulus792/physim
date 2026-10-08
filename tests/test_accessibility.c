#include "accessibility.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"a11y %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
 ps_a11y_model *m=malloc(sizeof *m);CHECK(m);ps_a11y_init(m);
 float box[]={1,2,100,24};
 ps_a11y_begin(m);CHECK(!ps_a11y_record(m,"main","Öffnen …",PS_A11Y_BUTTON,box,true));
 CHECK(!ps_a11y_record(m,"main","Bereit",PS_A11Y_TEXT,box,true));CHECK(ps_a11y_publish(m));
 CHECK(m->count==2 && !strcmp(m->nodes[0].label,"Öffnen …"));uint64_t button=m->nodes[0].id;
 CHECK(!ps_a11y_press(m,m->nodes[1].id));CHECK(ps_a11y_press(m,button));CHECK(!ps_a11y_press(m,button));
 box[0]=200;ps_a11y_begin(m);CHECK(ps_a11y_record(m,"main","Öffnen …",PS_A11Y_BUTTON,box,true));
 ps_a11y_publish(m);CHECK(m->nodes[0].id==button && !m->pending_press);
 /* Simulate an asynchronous native action after drawing, before publish. */
 ps_a11y_begin(m);CHECK(!ps_a11y_record(m,"main","Öffnen …",PS_A11Y_BUTTON,box,true));
 CHECK(ps_a11y_press(m,button));ps_a11y_publish(m);CHECK(m->pending_press==button);
 ps_a11y_begin(m);CHECK(ps_a11y_record(m,"main","Öffnen …",PS_A11Y_BUTTON,box,true));
 CHECK(!ps_a11y_record(m,"main","Öffnen …",PS_A11Y_BUTTON,box,true));
 ps_a11y_publish(m);CHECK(!m->pending_press);
 CHECK(ps_a11y_press(m,button));ps_a11y_begin(m);
 CHECK(!ps_a11y_record(m,"main","Öffnen …",PS_A11Y_BUTTON,box,false));ps_a11y_publish(m);CHECK(!ps_a11y_press(m,button));
 ps_a11y_begin(m);ps_a11y_publish(m);CHECK(!ps_a11y_find(m,button));CHECK(!ps_a11y_press(m,button));
 ps_a11y_begin(m);ps_a11y_record(m,"main","Öffnen …",PS_A11Y_BUTTON,box,true);ps_a11y_publish(m);CHECK(m->nodes[0].id!=button);
 ps_a11y_begin(m);ps_a11y_record(m,"one","X",PS_A11Y_BUTTON,box,true);ps_a11y_record(m,"two","X",PS_A11Y_BUTTON,box,true);ps_a11y_record(m,"one","X",PS_A11Y_BUTTON,box,true);ps_a11y_publish(m);
 CHECK(m->count==3 && m->nodes[0].id!=m->nodes[1].id && m->nodes[0].id!=m->nodes[2].id);
 char long_text[2050];memset(long_text,'x',1022);memcpy(long_text+1022,"🌍",5);
 ps_a11y_begin(m);ps_a11y_record(m,"main",long_text,PS_A11Y_TEXT,box,true);ps_a11y_publish(m);CHECK(strlen(m->nodes[0].label)==1022);
 ps_a11y_begin(m);box[2]=NAN;ps_a11y_record(m,"main","bad",PS_A11Y_BUTTON,box,true);box[2]=100;
 ps_a11y_record(m,"main","\xc0\xaf",PS_A11Y_TEXT,box,true);ps_a11y_publish(m);CHECK(m->count==0);
 ps_a11y_begin(m);CHECK(!ps_a11y_record_state(m,"main","Vektoren",PS_A11Y_CHECKBOX,box,true,false));ps_a11y_publish(m);
 uint64_t toggle=m->nodes[0].id;CHECK(!m->nodes[0].checked && ps_a11y_press(m,toggle));
 ps_a11y_begin(m);CHECK(ps_a11y_record_state(m,"main","Vektoren",PS_A11Y_CHECKBOX,box,true,false));CHECK(ps_a11y_publish(m));
 CHECK(m->nodes[0].id==toggle && m->nodes[0].checked);
 ps_a11y_begin(m);CHECK(!ps_a11y_record_state(m,"main","Vektoren",PS_A11Y_CHECKBOX,box,true,true));CHECK(!ps_a11y_publish(m));
 ps_a11y_begin(m);CHECK(!ps_a11y_record_state(m,"main","Vektoren",PS_A11Y_CHECKBOX,box,true,true));
 CHECK(ps_a11y_press(m,toggle));ps_a11y_publish(m);CHECK(m->pending_press==toggle);
 ps_a11y_begin(m);CHECK(ps_a11y_record_state(m,"main","Vektoren",PS_A11Y_CHECKBOX,box,true,true));ps_a11y_publish(m);
 CHECK(!m->nodes[0].checked && !m->pending_press);
 ps_a11y_begin(m);ps_a11y_record_state(m,"main","Vektoren",PS_A11Y_CHECKBOX,box,true,true);ps_a11y_publish(m);
 CHECK(ps_a11y_press(m,toggle));ps_a11y_begin(m);CHECK(!ps_a11y_record_state(m,"main","Vektoren",PS_A11Y_CHECKBOX,box,false,true));ps_a11y_publish(m);
 CHECK(m->nodes[0].checked && !ps_a11y_press(m,toggle));
 ps_a11y_begin(m);ps_a11y_publish(m);CHECK(!ps_a11y_press(m,toggle));
 ps_a11y_begin(m);for(unsigned i=0;i<PS_A11Y_MAX_NODES+2;i++)ps_a11y_record(m,"main","same",PS_A11Y_BUTTON,box,true);ps_a11y_publish(m);CHECK(m->count==PS_A11Y_MAX_NODES && m->dropped==2);
 CHECK(ps_a11y_press(m,m->nodes[0].id));ps_a11y_begin(m);ps_a11y_publish(m);CHECK(!m->pending_press);
 free(m);puts("Accessibility model: immutable publication, stable/expired IDs, disabled/stale activation, duplicate scoping, UTF-8 and capacity passed");return 0;
}
