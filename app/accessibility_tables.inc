/* Included after text_boundary; all storage belongs to one draft or publication. */
typedef struct {
 size_t begin,end,first_cell,count;float bounds[4];
} a11y_table_row;
struct ps_a11y_table_inventory {
 size_t count,row_count,cell_count;
 ps_a11y_table tables[PS_A11Y_TABLE_LIMIT];
 a11y_table_row rows[PS_A11Y_TABLE_ROWS];
 ps_a11y_cell_range cells[PS_A11Y_TABLE_CELLS];
};
static bool table_inventories_equal(const ps_a11y_document *a,const ps_a11y_document *b,bool geometry) {
 const ps_a11y_table_inventory *x=a?a->tables:NULL,*y=b?b->tables:NULL;
 size_t nx=x?x->count:0,ny=y?y->count:0;
 if(nx!=ny)return false;
 if(!nx)return true;
 if(x->row_count!=y->row_count || x->cell_count!=y->cell_count)return false;
 for(size_t i=0;i<nx;i++) {
  const ps_a11y_table *p=&x->tables[i],*q=&y->tables[i];
  if(p->id!=q->id || p->begin!=q->begin || p->end!=q->end || p->rows!=q->rows ||
     p->columns!=q->columns || p->column_headers!=q->column_headers)return false;
 }
 for(size_t i=0;i<x->row_count;i++) {
  const a11y_table_row *p=&x->rows[i],*q=&y->rows[i];
  if(p->begin!=q->begin || p->end!=q->end || p->first_cell!=q->first_cell || p->count!=q->count ||
     (geometry && memcmp(p->bounds,q->bounds,sizeof p->bounds)))return false;
 }
 for(size_t i=0;i<x->cell_count;i++)
  if(x->cells[i].begin!=y->cells[i].begin || x->cells[i].end!=y->cells[i].end)return false;
 return true;
}
static const ps_a11y_table *table_index(const ps_a11y_document *d,size_t index) {
 return d && d->active && d->tables && index<d->tables->count?&d->tables->tables[index]:NULL;
}
static const ps_a11y_table *table_id(const ps_a11y_document *d,uint64_t id) {
 if(!id || !d || !d->active || !d->tables)return NULL;
 for(size_t i=0;i<d->tables->count;i++)if(d->tables->tables[i].id==id)return &d->tables->tables[i];
 return NULL;
}
bool ps_a11y_document_table(ps_a11y_model *m,size_t begin,size_t end,size_t rows,size_t columns,
                           bool headers,const ps_a11y_table_row_input *input) {
 ps_a11y_document *d=m && m->building?m->document_draft:NULL;
 if(!d || !d->active || !input || !rows || rows>PS_A11Y_TABLE_ROWS || !columns ||
    columns>PS_A11Y_TABLE_CELLS || begin>=end || !text_boundary(d->text,d->length,begin) ||
    !text_boundary(d->text,d->length,end))return false;
 ps_a11y_table_inventory *v=d->tables;
 size_t count=v?v->count:0,nrows=v?v->row_count:0,ncells=v?v->cell_count:0,total=0;
 if(count==PS_A11Y_TABLE_LIMIT || rows>PS_A11Y_TABLE_ROWS-nrows ||
    (count && begin<v->tables[count-1].end))return false;
 if(input[0].begin!=begin || input[rows-1].end!=end)return false;
 size_t previous=begin;
 for(size_t i=0;i<rows;i++) {
  const ps_a11y_table_row_input *r=&input[i];
  if(r->begin<previous || r->begin>r->end || r->end>end || r->count>columns ||
     (r->count && !r->cells) || r->count>PS_A11Y_TABLE_CELLS-ncells-total ||
     !text_boundary(d->text,d->length,r->begin) || !text_boundary(d->text,d->length,r->end))return false;
  for(unsigned j=0;j<4;j++)if(!isfinite(r->bounds[j]))return false;
  if(r->bounds[2]<0 || r->bounds[3]<0)return false;
  previous=r->begin;
  for(size_t j=0;j<r->count;j++) {
   const ps_a11y_cell_range *c=&r->cells[j];
   if(c->begin<previous || c->begin>c->end || c->end>r->end ||
      !text_boundary(d->text,d->length,c->begin) || !text_boundary(d->text,d->length,c->end))return false;
   previous=c->end;
  }
  previous=r->end;total+=r->count;
 }
 const ps_a11y_document *old=m->document;
 const ps_a11y_table *prior=old && old->version==d->version?table_index(old,count):NULL;
 if(old && old->active && old->version==d->version) {
  if(!prior || prior->begin!=begin || prior->end!=end || prior->rows!=rows ||
     prior->columns!=columns || prior->column_headers!=headers)return false;
  for(size_t i=0;i<rows;i++) {
   const a11y_table_row *r=&old->tables->rows[prior->first_row+i];
   if(r->begin!=input[i].begin || r->end!=input[i].end || r->count!=input[i].count)return false;
   for(size_t j=0;j<r->count;j++) {
    const ps_a11y_cell_range *c=&old->tables->cells[r->first_cell+j];
    if(c->begin!=input[i].cells[j].begin || c->end!=input[i].cells[j].end)return false;
   }
  }
 }
 uint64_t reserve=1+(uint64_t)rows*(uint64_t)columns;
 if(!prior && (!m->next_id || reserve>UINT64_MAX-m->next_id))return false;
 if(!v){v=calloc(1,sizeof *v);if(!v)return false;d->tables=v;}
 ps_a11y_table table={0};table.id=prior?prior->id:m->next_id;table.begin=begin;table.end=end;
 table.rows=rows;table.columns=columns;table.column_headers=headers;table.first_row=nrows;
 for(size_t i=0;i<rows;i++) {
  a11y_table_row *r=&v->rows[nrows+i];
  r->begin=input[i].begin;r->end=input[i].end;r->first_cell=ncells;r->count=input[i].count;
  memcpy(r->bounds,input[i].bounds,sizeof r->bounds);
  if(r->count)memcpy(v->cells+ncells,input[i].cells,r->count*sizeof *v->cells);
  ncells+=r->count;
 }
 v->tables[count]=table;v->count=count+1;v->row_count=nrows+rows;v->cell_count=ncells;
 if(!prior)m->next_id+=reserve;
 return true;
}
size_t ps_a11y_table_count(const ps_a11y_model *m) {
 const ps_a11y_document *d=m?m->document:NULL;
 return d && d->active && d->tables?d->tables->count:0;
}
bool ps_a11y_table_at(const ps_a11y_model *m,size_t index,ps_a11y_table *out) {
 const ps_a11y_table *t=table_index(m?m->document:NULL,index);
 if(!t || !out)return false;*out=*t;return true;
}
bool ps_a11y_table_get(const ps_a11y_model *m,uint64_t id,ps_a11y_table *out) {
 const ps_a11y_table *t=table_id(m?m->document:NULL,id);
 if(!t || !out)return false;*out=*t;return true;
}
static void table_clip(const ps_a11y_model *m,const float bounds[4],float out[4]) {
 memset(out,0,4*sizeof *out);
 if(m->suspended)return;
 const float *view=m->document->viewport;
 float x=fmaxf(bounds[0],view[0]),y=fmaxf(bounds[1],view[1]);
 float right=fminf(bounds[0]+bounds[2],view[0]+view[2]);
 float bottom=fminf(bounds[1]+bounds[3],view[1]+view[3]);
 if(right>x && bottom>y){out[0]=x;out[1]=y;out[2]=right-x;out[3]=bottom-y;}
}
bool ps_a11y_table_cell_get(const ps_a11y_model *m,uint64_t id,size_t row,size_t column,ps_a11y_table_cell *out) {
 const ps_a11y_document *d=m?m->document:NULL;const ps_a11y_table *t=table_id(d,id);
 if(!out || !t || row>=t->rows || column>=t->columns)return false;
 const a11y_table_row *r=&d->tables->rows[t->first_row+row];
 ps_a11y_table_cell cell={0};cell.id=id+1+(uint64_t)row*t->columns+column;
 cell.table=id;cell.version=d->version;cell.row=row;cell.column=column;
 cell.column_header=t->column_headers && row==0;
 cell.begin=cell.end=r->end;
 if(column<r->count){cell.begin=d->tables->cells[r->first_cell+column].begin;cell.end=d->tables->cells[r->first_cell+column].end;}
 float width=r->bounds[2]/(float)t->columns;
 float bounds[]={r->bounds[0]+(float)column*width,r->bounds[1],width,r->bounds[3]};
 table_clip(m,bounds,cell.bounds);*out=cell;return true;
}
bool ps_a11y_table_cell_find(const ps_a11y_model *m,uint64_t id,ps_a11y_table_cell *out) {
 const ps_a11y_document *d=m?m->document:NULL;
 if(!id || !out || !d || !d->active || !d->tables)return false;
 for(size_t i=0;i<d->tables->count;i++) {
  const ps_a11y_table *t=&d->tables->tables[i];uint64_t offset=id-t->id-1;
  if(id>t->id && offset<(uint64_t)t->rows*t->columns)
   return ps_a11y_table_cell_get(m,t->id,(size_t)(offset/t->columns),(size_t)(offset%t->columns),out);
 }
 return false;
}
bool ps_a11y_table_column_header(const ps_a11y_model *m,uint64_t id,size_t column,ps_a11y_table_cell *out) {
 const ps_a11y_table *t=table_id(m?m->document:NULL,id);
 return t && t->column_headers && ps_a11y_table_cell_get(m,id,0,column,out);
}
bool ps_a11y_table_cell_for_range(const ps_a11y_model *m,size_t begin,size_t end,ps_a11y_table_cell *out) {
 const ps_a11y_document *d=m?m->document:NULL;
 if(!out || !d || !d->active || !d->tables || begin>end ||
    !text_boundary(d->text,d->length,begin) || !text_boundary(d->text,d->length,end))return false;
 for(size_t i=0;i<d->tables->count;i++) {
  const ps_a11y_table *t=&d->tables->tables[i];if(begin<t->begin || end>t->end)continue;
  for(size_t row=0;row<t->rows;row++) {
   const a11y_table_row *r=&d->tables->rows[t->first_row+row];if(begin<r->begin || end>r->end)continue;
   for(size_t column=0;column<r->count;column++) {
    const ps_a11y_cell_range *c=&d->tables->cells[r->first_cell+column];
    if(begin==end && begin==c->end && begin!=c->begin)continue;
    if(begin>=c->begin && end<=c->end)return ps_a11y_table_cell_get(m,t->id,row,column,out);
   }
   if(begin==r->end && end==begin && r->count<t->columns)return ps_a11y_table_cell_get(m,t->id,row,r->count,out);
  }
 }
 return false;
}
bool ps_a11y_table_cell_at_point(const ps_a11y_model *m,float x,float y,ps_a11y_table_cell *out) {
 const ps_a11y_document *d=m?m->document:NULL;
 if(!out || !d || !d->active || !d->tables || m->suspended || !isfinite(x) || !isfinite(y))return false;
 if(x<d->viewport[0] || y<d->viewport[1] || x>=d->viewport[0]+d->viewport[2] || y>=d->viewport[1]+d->viewport[3])return false;
 for(size_t i=0;i<d->tables->count;i++) {
  const ps_a11y_table *t=&d->tables->tables[i];
  for(size_t row=0;row<t->rows;row++) {
   const a11y_table_row *r=&d->tables->rows[t->first_row+row];
   if(r->bounds[2]<=0 || r->bounds[3]<=0 || x<r->bounds[0] || y<r->bounds[1] ||
      x>=r->bounds[0]+r->bounds[2] || y>=r->bounds[1]+r->bounds[3])continue;
   size_t column=(size_t)((x-r->bounds[0])/(r->bounds[2]/(float)t->columns));
   if(column>=t->columns)column=t->columns-1;
   return ps_a11y_table_cell_get(m,t->id,row,column,out);
  }
 }
 return false;
}
bool ps_a11y_table_bounds(const ps_a11y_model *m,uint64_t id,float out[4]) {
 const ps_a11y_table *t=table_id(m?m->document:NULL,id);if(!t || !out)return false;
 float result[4]={0};bool seen=false;
 for(size_t i=0;i<t->rows;i++) {
  float b[4];table_clip(m,m->document->tables->rows[t->first_row+i].bounds,b);
  if(b[2]<=0 || b[3]<=0)continue;
  if(!seen){memcpy(result,b,sizeof result);seen=true;continue;}
  float right=fmaxf(result[0]+result[2],b[0]+b[2]),bottom=fmaxf(result[1]+result[3],b[1]+b[3]);
  result[0]=fminf(result[0],b[0]);result[1]=fminf(result[1],b[1]);result[2]=right-result[0];result[3]=bottom-result[1];
 }
 memcpy(out,result,sizeof result);return true;
}
bool ps_a11y_table_cell_scroll(ps_a11y_model *m,uint64_t id) {
 ps_a11y_table_cell cell;
 return ps_a11y_table_cell_find(m,id,&cell) && ps_a11y_document_scroll(m,cell.version,cell.begin);
}
