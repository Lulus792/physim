#include "file_browser.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#define CHECK(x) do { if(!(x)){fprintf(stderr,"File browser line %d: %s\n",__LINE__,#x);return 1;} } while(0)
static bool write_file(const char *path) {
    const char *text="Daten λ\n";SDL_IOStream *file=SDL_IOFromFile(path,"wb");if(!file)return false;
    bool ok=SDL_WriteIO(file,text,strlen(text))==strlen(text);return SDL_CloseIO(file) && ok;
}
static const ps_browser_entry *find(const ps_browser_listing *listing,const char *name) {
    for(size_t i=0;i<listing->count;i++)if(!strcmp(ps_browser_entry_name(&listing->entries[i]),name))return &listing->entries[i];
    return NULL;
}
static bool cancel(void *user) {unsigned *remaining=user;if(!*remaining)return true;--*remaining;return false;}
#ifdef _WIN32
static bool wide_name(const char *path,wchar_t out[PS_BROWSER_PATH]) {
    int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,out,PS_BROWSER_PATH);
    for(int i=0;i<n;i++)if(out[i]==L'/')out[i]=L'\\';return n>0;
}
#endif
static int paths(void) {
    char output[PS_BROWSER_PATH]="preserve";
    CHECK(ps_browser_path(NULL,output)==PS_INVALID && !strcmp(output,"preserve"));
    CHECK(ps_browser_path("\xff",output)==PS_INVALID && !strcmp(output,"preserve"));
#ifdef _WIN32
    CHECK(ps_browser_path("D:\\a\\b\\..\\c/./",output)==PS_OK && !strcmp(output,"D:/a/c"));
    CHECK(ps_browser_parent("D:/a",output)==PS_OK && !strcmp(output,"D:/"));
    CHECK(ps_browser_parent("D:/",output)==PS_OK && !strcmp(output,"D:/"));
    CHECK(ps_browser_path("//server/share/a/../../b",output)==PS_OK && !strcmp(output,"//server/share/b"));
    CHECK(ps_browser_parent("//server/share",output)==PS_OK && !strcmp(output,"//server/share/"));
    strcpy(output,"preserve");
    CHECK(ps_browser_path("D:relative",output)==PS_INVALID && !strcmp(output,"preserve"));
    CHECK(ps_browser_path("//?/D:/",output)==PS_INVALID && !strcmp(output,"preserve"));
    CHECK(ps_browser_path("//server",output)==PS_INVALID && !strcmp(output,"preserve"));
#else
    CHECK(ps_browser_path("/a/b/../c/./",output)==PS_OK && !strcmp(output,"/a/c"));
    CHECK(ps_browser_path("/../../a",output)==PS_OK && !strcmp(output,"/a"));
    CHECK(ps_browser_parent("/a",output)==PS_OK && !strcmp(output,"/"));
    CHECK(ps_browser_parent("/",output)==PS_OK && !strcmp(output,"/"));
    CHECK(ps_browser_child("/a","back\\slash",output)==PS_OK && !strcmp(output,"/a/back\\slash"));
#endif
    strcpy(output,"preserve");CHECK(ps_browser_child(".","..",output)==PS_INVALID && !strcmp(output,"preserve"));
    CHECK(ps_browser_child(".","x/y",output)==PS_INVALID && !strcmp(output,"preserve"));
    char huge[PS_BROWSER_PATH+8];memset(huge,'x',sizeof huge);huge[sizeof huge-1]=0;
    CHECK(ps_browser_path(huge,output)==PS_LIMIT && !strcmp(output,"preserve"));
    CHECK(ps_browser_path(".",output)==PS_OK);
    char before[PS_BROWSER_PATH];strcpy(before,output);
    CHECK(ps_browser_path(output,output)==PS_OK && !strcmp(before,output));
    return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==3);CHECK(paths()==0);
    char root[PS_BROWSER_PATH],path[PS_BROWSER_PATH],sub[PS_BROWSER_PATH],absolute[PS_BROWSER_PATH];
    char base[PS_BROWSER_PATH];snprintf(base,sizeof base,"%s/browser-%llu",argv[1],(unsigned long long)SDL_GetTicksNS());
    CHECK(SDL_CreateDirectory(base));CHECK(ps_browser_path(base,root)==PS_OK);
    CHECK(ps_browser_create_directory(root,"Ordner λ",sub)==PS_BROWSER_CREATED);
    const char *names[]={"straße.C","Äpfel.phys","Alpha.PSRUN",".secret.txt","physim.project","z.txt","hidden.txt"};
    for(size_t i=0;i<sizeof names/sizeof *names;i++) {
        CHECK(ps_browser_child(root,names[i],path)==PS_OK);CHECK(write_file(path));
    }
#ifdef _WIN32
    CHECK(ps_browser_child(root,"hidden.txt",path)==PS_OK);
    wchar_t wide[PS_BROWSER_PATH];CHECK(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,wide,PS_BROWSER_PATH)>0);
    CHECK(SetFileAttributesW(wide,FILE_ATTRIBUTE_HIDDEN));
#endif
    ps_browser_listing listing={0};CHECK(ps_browser_listing_read(root,0,NULL,NULL,&listing)==PS_OK);
    CHECK(listing.count==8 && !listing.omitted && listing.entries[0].kind==PS_BROWSER_DIRECTORY);
    CHECK(!strcmp(ps_browser_entry_name(&listing.entries[0]),"Ordner λ"));
    const ps_browser_entry *entry=find(&listing,"straße.C");CHECK(entry && entry->kind==PS_BROWSER_C_SOURCE && entry->error==PS_OK);
    CHECK(entry->size==strlen("Daten λ\n") && entry->modified_ns>0);
    CHECK(ps_browser_entry_matches(entry,PS_BROWSER_FILES,"STRASSE",false));
    CHECK(!ps_browser_entry_matches(entry,PS_BROWSER_RUNS,"",true));
    CHECK(find(&listing,"Äpfel.phys")->kind==PS_BROWSER_PHYS_SOURCE);
    CHECK(find(&listing,"physim.project")->kind==PS_BROWSER_PROJECT);
    CHECK(find(&listing,".secret.txt")->hidden);
    CHECK(!ps_browser_entry_matches(find(&listing,".secret.txt"),PS_BROWSER_FILES,"",false));
    CHECK(ps_browser_entry_matches(find(&listing,".secret.txt"),PS_BROWSER_FILES,"SECRET",true));
#ifdef _WIN32
    CHECK(find(&listing,"hidden.txt")->hidden);
#endif
    CHECK(ps_browser_entry_matches(find(&listing,"Alpha.PSRUN"),PS_BROWSER_RUNS,"alpha",false));
    CHECK(ps_browser_entry_matches(&listing.entries[0],PS_BROWSER_RUNS,"",false));
    CHECK(!ps_browser_entry_matches(entry,PS_BROWSER_FOLDERS,"",true));
    CHECK(!ps_browser_entry_matches(entry,(ps_browser_mode)99,"",true));
    ps_browser_listing_sort(&listing,PS_BROWSER_SORT_TYPE);
    for(size_t i=1;i<listing.count;i++)CHECK(listing.entries[i-1].kind<=listing.entries[i].kind);
    CHECK(ps_browser_selectable(sub,PS_BROWSER_FOLDERS) && !ps_browser_selectable(sub,PS_BROWSER_FILES));
    CHECK(ps_browser_child(root,"Alpha.PSRUN",path)==PS_OK);
    CHECK(ps_browser_selectable(path,PS_BROWSER_RUNS) && ps_browser_selectable(path,PS_BROWSER_FILES));
    CHECK(SDL_RemovePath(path));CHECK(!ps_browser_selectable(path,PS_BROWSER_RUNS));
    ps_browser_entry *old=listing.entries;size_t count=listing.count;
    unsigned remaining=0;CHECK(ps_browser_listing_read(root,0,cancel,&remaining,&listing)==PS_EOF && listing.entries==old && listing.count==count);
    remaining=3;CHECK(ps_browser_listing_read(root,0,cancel,&remaining,&listing)==PS_EOF && listing.entries==old);
    CHECK(ps_browser_listing_read("\xff",0,NULL,NULL,&listing)==PS_INVALID && listing.entries==old);
    CHECK(ps_browser_child(root,"missing",path)==PS_OK);
    CHECK(ps_browser_listing_read(path,0,NULL,NULL,&listing)==PS_IO && listing.entries==old);
    CHECK(ps_browser_child(root,"z.txt",path)==PS_OK);
    CHECK(ps_browser_listing_read(path,0,NULL,NULL,&listing)==PS_IO && listing.entries==old);
    CHECK(ps_browser_listing_read(root,0,NULL,NULL,&listing)==PS_OK && listing.count==count-1 && !find(&listing,"Alpha.PSRUN"));
    CHECK(ps_browser_listing_read(root,2,NULL,NULL,&listing)==PS_LIMIT && listing.count==2 && listing.omitted>=1);
    CHECK(ps_browser_listing_read(sub,0,NULL,NULL,&listing)==PS_OK && !listing.count);
    strcpy(absolute,"preserve");
    CHECK(ps_browser_create_directory(root,"Ordner λ",absolute)==PS_BROWSER_COLLISION && !strcmp(absolute,"preserve"));
    CHECK(ps_browser_create_directory(root,"z.txt",absolute)==PS_BROWSER_COLLISION && !strcmp(absolute,"preserve"));
    CHECK(ps_browser_create_directory(root,"../escape",absolute)==PS_BROWSER_NAME_INVALID && !strcmp(absolute,"preserve"));
    CHECK(ps_browser_create_directory(path,"child",absolute)==PS_BROWSER_CREATE_FAILED && !strcmp(absolute,"preserve"));
#ifdef _WIN32
    CHECK(ps_browser_create_directory(root,"CON.txt",absolute)==PS_BROWSER_NAME_INVALID && !strcmp(absolute,"preserve"));
#endif
    /* Real >260-byte directory paths, not just a long synthetic string. */
    strcpy(path,sub);
    for(unsigned i=0;i<18;i++) {
        char name[64];snprintf(name,sizeof name,"segment-%02u-long-directory",i);
        CHECK(ps_browser_create_directory(path,name,absolute)==PS_BROWSER_CREATED);
        strcpy(path,absolute);
    }
    CHECK(strlen(path)>260 && ps_browser_selectable(path,PS_BROWSER_FOLDERS));
    CHECK(ps_browser_listing_read(path,0,NULL,NULL,&listing)==PS_OK && !listing.count);
    CHECK(ps_browser_parent(path,absolute)==PS_OK && strlen(absolute)<strlen(path));
    size_t long_bytes=strlen(path);unsigned windows_links=0;
    CHECK(ps_browser_create_directory(root,"many",sub)==PS_BROWSER_CREATED);
    for(unsigned i=0;i<200;i++) {
        char name[48];snprintf(name,sizeof name,"entry-%03u.txt",199-i);
        CHECK(ps_browser_child(sub,name,path)==PS_OK && write_file(path));
    }
    Uint64 started=SDL_GetTicksNS();CHECK(ps_browser_listing_read(sub,0,NULL,NULL,&listing)==PS_OK);
    Uint64 listing_ns=SDL_GetTicksNS()-started;
    CHECK(listing.count==200 && listing.capacity>=200);
    CHECK(!strcmp(ps_browser_entry_name(&listing.entries[0]),"entry-000.txt"));
    CHECK(!strcmp(ps_browser_entry_name(&listing.entries[199]),"entry-199.txt"));
    CHECK(ps_browser_listing_read(sub,127,NULL,NULL,&listing)==PS_LIMIT && listing.count==127 && listing.capacity<=127);
#ifdef _WIN32
    char target[PS_BROWSER_PATH],link[PS_BROWSER_PATH];wchar_t wide_target[PS_BROWSER_PATH],wide_link[PS_BROWSER_PATH];
    CHECK(ps_browser_create_directory(root,"link target",target)==PS_BROWSER_CREATED);
    CHECK(ps_browser_child(root,"directory link",link)==PS_OK && wide_name(target,wide_target) && wide_name(link,wide_link));
    bool symbolic=CreateSymbolicLinkW(wide_link,wide_target,SYMBOLIC_LINK_FLAG_DIRECTORY|SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE)!=0;
    if(!symbolic) {
        const char *arguments[]={"powershell.exe","-NoLogo","-NoProfile","-NonInteractive","-WindowStyle","Hidden","-File",argv[2],"-Root",root,"-Target",target,"-Link",link,NULL};
        SDL_Process *process=SDL_CreateProcess(arguments,false);CHECK(process);int result=-1;
        bool waited=SDL_WaitProcess(process,true,&result);SDL_DestroyProcess(process);CHECK(waited && result==0);
    }
    {
        CHECK(ps_browser_selectable(link,PS_BROWSER_FOLDERS));CHECK(RemoveDirectoryW(wide_target));
        CHECK(!ps_browser_selectable(link,PS_BROWSER_FOLDERS));CHECK(RemoveDirectoryW(wide_link));windows_links++;
        CHECK(ps_browser_child(root,"z.txt",target)==PS_OK && ps_browser_child(root,"selected.psrun",link)==PS_OK);
        CHECK(wide_name(target,wide_target) && wide_name(link,wide_link));
        if(symbolic) {
            CHECK(CreateSymbolicLinkW(wide_link,wide_target,SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE));
            CHECK(ps_browser_selectable(link,PS_BROWSER_RUNS));CHECK(DeleteFileW(wide_target));
            CHECK(!ps_browser_selectable(link,PS_BROWSER_RUNS));CHECK(DeleteFileW(wide_link));windows_links++;
        }
    }
#endif
#ifndef _WIN32
    char outer[PS_BROWSER_PATH],target[PS_BROWSER_PATH],nested[PS_BROWSER_PATH],link[PS_BROWSER_PATH];
    CHECK(ps_browser_create_directory(root,"outer",outer)==PS_BROWSER_CREATED);
    CHECK(ps_browser_create_directory(root,"target",target)==PS_BROWSER_CREATED);
    CHECK(ps_browser_create_directory(target,"nested",nested)==PS_BROWSER_CREATED);
    CHECK(ps_browser_child(target,"actual.txt",path)==PS_OK && write_file(path));
    CHECK(ps_browser_child(outer,"link",link)==PS_OK && symlink(nested,link)==0);
    CHECK(ps_browser_listing_read(outer,0,NULL,NULL,&listing)==PS_OK && find(&listing,"link"));
    CHECK(find(&listing,"link")->kind==PS_BROWSER_DIRECTORY && ps_browser_selectable(link,PS_BROWSER_FOLDERS));
    CHECK(snprintf(path,sizeof path,"%s/..",link)<(int)sizeof path);
    CHECK(ps_browser_listing_read(path,0,NULL,NULL,&listing)==PS_OK && find(&listing,"actual.txt"));
    CHECK(!strcmp(listing.directory,target));
    CHECK(ps_browser_create_directory(path,"new",absolute)==PS_BROWSER_CREATED);
    char expected[PS_BROWSER_PATH];CHECK(ps_browser_child(target,"new",expected)==PS_OK && !strcmp(absolute,expected));
    CHECK(snprintf(path,sizeof path,"%s/../actual.txt",link)<(int)sizeof path);
    CHECK(ps_browser_selectable(path,PS_BROWSER_FILES));
#endif
    ps_browser_listing_destroy(&listing);CHECK(!listing.entries && !listing.count && !listing.directory[0]);
    printf("{\"kind\":\"file_browser_model\",\"entries\":200,\"bounded_entries\":127,\"real_long_path_bytes\":%zu,\"listing_nanoseconds\":%llu,\"windows_link_cases\":%u}\n",long_bytes,(unsigned long long)listing_ns,windows_links);
    puts("File browser: paths, Unicode, actual metadata, filters, hidden files, sorting, stale selection, cancellation, limits, exclusive mkdir and real long paths passed");
    return 0;
}
