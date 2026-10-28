#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#endif
#include "file_browser.h"
#include "autosave.h"
#include "workspace_state.h"
#include <SDL3/SDL.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <sys/stat.h>
#endif

static size_t root_length(const char *path) {
#ifdef _WIN32
    if (path[0] && path[1]==':' && path[2]=='/') return 3;
    if (path[0]=='/' && path[1]=='/') {
        const char *server=path+2,*share=strchr(server,'/');
        if (!share || share==server || !share[1] || share[1]=='/') return 0;
        if ((share-server==1 && (*server=='?' || *server=='.'))) return 0;
        const char *end=strchr(share+1,'/');
        return end ? (size_t)(end-path)+1 : strlen(path)+1;
    }
    return 0;
#else
    return path[0]=='/' ? 1 : 0;
#endif
}
ps_result ps_browser_path(const char *path, char out[PS_BROWSER_PATH]) {
    if (!path || !out) return PS_INVALID;
    char absolute[PS_BROWSER_PATH],normalized[PS_BROWSER_PATH],input[PS_BROWSER_PATH];
    size_t input_size=SDL_strnlen(path,sizeof input);
    if(input_size==sizeof input)return PS_LIMIT;
    memcpy(input,path,input_size+1);
#ifdef _WIN32
    /* The shared resolver deliberately rejects Windows root-relative paths;
     * translate slash spelling first so //server/share remains a UNC path. */
    for(char *p=input;*p;p++)if(*p=='/')*p='\\';
#endif
    ps_result result=ps_workspace_absolute(input,absolute);
    if (result!=PS_OK) return result;
#ifdef _WIN32
    for (char *p=absolute;*p;p++) if (*p=='\\') *p='/';
#endif
    size_t root=root_length(absolute),length=strlen(absolute);
    if (!root) return PS_INVALID;
    if (root>=sizeof normalized) return PS_LIMIT;
    memcpy(normalized,absolute,root<=length?root:length);
    if (root>length) normalized[root-1]='/';
    size_t at=root<=length?root:length,n=root;
    size_t components[PS_BROWSER_PATH/2],count=0;
    while (absolute[at]) {
        while (absolute[at]=='/') at++;
        size_t start=at;while (absolute[at] && absolute[at]!='/') at++;
        size_t part=at-start;if (!part || (part==1 && absolute[start]=='.')) continue;
        if (part==2 && absolute[start]=='.' && absolute[start+1]=='.') {
            if (count) n=components[--count];
            continue;
        }
        size_t before=n;
        if (n>root) normalized[n++]='/';
        if (part>=sizeof normalized-n || count==PS_BROWSER_PATH/2) return PS_LIMIT;
        components[count++]=before;memcpy(normalized+n,absolute+start,part);n+=part;
    }
    normalized[n]=0;memcpy(out,normalized,n+1);return PS_OK;
}
ps_result ps_browser_parent(const char *path, char out[PS_BROWSER_PATH]) {
    if (!out) return PS_INVALID;
    char normalized[PS_BROWSER_PATH];ps_result result=ps_browser_path(path,normalized);
    if (result!=PS_OK) return result;
    size_t root=root_length(normalized),n=strlen(normalized);
    if (n>root) {
        char *slash=strrchr(normalized,'/');size_t cut=slash?(size_t)(slash-normalized):root;
        normalized[cut<root?root:cut]=0;
    }
    memcpy(out,normalized,strlen(normalized)+1);return PS_OK;
}
static bool component_valid(const char *name) {
    if (!name || !*name || !strcmp(name,".") || !strcmp(name,"..")) return false;
    size_t n=SDL_strnlen(name,PS_BROWSER_PATH);
    if (n==PS_BROWSER_PATH || !ps_source_text_valid(name,n) || strchr(name,'/')) return false;
#ifdef _WIN32
    if (strpbrk(name,"\\<>:\"|?*") || name[n-1]==' ' || name[n-1]=='.') return false;
    for (size_t i=0;i<n;i++) if ((unsigned char)name[i]<32) return false;
    size_t stem=strcspn(name,".");
    if (stem==3 && (!SDL_strncasecmp(name,"CON",3) || !SDL_strncasecmp(name,"PRN",3) ||
                   !SDL_strncasecmp(name,"AUX",3) || !SDL_strncasecmp(name,"NUL",3))) return false;
    if (stem==4 && name[3]>='1' && name[3]<='9' &&
        (!SDL_strncasecmp(name,"COM",3) || !SDL_strncasecmp(name,"LPT",3))) return false;
#endif
    return true;
}
ps_result ps_browser_child(const char *directory,const char *name,char out[PS_BROWSER_PATH]) {
    if (!out || !component_valid(name)) return PS_INVALID;
    char normalized[PS_BROWSER_PATH];ps_result result=ps_browser_path(directory,normalized);
    if (result!=PS_OK) return result;
    size_t n=strlen(normalized),part=strlen(name),separator=n && normalized[n-1]!='/'?1:0;
    if (part>=sizeof normalized-n-separator) return PS_LIMIT;
    if (separator) normalized[n++]='/';
    memcpy(normalized+n,name,part+1);
    memcpy(out,normalized,n+part+1);return PS_OK;
}
/* POSIX resolves link/.. through the link target, unlike lexical breadcrumb
 * shortening. Resolve an existing directory before using a normalized child. */
static ps_result directory_path(const char *path,char out[PS_BROWSER_PATH]) {
#ifdef _WIN32
    return ps_browser_path(path,out);
#else
    char absolute[PS_BROWSER_PATH];ps_result result=ps_workspace_absolute(path,absolute);
    if(result!=PS_OK)return result;
    char *resolved=realpath(absolute,NULL);
    if(!resolved)return errno==ENOMEM?PS_MEMORY:PS_IO;
    size_t n=strlen(resolved);
    result=n>=PS_BROWSER_PATH?PS_LIMIT:ps_source_text_valid(resolved,n)?PS_OK:PS_INVALID;
    if(result==PS_OK)memcpy(out,resolved,n+1);
    free(resolved);return result;
#endif
}
#ifdef _WIN32
/* Normalized absolute paths avoid the Win32 MAX_PATH form. No new filesystem
 * access is needed to form a drive or UNC extended-length UTF-16 path. */
static bool wide_path(const char *path,wchar_t out[PS_BROWSER_PATH+8]) {
    wchar_t converted[PS_BROWSER_PATH];
    int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,converted,PS_BROWSER_PATH);
    if (!n) return false;
    for (int i=0;i<n;i++) if (converted[i]==L'/') converted[i]=L'\\';
    const wchar_t *prefix=L"\\\\?\\",*suffix=converted;size_t offset=4;
    if (converted[0]==L'\\' && converted[1]==L'\\') {prefix=L"\\\\?\\UNC\\";suffix+=2;offset=8;}
    size_t length=wcslen(suffix);if (offset+length>=PS_BROWSER_PATH+8) return false;
    memcpy(out,prefix,offset*sizeof *out);memcpy(out+offset,suffix,(length+1)*sizeof *out);return true;
}
#endif
static bool filesystem_info(const char *path,SDL_PathInfo *info) {
#ifdef _WIN32
    char extended[PS_BROWSER_PATH+8];size_t n=strlen(path);
    const char *prefix=path[0]=='/' && path[1]=='/'?"\\\\?\\UNC\\":"\\\\?\\";
    const char *suffix=path+(path[0]=='/' && path[1]=='/'?2:0);size_t p=strlen(prefix);
    if(n+p>=sizeof extended)return false;
    memcpy(extended,prefix,p);memcpy(extended+p,suffix,strlen(suffix)+1);
    for(char *at=extended;*at;at++)if(*at=='/')*at='\\';
    return SDL_GetPathInfo(extended,info);
#else
    return SDL_GetPathInfo(path,info);
#endif
}
static bool hidden(const char *path,const char *name) {
#ifdef _WIN32
    wchar_t wide[PS_BROWSER_PATH+8];
    if (wide_path(path,wide)) {DWORD attributes=GetFileAttributesW(wide);if(attributes!=INVALID_FILE_ATTRIBUTES && (attributes&FILE_ATTRIBUTE_HIDDEN))return true;}
#else
    (void)path;
#endif
    return name[0]=='.';
}
static ps_browser_kind file_kind(const char *name) {
    if (!SDL_strcasecmp(name,"physim.project")) return PS_BROWSER_PROJECT;
    const char *extension=strrchr(name,'.');if (!extension) return PS_BROWSER_OTHER;
    if (!SDL_strcasecmp(extension,".c") || !SDL_strcasecmp(extension,".h")) return PS_BROWSER_C_SOURCE;
    if (!SDL_strcasecmp(extension,".phys")) return PS_BROWSER_PHYS_SOURCE;
    if (!SDL_strcasecmp(extension,".psrun")) return PS_BROWSER_RUN;
    const char *images[]={".png",".svg",".jpg",".jpeg",".bmp",".webp"};
    for (size_t i=0;i<sizeof images/sizeof *images;i++) if(!SDL_strcasecmp(extension,images[i]))return PS_BROWSER_IMAGE;
    const char *text[]={".txt",".md",".json",".csv",".tsv",".toml",".ini",".log",".yaml",".yml"};
    for (size_t i=0;i<sizeof text/sizeof *text;i++) if(!SDL_strcasecmp(extension,text[i]))return PS_BROWSER_TEXT;
    return PS_BROWSER_OTHER;
}
const char *ps_browser_entry_name(const ps_browser_entry *entry) {
    return entry && entry->path ? entry->path+entry->name_offset : "";
}
void ps_browser_listing_destroy(ps_browser_listing *listing) {
    if (!listing) return;
    for (size_t i=0;i<listing->count;i++) free(listing->entries[i].path);
    free(listing->entries);*listing=(ps_browser_listing){0};
}
typedef struct {
    ps_browser_listing listing;size_t limit;
    ps_browser_cancel cancel;void *user;ps_result fatal;
} read_context;
static SDL_EnumerationResult SDLCALL enumerate(void *userdata,const char *directory,const char *name) {
    read_context *read=userdata;ps_browser_listing *list=&read->listing;(void)directory;
    if (read->cancel && read->cancel(read->user)) {read->fatal=PS_EOF;return SDL_ENUM_FAILURE;}
    if (!strcmp(name,".") || !strcmp(name,"..")) return SDL_ENUM_CONTINUE;
    if (list->count==read->limit) {list->omitted++;list->status=PS_LIMIT;return SDL_ENUM_SUCCESS;}
    char path[PS_BROWSER_PATH];ps_result result=ps_browser_child(list->directory,name,path);
    if (result!=PS_OK) {list->omitted++;if(list->status!=PS_LIMIT)list->status=result==PS_LIMIT?PS_LIMIT:PS_CORRUPT;return SDL_ENUM_CONTINUE;}
    if (list->count==list->capacity) {
        size_t capacity=list->capacity?list->capacity*2:32;if(capacity>read->limit)capacity=read->limit;
        ps_browser_entry *entries=realloc(list->entries,capacity*sizeof *entries);
        if(!entries){read->fatal=PS_MEMORY;return SDL_ENUM_FAILURE;}
        list->entries=entries;list->capacity=capacity;
    }
    size_t n=strlen(path);char *owned=malloc(n+1);
    if(!owned){read->fatal=PS_MEMORY;return SDL_ENUM_FAILURE;}
    memcpy(owned,path,n+1);SDL_PathInfo info={0};bool available=filesystem_info(path,&info);
    bool regular=available && (info.type==SDL_PATHTYPE_FILE || info.type==SDL_PATHTYPE_DIRECTORY);
    list->entries[list->count++]=(ps_browser_entry){.path=owned,.name_offset=n-strlen(name),
        .size=available?info.size:0,.modified_ns=available?info.modify_time:0,
        .kind=available && info.type==SDL_PATHTYPE_DIRECTORY?PS_BROWSER_DIRECTORY:regular?file_kind(name):PS_BROWSER_OTHER,
        .error=regular?PS_OK:available?PS_INVALID:PS_IO,.hidden=hidden(path,name)};
    return SDL_ENUM_CONTINUE;
}
static int compare_name(const void *left,const void *right) {
    const ps_browser_entry *a=left,*b=right;
    if ((a->kind==PS_BROWSER_DIRECTORY)!=(b->kind==PS_BROWSER_DIRECTORY))return a->kind==PS_BROWSER_DIRECTORY?-1:1;
    int order=SDL_strcasecmp(ps_browser_entry_name(a),ps_browser_entry_name(b));
    return order?order:strcmp(a->path,b->path);
}
static int compare_type(const void *left,const void *right) {
    const ps_browser_entry *a=left,*b=right;
    if(a->kind!=b->kind)return a->kind<b->kind?-1:1;
    return compare_name(left,right);
}
void ps_browser_listing_sort(ps_browser_listing *listing,ps_browser_sort sort) {
    if(listing && listing->count>1)qsort(listing->entries,listing->count,sizeof *listing->entries,sort==PS_BROWSER_SORT_TYPE?compare_type:compare_name);
}
ps_result ps_browser_listing_read(const char *directory,size_t limit,ps_browser_cancel cancel,void *user,ps_browser_listing *out) {
    if(!out)return PS_INVALID;
    read_context read={.limit=limit && limit<PS_BROWSER_ENTRY_LIMIT?limit:PS_BROWSER_ENTRY_LIMIT,.cancel=cancel,.user=user};
    if(cancel && cancel(user))return PS_EOF;
    ps_result result=directory_path(directory,read.listing.directory);if(result!=PS_OK)return result;
    if(!ps_browser_selectable(read.listing.directory,PS_BROWSER_FOLDERS))return PS_IO;
    char native[PS_BROWSER_PATH+8];const char *scan=read.listing.directory;
#ifdef _WIN32
    bool unc=scan[0]=='/' && scan[1]=='/';
    snprintf(native,sizeof native,"%s%s",unc?"\\\\?\\UNC\\":"\\\\?\\",scan+(unc?2:0));
    for(char *at=native;*at;at++)if(*at=='/')*at='\\';scan=native;
#else
    (void)native;
#endif
    bool success=SDL_EnumerateDirectory(scan,enumerate,&read);
    if(read.fatal || !success || (cancel && cancel(user))) {
        result=read.fatal?read.fatal:success?PS_EOF:PS_IO;ps_browser_listing_destroy(&read.listing);return result;
    }
    ps_browser_listing_sort(&read.listing,PS_BROWSER_SORT_NAME);ps_browser_listing_destroy(out);*out=read.listing;return out->status;
}
bool ps_browser_entry_matches(const ps_browser_entry *entry,ps_browser_mode mode,const char *query,bool show_hidden) {
    if(!entry || !entry->path || mode<PS_BROWSER_FILES || mode>PS_BROWSER_RUNS || (!show_hidden && entry->hidden))return false;
    if(entry->error==PS_OK && mode==PS_BROWSER_FOLDERS && entry->kind!=PS_BROWSER_DIRECTORY)return false;
    if(entry->error==PS_OK && mode==PS_BROWSER_RUNS && entry->kind!=PS_BROWSER_DIRECTORY && entry->kind!=PS_BROWSER_RUN)return false;
    return !query || !*query || SDL_strcasestr(ps_browser_entry_name(entry),query)!=NULL;
}
bool ps_browser_selectable(const char *path,ps_browser_mode mode) {
    if(mode<PS_BROWSER_FILES || mode>PS_BROWSER_RUNS)return false;
    char absolute[PS_BROWSER_PATH];SDL_PathInfo info;
#ifdef _WIN32
    if(ps_browser_path(path,absolute)!=PS_OK)return false;
    wchar_t wide[PS_BROWSER_PATH+8];if(!wide_path(absolute,wide))return false;
    /* Follow reparse targets for confirmation. Attribute-only stat describes
     * the link itself and can still succeed for a dangling directory link. */
    HANDLE handle=CreateFileW(wide,0,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,
                              OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
    if(handle==INVALID_HANDLE_VALUE)return false;
    BY_HANDLE_FILE_INFORMATION current;
    bool available=GetFileInformationByHandle(handle,&current)!=0 && GetFileType(handle)==FILE_TYPE_DISK;
    CloseHandle(handle);if(!available)return false;
    info.type=current.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY?SDL_PATHTYPE_DIRECTORY:SDL_PATHTYPE_FILE;
#else
    if(ps_workspace_absolute(path,absolute)!=PS_OK)return false;
    if(!filesystem_info(absolute,&info))return false;
#endif
    if(mode==PS_BROWSER_FOLDERS)return info.type==SDL_PATHTYPE_DIRECTORY;
    if(info.type!=SDL_PATHTYPE_FILE)return false;
    return mode!=PS_BROWSER_RUNS || file_kind(absolute)==PS_BROWSER_RUN;
}
ps_browser_create_result ps_browser_create_directory(const char *parent,const char *name,char out[PS_BROWSER_PATH]) {
    if(!out || !component_valid(name))return PS_BROWSER_NAME_INVALID;
    char path[PS_BROWSER_PATH],directory[PS_BROWSER_PATH];
    ps_result resolved=directory_path(parent,directory);
    if(resolved!=PS_OK)return resolved==PS_INVALID || resolved==PS_LIMIT?PS_BROWSER_NAME_INVALID:PS_BROWSER_CREATE_FAILED;
    if(ps_browser_child(directory,name,path)!=PS_OK)return PS_BROWSER_NAME_INVALID;
    bool created;
#ifdef _WIN32
    wchar_t wide[PS_BROWSER_PATH+8];if(!wide_path(path,wide))return PS_BROWSER_NAME_INVALID;
    created=CreateDirectoryW(wide,NULL)!=0;
    if(!created && (GetLastError()==ERROR_ALREADY_EXISTS || GetLastError()==ERROR_FILE_EXISTS))return PS_BROWSER_COLLISION;
#else
    created=mkdir(path,0777)==0;if(!created && errno==EEXIST)return PS_BROWSER_COLLISION;
#endif
    if(!created)return PS_BROWSER_CREATE_FAILED;
    memcpy(out,path,strlen(path)+1);return PS_BROWSER_CREATED;
}
