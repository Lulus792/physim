#ifndef PHYSIM_LANGUAGE_DIAGNOSTIC_H
#define PHYSIM_LANGUAGE_DIAGNOSTIC_H
#include "language_string.h"
#include "diagnostic.h"
static inline void psrt_diagnostic_check(ps_result result,psrt_site site) {
    if(result!=PS_OK)psrt_fail_code(site,result,ps_result_string(result));
}
static inline ps_diagnostic psrt_diagnostic_make(int64_t code,const char *operation,const char *argument,
    const char *source,int64_t line,int64_t column,const char *message,psrt_site site) {
    if(code<PS_INVALID || code>PS_NUMERIC || line<0 || (uint64_t)line>UINT32_MAX || column<0 || (uint64_t)column>UINT32_MAX)
        psrt_fail_code(site,PS_INVALID,"Invalid diagnostic code or source coordinates");
    ps_diagnostic result;psrt_diagnostic_check(ps_diagnostic_set(&result,(ps_result)code,operation,argument,source,(uint32_t)line,(uint32_t)column,message),site);return result;
}
static inline ps_diagnostic psrt_diagnostic_here(int64_t code,const char *operation,const char *argument,const char *message,psrt_site site) {
    return psrt_diagnostic_make(code,operation,argument,site.file,(int64_t)site.line,(int64_t)site.column,message,site);
}
static inline ps_diagnostic psrt_diagnostic_empty(psrt_site site){(void)site;ps_diagnostic result;ps_diagnostic_clear(&result);return result;}
static inline bool psrt_diagnostic_valid(ps_diagnostic value,psrt_site site){(void)site;return ps_diagnostic_valid(&value);}
static inline int64_t psrt_diagnostic_code(ps_diagnostic value,psrt_site site){(void)site;return value.code;}
static inline int64_t psrt_diagnostic_line(ps_diagnostic value,psrt_site site){(void)site;return value.line;}
static inline int64_t psrt_diagnostic_column(ps_diagnostic value,psrt_site site){(void)site;return value.column;}
static inline psrt_string psrt_diagnostic_string(ps_allocator allocator,ps_diagnostic value,unsigned field,psrt_site site) {
    char formatted[PS_DIAGNOSTIC_WIRE_MAX+256];const char *text=field==0?value.operation:field==1?value.argument:field==2?value.source:value.message;
    if(field==4){psrt_diagnostic_check(ps_diagnostic_format(&value,formatted,sizeof formatted),site);text=formatted;}
    psrt_string result;psrt_diagnostic_check(psrt_string_make(allocator,text,strlen(text),&result),site);return result;
}
#define PSRT_DIAGNOSTIC_TEXT(name,field) static inline psrt_string psrt_diagnostic_##name(ps_allocator a,ps_diagnostic d,psrt_site s){return psrt_diagnostic_string(a,d,field,s);}
PSRT_DIAGNOSTIC_TEXT(operation,0)
PSRT_DIAGNOSTIC_TEXT(argument,1)
PSRT_DIAGNOSTIC_TEXT(source,2)
PSRT_DIAGNOSTIC_TEXT(message,3)
PSRT_DIAGNOSTIC_TEXT(formatted,4)
#undef PSRT_DIAGNOSTIC_TEXT
static inline void psrt_diagnostic_save(ps_diagnostic value,const char *path,psrt_site site){psrt_diagnostic_check(ps_diagnostic_save(path,&value),site);}
static inline ps_diagnostic psrt_diagnostic_load(const char *path,psrt_site site){ps_diagnostic result;psrt_diagnostic_check(ps_diagnostic_load(path,&result),site);return result;}
static inline psrt_array psrt_diagnostic_encoded(ps_allocator allocator,ps_diagnostic value,psrt_site site) {
    unsigned char data[PS_DIAGNOSTIC_WIRE_MAX];size_t n=ps_diagnostic_encode(data,sizeof data,&value);
    if(!n)psrt_fail_code(site,PS_INVALID,"Cannot encode an empty or invalid diagnostic");
    int64_t bytes[PS_DIAGNOSTIC_WIRE_MAX];for(size_t i=0;i<n;i++)bytes[i]=data[i];
    static const psrt_element_type element={sizeof(int64_t),NULL,NULL};psrt_array result;
    psrt_diagnostic_check(psrt_array_init(&element,allocator,n,&result),site);
    ps_result status=psrt_array_build_begin(&result,n);
    if(status==PS_OK)status=psrt_array_builder_append(&result,bytes,n);
    if(status!=PS_OK){psrt_array_destroy(&result);psrt_diagnostic_check(status,site);}return result;
}
static inline ps_diagnostic psrt_diagnostic_decoded(const int64_t *bytes,size_t count,psrt_site site) {
    if(!bytes || count>PS_DIAGNOSTIC_WIRE_MAX)psrt_fail_code(site,PS_INVALID,"Diagnostic bytes exceed the bounded payload");
    unsigned char data[PS_DIAGNOSTIC_WIRE_MAX];for(size_t i=0;i<count;i++) {
        if(bytes[i]<0 || bytes[i]>255)psrt_fail_code(site,PS_INVALID,"Diagnostic bytes must be in 0..255");data[i]=(unsigned char)bytes[i];
    }
    ps_diagnostic result;psrt_diagnostic_check(ps_diagnostic_decode(data,count,&result),site);return result;
}
static inline void psrt_diagnostic_raise(ps_diagnostic value,psrt_site site) {
    if(!ps_diagnostic_valid(&value) || value.code==PS_OK)psrt_fail_code(site,PS_INVALID,"Cannot raise an empty diagnostic");
    psrt_raise(site,value.code,value.message,&value);
}
#endif
