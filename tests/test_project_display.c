#include "project_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"Project display line %d: %s\n",__LINE__,#x);return 1;} } while(0)
static bool write_text(const char *path,const char *text) {
    FILE *file=fopen(path,"wb");if(!file)return false;
    bool ok=fwrite(text,1,strlen(text),file)==strlen(text);return fclose(file)==0 && ok;
}
int main(int argc,char **argv) {
    CHECK(argc==2);char path[4096],backup[4096];
    snprintf(path,sizeof path,"%s/physim.project",argv[1]);snprintf(backup,sizeof backup,"%s.bak",path);
    const char *original="physim_project=2\r\nkind=experiment\r\n# project α\r\nexperiment=main.phys\r\nanalysis=analysis.c\r\nmodules=core,properties\r\nprofile=Release\r\nparameter.mass=2.5\r\ndisplay.future=preserved\r\ncustom.path=relative/asset";
    CHECK(write_text(path,original));ps_text_document doc={0};ps_project_settings read;
    CHECK(ps_text_document_open(&doc,path)==PS_DOCUMENT_OK && ps_project_settings_read_document(&doc,&read)==PS_DOCUMENT_OK && !read.display.defined);
    ps_project_display display={.defined=true,.planar=true,.orthographic=false,.plane=1,.flags=PS_PROJECT_VIEW_VECTORS|PS_PROJECT_VIEW_LABELS};
    CHECK(ps_project_display_save_document(&doc,&display)==PS_DOCUMENT_OK);
    CHECK(ps_project_settings_read_document(&doc,&read)==PS_DOCUMENT_OK && read.display.defined && read.display.planar &&
          !read.display.orthographic && read.display.plane==1 && read.display.flags==9);
    CHECK(strstr(doc.saved,original)==doc.saved && strstr(doc.saved,"display.camera.mode=2d\r\n") &&
          strstr(doc.saved,"display.camera.plane=yz\r\n") && strstr(doc.saved,"display.camera.projection=perspective\r\n"));
    CHECK(strstr(doc.saved,"display.future=preserved\r\n") && read.release && read.parameters.count==1 && !strcmp(read.parameters.selected[0],"2.5"));
    ps_text_document bak={0};CHECK(ps_text_document_open(&bak,backup)==PS_DOCUMENT_OK && !strcmp(bak.saved,original));
    /* A no-op preserves the original backup. */
    CHECK(ps_project_display_save_document(&doc,&display)==PS_DOCUMENT_OK);
    ps_text_document_destroy(&bak);CHECK(ps_text_document_open(&bak,backup)==PS_DOCUMENT_OK && !strcmp(bak.saved,original));
    char *saved=malloc(doc.length+1);CHECK(saved);memcpy(saved,doc.saved,doc.length+1);size_t length=doc.length;
    ps_project_display invalid=display;invalid.plane=3;CHECK(ps_project_display_save_document(&doc,&invalid)==PS_DOCUMENT_INVALID);
    invalid=display;invalid.flags=32;CHECK(ps_project_display_save_document(&doc,&invalid)==PS_DOCUMENT_INVALID && doc.length==length && !memcmp(saved,doc.saved,length));
    /* Simulation/profile saves preserve separately owned view defaults. */
    read.speed=4;CHECK(ps_project_settings_save(path,&read)==PS_DOCUMENT_OK);
    CHECK(ps_text_document_open(&doc,path)==PS_DOCUMENT_OK && ps_project_settings_read_document(&doc,&read)==PS_DOCUMENT_OK && read.speed==4 && read.display.flags==9 && read.display.plane==1);
    for(unsigned plane=0;plane<3;plane++)for(unsigned mode=0;mode<2;mode++)for(unsigned projection=0;projection<2;projection++)for(unsigned flags=0;flags<=31;flags++) {
        display=(ps_project_display){true,mode!=0,projection!=0,plane,flags};
        CHECK(ps_project_display_save_document(&doc,&display)==PS_DOCUMENT_OK && ps_project_settings_read_document(&doc,&read)==PS_DOCUMENT_OK);
        CHECK(read.display.defined && read.display.plane==plane && read.display.planar==(mode!=0) && read.display.orthographic==(projection!=0) && read.display.flags==flags);
    }
    /* External edits must survive an old snapshot's attempted update. */
    CHECK(write_text(path,"physim_project=2\nkind=experiment\n# external edit\n"));
    CHECK(ps_project_display_save_document(&doc,&display)==PS_DOCUMENT_CONFLICT);
    ps_text_document external={0};CHECK(ps_text_document_open(&external,path)==PS_DOCUMENT_OK && strstr(external.saved,"# external edit"));ps_text_document_destroy(&external);
    CHECK(write_text(path,original) && ps_text_document_open(&doc,path)==PS_DOCUMENT_OK);
    const char *bad[]={"display.camera.mode=2d\n", "display.camera.plane=../xy\n", "display.camera.mode=unknown\n", "display.vectors=2\n", "display.vectors=1\ndisplay.vectors=1\n"};
    for(unsigned i=0;i<sizeof bad/sizeof *bad;i++) {
        char text[4096];snprintf(text,sizeof text,"physim_project=2\nkind=experiment\n%s",bad[i]);CHECK(write_text(path,text));
        ps_project_settings before;memcpy(&before,&read,sizeof before);
        CHECK(ps_project_settings_read(path,&read)==PS_DOCUMENT_INVALID && !memcmp(&before,&read,sizeof before));
    }
    CHECK(write_text(path,original) && ps_text_document_open(&doc,path)==PS_DOCUMENT_OK);
    CHECK(ps_project_display_save_document(&doc,&(ps_project_display){0})==PS_DOCUMENT_OK && !strstr(doc.saved,"display.camera.mode=") && strstr(doc.saved,original)==doc.saved);
    /* A valid nearly full manifest cannot be truncated to make room for views. */
    size_t capacity=256u*1024u;char *large=malloc(capacity+1);CHECK(large);
    const char *header="physim_project=2\nkind=experiment\n";size_t used=strlen(header);memcpy(large,header,used);
    while(used+9<=capacity){memcpy(large+used,"# filler\n",9);used+=9;}
    large[used]=0;CHECK(write_text(path,large) && ps_text_document_open(&doc,path)==PS_DOCUMENT_OK);
    size_t before_length=doc.length;
    CHECK(ps_project_display_save_document(&doc,&display)==PS_DOCUMENT_LIMIT && doc.length==before_length && !memcmp(doc.saved,large,used));
    ps_text_document limit_file={0};CHECK(ps_text_document_open(&limit_file,path)==PS_DOCUMENT_OK && limit_file.length==used && !memcmp(limit_file.saved,large,used));
    ps_text_document_destroy(&limit_file);free(large);
    free(saved);ps_text_document_destroy(&doc);ps_text_document_destroy(&bak);
    puts("Project display: 384 complete combinations, SI-free stable choices, legacy/extensions, exact backup, atomic errors and external-edit conflicts passed");return 0;
}
