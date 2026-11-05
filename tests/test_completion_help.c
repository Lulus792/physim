#include "completion_help.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Completion help line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {unsigned loads;bool fail,empty;} fixture;
static ps_result load(void *user,const char *relative,ps_document **out){
    fixture *f=user;f->loads++;
    if(f->fail || strstr(relative,"missing"))return PS_IO;
    if(f->empty)return PS_OK;
    const char *text=!strcmp(relative,"docs/language.md")?
        "# Sprache\n\n## Statische Prüfung 0.1\n\nAllgemeiner Sprachhinweis.\n":
        "# API\n\n## alpha\n\nErste Beschreibung α.\n\n```c\nvoid alpha(int value);\n```\n\n| Argument | Bedeutung |\n| --- | --- |\n| value | Aktueller Wert |\n| extra | Zweiter Eintrag |\n| third | Dritter Eintrag |\n\n## beta\n\nZweite Beschreibung.\n\n```c\nvoid beta(void);\n```\n";
    return ps_document_parse(text,strlen(text),false,out);
}
int main(void){
    ps_completion_symbol alpha={"alpha","core","void alpha(int value);",PS_COMPLETION_FUNCTION,false,false};
    ps_completion_symbol beta={"beta","core","void beta(void);",PS_COMPLETION_FUNCTION,false,false};
    ps_completion_symbol absent={"absent","core","int absent;",PS_COMPLETION_VALUE,false,false};
    ps_completion_symbol missing={"missing","missing","int missing;",PS_COMPLETION_VALUE,false,false};
    ps_completion_symbol missing2={"another","missing","int another;",PS_COMPLETION_VALUE,false,false};
    ps_completion_symbol type={"RareType","Physim","RareType",PS_COMPLETION_TYPE,true,false};
    ps_completion_help help={0},other={0};fixture f={0};
    CHECK(ps_completion_help_select(&help,&alpha,load,&f)==PS_OK && f.loads==1);
    CHECK(help.symbol==&alpha && help.document && help.first<help.end && !help.guide);
    CHECK(help.preview.summary!=SIZE_MAX && !strcmp(help.document->text+help.document->blocks[help.preview.summary].text,"Erste Beschreibung α."));
    CHECK(help.preview.arguments!=SIZE_MAX && help.document->blocks[help.preview.arguments].table_rows==4);
    ps_document *first_page=help.document;
    CHECK(ps_completion_help_select(&help,&alpha,load,&f)==PS_OK && f.loads==1);
    CHECK(ps_completion_help_select(&help,&beta,load,&f)==PS_OK && f.loads==1 && help.document==first_page);
    CHECK(!strcmp(help.document->text+help.document->blocks[help.preview.summary].text,"Zweite Beschreibung."));
    CHECK(help.preview.arguments==SIZE_MAX && help.preview.example==SIZE_MAX);
    CHECK(ps_completion_help_select(&other,&alpha,load,&f)==PS_OK && other.document!=help.document && f.loads==2);
    CHECK(ps_completion_help_select(&help,&absent,load,&f)==PS_INVALID && help.symbol==&absent && help.first==0 && help.end==0 && help.preview.summary==SIZE_MAX);
    CHECK(ps_completion_help_select(&help,&beta,load,&f)==PS_OK && f.loads==2);
    CHECK(ps_completion_help_select(&help,&missing,load,&f)==PS_IO && !help.document && f.loads==3);
    CHECK(ps_completion_help_select(&help,&missing2,load,&f)==PS_IO && !help.document && f.loads==3);
    CHECK(other.symbol==&alpha && !strcmp(other.document->text+other.document->blocks[other.preview.summary].text,"Erste Beschreibung α."));
    CHECK(ps_completion_help_select(&help,&type,load,&f)==PS_OK && help.guide && !strcmp(help.path,"docs/language.md") && f.loads==5);
    CHECK(ps_completion_help_select(&help,&type,load,&f)==PS_OK && f.loads==5);
    ps_completion_help saved=help;
    ps_completion_symbol local={"alpha","Lokal","let alpha: Int64",PS_COMPLETION_VARIABLE,true,false};
    CHECK(ps_completion_help_select(&help,&local,load,&f)==PS_INVALID && f.loads==5 && !memcmp(&help,&saved,sizeof help));
    CHECK(ps_completion_help_select(&help,NULL,load,&f)==PS_INVALID && !memcmp(&help,&saved,sizeof help));
    CHECK(ps_completion_help_select(&help,&alpha,NULL,&f)==PS_INVALID && !memcmp(&help,&saved,sizeof help));
    char long_module[300];memset(long_module,'x',sizeof long_module-1);long_module[sizeof long_module-1]=0;
    ps_completion_symbol oversized={"alpha",long_module,"alpha",PS_COMPLETION_VALUE,false,false};
    CHECK(ps_completion_help_select(&help,&oversized,load,&f)==PS_LIMIT && !help.document && !*help.path && help.first==0 && help.end==0 && f.loads==5);
    ps_completion_help_destroy(&help);CHECK(!help.document && !help.symbol && !help.attempted);
    f.fail=true;CHECK(ps_completion_help_select(&help,&alpha,load,&f)==PS_IO && f.loads==6);
    f.fail=false;CHECK(ps_completion_help_select(&help,&alpha,load,&f)==PS_IO && f.loads==6);
    ps_completion_help_destroy(&help);CHECK(ps_completion_help_select(&help,&alpha,load,&f)==PS_OK && f.loads==7);
    ps_completion_help_destroy(&help);f.empty=true;
    CHECK(ps_completion_help_select(&help,&alpha,load,&f)==PS_INVALID && !help.document);
    ps_completion_help_destroy(&help);ps_completion_help_destroy(&other);ps_completion_help_destroy(NULL);
    puts("Completion help: own documents, same-page reuse, selection identity, complete argument table, failure caching, no stale section, guide identity, retry after close and invalid-input preservation passed");
    return 0;
}
