#include "accessibility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Suspend %d: %s\n",__LINE__,#x);return 1;}}while(0)
static void frame(ps_a11y_model *m,char *text,double *value) {
 float box[]={1,2,200,30};ps_a11y_begin(m);
 ps_a11y_document_begin(m,1,"Document","θ linked",box);
 ps_a11y_document_box(m,0,2,box);
 ps_a11y_record(m,"view","Button",PS_A11Y_BUTTON,box,true);
 ps_a11y_record_edit(m,"view","Edit",box,true,text,32);
 ps_a11y_record_range(m,"view","Range",box,true,0,10,1,value);
 ps_a11y_document_link(m,"view","θ",1,0,2,box,true);
}
int main(void) {
 ps_a11y_model *m=malloc(sizeof *m);CHECK(m);ps_a11y_init(m);
 char text[32]="θ🌍";double value=5;frame(m,text,&value);CHECK(ps_a11y_publish(m));
 uint64_t button=m->nodes[0].id,edit=m->nodes[1].id,range=m->nodes[2].id,link=m->nodes[3].id,version=m->nodes[1].text_version;
 CHECK(ps_a11y_document_preserve_position(m,1));CHECK(ps_a11y_set_text(m,edit,"queued"));ps_a11y_suspend(m);
 CHECK(m->suspended && !m->pending_text_id && !m->focused_id && !m->keyboard_focus);
 CHECK(!ps_a11y_set_text(m,edit,"hidden") && !ps_a11y_press(m,button) && !ps_a11y_press(m,link));
 CHECK(!ps_a11y_focus(m,edit) && !ps_a11y_set_value(m,range,6) && !ps_a11y_adjust(m,range,1));
 CHECK(!ps_a11y_edit_select(m,edit,version,0,2) && !ps_a11y_edit_scroll(m,edit,version,0,2,true));
 CHECK(!ps_a11y_document_scroll(m,1,0) && !ps_a11y_document_preserve_position(m,1));
 CHECK(m->document->active && !strcmp(m->document->text,"θ linked") && !strcmp(m->nodes[1].text_value,"θ🌍"));
 frame(m,text,&value);CHECK(m->suspended && !strcmp(text,"θ🌍") && value==5);CHECK(!ps_a11y_focus(m,edit));
 CHECK(ps_a11y_publish(m) && !m->suspended);CHECK(m->nodes[0].id==button && m->nodes[1].id==edit && m->nodes[2].id==range && m->nodes[3].id==link);
 CHECK(m->nodes[1].text_version==version);
 CHECK(ps_a11y_set_value(m,range,6));ps_a11y_suspend(m);CHECK(!m->pending_value_id && !m->pending_focus);
 frame(m,text,&value);ps_a11y_publish(m);CHECK(value==5);
 CHECK(ps_a11y_edit_select(m,edit,version,0,2));CHECK(ps_a11y_edit_scroll(m,edit,version,0,2,true));CHECK(ps_a11y_document_scroll(m,1,0));
 ps_a11y_suspend(m);CHECK(!m->pending_selection_id && !m->pending_edit_scroll_id && !m->document_scroll_version);
 frame(m,text,&value);ps_a11y_publish(m);CHECK(ps_a11y_press(m,button));ps_a11y_suspend(m);CHECK(!m->pending_press && !m->pending_focus);
 frame(m,text,&value);ps_a11y_publish(m);CHECK(ps_a11y_focus(m,edit));ps_a11y_suspend(m);CHECK(!m->pending_focus);
 ps_a11y_suspend(m);CHECK(m->suspended && m->document->active);ps_a11y_suspend(NULL);
 ps_a11y_destroy(m);free(m);puts("Suspended accessibility: preserved content/IDs, cancelled input, rejected actions and publication-only resume passed");return 0;
}
