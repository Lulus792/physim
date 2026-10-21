/* CSS line boxes and tracking over native fonts. Measurement and drawing share
 * the same UTF-8 line breaks; no glyph is discarded at a narrow panel edge. */
static size_t workbench_spaced_line(const struct nk_user_font *font,const char *text,size_t bytes,float width,float tracking,size_t *length) {
    size_t at=0,space=SIZE_MAX;float advance=0;
    while(at<bytes) {
        if(text[at]=='\n'){*length=at;return at+1;}
        nk_rune rune;int count=nk_utf_decode(text+at,&rune,(int)(bytes-at));if(count<=0)count=1;
        float glyph=font->width(font->userdata,font->height,text+at,count),gap=at?tracking:0;
        if(at && advance+gap+glyph>width) {
            if(space!=SIZE_MAX && space>0){*length=space;return space+1;}
            *length=at;return at;
        }
        if(text[at]==' ')space=at;
        advance+=gap+glyph;at+=(size_t)count;
    }
    *length=bytes;return bytes;
}
static float workbench_text_height(const struct nk_user_font *font,const char *text,size_t bytes,float width,float pitch,float tracking) {
    size_t at=0,lines=0;
    while(at<bytes){size_t length=0,used=workbench_spaced_line(font,text+at,bytes-at,fmaxf(1,width),tracking,&length);if(!used)break;at+=used;lines++;}
    return lines*fmaxf(pitch,font->height);
}
static void workbench_text_draw(app *a,const struct nk_user_font *font,struct nk_rect r,const char *text,size_t bytes,float pitch,float tracking,struct nk_color color) {
    struct nk_command_buffer *canvas=nk_window_get_canvas(a->ui);size_t at=0;pitch=fmaxf(pitch,font->height);float y=r.y+(pitch-font->height)/2;
    while(at<bytes) {
        size_t length=0,used=workbench_spaced_line(font,text+at,bytes-at,fmaxf(1,r.w),tracking,&length);if(!used)break;
        if(tracking==0)nk_draw_text(canvas,nk_rect(r.x,y,r.w,font->height),text+at,(int)length,font,nk_rgba(0,0,0,0),color);
        else {
            size_t offset=0;float x=r.x;
            while(offset<length) {
                nk_rune rune;int count=nk_utf_decode(text+at+offset,&rune,(int)(length-offset));if(count<=0)count=1;
                float width=font->width(font->userdata,font->height,text+at+offset,count);
                nk_draw_text(canvas,nk_rect(x,y,width+1,font->height),text+at+offset,count,font,nk_rgba(0,0,0,0),color);
                x+=width+tracking;offset+=(size_t)count;
            }
        }
        at+=used;y+=pitch;
    }
}
static struct nk_rect workbench_text_row(app *a,const struct nk_user_font *font,const char *text,float pitch,float tracking,struct nk_color color) {
    float width=nk_window_get_content_region(a->ui).w;
    ps_ui_row_exact_dynamic(a->ui,workbench_text_height(font,text,strlen(text),width,pitch,tracking),1);
    struct nk_rect r;nk_widget(&r,a->ui);workbench_text_draw(a,font,r,text,strlen(text),pitch,tracking,color);
    ps_ui_accessibility_label(a->ui,r,text);return r;
}
