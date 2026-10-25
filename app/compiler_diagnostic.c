#include "compiler_diagnostic.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

static bool number(const char **cursor, const char *begin, int *out) {
    const char *end=*cursor,*start=end;
    while(start>begin && start[-1]>='0' && start[-1]<='9')--start;
    if(start==end)return false;
    unsigned value=0;
    for(const char *p=start;p<end;++p) {
        unsigned digit=(unsigned)(*p-'0');
        if(value>((unsigned)INT_MAX-digit)/10)return false;
        value=value*10+digit;
    }
    if(!value)return false;
    *out=(int)value;*cursor=start;return true;
}
bool ps_compiler_diagnostic_parse(const char *text, ps_compiler_diagnostic *out) {
    if(!text || !out)return false;
    const char *begin=text;
    while(*begin==' ' || *begin=='\t')++begin;
    for(const char *delimiter=strchr(begin,':');delimiter;delimiter=strchr(delimiter+1,':')) {
        const char *message=delimiter+1;
        while(*message==' ' || *message=='\t')++message;
        size_t severity=!strncmp(message,"runtime error",13)?13:!strncmp(message,"fatal error",11)?11:
                        !strncmp(message,"warning",7)?7:!strncmp(message,"error",5)?5:0;
        if(!severity || (message[severity]!=':' && message[severity]!=' ' && message[severity]!='\t'))continue;
        const char *end=delimiter;
        while(end>begin && (end[-1]==' ' || end[-1]=='\t'))--end;
        int line=0,column=1;
        const char *source_end=NULL,*cursor=end;
        if(cursor>begin && cursor[-1]==')') {
            --cursor;int last;
            if(!number(&cursor,begin,&last))continue;
            if(cursor>begin && cursor[-1]==',') {
                column=last;--cursor;
                if(!number(&cursor,begin,&line))continue;
            } else line=last;
            if(cursor==begin || cursor[-1]!='(')continue;
            source_end=cursor-1;
        } else {
            int last;
            if(!number(&cursor,begin,&last) || cursor==begin || cursor[-1]!=':')continue;
            source_end=cursor-1;line=last;
            const char *previous=source_end,*digits=previous;int row;
            while(digits>begin && digits[-1]>='0' && digits[-1]<='9')--digits;
            if(digits>begin+1 && (digits[-1]=='-' || digits[-1]=='+') && digits[-2]==':')continue;
            if(digits<previous && digits>begin && digits[-1]==':') {
                if(!number(&previous,begin,&row))continue;
                line=row;column=last;source_end=previous-1;
            }
        }
        while(source_end>begin && (source_end[-1]==' ' || source_end[-1]=='\t'))--source_end;
        size_t length=(size_t)(source_end-begin);
        if(!length || length>=sizeof out->source)continue;
        bool valid=true;
        for(size_t i=0;i<length;i++)valid=valid && (unsigned char)begin[i]>=32 && begin[i]!=127;
        if(!valid)continue;
        ps_compiler_diagnostic value={.line=line,.column=column,.level=severity==7?PS_LOG_WARNING:PS_LOG_ERROR};
        memcpy(value.source,begin,length);value.source[length]=0;
        snprintf(value.message,sizeof value.message,"%s",message);
        *out=value;return true;
    }
    return false;
}
