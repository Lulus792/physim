#include "accessibility.h"
/* Copy complete tables, including offscreen rows, after their actual layout.
 * Small tables use stack scratch; large tables allocate only their used rows
 * and cells. Publication copies all data before this scratch is released. */
static bool documentation_publish_tables(app *a,const ps_document *d) {
 for(size_t start=0;start<d->count;start++) {
  const ps_doc_block *first=&d->blocks[start];
  if(first->kind!=PS_DOC_TABLE_ROW || first->table_first!=start || !first->table_columns)continue;
  size_t count=first->table_rows,cells=0;
  for(size_t j=start,found=0;j<d->count && found<count;j++)
   if(d->blocks[j].kind==PS_DOC_TABLE_ROW && d->blocks[j].table_first==start){cells+=d->blocks[j].cell_count;++found;}
  ps_a11y_table_row_input row_stack[64],*rows=count<=64?row_stack:malloc(count*sizeof *rows);
  ps_a11y_cell_range cell_stack[256],*ranges=cells<=256?cell_stack:malloc(cells*sizeof *ranges);
  if(!rows || !ranges){if(rows!=row_stack)free(rows);if(ranges!=cell_stack)free(ranges);return false;}
  size_t index=0,used=0,end=first->reading_begin;
  for(size_t j=start;j<d->count && index<count;j++) {
   const ps_doc_block *b=&d->blocks[j];
   if(b->kind!=PS_DOC_TABLE_ROW || b->table_first!=start)continue;
   ps_a11y_table_row_input *r=&rows[index++];
   r->begin=b->reading_begin;r->end=b->reading_begin+b->reading_length;r->count=b->cell_count;
   r->cells=ranges+used;memcpy(r->bounds,b->hit,sizeof r->bounds);end=r->end;
   for(size_t column=0;column<b->cell_count;column++) {
    const ps_doc_cell *cell=&d->cells[b->first_cell+column];
    size_t begin=b->reading_begin+cell->text-b->text;
    ranges[used++]=(ps_a11y_cell_range){begin,begin+cell->length};
   }
  }
  bool ok=index==count && nk_sdl_accessibility_document_table(a->doc_ui,first->reading_begin,end,
      count,first->table_columns,first->level!=0,rows);
  if(rows!=row_stack)free(rows);if(ranges!=cell_stack)free(ranges);
  if(!ok)return false;
 }
 return true;
}
