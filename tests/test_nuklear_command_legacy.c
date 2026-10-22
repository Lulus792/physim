/* The optional application Float path must not silently change standalone
 * Nuklear's original command ABI. This TU intentionally does not include ui.h. */
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_IMPLEMENTATION
#include "nuklear.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Legacy command %d: %s\n",__LINE__,#x);return 1;}}while(0)
static float width(nk_handle h,float size,const char *text,int len){(void)h;(void)size;(void)text;return (float)len;}
int main(void) {
    CHECK(sizeof(nk_command_coord)==sizeof(short) && sizeof(nk_command_size)==sizeof(unsigned short) && sizeof(nk_command_point)==sizeof(struct nk_vec2i));
    struct nk_user_font font={0};font.height=10;font.width=width;struct nk_context ctx;CHECK(nk_init_default(&ctx,&font));
    int found=0;if(nk_begin(&ctx,"legacy",nk_rect(0,0,200,100),NK_WINDOW_NO_SCROLLBAR))nk_stroke_line(nk_window_get_canvas(&ctx),10.25f,20.75f,40.5f,20.75f,1.5f,nk_rgb(200,20,30));nk_end(&ctx);
    const struct nk_command *cmd;nk_foreach(cmd,&ctx)if(cmd->type==NK_COMMAND_LINE){const struct nk_command_line *c=(const struct nk_command_line*)cmd;CHECK(c->begin.x==10 && c->begin.y==20 && c->end.x==40 && c->line_thickness==1);found++;}
    CHECK(found==1);nk_free(&ctx);puts("Standalone Nuklear: original short command ABI retained");return 0;
}
