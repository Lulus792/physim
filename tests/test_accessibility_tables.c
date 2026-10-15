#include "accessibility.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Table accessibility %d: %s\n",__LINE__,#x);return 1;}}while(0)
static const char text[]="Name | θ🌍\n\nx | value\n\n";
static bool document(ps_a11y_model *m,uint64_t version) {
 const float view[]={10,20,200,100};
 ps_a11y_begin(m);return ps_a11y_document_begin(m,version,"Tables",text,view);
}
int main(void) {
 ps_a11y_model *m=malloc(sizeof *m);CHECK(m);ps_a11y_init(m);
 ps_a11y_cell_range head[]={{0,4},{7,13}},body[]={{15,16}};
 ps_a11y_table_row_input rows[]={
  {.begin=0,.end=13,.count=2,.cells=head,.bounds={5,15,220,30}},
  {.begin=15,.end=24,.count=1,.cells=body,.bounds={5,150,220,30}}
 };
 const size_t end=24;
 CHECK(strlen(text)==26 && !ps_a11y_document_table(m,0,end,2,2,true,rows));
 CHECK(document(m,1));CHECK(ps_a11y_document_table(m,0,end,2,2,true,rows));
 CHECK(ps_a11y_table_count(m)==0 && ps_a11y_publish(m) && ps_a11y_table_count(m)==1);
 ps_a11y_table table;CHECK(ps_a11y_table_at(m,0,&table) && table.rows==2 && table.columns==2 && table.column_headers);
 uint64_t id=table.id;ps_a11y_table_cell cell,header;
 CHECK(ps_a11y_table_cell_get(m,id,0,1,&header) && header.begin==7 && header.end==13 && header.column_header);
 CHECK(header.bounds[0]==115 && header.bounds[1]==20 && header.bounds[2]==95 && header.bounds[3]==25);
 CHECK(ps_a11y_table_column_header(m,id,1,&cell) && cell.id==header.id);
 CHECK(ps_a11y_table_cell_get(m,id,1,1,&cell) && cell.begin==24 && cell.end==24 && !cell.column_header);
 uint64_t empty=cell.id;CHECK(cell.bounds[2]==0 && ps_a11y_table_cell_find(m,empty,&cell));
 CHECK(!ps_a11y_find(m,empty) && !ps_a11y_press(m,empty));
 CHECK(ps_a11y_table_cell_scroll(m,empty));size_t at=0;
 CHECK(ps_a11y_document_take_scroll(m,1,&at) && at==24);
 float bounds[4];CHECK(ps_a11y_table_bounds(m,id,bounds) && bounds[0]==10 && bounds[1]==20 && bounds[2]==200 && bounds[3]==25);
 cell.begin=999;CHECK(!ps_a11y_table_cell_get(m,id,2,0,&cell) && cell.begin==999);
 CHECK(!ps_a11y_table_cell_get(m,id,0,2,&cell) && cell.begin==999);
 CHECK(!ps_a11y_table_cell_find(m,id,&cell) && !ps_a11y_table_at(m,1,&table));
 /* Identical publication is quiet; geometry changes retain all cell IDs. */
 CHECK(document(m,1) && ps_a11y_document_table(m,0,end,2,2,true,rows) && !ps_a11y_publish(m));
 rows[1].bounds[1]=60;
 CHECK(document(m,1) && ps_a11y_document_table(m,0,end,2,2,true,rows) && ps_a11y_publish(m));
 CHECK(ps_a11y_table_cell_find(m,empty,&cell) && cell.bounds[0]==115 && cell.bounds[1]==60 && cell.bounds[2]==95);
 ps_a11y_suspend(m);CHECK(ps_a11y_table_cell_find(m,empty,&cell) && cell.bounds[2]==0);
 CHECK(ps_a11y_table_count(m)==1 && !ps_a11y_table_cell_scroll(m,empty));
 CHECK(document(m,1) && ps_a11y_document_table(m,0,end,2,2,true,rows) && ps_a11y_publish(m));
 CHECK(ps_a11y_table_cell_find(m,empty,&cell) && cell.bounds[2]>0);
 /* A missing or changed same-generation inventory cannot replace publication. */
 CHECK(document(m,1));CHECK(!ps_a11y_publish(m) && ps_a11y_table_cell_find(m,empty,&cell));
 CHECK(!ps_a11y_document_table(m,0,end,2,3,true,rows));
 CHECK(!ps_a11y_document_table(m,0,end,2,2,false,rows));
 CHECK(ps_a11y_document_table(m,0,end,2,2,true,rows) && !ps_a11y_publish(m));
 /* Every rejected declaration leaves the draft and ID counter available. */
 CHECK(document(m,2));uint64_t next=m->next_id;
 head[1].begin=8;CHECK(!ps_a11y_document_table(m,0,end,2,2,true,rows));head[1].begin=7;
 head[1].end=12;CHECK(!ps_a11y_document_table(m,0,end,2,2,true,rows));head[1].end=13;
 rows[0].bounds[0]=NAN;CHECK(!ps_a11y_document_table(m,0,end,2,2,true,rows));rows[0].bounds[0]=5;
 rows[0].bounds[2]=-1;CHECK(!ps_a11y_document_table(m,0,end,2,2,true,rows));rows[0].bounds[2]=220;
 rows[1].begin=12;CHECK(!ps_a11y_document_table(m,0,end,2,2,true,rows));rows[1].begin=15;
 rows[0].count=3;CHECK(!ps_a11y_document_table(m,0,end,2,2,true,rows));rows[0].count=2;
 CHECK(!ps_a11y_document_table(m,0,end,0,2,true,rows));
 CHECK(!ps_a11y_document_table(m,0,end,PS_A11Y_TABLE_ROWS+1,2,true,rows));
 CHECK(!ps_a11y_document_table(m,0,end,2,PS_A11Y_TABLE_CELLS+1,true,rows));
 CHECK(m->next_id==next && ps_a11y_table_cell_find(m,empty,&cell));
 CHECK(ps_a11y_document_table(m,0,end,2,2,true,rows));
 CHECK(!ps_a11y_document_table(m,0,end,2,2,true,rows)); /* duplicate/overlap */
 CHECK(ps_a11y_publish(m) && !ps_a11y_table_cell_find(m,empty,&cell));
 CHECK(ps_a11y_table_at(m,0,&table) && table.id!=id);
 uint64_t replacement=table.id;
 CHECK(document(m,3) && ps_a11y_document_table(m,0,end,2,2,false,rows));
 ps_a11y_document_abort(m);
 CHECK(!ps_a11y_publish(m) && m->document->version==2 && ps_a11y_table_get(m,replacement,&table));
 /* Tables never consume the bounded actionable-control inventory. */
 CHECK(document(m,3) && ps_a11y_document_table(m,0,end,2,2,false,rows));
 for(unsigned i=0;i<PS_A11Y_MAX_NODES;i++) {
  char name[32];snprintf(name,sizeof name,"Button %u",i);float box[]={10,20,30,20};
  ps_a11y_record(m,"UI",name,PS_A11Y_BUTTON,box,true);
 }
 CHECK(ps_a11y_publish(m) && m->count==PS_A11Y_MAX_NODES && !m->dropped && ps_a11y_table_count(m)==1);
 CHECK(ps_a11y_table_at(m,0,&table) && !ps_a11y_table_column_header(m,table.id,0,&cell));
 /* The full sparse-cell capacity is usable, then rejects an extra cell. */
 CHECK(document(m,4));
 ps_a11y_cell_range *many=calloc(PS_A11Y_TABLE_CELLS,sizeof *many);CHECK(many);
 ps_a11y_table_row_input large={.begin=0,.end=24,.count=PS_A11Y_TABLE_CELLS,.cells=many,.bounds={0,0,200,20}};
 CHECK(ps_a11y_document_table(m,0,24,1,PS_A11Y_TABLE_CELLS,false,&large));
 ps_a11y_cell_range extra={24,24};
 ps_a11y_table_row_input tail={.begin=24,.end=26,.count=1,.cells=&extra};
 CHECK(!ps_a11y_document_table(m,24,26,1,1,false,&tail));
 CHECK(ps_a11y_publish(m) && ps_a11y_table_at(m,0,&table));
 CHECK(ps_a11y_table_cell_get(m,table.id,0,PS_A11Y_TABLE_CELLS-1,&cell) && cell.begin==0 && cell.end==0);
 free(many);
 CHECK(document(m,5));m->next_id=UINT64_MAX-3;
 CHECK(!ps_a11y_document_table(m,0,end,2,2,true,rows));
 ps_a11y_destroy(m);free(m);
 puts("Table accessibility: Unicode ranges, sparse cells, headers, geometry, identity, publication and control isolation passed");
 return 0;
}
