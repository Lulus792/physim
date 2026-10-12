/* Use the supplied SDK/private header search path; no copied implementation. */
#include <documentation.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv) {
    bool page=argc==4 && !strcmp(argv[1],"--page");
    bool code=page && !strcmp(argv[3],"code");
    if(argc!=4 || (!page && strcmp(argv[1],"--anchor")))return 2;
    FILE *file=fopen(argv[2],"rb");if(!file)return 2;
    if(fseek(file,0,SEEK_END)){fclose(file);return 2;}
    long length=ftell(file);
    if(length<0 || (size_t)length>(code?PS_DOC_MAX_CODE_BYTES:PS_DOC_MAX_BYTES) || fseek(file,0,SEEK_SET)){fclose(file);return 2;}
    char *text=malloc((size_t)length+1);if(!text){fclose(file);return 2;}
    size_t read=fread(text,1,(size_t)length,file);int closed=fclose(file);
    if(read!=(size_t)length || closed){free(text);return 2;}
    text[length]=0;size_t block=SIZE_MAX;
    ps_document *document=NULL;
    ps_result result=ps_document_parse(text,(size_t)length,code,&document);
    if(result==PS_OK && !page)result=ps_document_anchor(document,argv[3],&block);
    if(result==PS_OK && !page)printf("%zu: %s\n",block,document->text+document->blocks[block].text);
    free(document);free(text);return result==PS_OK?0:1;
}
