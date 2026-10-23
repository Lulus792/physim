set pagination off
set confirm off
set disable-randomization off
set environment ASAN_OPTIONS detect_leaks=0
set environment PHYSIM_TEST_TRACE 1
set $seen=0
break /mnt/d/Physim/app/ui_backend.c:498 if activated
commands
silent
printf "TRANSPORT DEBUG BEFORE: activated=%d clicked=%d owner=%s active=%s flags=%u layout=%u popup-active=%d popup=%p\n",activated,clicked,window->name_string,ctx->active->name_string,window->flags,window->layout->flags,window->popup.active,window->popup.win
set $seen=1
continue
end
break /mnt/d/Physim/app/ui_backend.c:499 if $seen
commands
silent
printf "TRANSPORT DEBUG AFTER: open=%d current=%s active=%s current-flags=%u layout=%u\n",open,ctx->current->name_string,ctx->active->name_string,ctx->current->flags,ctx->current->layout->flags
quit
end
run
quit
