set pagination off
set confirm off
set disable-randomization off
set environment ASAN_OPTIONS detect_leaks=0
set environment PHYSIM_TEST_TRACE 1
set environment PHYSIM_TEST_LAB_PROJECT /tmp/physim-transport-gdb-source2/project
break /mnt/d/Physim/app/ui_backend.c:332 if ctx->current->parent && role==1 && text[0]=='1' && text[1]!='6'
commands
silent
printf "TRANSPORT EMIT: text=%s window=%s parent=%s flags=%u layout=%u widgets-disabled=%d enabled-before=%d routed=%d clip=%.2f,%.2f,%.2f,%.2f bounds=%.2f,%.2f,%.2f,%.2f\n",text,ctx->current->name_string,ctx->current->parent->name_string,ctx->current->flags,ctx->current->layout->flags,ctx->current->widgets_disabled,enabled,routed,clip.x,clip.y,clip.w,clip.h,bounds.x,bounds.y,bounds.w,bounds.h
set $next=ctx->current->next
while $next
printf "TRANSPORT ABOVE: %s flags=%u bounds=%.2f,%.2f,%.2f,%.2f\n",$next->name_string,$next->flags,$next->bounds.x,$next->bounds.y,$next->bounds.w,$next->bounds.h
set $next=$next->next
end
quit
end
run
quit
