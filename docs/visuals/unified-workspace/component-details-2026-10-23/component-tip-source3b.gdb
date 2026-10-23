set pagination off
set confirm off
set environment ASAN_OPTIONS detect_leaks=0
set environment PHYSIM_TEST_TRACE 1
break workspace_tip_prepare if a->ui->input.mouse.scroll_delta.y != 0
commands
silent
printf "WHEEL prepare visible=%d seq=%u tipseq=%u mouse=%g,%g wheel=%g owner=%p ui=%p\n",a->workspace_tip.visible,a->ui->seq,a->workspace_tip.sequence,a->ui->input.mouse.pos.x,a->ui->input.mouse.pos.y,a->ui->input.mouse.scroll_delta.y,a->workspace_tip.owner,a->ui
continue
end
break workspace_tip_content if a->ui->input.mouse.scroll_delta.y != 0
commands
silent
printf "WHEEL content held=%d visible=%d scroll=%u delta=%g\n",guard.held,a->workspace_tip.visible,a->workspace_tip.scroll,a->ui->input.mouse.scroll_delta.y
continue
end
break workspace_path_tooltip.inc:83 if a->workspace_tip.scroll > 0
commands
silent
printf "WHEEL before popup scroll=%u previous_popup=%p\n",a->workspace_tip.scroll,a->ui->current->popup.win
continue
end
break workspace_path_tooltip.inc:93 if a->workspace_tip.scroll > 0
commands
silent
printf "WHEEL after begin cached=%u popup_scroll=%u popup=%p\n",a->workspace_tip.scroll,a->ui->current->scrollbar.y,a->ui->current
continue
end
break workspace_path_tooltip.inc:112 if a->workspace_tip.scroll > 0 || guard.mouse.scroll_delta.y != 0
commands
silent
printf "WHEEL after end cached=%u\n",a->workspace_tip.scroll
quit
end
run --workspace-state-test /mnt/d/Physim/build/native/component-tip-gdb-source3b component-tip-dark
