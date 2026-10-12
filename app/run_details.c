#include "run_details.h"
#include "text_validation.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
typedef struct {const char *value;size_t length;} field;
static bool append(char *text,size_t *used,const char *format,...) {
    va_list args;va_start(args,format);
    int n=vsnprintf(text+*used,PS_RUN_DETAILS_TEXT_CAPACITY-*used,format,args);va_end(args);
    if(n<0 || (size_t)n>=PS_RUN_DETAILS_TEXT_CAPACITY-*used)return false;
    *used+=(size_t)n;return true;
}
static bool find(const char *metadata,const char *key,field *out) {
    size_t key_length=strlen(key);bool seen=false;
    for(const char *line=metadata;*line;) {
        size_t n=strcspn(line,"\r\n");
        if(n>key_length && !memcmp(line,key,key_length) && line[key_length]=='=') {
            if(seen)return false;
            *out=(field){line+key_length+1,n-key_length-1};seen=true;
        }
        line+=n;if(*line)line++;
    }
    return true;
}
ps_result ps_run_details_format(const char *metadata,size_t length,char *out,size_t capacity) {
    if(!out || (!metadata && length) || length>=8192)return PS_INVALID;
    if(length && memchr(metadata,0,length))return PS_CORRUPT;
    char source[8192],text[PS_RUN_DETAILS_TEXT_CAPACITY];size_t used=0;
    if(length)memcpy(source,metadata,length);source[length]=0;
    if(!ps_text_valid(source,sizeof source,true))return PS_CORRUPT;
    const char *keys[]={"geometry","laws","medium","losses","numerics","measurement","sampling"};
    const char *labels[]={"Geometrie und Anfangszustand","Ideale physikalische Gesetze","Material und Medium","Verlust- und Störmodelle","Numerische Methode und Toleranzen","Messgerät und Unsicherheit","Verteilungen und Monte Carlo"};
    if(!append(text,&used,"MODELLSCHICHTEN\nAngaben des Experimentmoduls; fehlende Angaben bedeuten nicht, dass eine Schicht inaktiv ist.\n\n"))return PS_LIMIT;
    for(unsigned i=0;i<7;i++) {
        char key[64];snprintf(key,sizeof key,"layer.%s",keys[i]);field f={0};
        if(!find(source,key,&f))return PS_CORRUPT;
        const char *state="Nicht dokumentiert",*detail="";size_t detail_length=0;
        if(f.value) {
            const char *separator=memchr(f.value,'|',f.length);if(!separator || separator+1==f.value+f.length)return PS_CORRUPT;
            size_t n=(size_t)(separator-f.value);
            if(n==6 && !memcmp(f.value,"active",6))state="Aktiv";
            else if(n==5 && !memcmp(f.value,"ideal",5))state="Idealisiert";
            else if(n==7 && !memcmp(f.value,"omitted",7))state="Nicht modelliert";
            else return PS_CORRUPT;
            detail=separator+1;detail_length=f.length-n-1;
        }
        if(!append(text,&used,"%s\n%s%s%.*s\n\n",labels[i],state,detail_length?" · ":"",(int)detail_length,detail))return PS_LIMIT;
    }
    const char *names[]={"model","excluded","experiment","dt_s","seed","step_mode","integrator","adaptive_integrator","rk45_absolute_tolerance","rk45_relative_tolerance","adaptive_absolute_tolerance","adaptive_relative_tolerance","start_unix_seconds","start_nanoseconds","core_compiler","core_build","module_compiler","module_build","runner_compiler","runner_build","project_id","experiment_id"};
    const char *titles[]={"Modell","Ausgeschlossene Effekte","Experiment","Startschritt [s]","Seed","Schrittmodus","Integrator","Adaptiver Integrator","RK45 absolute Toleranz","RK45 relative Toleranz","Adaptive absolute Toleranz","Adaptive relative Toleranz","Startzeit: Unixsekunden [UTC]","Startzeit: Nanosekundenanteil","Core-Compiler","Core-Build","Modul-Compiler","Modul-Build","Runner-Compiler","Runner-Build","Projekt-ID","Experiment-ID"};
    if(!append(text,&used,"MODELL, LAUF UND HERKUNFT\n"))return PS_LIMIT;
    for(unsigned i=0;i<sizeof names/sizeof *names;i++) {
        field f={0};if(!find(source,names[i],&f))return PS_CORRUPT;
        if(!append(text,&used,"%s: %.*s\n",titles[i],(int)(f.value?f.length:strlen("Nicht dokumentiert")),f.value?f.value:"Nicht dokumentiert"))return PS_LIMIT;
    }
    if(!append(text,&used,"\nARCHIVIERTE PARAMETER\n"))return PS_LIMIT;
    bool parameters=false;
    for(const char *line=source;*line;) {
        size_t n=strcspn(line,"\r\n");
        if(n>=10 && (!memcmp(line,"parameter.",10) || !memcmp(line,"parameter_",10))) {
            if(!append(text,&used,"%.*s\n",(int)n,line))return PS_LIMIT;
            parameters=true;
        }
        line+=n;if(*line)line++;
    }
    if(!parameters && !append(text,&used,"Nicht dokumentiert\n"))return PS_LIMIT;
    if(used>=capacity)return PS_LIMIT;
    memcpy(out,text,used+1);return PS_OK;
}
