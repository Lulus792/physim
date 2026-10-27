/* CSS measurements use rendered widget rectangles; input steps need no images. */
static void protocol_typography_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned rendered,attempts;
    if(*stage<3){(*stage)++;return;}if(rendered++<2)return;rendered=0;
    ps_protocol_ui *p=&a->protocol_view;float f=a->preferences.ui_size/16.f;bool ok=true;
    if(*stage==3){test_mouse(a,nk_rect(p->bounds.x+4,p->bounds.y+16,1,1),true);(*stage)++;return;}
    if(*stage==4){test_mouse(a,nk_rect(p->bounds.x+4,p->bounds.y+16,1,1),false);(*stage)++;return;}
    if(*stage==5) {
        float top=p->bounds.y+2,bottom=p->bounds.y+p->bounds.h-30;
        /* Pane geometry is current even when its clipped group was not drawn.
         * The previous header rectangle can then be a stale observation. */
        float y=p->panes[2].y+20*f-(float)p->detail_scroll_y;
        float height=p->detail_header.w?p->detail_header.h:20*f;
        if((!p->detail_header.w || y<top || y+height>bottom) && attempts++<100) {
            test_plot_wheel(a,p->bounds.x+8,p->bounds.y+p->bounds.h*.5f,y<top?3:-3);return;
        }
        (*stage)++;return;
    }
    bool long_meta=strstr(mode,"long")!=NULL;
    float title_line=17.5f*1.55f*f,body_line=11.2f*1.55f*f;
    ok &= fabsf(p->detail_meta_font/(a->workbench_regular_em*f)-10.92f)<.02f;
    ok &= fabsf(p->detail_title_line-title_line)<.02f && fabsf(p->detail_impact_line-body_line)<.02f;
    ok &= fabsf(p->detail_badge.h-19.92f*f)<.02f;
    ok &= fabsf(p->detail_title.y-p->detail_header.y-p->detail_header.h-10*f)<.05f;
    ok &= fabsf(p->detail_impact.y-p->detail_title.y-p->detail_title.h-17*f)<.05f;
    ok &= p->detail_property_count==(long_meta?3u:2u);
    ok &= fabsf(p->detail_property_line-body_line)<.02f;
    if(p->detail_property_count) {
        ok &= fabsf(p->detail_properties[0].y-p->detail_impact.y-p->detail_impact.h-17*f)<.05f;
        unsigned last=p->detail_property_count-1;
        ok &= fabsf(p->original_toggle.y-p->detail_properties[last].y-p->detail_properties[last].h-18*f)<.05f;
    }
    for(unsigned i=0;i<p->detail_property_count;i++) {
        struct nk_rect label=p->detail_labels[i],value=p->detail_values[i],row=p->detail_properties[i];
        ok &= fabsf(value.x-label.x-label.w-14*f)<.05f && fabsf(label.w-value.w)<.05f;
        ok &= fabsf(row.h/body_line-roundf(row.h/body_line))<.01f;
        if(i)ok &= fabsf(row.y-p->detail_properties[i-1].y-p->detail_properties[i-1].h-10*f)<.05f;
    }
    ok &= fabsf(p->original_toggle.h-(11.2f*1.55f+10)*f)<.05f;
    if(long_meta)ok &= p->detail_meta.y>p->detail_header.y+19.92f*f && p->detail_meta.h>16.926f*f;
    else ok &= !strcmp(p->detail_meta_text,"Build · main.c:13:14") && fabsf(p->detail_header.h-19.92f*f)<.05f;
    uint32_t crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
    ok &= crc==protocol_test_source_crc;
    printf("{\"kind\":\"protocol_typography\",\"passed\":%s,\"source_crc\":%u,\"scale\":%.4f,\"meta_font\":%.4f,\"title_line\":%.4f,\"impact_line\":%.4f,\"property_line\":%.4f,\"header\":[%.4f,%.4f,%.4f,%.4f],\"badge_height\":%.4f,\"meta\":[%.4f,%.4f,%.4f,%.4f],\"title\":[%.4f,%.4f,%.4f,%.4f],\"impact\":[%.4f,%.4f,%.4f,%.4f],\"summary\":[%.4f,%.4f,%.4f,%.4f],\"properties\":[",
        ok?"true":"false",(unsigned)crc,f,p->detail_meta_font/(a->workbench_regular_em*f),p->detail_title_line,p->detail_impact_line,p->detail_property_line,
        p->detail_header.x,p->detail_header.y,p->detail_header.w,p->detail_header.h,p->detail_badge.h,
        p->detail_meta.x,p->detail_meta.y,p->detail_meta.w,p->detail_meta.h,p->detail_title.x,p->detail_title.y,p->detail_title.w,p->detail_title.h,
        p->detail_impact.x,p->detail_impact.y,p->detail_impact.w,p->detail_impact.h,p->original_toggle.x,p->original_toggle.y,p->original_toggle.w,p->original_toggle.h);
    for(unsigned i=0;i<p->detail_property_count;i++) {
        struct nk_rect r=p->detail_properties[i],l=p->detail_labels[i],v=p->detail_values[i];
        printf("%s{\"row\":[%.4f,%.4f,%.4f,%.4f],\"label\":[%.4f,%.4f,%.4f,%.4f],\"value\":[%.4f,%.4f,%.4f,%.4f]}",
            i?",":"",r.x,r.y,r.w,r.h,l.x,l.y,l.w,l.h,v.x,v.y,v.w,v.h);
    }
    printf("]}\nPROTOCOL TYPOGRAPHY SELF-TEST: %s\n",ok?"PASSED":"FAILED");fflush(stdout);
    *capture=ok?"protocol-detail-type.png":"protocol-detail-type-failure.png";*code=ok?0:1;a->quitting=true;(*stage)++;
}
