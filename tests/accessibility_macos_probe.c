/* Independent process: only Apple's public AX/CF APIs, no Physim provider code. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>

static AXError last_error;
#define CHECK(condition) do {if(!(condition)){fprintf(stderr,"MAC AX line %d: %s (AXError %d)\n",__LINE__,#condition,(int)last_error);exit(1);}}while(0)
static CFTypeRef attribute(AXUIElementRef node,CFStringRef name) {
    CFTypeRef value=NULL;last_error=AXUIElementCopyAttributeValue(node,name,&value);
    return last_error==kAXErrorSuccess?value:NULL;
}
static int string_equal(CFTypeRef value,CFStringRef expected) {
    return value && CFGetTypeID(value)==CFStringGetTypeID() && CFStringCompare(value,expected,0)==kCFCompareEqualTo;
}
static AXUIElementRef document_in(AXUIElementRef node,unsigned depth,unsigned *visits) {
    if(depth>12 || ++*visits>4096)return NULL;
    CFTypeRef identifier=attribute(node,CFSTR("AXIdentifier"));
    int match=identifier && CFGetTypeID(identifier)==CFStringGetTypeID() &&
        CFStringHasPrefix(identifier,CFSTR("physim-document-"));
    if(identifier)CFRelease(identifier);
    if(match) {
        CFTypeRef role=attribute(node,kAXRoleAttribute);match=string_equal(role,kAXStaticTextRole);
        if(role)CFRelease(role);
        if(match)return (AXUIElementRef)CFRetain(node);
    }
    CFTypeRef children=attribute(node,kAXChildrenAttribute);AXUIElementRef result=NULL;
    if(children && CFGetTypeID(children)==CFArrayGetTypeID()) {
        CFIndex count=CFArrayGetCount(children);
        for(CFIndex i=0;i<count && !result;i++) {
            CFTypeRef child=CFArrayGetValueAtIndex(children,i);
            if(child && CFGetTypeID(child)==AXUIElementGetTypeID())result=document_in((AXUIElementRef)child,depth+1,visits);
        }
    }
    if(children)CFRelease(children);return result;
}
static AXUIElementRef find_document(AXUIElementRef application) {
    CFTypeRef windows=attribute(application,kAXWindowsAttribute);AXUIElementRef result=NULL;
    if(windows && CFGetTypeID(windows)==CFArrayGetTypeID()) {
        for(CFIndex i=0;i<CFArrayGetCount(windows) && !result;i++) {
            AXUIElementRef window=(AXUIElementRef)CFArrayGetValueAtIndex(windows,i);
            CFTypeRef title=attribute(window,kAXTitleAttribute);
            if(string_equal(title,CFSTR("Physim | Dokumentation"))){unsigned visits=0;result=document_in(window,0,&visits);}
            if(title)CFRelease(title);
        }
    }
    if(windows)CFRelease(windows);return result;
}
static CFTypeRef range_attribute(AXUIElementRef node,CFStringRef name,CFRange range) {
    AXValueRef parameter=AXValueCreate(kAXValueCFRangeType,&range);CFTypeRef result=NULL;
    CHECK(parameter);last_error=AXUIElementCopyParameterizedAttributeValue(node,name,parameter,&result);
    CFRelease(parameter);CHECK(last_error==kAXErrorSuccess && result);return result;
}
int main(int argc,char **argv) {
    Boolean trusted=AXIsProcessTrusted();printf("MAC AX TRUST: %s\n",trusted?"trusted":"unavailable");fflush(stdout);
    if(argc==2 && !strcmp(argv[1],"--trust"))return trusted?0:77;
    if(!trusted)return 77;
    if(argc!=3){fputs("Usage: accessibility-macos-probe PID TRANSCRIPT\n",stderr);return 2;}
    char *end=NULL;long number=strtol(argv[1],&end,10);
    CHECK(end && !*end && number>0 && number<=INT_MAX);
    printf("MAC AX CLIENT: pid=%ld application_pid=%ld\n",(long)getpid(),number);fflush(stdout);
    AXUIElementRef application=AXUIElementCreateApplication((pid_t)number);CHECK(application);
    last_error=AXUIElementSetMessagingTimeout(application,1.0f);CHECK(last_error==kAXErrorSuccess);
    AXUIElementRef document=NULL;CFAbsoluteTime until=CFAbsoluteTimeGetCurrent()+15;
    do {
        document=find_document(application);
        if(!document){struct timespec pause={0,20000000};nanosleep(&pause,NULL);}
    }while(!document && CFAbsoluteTimeGetCurrent()<until);
    CHECK(document);
    CFTypeRef raw=attribute(document,kAXValueAttribute);CHECK(raw && CFGetTypeID(raw)==CFStringGetTypeID());
    CFStringRef text=raw;CFIndex count=CFStringGetLength(text);CHECK(count>256*1024 && count<32*1024*1024);
    CFTypeRef count_value=attribute(document,kAXNumberOfCharactersAttribute);SInt64 native_count=-1;
    CHECK(count_value && CFGetTypeID(count_value)==CFNumberGetTypeID());
    CHECK(CFNumberGetValue(count_value,kCFNumberSInt64Type,&native_count) && native_count==count);CFRelease(count_value);
    CFIndex capacity=CFStringGetMaximumSizeForEncoding(count,kCFStringEncodingUTF8)+1;
    CHECK(capacity>0 && capacity<128*1024*1024);char *utf8=malloc((size_t)capacity);CHECK(utf8);
    CHECK(CFStringGetCString(text,utf8,capacity,kCFStringEncodingUTF8));
    FILE *file=fopen(argv[2],"wb");CHECK(file);size_t bytes=strlen(utf8);
    CHECK(fwrite(utf8,1,bytes,file)==bytes && fclose(file)==0);free(utf8);
    CFRange tail=CFRangeMake(count-128,128);
    UniChar first=CFStringGetCharacterAtIndex(text,tail.location);
    if(first>=0xdc00 && first<=0xdfff){--tail.location;++tail.length;}
    CFTypeRef substring=range_attribute(document,kAXStringForRangeParameterizedAttribute,tail);
    CFStringRef expected=CFStringCreateWithSubstring(NULL,text,tail);
    CHECK(string_equal(substring,expected));CFRelease(expected);CFRelease(substring);
    CFTypeRef geometry=range_attribute(document,kAXBoundsForRangeParameterizedAttribute,CFRangeMake(0,1));
    CGRect frame=CGRectZero;
    CHECK(CFGetTypeID(geometry)==AXValueGetTypeID() && AXValueGetValue(geometry,kAXValueCGRectType,&frame));
    CHECK(isfinite(frame.origin.x) && isfinite(frame.origin.y) && frame.size.width>0 && frame.size.height>0);CFRelease(geometry);
    Boolean writable=true;last_error=AXUIElementIsAttributeSettable(document,kAXValueAttribute,&writable);
    CHECK(last_error==kAXErrorSuccess && !writable);
    last_error=AXUIElementIsAttributeSettable(document,kAXVisibleCharacterRangeAttribute,&writable);
    CHECK(last_error==kAXErrorSuccess && writable);
    CFIndex last=count;
    while(last) {UniChar ch=CFStringGetCharacterAtIndex(text,last-1);if(ch!=' ' && ch!='\t' && ch!='\r' && ch!='\n')break;--last;}
    CHECK(last>0);CFRange target=CFRangeMake(last-1,1);AXValueRef range=AXValueCreate(kAXValueCFRangeType,&target);CHECK(range);
    last_error=AXUIElementSetAttributeValue(document,kAXVisibleCharacterRangeAttribute,range);CFRelease(range);
    CHECK(last_error==kAXErrorSuccess);
    printf("MAC AX READING: bytes=%zu utf16=%ld complete=1 substring=1 initial_geometry=1 readonly=1 reveal_requested=1\n",bytes,(long)count);
    CFRelease(text);CFRelease(document);CFRelease(application);return 0;
}
#else
int main(void){fputs("MAC AX TRUST: requires macOS\n",stderr);return 77;}
#endif
