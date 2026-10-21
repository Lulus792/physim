#include "documentation.h"
#include "completion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"API reference line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static ps_document *read_page(const char *root,const char *relative) {
    char path[4096];snprintf(path,sizeof path,"%s/%s",root,relative);
    FILE *file=fopen(path,"rb");if(!file)return NULL;
    if(fseek(file,0,SEEK_END)){fclose(file);return NULL;}long n=ftell(file);
    if(n<0 || n>PS_DOC_MAX_BYTES || fseek(file,0,SEEK_SET)){fclose(file);return NULL;}
    char *text=malloc((size_t)n+1);if(!text){fclose(file);return NULL;}
    bool ok=fread(text,1,(size_t)n,file)==(size_t)n;ok &= fclose(file)==0;text[n]=0;
    ps_document *doc=NULL;if(ok)ps_document_parse(text,(size_t)n,false,&doc);free(text);return doc;
}
int main(int argc,char **argv) {
    const char *text="# API\n\n## ps_vlength\n\nVector norm.\n\n```c\ndouble ps_vlength(ps_vec3 value);\n```\n\n### ps_vec3\n\n```c\ntypedef struct {double x,y,z;} ps_vec3;\n```\n\n## Constants\n\n```c\n#define PS_OK 0\n#define PS_OKAY 1\n```\n\n## Vec3.normalized\n\nNormalized.\n\n## String.prefix(_:)\n\nCount prefix.\n\n## String.prefix(while:)\n\nPredicate prefix.\n\n## String.suffix(_:)\n\nSuffix.\n";
    ps_document *d=NULL;CHECK(ps_document_parse(text,strlen(text),false,&d)==PS_OK);
    size_t first=99,end=98;
    CHECK(ps_document_api_section(d,"ps_vlength",false,&first,&end)==PS_OK && !strcmp(d->text+d->blocks[first].text,"ps_vlength") && end==first+3);
    ps_doc_api_preview preview;
    CHECK(ps_document_api_preview(d,first,end,&preview)==PS_OK && preview.summary==first+1 && preview.signature==first+2 && preview.example==SIZE_MAX && preview.arguments==SIZE_MAX);
    CHECK(ps_document_api_section(d,"ps_vec3",false,&first,&end)==PS_OK && d->blocks[first].kind==PS_DOC_HEADING);
    CHECK(ps_document_api_section(d,"PS_OK",false,&first,&end)==PS_OK && d->blocks[first].kind==PS_DOC_CODE);
    CHECK(ps_document_api_section(d,"PS_O",false,&first,&end)==PS_INVALID);
    CHECK(ps_document_api_section(d,"String.prefix",true,&first,&end)==PS_OK && end-first==4);
    CHECK(ps_document_api_section(d,"String",true,&first,&end)==PS_OK && end-first==6);
    size_t before_first=first,before_end=end;
    CHECK(ps_document_api_section(d,"Vec3.norm",true,&first,&end)==PS_INVALID && first==before_first && end==before_end);
    CHECK(ps_document_api_section(NULL,"x",false,&first,&end)==PS_INVALID && ps_document_api_section(d,"",false,&first,&end)==PS_INVALID);
    CHECK(ps_document_api_section(d,"x",false,NULL,&end)==PS_INVALID);
    free(d);
    const char *example="# API\n\n## call\n\nDescription.\n\n```c\nvoid call(void);\n```\n\n```c\nvoid overload(int x);\n```\n\nBeispiel für den Editor:\n\n```c\ncall();\n```\n\n| Argument | Bedeutung |\n| --- | --- |\n| x | Value |\n\n## next\n\nBeispiel für den Editor:\n\n```c\nnext();\n```\n";
    CHECK(ps_document_parse(example,strlen(example),false,&d)==PS_OK && ps_document_api_section(d,"call",false,&first,&end)==PS_OK);
    CHECK(ps_document_api_preview(d,first,end,&preview)==PS_OK && !strcmp(d->text+d->blocks[preview.example].text,"call();\n") && preview.arguments!=SIZE_MAX);
    CHECK(!strcmp(d->text+d->blocks[preview.signature].text,"void call(void);\n"));
    ps_doc_api_preview saved=preview;
    CHECK(ps_document_api_preview(d,end,first,&preview)==PS_INVALID && !memcmp(&preview,&saved,sizeof saved));
    CHECK(ps_document_api_preview(d,first,d->count+1,&preview)==PS_INVALID && !memcmp(&preview,&saved,sizeof saved));
    free(d);
    const char *export_note="# Module\n\n`PS_EXPORT` exports the module entry point.\n\n## Types\n";
    CHECK(ps_document_parse(export_note,strlen(export_note),false,&d)==PS_OK);
    CHECK(ps_document_api_section(d,"PS_EXPORT",false,&first,&end)==PS_OK && d->blocks[first].kind==PS_DOC_TEXT);
    free(d);
    CHECK(argc==2);char previous[256]="";d=NULL;size_t fallbacks=0;
    for(size_t i=0;i<ps_completion_count();i++) {
        const ps_completion_symbol *symbol=ps_completion_at(i);char path[256];
        snprintf(path,sizeof path,symbol->language?"docs/reference/language-library.md":"docs/reference/%s.md",symbol->module);
        if(strcmp(path,previous)){free(d);d=read_page(argv[1],path);snprintf(previous,sizeof previous,"%s",path);CHECK(d);}
        ps_result result=ps_document_api_section(d,symbol->name,symbol->language,&first,&end);
        if(result!=PS_OK && symbol->language && symbol->kind==PS_COMPLETION_TYPE) {
            ps_document *fallback=read_page(argv[1],"docs/language.md");CHECK(fallback);
            result=ps_document_api_section(fallback,"Statische Prüfung 0.1",false,&first,&end);free(fallback);fallbacks++;
        }
        if(result!=PS_OK)fprintf(stderr,"Missing bundled reference: %s (%s)\n",symbol->name,path);
        CHECK(result==PS_OK && first<end);
    }
    free(d);printf("API references: %zu generated symbols resolved (%zu language type guide fallbacks); exact sections, constants, overloads and failure preservation passed\n",ps_completion_count(),fallbacks);return 0;
}
