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
    CHECK(ps_document_find(d->text + d->blocks[1].text, "CLEAR") != NULL);
    CHECK(ps_document_find("Überblick für Größen", "überblick") != NULL);
    CHECK(ps_document_find("Überblick für Größen", "GRÖẞEN") != NULL);
    CHECK(ps_document_find("abcdef", "x") == NULL);
    ps_document *unchanged = d;
    CHECK(ps_document_parse("x\0y", 3, false, &unchanged) == PS_INVALID && unchanged == d);
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
    free(d);
    puts("Documentation parser: prose, code, tables, links, search and bounds passed");
    return 0;
}
