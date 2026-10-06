#include "report_internal.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
static void csv_text(FILE *f, const char *s) {
    fputc('"', f);
    for (; *s; s++) {
        if (*s == '"')
            fputc('"', f);
        fputc(*s, f);
    }
    fputc('"', f);
}
static void xml(FILE *f, const char *s) {
    for (; *s; s++) {
        switch (*s) {
        case '&':
            fputs("&amp;", f);
            break;
        case '<':
            fputs("&lt;", f);
            break;
        case '>':
            fputs("&gt;", f);
            break;
        case '"':
            fputs("&quot;", f);
            break;
        case '\'':
            fputs("&apos;", f);
            break;
        default:
            fputc(*s, f);
            break;
        }
    }
}
static ps_result finish(FILE *f) {
    bool ok = !ferror(f);
    if (fclose(f))
        ok = false;
    return ok ? PS_OK : PS_IO;
}
ps_result ps_report_export_table_csv(const ps_report *r, uint32_t table, const char *path) {
    ps_table_info info;
    if (!path || ps_report_table_read(r, table, &info) != PS_OK)
        return PS_INVALID;
    FILE *f = fopen(path, "wbx");
    if (!f)
        return PS_IO;
    fputs("row", f);
    for (uint32_t j = 0; j < info.columns; j++) {
        char name[176];
        snprintf(name, sizeof name, "%s [%s]", info.column[j].label, info.column[j].unit.symbol);
        fputc(',', f);
        csv_text(f, name);
    }
    fputc('\n', f);
    for (uint32_t i = 0; i < info.rows; i++) {
        ps_table_row row;
        ps_report_row_read(r, table, i, &row);
        csv_text(f, row.label);
        for (uint32_t j = 0; j < info.columns; j++)
            fprintf(f, ",%.17g", row.values[j]);
        fputc('\n', f);
    }
    return finish(f);
}
ps_result ps_report_export_plot_csv(const ps_report *r, uint32_t plot, const char *path) {
    ps_plot_info info;
    if (!path || ps_report_plot_read(r, plot, &info) != PS_OK)
        return PS_INVALID;
    ps_allocator allocator = ps_report_allocator_internal(r);
    void *storage;
    ps_result allocation = ps_memory_allocate(allocator, sizeof(ps_curve_data), &storage);
    if (allocation != PS_OK)
        return allocation;
    ps_curve_data *c = storage;
    FILE *f = fopen(path, "wbx");
    if (!f) {
        ps_memory_free(allocator, c, sizeof *c);
        return PS_IO;
    }
    bool masked=false;const uint8_t *mask=NULL;
    for(uint32_t i=0;i<info.curves;i++){ps_report_curve_mask(r,plot,i,&mask);masked |= mask!=NULL;}
    fputs(masked?"curve,kind,x,y,bar_width,source_samples,x_unit,y_unit,valid,segment_start\n":"curve,kind,x,y,bar_width,source_samples,x_unit,y_unit\n",f);
    for (uint32_t i = 0; i < info.curves; i++) {
        ps_report_curve_read(r, plot, i, c);ps_report_curve_mask(r,plot,i,&mask);
        for (uint32_t j = 0; j < c->count; j++) {
            csv_text(f, c->label);
            fprintf(f,",%u,",(unsigned)c->kind);
            if(!mask || (mask[j]&1))fprintf(f,"%.17g,%.17g",c->x[j],c->y[j]);else fputc(',',f);
            fprintf(f,",%.17g,%llu,",c->bar_width,(unsigned long long)c->source_count);
            csv_text(f, info.x_unit.symbol);
            fputc(',', f);
            csv_text(f, info.y_unit.symbol);
            if(masked)fprintf(f,",%u,%u",mask?mask[j]&1:1,mask?(mask[j]&2)!=0:0);
            fputc('\n', f);
        }
    }
    ps_memory_free(allocator, c, sizeof *c);
    return finish(f);
}
ps_result ps_report_export_svg(const ps_report *r, uint32_t plot, const char *path) {
    return ps_report_export_svg_region(r, plot, NULL, path);
}
ps_result ps_report_export_svg_region(const ps_report *r, uint32_t plot, const double region[4],
                                      const char *path) {
    ps_plot_info info;
    double b[4];
    if (!path || ps_report_plot_read(r, plot, &info) != PS_OK ||
        ps_report_plot_bounds(r, plot, b) != PS_OK)
        return PS_INVALID;
    if (region) {
        for (unsigned i = 0; i < 4; i++)
            if (!isfinite(region[i]))
                return PS_INVALID;
        if (!(region[0] < region[1] && region[2] < region[3]))
            return PS_INVALID;
        for (unsigned i = 0; i < 4; i++) {
            unsigned axis = i & ~1u;
            double fraction = ps_report_axis_fraction(b[i], region[axis], region[axis + 1]);
            if (!isfinite(fraction) || fabs(fraction) > DBL_MAX / 4096)
                return PS_LIMIT;
        }
        memcpy(b, region, sizeof b);
    }
    ps_allocator allocator = ps_report_allocator_internal(r);
    void *storage;
    ps_result allocation = ps_memory_allocate(allocator, sizeof(ps_curve_data), &storage);
    if (allocation != PS_OK)
        return allocation;
    ps_curve_data *c = storage;
    FILE *f = fopen(path, "wbx");
    if (!f) {
        ps_memory_free(allocator, c, sizeof *c);
        return PS_IO;
    }
    fputs("<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1200\" height=\"740\" viewBox=\"0 0 "
          "1200 740\">\n"
          "<rect width=\"1200\" height=\"740\" fill=\"white\"/>\n"
          "<g font-family=\"sans-serif\" fill=\"#24262b\">\n"
          "<text x=\"80\" y=\"38\" font-size=\"22\">",
          f);
    xml(f, info.title);
    fputs("</text>\n", f);
    double xo = 0, yo = 0;
    if (region) {
        xo = isfinite(b[1] - b[0]) ? b[0] : 0;
        yo = isfinite(b[3] - b[2]) ? b[2] : 0;
        fprintf(f,
                "<text x=\"110\" y=\"55\" font-size=\"12\">Y-Offset: %.17g; Wert = Offset + "
                "Achsenwert</text>\n",
                yo);
        fprintf(f,
                "<text x=\"110\" y=\"637\" font-size=\"12\">X-Offset: %.17g; Wert = Offset + "
                "Achsenwert</text>\n",
                xo);
        fputs("<defs><clipPath id=\"plot-region\"><rect x=\"110\" y=\"90\" width=\"1040\" "
              "height=\"480\"/></clipPath></defs>\n",
              f);
    }
    for (int i = 0; i <= 4; i++) {
        double fraction = i / 4.;
        double x = 110 + fraction * 1040, y = 90 + (1 - fraction) * 480;
        double xv = (1 - fraction) * (b[0] - xo) + fraction * (b[1] - xo),
               yv = (1 - fraction) * (b[2] - yo) + fraction * (b[3] - yo);
        fprintf(f, "<path d=\"M110 %.3f H1150 M%.3f 90 V570\" stroke=\"#dce0e6\"/>\n", y, x);
        fprintf(f, "<text x=\"100\" y=\"%.3f\" text-anchor=\"end\" font-size=\"12\">%.5g</text>\n",
                y + 4, yv);
        fprintf(f,
                "<text x=\"%.3f\" y=\"594\" text-anchor=\"middle\" font-size=\"12\">%.5g</text>\n",
                x, xv);
    }
    fputs("<text x=\"630\" y=\"620\" text-anchor=\"middle\" font-size=\"15\">", f);
    xml(f, info.x_label);
    if(info.x_unit.symbol[0]){fputs(" [",f);xml(f,info.x_unit.symbol);fputs("]",f);}
    fputs("</text>\n", f);
    fputs("<text x=\"110\" y=\"74\" font-size=\"15\">", f);
    xml(f, info.y_label);
    if(info.y_unit.symbol[0]){fputs(" [",f);xml(f,info.y_unit.symbol);fputs("]",f);}
    fputs("</text>\n", f);
    static const char *colors[] = {"#176dd1", "#bc4b12", "#178450", "#8d4ab8",
                                   "#a22556", "#147f8b", "#756313", "#536173"};
    bool reduced = false;
    for (uint32_t i = 0; i < info.curves; i++) {
        ps_report_curve_read(r, plot, i, c);
        const uint8_t *mask=NULL;ps_report_curve_mask(r,plot,i,&mask);
        if (c->kind != PS_PLOT_HISTOGRAM && c->count < c->source_count)
            reduced = true;
        if (region)
            fputs("<g clip-path=\"url(#plot-region)\">\n", f);
        if (c->kind == PS_PLOT_LINE && c->count > 1 && !mask)
            fprintf(f, "<polyline fill=\"none\" stroke=\"%s\" stroke-width=\"2\" points=\"",
                    colors[i]);
        if(c->kind==PS_PLOT_LINE && c->count>1 && mask) {
            fprintf(f,"<path fill=\"none\" stroke=\"%s\" stroke-width=\"2\" d=\"",colors[i]);
            for(uint32_t j=0;j<c->count;j++)if(mask[j]&1) {
                double x=110+1040*ps_report_axis_fraction(c->x[j],b[0],b[1]);
                double y=570-480*ps_report_axis_fraction(c->y[j],b[2],b[3]);
                bool connected=j && (mask[j-1]&1) && !(mask[j]&2);
                fprintf(f,"%c%.3f %.3f ",connected?'L':'M',x,y);
            }
            fputs("\"/>\n",f);
        }
        for (uint32_t j = 0; j < c->count; j++) {
            if(mask && !(mask[j]&1))continue;
            double x = 110 + 1040 * ps_report_axis_fraction(c->x[j], b[0], b[1]);
            double y = 570 - 480 * ps_report_axis_fraction(c->y[j], b[2], b[3]);
            if (c->kind == PS_PLOT_LINE && c->count > 1) {
                if(!mask)fprintf(f,"%.3f,%.3f ",x,y);
                else if((!j || !(mask[j-1]&1) || (mask[j]&2)) &&
                        (j+1==c->count || !(mask[j+1]&1) || (mask[j+1]&2)))
                    fprintf(f,"<circle cx=\"%.3f\" cy=\"%.3f\" r=\"3\" fill=\"%s\"/>\n",x,y,colors[i]);
            }
            else if (c->kind == PS_PLOT_HISTOGRAM) {
                double left =
                    110 + 1040 * ps_report_axis_fraction(c->x[j] - c->bar_width / 2, b[0], b[1]);
                double right =
                    110 + 1040 * ps_report_axis_fraction(c->x[j] + c->bar_width / 2, b[0], b[1]);
                double zero = 570 - 480 * ps_report_axis_fraction(0, b[2], b[3]);
                fprintf(f,
                        "<rect x=\"%.3f\" y=\"%.3f\" width=\"%.3f\" height=\"%.3f\" fill=\"%s\" "
                        "fill-opacity=\"0.65\"/>\n",
                        left, y, fmax(0, right - left), fmax(0, zero - y), colors[i]);
            } else
                fprintf(f, "<circle cx=\"%.3f\" cy=\"%.3f\" r=\"3\" fill=\"%s\"/>\n", x, y,
                        colors[i]);
        }
        if (c->kind == PS_PLOT_LINE && c->count > 1 && !mask)
            fputs("\"/>\n", f);
        if (region)
            fputs("</g>\n", f);
        fprintf(f, "<text x=\"%u\" y=\"%u\" fill=\"%s\" font-size=\"13\">", 110 + (i % 4) * 270,
                654 + (i / 4) * 26, colors[i]);
        xml(f, c->label);
        fputs("</text>\n", f);
    }
    if (reduced)
        fputs("<text x=\"110\" y=\"722\" font-size=\"12\">Reduzierte Diagrammvorschau; "
              "vollständige Messwerte im Quelldatensatz.</text>\n",
              f);
    fputs("</g></svg>\n", f);
    ps_memory_free(allocator, c, sizeof *c);
    return finish(f);
}
