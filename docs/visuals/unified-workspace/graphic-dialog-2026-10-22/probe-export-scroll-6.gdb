set pagination off
set confirm off
set disable-randomization off
set environment ASAN_OPTIONS detect_leaks=0
set environment PHYSIM_TEST_GRAPHIC_DIALOG 1
set environment PHYSIM_TEST_TRACE 1
set $graphic_probes=0
break app/graphic_export_ui.inc:387 if a->preferences.ui_size==22
commands
silent
set $graphic_probes=$graphic_probes+1
printf "BEFORE_END n=%u win=%p flags=%x rowtype=%d rowheight=%g rowindex=%d columns=%d at_y=%g bounds=%g,%g,%g,%g scrollbar=%u offset=%u focus=%u total=%g\n",$graphic_probes,ui->current,ui->current->flags,ui->current->layout->row.type,ui->current->layout->row.height,ui->current->layout->row.index,ui->current->layout->row.columns,ui->current->layout->at_y,ui->current->layout->bounds.x,ui->current->layout->bounds.y,ui->current->layout->bounds.w,ui->current->layout->bounds.h,ui->current->scrollbar.y,*ui->current->layout->offset_y,d->focus_target,total
continue
end
break third_party/nuklear.h:20692 if ctx->current->name_string[0]=='g' && ctx->current->name_string[14]==0 && ctx->current->bounds.w<700
commands
silent
printf "AFTER_SCROLL n=%u target=%g result=%g offset=%u rowheight=%g at_y=%g bounds_h=%g flags=%x active=%u wheel=%g keys=%u,%u,%u,%u\n",$graphic_probes,scroll_target,scroll_offset,*layout->offset_y,layout->row.height,layout->at_y,layout->bounds.h,layout->flags,ctx->current==ctx->active,in?in->mouse.scroll_delta.y:0,in?in->keyboard.keys[NK_KEY_SCROLL_START].down:0,in?in->keyboard.keys[NK_KEY_SCROLL_END].down:0,in?in->keyboard.keys[NK_KEY_SCROLL_UP].down:0,in?in->keyboard.keys[NK_KEY_SCROLL_DOWN].down:0
if $graphic_probes==8
printf "DIAGNOSTIC ONLY: intentionally stop own inferior after eight complete sheet/scroll probes. No application completion claimed.\n"
quit
end
continue
end
run
