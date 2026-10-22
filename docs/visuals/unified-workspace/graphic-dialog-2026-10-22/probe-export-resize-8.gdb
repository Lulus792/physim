set pagination off
set confirm off
set disable-randomization off
set environment ASAN_OPTIONS detect_leaks=0
set environment PHYSIM_TEST_GRAPHIC_DIALOG 1
set environment PHYSIM_TEST_TRACE 1
break app/plot_tests.inc:275 if a->preferences.ui_size==16 && a->graphic_export->bounds.w>1000 && a->graphic_export->fields[1].y<0
commands
silent
printf "RESIZE_PROBE window=%s flags=%x scroll=%u target=%g,%g,%g,%g modal=%g,%g,%g,%g content=%g\n",a->ui->end->name_string,a->ui->end->flags,a->ui->end->scrollbar.y,target.x,target.y,target.w,target.h,d->bounds.x,d->bounds.y,d->bounds.w,d->bounds.h,d->content_height
printf "DIAGNOSTIC ONLY: intentionally stop own inferior after observing restored wide-sheet bounds. No application completion claimed.\n"
quit
end
run
