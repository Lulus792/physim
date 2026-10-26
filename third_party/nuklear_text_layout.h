/* Physim's optional immutable visual layout for readonly Nuklear text fields.
 * Positions are Unicode scalar indices; byte offsets address the original UTF-8.
 * Rows include hard separators in limit, but exclude them in end. No separator
 * bytes are introduced for soft wraps. The owner outlives each edit call. */
#ifndef NK_TEXT_LAYOUT_H_
#define NK_TEXT_LAYOUT_H_
struct nk_text_visual_glyph {
    int byte;
    float x, advance;
    unsigned char boundary, draw;
};
struct nk_text_visual_row {
    int first, end, limit;
    float width, edge, hyphen_x;
    unsigned char hyphen;
};
struct nk_text_visual {
    const struct nk_text_visual_glyph *glyphs; /* glyph_count + 1 entries */
    const struct nk_text_visual_row *rows;
    int glyph_count, row_count;
    float row_height, width;
};
static inline int nk_text_visual_row_at(const struct nk_text_visual *v,int scalar,int affinity) {
    int low=0,high=v->row_count;
    while(low+1<high) {
        int mid=low+(high-low)/2;
        if(v->rows[mid].first<=scalar)low=mid;else high=mid;
    }
    if(affinity && low>0 && v->rows[low-1].end==scalar && v->rows[low-1].limit==scalar)low--;
    return low;
}
static inline float nk_text_visual_x(const struct nk_text_visual *v,int scalar,int row) {
    if(scalar>=v->rows[row].end)return v->rows[row].edge;
    return v->glyphs[scalar<v->rows[row].first?v->rows[row].first:scalar].x;
}
static inline int nk_text_visual_hit(const struct nk_text_visual *v,float x,float y,int *affinity) {
    int row=y<=0?0:(y/v->row_height>=(float)v->row_count?v->row_count-1:(int)(y/v->row_height));
    const struct nk_text_visual_row *r=&v->rows[row];
    int previous=r->first,chosen=r->end;
    float previous_x=0;
    for(int at=r->first+1;at<=r->end;at++)if(v->glyphs[at].boundary) {
        float next_x=nk_text_visual_x(v,at,row);
        if(x<previous_x+(next_x-previous_x)*.5f){chosen=previous;break;}
        previous=at;previous_x=next_x;
    }
    if(affinity)*affinity=chosen==r->end && r->end==r->limit && row+1<v->row_count;
    return chosen;
}
#endif
