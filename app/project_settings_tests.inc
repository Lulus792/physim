static bool project_controls_test_start(app *a,const char *root,const char *mode) {
    join(a->project_input,sizeof a->project_input,root,"project");
    if(!strcmp(mode,"project-settings-controls-write")){a->example=0;new_project(a);}
    else open_project(a);
    a->workspace_disclosure=NK_MINIMIZED;a->project_build_disclosure=NK_MAXIMIZED;
    SDL_SetWindowSize(a->window,1080,740);
    return a->loaded && a->profile==0 && !a->built && !a->project_settings_dirty;
}
static bool project_controls_build(app *a,bool release) {
    char module[4096];join(module,sizeof module,a->project,release?"build/Release/experiment" MODULE_EXT:"build/Debug/experiment" MODULE_EXT);
    return a->built && a->profile==(int)release && a->build_info_valid && a->build_info.release==release &&
        a->build_info.api_version==PS_API_VERSION && a->build_info.abi_version==PS_ABI_VERSION &&
        a->build_info.compiler[0] && exists(module) && a->source_revision==a->build_revision && !a->project_settings_dirty;
}
static void project_controls_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    bool read=!strcmp(mode,"project-settings-controls-read"),ok=true;
    if(a->library_thread)return;
    switch((*stage)++) {
    case 0:test_window_key_mod(a->window,SDLK_F5,0);break;
    case 1:
        if(!idle(a)){--*stage;return;}
        ok=project_controls_build(a,false);*capture=read?"build-control-restored-debug.bmp":"build-control-debug.bmp";
        if(read){a->quitting=true;break;}
        break;
    case 2:ok=a->project_profile_bounds.w>0;test_mouse(a,a->project_profile_bounds,true);break;
    case 3:test_mouse(a,a->project_profile_bounds,false);break;
    case 4:ok=a->project_profile_choices[1].w>0;test_mouse(a,a->project_profile_choices[1],true);break;
    case 5:test_mouse(a,a->project_profile_choices[1],false);break;
    case 6:
        ok=a->profile==1 && !a->built && a->project_settings_dirty && a->build_info_valid && !a->build_info.release;
        test_window_key_mod(a->window,SDLK_F6,0);break;
    case 7:
        ok=!a->runner.running && !a->last_run[0] && !a->built && a->editor_build_bounds.w>0;
        test_mouse(a,a->editor_build_bounds,true);break;
    case 8:test_mouse(a,a->editor_build_bounds,false);break;
    case 9:
        if(!idle(a)){--*stage;return;}
        ok=project_controls_build(a,true);*capture="build-control-release.bmp";break;
    case 10:test_mouse(a,a->project_profile_bounds,true);break;
    case 11:test_mouse(a,a->project_profile_bounds,false);break;
    case 12:ok=a->project_profile_choices[0].w>0;test_mouse(a,a->project_profile_choices[0],true);break;
    case 13:test_mouse(a,a->project_profile_choices[0],false);break;
    case 14:
        ok=a->profile==0 && !a->built && a->project_settings_dirty && a->build_info_valid && a->build_info.release;
        if(ok){SDL_Event event={0};event.type=SDL_EVENT_QUIT;ok=SDL_PushEvent(&event);}
        printf("BUILD CONTROLS SELF-TEST: %s\n",ok?"PASSED":"FAILED");break;
    }
    if(!ok){fprintf(stderr,"Build controls stage=%d profile=%d built=%u dirty=%u status=%s\n",*stage-1,a->profile,a->built,a->project_settings_dirty,a->status);*code=1;a->quitting=true;}
    if(read && a->quitting)printf("BUILD CONTROLS SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
static bool project_settings_test_start(app *a, const char *root, const char *mode) {
    if(!strncmp(mode,"project-settings-controls-",26))return project_controls_test_start(a,root,mode);
    join(a->project_input, sizeof a->project_input, root, "project");
    if (!strcmp(mode, "project-settings-write")) {
        a->example = 18; /* Collision language template exposes friction. */
        new_project(a);
        if (!a->loaded || a->profile != 0 || a->build_info_valid)
            return false;
        select_build_profile(a, 1);
        a->dt = .125;
        snprintf(a->seed, sizeof a->seed, "%llu", (unsigned long long)UINT64_MAX);
        return a->project_settings_dirty && !a->built &&
               ps_parameter_catalog_restore(&a->parameters, "friction", "0.25");
    }
    open_project(a);
    if (!a->loaded || a->project_settings_dirty || a->build_info_valid)
        return false;
    int expected = strcmp(mode, "project-settings-debug") != 0;
    if (a->profile != expected)
        return false;
    if (a->dt != .125 || strcmp(a->seed, "18446744073709551615"))
        return false;
    bool restored = false;
    for (uint32_t i = 0; i < a->parameters.count; i++)
        if (!strcmp(a->parameters.entries[i].name, "friction") &&
            !strcmp(a->parameters.selected[i], "0.25"))
            restored = true;
    return restored;
}
static void project_settings_test_frame(app *a, const char *mode, int *stage, int *code, const char **capture) {
    if(!strncmp(mode,"project-settings-controls-",26)){project_controls_test_frame(a,mode,stage,code,capture);return;}
    static int reported_stage = -1;
    if (*stage != reported_stage) {
        fprintf(stderr, "Project settings %s stage %d: %s\n", mode, *stage, a->status);
        reported_stage = *stage;
    }
    if (a->library_thread)
        return;
    bool read = !strcmp(mode, "project-settings-read");
    if (read && *stage >= 2 && *stage <= 3 && !a->runner.running) {
        fprintf(stderr, "Runner stopped before timestep verification: %s\n", a->status);
        *code = 1;
        a->quitting = true;
        return;
    }
    if (read && *stage == 0) {
        build_project(a);
        if (!a->job.running) {
            *code = 1;
            a->quitting = true;
        }
        ++*stage;
        return;
    }
    if (read && *stage == 1) {
        if (a->job.running)
            return;
        char path[4096];
        join(path, sizeof path, a->project, "build/Release/experiment" MODULE_EXT);
        if (!a->built || a->profile != 1 || !exists(path) || !a->build_info_valid ||
            !a->build_info.release || !a->build_info.compiler[0] ||
            a->build_info.api_version!=PS_API_VERSION || a->build_info.abi_version!=PS_ABI_VERSION) {
            *code = 1;
            a->quitting = true;
            return;
        }
        a->project_build_disclosure=NK_MAXIMIZED;
        snprintf(a->seed, sizeof a->seed, "18446744073709551616");
        a->project_settings_dirty = true;
        start_run(a);
        if (a->runner.running || a->last_run[0] || !strstr(a->status, "Zufallsseed") ||
            !a->project_settings_dirty) {
            *code = 1;
            a->quitting = true;
            return;
        }
        save_project(a);
        if (!a->project_settings_dirty) {
            *code = 1;
            a->quitting = true;
            return;
        }
        snprintf(a->seed, sizeof a->seed, "%llu", (unsigned long long)UINT64_MAX);
        start_run(a);
        if (!a->runner.running || a->project_settings_dirty || !a->built) {
            *code = 1;
            a->quitting = true;
            return;
        }
        *stage = 2;
        return;
    }
    if (read && *stage == 2) {
        if (!a->hello)
            return;
        /* The app sends RUN after HELLO. Pause before requesting a single step. */
        if (!command(a, PS_MSG_PAUSE) || !command(a, PS_MSG_STEP)) {
            *code = 1;
            a->quitting = true;
            return;
        }
        *stage = 3;
        return;
    }
    if (read && *stage == 3) {
        if (a->simulation_time == 0)
            return;
        if (a->simulation_time < .125 || fmod(a->simulation_time, .125) != 0) {
            *code = 1;
            a->quitting = true;
            return;
        }
        command(a, PS_MSG_STOP);
        a->stop_at = ps_clock();
        *stage = 4;
        return;
    }
    if (read && *stage == 4) {
        if (!idle(a))
            return;
        ps_run_reader run;
        if (ps_run_open(&run, a->last_run) != PS_OK) {
            *code = 1;
            a->quitting = true;
            return;
        }
        bool valid = strstr(run.metadata, "\ndt_s=0.125\n") &&
                     strstr(run.metadata, "\nseed=18446744073709551615\n");
        double time, values[PS_MAX_CHANNELS];
        valid = valid && ps_run_next(&run, &time, values) == PS_OK && time == 0 &&
                ps_run_next(&run, &time, values) == PS_OK && time == .125;
        double previous_time = .125;
        ps_result next = PS_INVALID;
        while (valid && (next = ps_run_next(&run, &time, values)) == PS_OK) {
            valid = time - previous_time == .125;
            previous_time = time;
        }
        valid = valid && next == PS_EOF;
        ps_run_reader_close(&run);
        if (!valid) {
            *code = 1;
            a->quitting = true;
            return;
        }
        a->tab=0;
        a->workspace_disclosure=NK_MINIMIZED;
        *capture="build-info.bmp";
        *stage=5;
        return;
    }
    if (read && *stage == 5) {
        select_build_profile(a, 0);
        if (a->built || !a->project_settings_dirty || !a->build_info_valid ||
            !a->build_info.release || a->build_info.api_version!=PS_API_VERSION ||
            a->build_info.abi_version!=PS_ABI_VERSION) {
            *code = 1;
            a->quitting = true;
            return;
        }
    }
    SDL_Event event = {0};
    event.type = SDL_EVENT_QUIT; /* Exercise normal automatic project saving. */
    if (!SDL_PushEvent(&event)) {
        *code = 1;
        a->quitting = true;
    }
    printf("PROJECT SETTINGS %s: %s\n", mode, *code ? "FAILED" : "PASSED");
    ++*stage;
}
