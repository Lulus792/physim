#include "docking.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Dock line %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(void) {
    ps_dock_layout d=PS_DOCK_DEFAULT;
    CHECK(ps_dock_valid(&d));
    CHECK(ps_dock_move(&d,PS_DOCK_LOG,PS_DOCK_WORKSPACE,PS_DOCK_TAB));
    uint32_t group=ps_dock_group(&d,PS_DOCK_WORKSPACE);
    CHECK(group==ps_dock_group(&d,PS_DOCK_LOG) && d.nodes[group].active==PS_DOCK_LOG);
    CHECK(ps_dock_select(&d,PS_DOCK_WORKSPACE) && d.nodes[group].active==PS_DOCK_WORKSPACE);
    CHECK(ps_dock_move(&d,PS_DOCK_SIDEBAR,PS_DOCK_LOG,PS_DOCK_RIGHT));
    CHECK(ps_dock_valid(&d));
    CHECK(ps_dock_float_panel(&d,PS_DOCK_WORKSPACE,(ps_dock_float){20,90,720,580}));
    CHECK((d.floating&2) && ps_dock_group(&d,PS_DOCK_WORKSPACE)==PS_DOCK_NONE);
    CHECK(ps_dock_hide(&d,PS_DOCK_SIDEBAR));
    CHECK(ps_dock_hide(&d,PS_DOCK_LOG));
    CHECK(ps_dock_hide(&d,PS_DOCK_INSPECTOR));
    CHECK(d.root==PS_DOCK_NONE && d.hidden==13 && ps_dock_valid(&d));
    CHECK(ps_dock_reveal(&d,PS_DOCK_LOG,PS_DOCK_BOTTOM));
    CHECK(d.hidden==9 && ps_dock_group(&d,PS_DOCK_LOG)!=PS_DOCK_NONE && ps_dock_valid(&d));
    CHECK(ps_dock_hide(&d,PS_DOCK_LOG));
    CHECK(ps_dock_move(&d,PS_DOCK_WORKSPACE,PS_DOCK_NONE,PS_DOCK_TAB));
    CHECK(ps_dock_move(&d,PS_DOCK_SIDEBAR,PS_DOCK_WORKSPACE,PS_DOCK_LEFT));
    CHECK(ps_dock_move(&d,PS_DOCK_LOG,PS_DOCK_WORKSPACE,PS_DOCK_BOTTOM));
    ps_dock_layout before=d;
    CHECK(!ps_dock_move(&d,PS_DOCK_PANELS,0,0) && !memcmp(&d,&before,sizeof d));
    CHECK(!ps_dock_float_panel(&d,0,(ps_dock_float){0,0,10,10}) && !memcmp(&d,&before,sizeof d));
    for (unsigned i=0;i<1000;i++) {
        unsigned panel=i%PS_DOCK_PANELS,target=(panel+1+i/PS_DOCK_PANELS)%PS_DOCK_PANELS;
        if(panel==target) target=(target+1)%PS_DOCK_PANELS;
        before=d;
        bool moved=ps_dock_move(&d,panel,target,i%5);
        CHECK(ps_dock_valid(&d) && (moved || !memcmp(&d,&before,sizeof d)));
        if(i%7==0) CHECK(ps_dock_float_panel(&d,panel,(ps_dock_float){i%400,90,720,580}));
        if(i%11==0) CHECK(ps_dock_hide(&d,panel));
    }
    for(unsigned panel=0;panel<PS_DOCK_PANELS;panel++)for(unsigned side=PS_DOCK_LEFT;side<=PS_DOCK_BOTTOM;side++) {
        d=PS_DOCK_DEFAULT;CHECK(ps_dock_hide(&d,panel));
        CHECK(ps_dock_reveal_outer(&d,panel,side) && ps_dock_valid(&d));
        unsigned leaf=ps_dock_group(&d,panel);CHECK(leaf!=PS_DOCK_NONE);
        CHECK(d.nodes[d.root].kind==(side<=PS_DOCK_RIGHT?PS_DOCK_X:PS_DOCK_Y));
        CHECK((side==PS_DOCK_LEFT || side==PS_DOCK_TOP?d.nodes[d.root].first:d.nodes[d.root].second)==leaf);
        unsigned char encoded[PS_DOCK_WIRE_BYTES];ps_dock_layout decoded;
        CHECK(ps_dock_encode(&d,encoded,sizeof encoded) && ps_dock_decode(encoded,sizeof encoded,&decoded) && !memcmp(&d,&decoded,sizeof d));
        before=d;CHECK(!ps_dock_reveal_outer(&d,PS_DOCK_PANELS,side) && !memcmp(&d,&before,sizeof d));
    }
    d=PS_DOCK_DEFAULT;CHECK(ps_dock_float_panel(&d,PS_DOCK_INSPECTOR,(ps_dock_float){100,100,400,500}));before=d;
    CHECK(ps_dock_reveal_outer(&d,PS_DOCK_INSPECTOR,PS_DOCK_RIGHT) && !memcmp(&d,&before,sizeof d));
    for(unsigned p=0;p<PS_DOCK_PANELS;p++)CHECK(ps_dock_hide(&d,p));
    CHECK(ps_dock_reveal_outer(&d,PS_DOCK_WORKSPACE,PS_DOCK_RIGHT) && ps_dock_valid(&d));
    d=PS_DOCK_DEFAULT;d.nodes[5].first=5;CHECK(!ps_dock_valid(&d));
    d=PS_DOCK_DEFAULT;d.nodes[1].panels=1;CHECK(!ps_dock_valid(&d));
    d=PS_DOCK_DEFAULT;d.floating=1;CHECK(!ps_dock_valid(&d));
    puts("Dock split/tab/float/hide, empty-root recovery, graph validation and transactional moves passed.");
    return 0;
}
