set pagination off
set confirm off
set disable-randomization off
set environment ASAN_OPTIONS detect_leaks=0
set environment PHYSIM_TEST_TRACE 1
set environment PHYSIM_TEST_LAB_PROJECT /tmp/physim-transport-gdb-source2/project
set $visits=0
break /mnt/d/Physim/app/transport_design_tests.inc:101
commands
silent
set $visits=$visits+1
if $visits>=3
set $c=a->ui
set $s=(struct nk_sdl *)$c->userdata.ptr
set $m=$s->accessibility
printf "TRANSPORT DEBUG MODEL: count=%zu focus=%llu pending=%llu keyboard=%d active=%s\n",$m->count,$m->focused_id,$m->pending_focus,$m->keyboard_focus,$c->active->name_string
set $i=0
while $i<$m->count
if $m->nodes[$i].window[0]=='l'
printf "NODE id=%llu role=%d enabled=%d focusable=%d window=%s label=%s\n",$m->nodes[$i].id,$m->nodes[$i].role,$m->nodes[$i].enabled,$m->nodes[$i].focusable,$m->nodes[$i].window,$m->nodes[$i].label
end
set $i=$i+1
end
quit
end
continue
end
run
quit
