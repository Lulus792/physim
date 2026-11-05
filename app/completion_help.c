#include "completion_help.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ps_completion_help_destroy(ps_completion_help *help){
    if(help){free(help->document);memset(help,0,sizeof *help);}
}
static ps_result page(ps_completion_help *help,const char *relative,ps_completion_help_loader load,void *user){
    if(strcmp(help->path,relative)){
        free(help->document);help->document=NULL;
        snprintf(help->path,sizeof help->path,"%s",relative);
        help->page_status=load(user,relative,&help->document);
        if(help->page_status!=PS_OK){free(help->document);help->document=NULL;}
        else if(!help->document)help->page_status=PS_INVALID;
    }
    return help->page_status;
}
ps_result ps_completion_help_select(ps_completion_help *help,const ps_completion_symbol *symbol,
                                    ps_completion_help_loader load,void *user){
    if(!help || !symbol || !symbol->name || !symbol->module || !load || symbol->kind==PS_COMPLETION_VARIABLE)return PS_INVALID;
    if(help->attempted && help->symbol==symbol)return help->status;
    help->attempted=true;help->symbol=symbol;help->guide=false;help->first=help->end=0;
    help->preview=(ps_doc_api_preview){SIZE_MAX,SIZE_MAX,SIZE_MAX,SIZE_MAX};
    char relative[256];int n=snprintf(relative,sizeof relative,symbol->language?
        "docs/reference/language-library.md":"docs/reference/%s.md",symbol->module);
    ps_result result;
    if(n<0 || (size_t)n>=sizeof relative){
        free(help->document);help->document=NULL;help->path[0]=0;
        result=help->page_status=PS_LIMIT;
    }else result=page(help,relative,load,user);
    if(result==PS_OK)result=ps_document_api_section(help->document,symbol->name,symbol->language,&help->first,&help->end);
    if(result!=PS_OK && symbol->language && symbol->kind==PS_COMPLETION_TYPE){
        help->guide=true;result=page(help,"docs/language.md",load,user);
        if(result==PS_OK)result=ps_document_api_section(help->document,"Statische Prüfung 0.1",false,&help->first,&help->end);
    }
    if(result==PS_OK)result=ps_document_api_preview(help->document,help->first,help->end,&help->preview);
    if(result!=PS_OK)help->first=help->end=0;
    help->status=result;return result;
}
