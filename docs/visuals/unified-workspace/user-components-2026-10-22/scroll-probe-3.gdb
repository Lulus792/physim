set pagination off
set confirm off
set disable-randomization off
set environment ASAN_OPTIONS detect_leaks=0
set environment PHYSIM_TEST_SMALL 1
set environment PHYSIM_TEST_TRACE 1
set $scroll_probes=0
break app/camera_plane_tests.inc:20
commands
silent
set $scroll_probes=$scroll_probes+1
printf "SCROLL_PROBE n=%d target=%g,%g,%g,%g body=%g,%g,%g,%g scroll=%u mouse=%g,%g wheel=%g\n", $scroll_probes,r.x,r.y,r.w,r.h,a->laboratory_inspector_body.x,a->laboratory_inspector_body.y,a->laboratory_inspector_body.w,a->laboratory_inspector_body.h,a->laboratory_scroll_y,a->ui->input.mouse.pos.x,a->ui->input.mouse.pos.y,a->ui->input.mouse.scroll_delta.y
printf "SCROLL_FIELD vector=%g,%g,%g,%g panel=%g,%g,%g,%g\n", a->display_vector_bounds.x,a->display_vector_bounds.y,a->display_vector_bounds.w,a->display_vector_bounds.h,panel.x,panel.y,panel.w,panel.h
if $scroll_probes==12
printf "DIAGNOSTIC ONLY: intentionally stop own inferior after 12 geometry probes; no application completion claimed\n"
quit
end
continue
end
run
