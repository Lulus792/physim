/* H's SVG labels use an independent italic serif face. Keep the atlas's ASCII
 * invariant plus theta; this is not the handbook's full fallback repertoire. */
static struct nk_font *documentation_figure_font(struct nk_font_atlas *atlas,const char *root,float size) {
    static const nk_rune ranges[]={32,126,0x3b8,0x3b8,0};
    struct nk_font_config config=nk_font_config(size);config.range=ranges;
    config.oversample_h=2;config.oversample_v=1;
    const char *bundled=SDL_getenv("PHYSIM_TEST_BUNDLED_FONTS");
    bool force_bundled=bundled && !strcmp(bundled,"1");
    char path[4096];struct nk_font *font=NULL;
#ifdef _WIN32
    const char *windows=SDL_getenv("WINDIR");
    snprintf(path,sizeof path,"%s/Fonts/georgiai.ttf",windows?windows:"C:/Windows");
    if(!force_bundled && exists(path))font=nk_font_atlas_add_from_file(atlas,path,size,&config);
#elif defined(__APPLE__)
    const char *paths[]={"/System/Library/Fonts/Supplemental/Georgia Italic.ttf","/Library/Fonts/Georgia Italic.ttf"};
    for(unsigned i=0;!force_bundled && !font && i<sizeof paths/sizeof paths[0];i++) {
        snprintf(path,sizeof path,"%s",paths[i]);
        if(exists(path))font=nk_font_atlas_add_from_file(atlas,path,size,&config);
    }
#else
    const char *paths[]={"/usr/share/fonts/truetype/msttcorefonts/Georgia_Italic.ttf",
        "/usr/share/fonts/truetype/msttcorefonts/georgiai.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Italic.ttf","/usr/share/fonts/dejavu/DejaVuSerif-Italic.ttf"};
    for(unsigned i=0;!force_bundled && !font && i<sizeof paths/sizeof paths[0];i++) {
        snprintf(path,sizeof path,"%s",paths[i]);
        if(exists(path))font=nk_font_atlas_add_from_file(atlas,path,size,&config);
    }
#endif
    if(!font) {
        int n=snprintf(path,sizeof path,"%s/third_party/fonts/NotoSerif-Italic.ttf",root);
        if(n<0 || (size_t)n>=sizeof path)return NULL;
        font=nk_font_atlas_add_from_file(atlas,path,size,&config);
    }
    if(font)printf("F1 figure font: %s\n",path);
    return font;
}
