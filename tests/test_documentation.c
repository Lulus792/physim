#include "documentation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Documentation line %d: %s\n", __LINE__, #x);                          \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv) {
    ps_doc_history history={0};
    CHECK(!ps_doc_history_peek(&history,-1) && !ps_doc_history_move(&history,1));
    ps_doc_location location={.topic=1,.track=1,.scroll_y=123};
    location.match=3;
    snprintf(location.query,sizeof location.query,"θ");
    CHECK(ps_doc_history_visit(&history,&location));
    location.topic=2;location.scroll_y=456;CHECK(ps_doc_history_visit(&history,&location));
    location.topic=3;location.scroll_y=789;CHECK(ps_doc_history_visit(&history,&location));
    CHECK(ps_doc_history_peek(&history,-1)->topic==2 && ps_doc_history_move(&history,-1));
    CHECK(ps_doc_history_peek(&history,-1)->scroll_y==123 && ps_doc_history_move(&history,-1));
    CHECK(!ps_doc_history_move(&history,-1) && history.entries[history.index].track==1);
    CHECK(history.entries[history.index].match==3 && !strcmp(history.entries[history.index].query,"θ"));
    CHECK(ps_doc_history_move(&history,1) && ps_doc_history_peek(&history,1)->scroll_y==789);
    location.topic=4;CHECK(ps_doc_history_visit(&history,&location));
    CHECK(!ps_doc_history_peek(&history,1) && history.count==3);
    for(unsigned i=0;i<PS_DOC_HISTORY_LIMIT+10;i++){location.topic=(int)i+10;CHECK(ps_doc_history_visit(&history,&location));}
    CHECK(history.count==PS_DOC_HISTORY_LIMIT && history.index==PS_DOC_HISTORY_LIMIT-1);
    CHECK(history.entries[0].topic==20 && history.entries[history.index].topic==83);
    ps_doc_history saved=history;location.topic=-1;
    CHECK(!ps_doc_history_visit(&history,&location) && !memcmp(&history,&saved,sizeof history));
    CHECK(!ps_doc_history_move(&history,0) && !memcmp(&history,&saved,sizeof history));
    for (int i = 1; i < argc; i++) {
        FILE *file = fopen(argv[i], "rb");
        CHECK(file && fseek(file, 0, SEEK_END) == 0);
        long length = ftell(file);
        CHECK(length >= 0 && length <= PS_DOC_MAX_BYTES && fseek(file, 0, SEEK_SET) == 0);
        char *text = malloc((size_t)length + 1);
        CHECK(text && fread(text, 1, (size_t)length, file) == (size_t)length && fclose(file) == 0);
        text[length] = 0;
        ps_document *page = NULL;
        CHECK(ps_document_parse(text, (size_t)length, false, &page) == PS_OK && page->count > 0);
        printf("Parsed bundled page: %s (%ld bytes, %zu blocks)\n", argv[i], length, page->count);
        free(page); free(text);
    }
    const char *input =
        "# API\r\n\r\nA **clear** paragraph\ncontinued with `code`.\n\n```c\nif (x < 4) {\n    "
        "x++;\n}\n```\n\n[Units](units.md) and [Source](https://example.org).\n";
    ps_document *d = NULL;
    CHECK(ps_document_parse(input, strlen(input), false, &d) == PS_OK);
    CHECK(d->count == 6 && d->blocks[0].kind == PS_DOC_HEADING);
    CHECK(!strcmp(d->text + d->blocks[1].text, "A clear paragraph continued with code."));
    CHECK(d->blocks[2].kind == PS_DOC_CODE &&
          strstr(d->text + d->blocks[2].text, "    x++;") != NULL);
    CHECK(d->blocks[4].kind == PS_DOC_LINK && !strcmp(d->text + d->blocks[4].target, "units.md"));
    CHECK(d->blocks[1].span_count==5);
    const ps_doc_span *bold=&d->spans[d->blocks[1].first_span+1];
    CHECK(bold->style==PS_DOC_BOLD && bold->length==5 && !memcmp(d->text+bold->text,"clear",5));
    const ps_doc_span *inline_code=&d->spans[d->blocks[1].first_span+3];
    CHECK(inline_code->style==PS_DOC_INLINE_CODE && inline_code->length==4);
    CHECK(d->blocks[4].inline_parent==3 && d->blocks[5].inline_parent==3);
    CHECK(d->spans[d->blocks[3].first_span].link==4);
    CHECK(!strcmp(d->reading,"API\n\nA clear paragraph continued with code.\n\nif (x < 4) {\n    x++;\n}\n\n\nUnits and Source.\n\n"));
    CHECK(d->blocks[4].reading_begin==d->blocks[3].reading_begin && d->blocks[4].reading_length==5);
    CHECK(!memcmp(d->reading+d->blocks[4].reading_begin,"Units",5));
    CHECK(d->blocks[5].reading_begin==d->blocks[3].reading_begin+10 && d->blocks[5].reading_length==6);
    CHECK(d->blocks[2].reading_length==d->blocks[2].length);
    CHECK(!memcmp(d->reading+d->blocks[2].reading_begin,d->text+d->blocks[2].text,d->blocks[2].length));
    CHECK(ps_document_find(d->text + d->blocks[1].text, "CLEAR") != NULL);
    CHECK(ps_document_find("Überblick für Größen", "überblick") != NULL);
    CHECK(ps_document_find("Überblick für Größen", "GRÖẞEN") != NULL);
    CHECK(ps_document_find("abcdef", "x") == NULL);
    ps_document *unchanged = d;
    CHECK(ps_document_parse("x\0y", 3, false, &unchanged) == PS_INVALID && unchanged == d);
    free(d);
    const char *wrapped_list="- First **bold** [guide](guide.md)\r\n"
        "  continued with `code`.\r\n- Second item\n  same item\n\nAfter.\n\n"
        "10. Third item\n    continuation\n";
    CHECK(ps_document_parse(wrapped_list,strlen(wrapped_list),false,&d)==PS_OK);
    CHECK(d->count==5 && !strcmp(d->text+d->blocks[0].text,"- First bold guide continued with code."));
    CHECK(d->blocks[1].kind==PS_DOC_LINK && d->blocks[1].inline_parent==0);
    CHECK(!strcmp(d->text+d->blocks[2].text,"- Second item same item"));
    CHECK(!strcmp(d->text+d->blocks[3].text,"After."));
    CHECK(!strcmp(d->text+d->blocks[4].text,"10. Third item continuation"));
    CHECK(strstr(d->reading,"- First bold guide continued with code.\n\n- Second item same item\n\nAfter."));
    CHECK(d->blocks[1].reading_begin==d->blocks[0].reading_begin+13);
    free(d);
    const char *list_boundaries="- Parent\n  - Child\n    child continuation\n+ Plus\n* Star\n12) Ordered\n"
        " \t \n1.5 remains prose\n2.5 continues prose\n\n- Before code\n```c\n  x++;\n```\n"
        "- Before formula\n$$\nz=0\n$$\n- Before table\n| A | B |\n| --- | --- |\n| C | D |\n# End\n";
    CHECK(ps_document_parse(list_boundaries,strlen(list_boundaries),false,&d)==PS_OK && d->count==14);
    CHECK(!strcmp(d->text+d->blocks[1].text,"  - Child child continuation"));
    CHECK(!strcmp(d->text+d->blocks[4].text,"12) Ordered"));
    CHECK(!strcmp(d->text+d->blocks[5].text,"1.5 remains prose 2.5 continues prose"));
    CHECK(d->blocks[7].kind==PS_DOC_CODE && !strcmp(d->text+d->blocks[7].text,"  x++;\n"));
    CHECK(d->blocks[9].kind==PS_DOC_MATH && d->blocks[11].kind==PS_DOC_TABLE_ROW);
    CHECK(d->blocks[11].table_rows==2 && d->blocks[13].kind==PS_DOC_HEADING);
    free(d);
    const char *rich="Escaped \\* and \\` remain; **bold `x[0]`** [**go**](guide.md#part) and `a **literal**`.\n";
    CHECK(ps_document_parse(rich,strlen(rich),false,&d)==PS_OK);
    CHECK(d->count==2 && !strcmp(d->text+d->blocks[0].text,"Escaped * and ` remain; bold x[0] go and a **literal**."));
    bool saw_nested=false,saw_link=false;
    for(size_t i=0;i<d->span_count;i++) {
        const ps_doc_span *span=&d->spans[i];
        if(span->style==(PS_DOC_BOLD|PS_DOC_INLINE_CODE))saw_nested=true;
        if(span->style==(PS_DOC_BOLD|PS_DOC_INLINE_LINK) && span->link==1)saw_link=true;
        CHECK(span->text+span->length<=d->used);
    }
    CHECK(saw_nested && saw_link && !strcmp(d->text+d->blocks[1].text,"go"));free(d);
    const char *unmatched="Literal **half and `open [no target]";
    CHECK(ps_document_parse(unmatched,strlen(unmatched),false,&d)==PS_OK);
    CHECK(d->count==1 && !strcmp(d->text+d->blocks[0].text,unmatched));free(d);
    const char *nested_url="[A](https://example.org/a(b)) and `[B](fake.md)`";
    CHECK(ps_document_parse(nested_url,strlen(nested_url),false,&d)==PS_OK);
    CHECK(d->count==2 && !strcmp(d->text+d->blocks[1].target,"https://example.org/a(b)"));free(d);
    CHECK(ps_document_parse("preserved",9,false,&d)==PS_OK);
    ps_document *before_limit=d;
    size_t repetitions=PS_DOC_MAX_SPANS/2+1;
    char *too_many_spans=malloc(repetitions*8);CHECK(too_many_spans);
    for(size_t i=0;i<repetitions;i++)memcpy(too_many_spans+i*8,"x **b** ",8);
    CHECK(ps_document_parse(too_many_spans,repetitions*8,false,&d)==PS_LIMIT && d==before_limit);
    free(too_many_spans);free(d);
    const char *headings="# Hello, `World`!\n# Über Größen & Kräfte\n# Repeat\n# Repeat-1\n# Repeat\n# Repeat\n";
    CHECK(ps_document_parse(headings,strlen(headings),false,&d)==PS_OK);
    size_t block=SIZE_MAX;
    CHECK(ps_document_anchor(d,"#hello-world",&block)==PS_OK && block==0);
    CHECK(ps_document_anchor(d,"%C3%BCber-gr%C3%B6%C3%9Fen--kr%C3%A4fte",&block)==PS_OK && block==1);
    CHECK(ps_document_anchor(d,"repeat-1",&block)==PS_OK && block==3);
    CHECK(ps_document_anchor(d,"repeat-2",&block)==PS_OK && block==4);
    CHECK(ps_document_anchor(d,"repeat-3",&block)==PS_OK && block==5);
    const char *bad[]={"missing","","#","%","%xz","%00","%ED%A0%80","%FF"};
    for(size_t i=0;i<sizeof bad/sizeof bad[0];i++)CHECK(ps_document_anchor(d,bad[i],&block)==PS_INVALID && block==5);
    free(d);
    char *duplicates=malloc(PS_DOC_MAX_BLOCKS*4);CHECK(duplicates);
    for(size_t i=0;i<PS_DOC_MAX_BLOCKS;i++)memcpy(duplicates+4*i,"# X\n",4);
    CHECK(ps_document_parse(duplicates,PS_DOC_MAX_BLOCKS*4,false,&d)==PS_OK);
    CHECK(ps_document_anchor(d,"x-4095",&block)==PS_OK && block==4095);
    CHECK(ps_document_anchor(d,"x-4096",&block)==PS_INVALID && block==4095);
    free(d);free(duplicates);
    char *large_code=malloc(PS_DOC_MAX_CODE_BYTES+1);CHECK(large_code);
    memset(large_code,'x',PS_DOC_MAX_CODE_BYTES+1);
    CHECK(ps_document_parse(large_code,PS_DOC_MAX_CODE_BYTES,true,&d)==PS_OK && d->count==1 && d->blocks[0].length==PS_DOC_MAX_CODE_BYTES);
    ps_document *preserved=d;
    CHECK(ps_document_parse(large_code,PS_DOC_MAX_CODE_BYTES+1,true,&preserved)==PS_INVALID && preserved==d);
    CHECK(ps_document_parse(large_code,PS_DOC_MAX_BYTES+1,false,&preserved)==PS_INVALID && preserved==d);
    free(d);free(large_code);
    const char *image="![Pendel: x nach rechts, y nach oben](visuals/pendulum.svg)\n\n[Modell](pendulum-tutorial.md)\n";
    CHECK(ps_document_parse(image,strlen(image),false,&d)==PS_OK);
    CHECK(d->count==4 && d->blocks[0].kind==PS_DOC_TEXT &&
        !strcmp(d->text+d->blocks[0].text,"Pendel: x nach rechts, y nach oben") &&
        d->blocks[1].kind==PS_DOC_IMAGE && !strcmp(d->text+d->blocks[1].target,"visuals/pendulum.svg") &&
        d->blocks[3].kind==PS_DOC_LINK && !strcmp(d->text+d->blocks[3].target,"pendulum-tutorial.md"));
    free(d);
    CHECK(ps_document_parse("before\n---\nafter", 16, false, &d) == PS_OK);
    CHECK(d->count == 3 && !strcmp(d->text + d->blocks[0].text, "before") &&
          d->blocks[1].kind == PS_DOC_RULE);
    free(d);
    CHECK(ps_document_parse("# not heading\n/* code */", 24, true, &d) == PS_OK && d->count == 1 &&
          d->blocks[0].kind == PS_DOC_CODE);
    free(d);
    char *many = malloc(5000 * 3);
    CHECK(many != NULL);
    for (int i = 0; i < 5000; i++)
        memcpy(many + i * 3, "x\n\n", 3);
    CHECK(ps_document_parse(many, 15000, false, &d) == PS_LIMIT);
    free(many);
    const char *table = "| Form | Meaning |\n| --- | :---: |\n| Sphere | **Radius** |\n";
    CHECK(ps_document_parse(table, strlen(table), false, &d) == PS_OK);
    CHECK(d->count == 2 && d->blocks[0].kind == PS_DOC_TABLE_ROW && d->blocks[0].level == 1);
    CHECK(d->blocks[1].kind == PS_DOC_TABLE_ROW && d->blocks[1].level == 0);
    CHECK(strstr(d->text + d->blocks[1].text, "Radius") != NULL);
    CHECK(d->cell_count==4 && d->blocks[0].cell_count==2 && d->blocks[1].first_cell==2);
    CHECK(d->blocks[0].table_first==0 && d->blocks[1].table_first==0 && d->blocks[1].table_row==1);
    CHECK(d->blocks[0].table_rows==2 && d->blocks[0].table_columns==2);
    free(d);
    const char *separate_tables="| first |\n\nordinary text\n\n| --- |\n| second |\n| --- |\n";
    CHECK(ps_document_parse(separate_tables,strlen(separate_tables),false,&d)==PS_OK);
    CHECK(d->count==3 && d->blocks[0].level==0 && d->blocks[0].table_first==0 &&
        d->blocks[1].table_first==SIZE_MAX && d->blocks[2].table_first==2 &&
        d->blocks[2].table_row==0 && d->blocks[2].level==1);
    free(d);
    const char *rich_table="| Form | Meaning | Empty |\n| --- | --- | --- |\n"
        "| `x|y` | [θ|🌍](units.md) | |\n"
        "| a\\|b | **bold|text** | tail\n"
        "| ||\n";
    CHECK(ps_document_parse(rich_table,strlen(rich_table),false,&d)==PS_OK);
    const char *expected_cells[]={"Form","Meaning","Empty","x|y","θ|🌍","",
        "a|b","bold|text","tail","",""};
    CHECK(d->cell_count==sizeof expected_cells/sizeof expected_cells[0]);
    CHECK(d->blocks[0].table_rows==4 && d->blocks[0].table_columns==3);
    for(size_t i=0;i<d->cell_count;i++) {
        const ps_doc_cell *cell=&d->cells[i];size_t length=strlen(expected_cells[i]);
        CHECK(cell->length==length && !memcmp(d->text+cell->text,expected_cells[i],length));
    }
    CHECK(d->blocks[1].cell_count==3 && d->blocks[1].reading_begin!=SIZE_MAX);
    const ps_doc_block *link=&d->blocks[2],*row=&d->blocks[1];
    const ps_doc_cell *linked_cell=&d->cells[row->first_cell+1];
    CHECK(link->kind==PS_DOC_LINK && link->reading_length==linked_cell->length &&
        link->reading_begin==row->reading_begin+linked_cell->text-row->text);
    free(d);
    /* No silent renderer truncation after column 16. */
    char wide[256]="|";
    for(unsigned i=0;i<20;i++)strcat(wide,"x|");
    CHECK(ps_document_parse(wide,strlen(wide),false,&d)==PS_OK && d->blocks[0].cell_count==20);
    free(d);
    /* Capacity failure preserves the caller's existing document. */
    char *many_cells=malloc(PS_DOC_MAX_CELLS+3);CHECK(many_cells);
    memset(many_cells,'|',PS_DOC_MAX_CELLS+2);many_cells[PS_DOC_MAX_CELLS+2]=0;
    CHECK(ps_document_parse(many_cells,PS_DOC_MAX_CELLS+1,false,&d)==PS_OK &&
        d->cell_count==PS_DOC_MAX_CELLS);
    free(d);
    CHECK(ps_document_parse("preserved",9,false,&d)==PS_OK);
    ps_document *before_cells=d;
    CHECK(ps_document_parse(many_cells,PS_DOC_MAX_CELLS+2,false,&d)==PS_LIMIT && d==before_cells);
    free(many_cells);free(d);
    const char *formula_source="before\n\n$$\r\nT_0=2\\pi\\sqrt{\\frac{L}{g}}\r\n$$\r\n\nafter\n";
    CHECK(ps_document_parse(formula_source,strlen(formula_source),false,&d)==PS_OK && d->count==3);
    CHECK(d->blocks[1].kind==PS_DOC_MATH && !strcmp(d->text+d->blocks[1].text,"T₀ = 2π√(L/g)"));
    CHECK(strstr(d->reading,"T₀ = 2π√(L/g)") && !strstr(d->reading,"\\frac"));
    const ps_doc_formula *formula=ps_document_formula(d->text+d->blocks[1].target,strlen(d->text+d->blocks[1].target));
    CHECK(formula && formula->count==7 && formula->variants[0].raster_width && formula->variants[0].packed_size);
    CHECK(formula->inline_variant && formula->inline_variant->width>50); /* Entire equation, not just T_0. */
    CHECK(d->blocks[1].reading_begin!=SIZE_MAX && d->blocks[1].reading_length==strlen(formula->reading));
    free(d);
    const char *unknown_formula="$$\nx+unknown\n$$\n";
    CHECK(ps_document_parse(unknown_formula,strlen(unknown_formula),false,&d)==PS_OK && d->count==1 && d->blocks[0].kind==PS_DOC_CODE);
    CHECK(strstr(d->reading,"x+unknown"));ps_document *before_formula=d;
    CHECK(ps_document_parse("$$\nx+1",6,false,&d)==PS_INVALID && d==before_formula);
    free(d);
    const char *fenced_formula="```c\n$$\nx+1\n$$\n```\n";
    CHECK(ps_document_parse(fenced_formula,strlen(fenced_formula),false,&d)==PS_OK && d->count==1 && d->blocks[0].kind==PS_DOC_CODE);
    CHECK(strstr(d->text+d->blocks[0].text,"$$"));free(d);
    const char *inline_math="Before $\\nu=\\frac\\mu\\rho$ after **$E=\\frac12mv^2$** "
        "[$x=L\\sin\\theta$](units.md). Code `$E=\\frac12mv^2$`, escaped \\$ and unknown $x+unknown$.";
    CHECK(ps_document_parse(inline_math,strlen(inline_math),false,&d)==PS_OK && d->count==2);
    CHECK(strstr(d->reading,"Before ν = μ/ρ after E = ½mv² x = L sin θ."));
    CHECK(strstr(d->reading,"Code $E=\\frac12mv^2$, escaped $ and unknown $x+unknown$."));
    unsigned inline_count=0;
    for(size_t i=0;i<d->span_count;i++)if(d->spans[i].formula) {
        const ps_doc_span *s=&d->spans[i];++inline_count;
        CHECK((s->style&PS_DOC_INLINE_MATH) && s->length==strlen(s->formula->reading));
        CHECK(!memcmp(d->text+s->text,s->formula->reading,s->length));
        CHECK(s->formula->inline_variant->baseline>0);
    }
    CHECK(inline_count==3 && !strcmp(d->text+d->blocks[1].text,"x = L sin θ"));free(d);
    const char *adjacent_math="$z=0$$z=0$";
    CHECK(ps_document_parse(adjacent_math,strlen(adjacent_math),false,&d)==PS_OK && d->span_count==2);
    CHECK(d->spans[0].formula && d->spans[1].formula && !strcmp(d->reading,"z = 0z = 0\n\n"));free(d);
    const char *table_math="| Quantity | Formula |\n| --- | --- |\n| viscosity | $\\nu=\\frac\\mu\\rho$ |\n";
    CHECK(ps_document_parse(table_math,strlen(table_math),false,&d)==PS_OK && d->cell_count==4);
    CHECK(d->cells[3].length==strlen("ν = μ/ρ") && !memcmp(d->text+d->cells[3].text,"ν = μ/ρ",d->cells[3].length));free(d);
    puts("Documentation parser: prose, code, tables, links, TeX formulas, inline math, search and bounds passed");
    return 0;
}
