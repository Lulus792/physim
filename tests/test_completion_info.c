#include "completion_info.h"
#include "autosave.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Completion info line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
    CHECK(PS_COMPLETION_INFO_DELAY==.280);
    int editor=0,other=0;char text[]="int alpha;\nps_v";
    ps_completion_info_key key={.editor=&editor,.candidate=17,.cursor=sizeof text-1,.bytes=sizeof text-1,.text=text};
    ps_completion_info info={0};CHECK(!ps_completion_info_update(&info,&key,1));
    CHECK(ps_completion_info_request(&info,&key,10,false)==PS_OK && info.mode==PS_COMPLETION_INFO_PENDING && info.text!=text);
    double deadline=info.due;CHECK(deadline==10+PS_COMPLETION_INFO_DELAY);
    CHECK(!ps_completion_info_update(&info,&key,10.100) && info.due==deadline);
    CHECK(!ps_completion_info_update(&info,&key,deadline-.000001) && info.mode==PS_COMPLETION_INFO_PENDING);
    CHECK(ps_completion_info_update(&info,&key,deadline) && info.mode==PS_COMPLETION_INFO_VISIBLE && info.due==0);
    CHECK(ps_completion_info_update(&info,&key,20));
    char *snapshot=info.text;key.candidate=18;
    CHECK(ps_completion_info_request(&info,&key,20,false)==PS_OK && info.text==snapshot && info.mode==PS_COMPLETION_INFO_PENDING);
    CHECK(!ps_completion_info_update(&info,&key,20.100));
    CHECK(ps_completion_info_request(&info,&key,20.150,false)==PS_OK);
    CHECK(!ps_completion_info_update(&info,&key,20.300));
    CHECK(ps_completion_info_update(&info,&key,20.150+PS_COMPLETION_INFO_DELAY));
    for(unsigned dimension=0;dimension<6;dimension++){
        CHECK(ps_completion_info_request(&info,&key,30,false)==PS_OK);
        ps_completion_info_key changed=key;
        if(dimension==0)changed.editor=&other;
        if(dimension==1)changed.candidate++;
        if(dimension==2)changed.cursor--;
        if(dimension==3)changed.scroll_y=1;
        if(dimension==4)changed.scroll_x=2;
        if(dimension==5)text[4]='b'; /* Same length, outside the completion prefix. */
        CHECK(!ps_completion_info_update(&info,&changed,31) && info.mode==PS_COMPLETION_INFO_HIDDEN && !info.text);
        CHECK(!ps_completion_info_update(&info,&key,40));text[4]='a';
    }
    CHECK(ps_completion_info_request(&info,&key,40,true)==PS_OK && info.mode==PS_COMPLETION_INFO_VISIBLE && info.due==0);
    CHECK(ps_completion_info_update(&info,&key,40));snapshot=info.text;
    ps_completion_info_key bad=key;bad.cursor=key.bytes+1;
    CHECK(ps_completion_info_request(&info,&bad,40,false)==PS_INVALID && info.text==snapshot && info.mode==PS_COMPLETION_INFO_VISIBLE);
    bad=key;bad.bytes=PS_SOURCE_MAX_BYTES+1;
    CHECK(ps_completion_info_request(&info,&bad,40,false)==PS_LIMIT && info.text==snapshot);
    CHECK(ps_completion_info_request(&info,&key,NAN,false)==PS_INVALID && info.text==snapshot);
    CHECK(!ps_completion_info_update(&info,&key,NAN) && !info.text);
    key.text=NULL;key.bytes=key.cursor=0;
    CHECK(ps_completion_info_request(&info,&key,50,true)==PS_OK && info.text && !*info.text);
    ps_completion_info_cancel(&info);ps_completion_info_cancel(&info);ps_completion_info_cancel(NULL);
    CHECK(!info.text && !info.due && info.mode==PS_COMPLETION_INFO_HIDDEN);
    puts("Completion info: exact delay boundary, idle deadline, rapid reselection, immediate show, owned full-source binding, editor/candidate/cursor/viewport invalidation and cancel passed");return 0;
}
