set pagination off
set confirm off
set disable-randomization off
set environment ASAN_OPTIONS detect_leaks=0
set environment PHYSIM_TEST_GRAPHIC_DIALOG 1
set environment PHYSIM_TEST_TRACE 1
break app/ui_backend.c:493 if ctx->current->name_string[0]=='g' && ctx->current->name_string[14]==0 && ctx->current->layout->row.height>1000
commands
silent
printf "WRAPPER_MUTATION label=%s sheet_row=%g requested_button_height=%g slot_h=%g row_type=%d\n",text,ctx->current->layout->row.height,height,ctx->current->layout->row.item.h,ctx->current->layout->row.type
bt 4
printf "DIAGNOSTIC ONLY: intentionally stop own inferior immediately before the measured row assignment. No app completion claimed.\n"
quit
end
run
