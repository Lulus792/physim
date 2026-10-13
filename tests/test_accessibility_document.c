#include "accessibility.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Document accessibility %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
 ps_a11y_model *m=malloc(sizeof *m);CHECK(m);ps_a11y_init(m);
 float view[]={10,20,200,100},box[]={5,15,30,30};
 const char *text="Titel θ\n\n  x = 1; 🌍\n";
 CHECK(!ps_a11y_document_begin(m,1,"Lesen",text,view));ps_a11y_begin(m);
 CHECK(ps_a11y_document_begin(m,1,"Lesen",text,view));
 CHECK(ps_a11y_document_box(m,6,8,box));CHECK(!ps_a11y_document_box(m,7,8,box));
 CHECK(!ps_a11y_document_box(m,6,7,box));
 CHECK(!m->document && m->document_draft->count==1);CHECK(ps_a11y_publish(m));
 CHECK(m->document->active && m->document->version==1 && !strcmp(m->document->text,text));
 CHECK(m->document->boxes[0].bounds[0]==10 && m->document->boxes[0].bounds[1]==20 && m->document->boxes[0].bounds[2]==25);
 CHECK(ps_a11y_document_scroll(m,1,6));CHECK(!ps_a11y_document_scroll(m,1,8));
 size_t byte=99;CHECK(!ps_a11y_document_take_scroll(m,2,&byte) && byte==99);
 ps_a11y_begin(m);CHECK(ps_a11y_document_begin(m,1,"Lesen",text,view));
 CHECK(ps_a11y_document_box(m,6,8,box));CHECK(!ps_a11y_publish(m));
 CHECK(ps_a11y_document_take_scroll(m,1,&byte) && byte==6);CHECK(!ps_a11y_document_take_scroll(m,1,&byte));
 CHECK(!ps_a11y_document_scroll(m,1,7));CHECK(!ps_a11y_document_scroll(m,2,6));
 ps_a11y_begin(m);CHECK(!ps_a11y_document_begin(m,1,"Lesen","Changed",view));
 CHECK(!m->document_draft->active && !strcmp(m->document->text,text));
 CHECK(ps_a11y_document_begin(m,2,"Anders","New text",view));CHECK(ps_a11y_publish(m));
 CHECK(m->document->version==2 && !strcmp(m->document->text,"New text"));
 CHECK(!ps_a11y_document_scroll(m,1,0));CHECK(ps_a11y_document_scroll(m,2,0));
 ps_a11y_begin(m);CHECK(ps_a11y_publish(m));CHECK(!m->document->active && !m->document_scroll_version);
 CHECK(!ps_a11y_document_scroll(m,2,0));
 ps_a11y_begin(m);view[2]=NAN;CHECK(!ps_a11y_document_begin(m,3,"Lesen",text,view));view[2]=200;
 CHECK(!ps_a11y_document_begin(m,3,"Lesen","\xc0\xaf",view));
 CHECK(ps_a11y_document_begin(m,3,"Lesen",text,view));
 for(size_t i=0;i<PS_A11Y_TEXT_BOXES;i++)CHECK(ps_a11y_document_box(m,0,1,box));
 CHECK(!ps_a11y_document_box(m,0,1,box) && m->document_draft->dropped==1);CHECK(ps_a11y_publish(m));
 ps_a11y_destroy(m);CHECK(!m->document && !m->document_draft);ps_a11y_destroy(m);free(m);
 puts("Accessible document: complete text, immutable publication, geometry, Unicode boundaries, stale scroll and cleanup passed");return 0;
}
