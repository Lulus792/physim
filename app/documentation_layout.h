#ifndef PHYSIM_DOCUMENTATION_LAYOUT_H
#define PHYSIM_DOCUMENTATION_LAYOUT_H
/* Logical coordinates: each side is anchored independently to the window. */
typedef struct { float x,y,w,h; } ps_doc_rect;
typedef struct { ps_doc_rect header,left,article,right,footer; } ps_doc_layout;
static ps_doc_layout ps_documentation_layout(float width,float height,float ui_size,
                                            int left_hidden,int right_hidden) {
    float side=width<1000?220:250;
    float header=ui_size+36>60?ui_size+36:60;
    float footer=ui_size+16>32?ui_size+16:32;
    float left=left_hidden?0:side,right=right_hidden?0:side;
    ps_doc_layout r={{0,0,width,header},{0,header,left,height-header-footer},
        {left,header,width-left-right,height-header-footer},
        {width-right,header,right,height-header-footer},{0,height-footer,width,footer}};
    return r;
}
#endif
