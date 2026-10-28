#include "project_wizard_remote_tests.inc"
#include "project_wizard_help_tests.inc"
/* Geometry counterexample and actual next/back interaction for selected B. */
static void project_wizard_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned settle;static char original_name[256];
    if(settle++<2)return;settle=0;
    bool ok=true;float f=a->preferences.ui_size/16.f;
    switch((*stage)++) {
    case 0:open_project_manager(a,"Neues Projekt");snprintf(original_name,sizeof original_name,"%s",a->manager_name);break;
    case 1:case 2:break;
    case 3:
        ok=a->manager_step==0 && a->manager_bounds.w>0 && a->manager_body.w>0;
        ok &= fabsf(a->manager_body.w-fminf(850*f,a->manager_bounds.w-2-a->manager_scrollbar))<2;
        ok &= a->manager_focus_bounds[11].w>0 && a->manager_focus_bounds[18].h>0;
        ok &= a->manager_focus_bounds[11].w==a->manager_focus_bounds[12].w;
        printf("{\"kind\":\"project_wizard\",\"stage\":0,\"passed\":%s,\"scale\":%.4f,\"scrollbar\":%.4f,\"modal\":[%.4f,%.4f,%.4f,%.4f],\"body\":[%.4f,%.4f,%.4f,%.4f],\"card\":[%.4f,%.4f,%.4f,%.4f]}\n",
            ok?"true":"false",f,a->manager_scrollbar,a->manager_bounds.x,a->manager_bounds.y,a->manager_bounds.w,a->manager_bounds.h,
            a->manager_body.x,a->manager_body.y,a->manager_body.w,a->manager_body.h,
            a->manager_focus_bounds[11].x,a->manager_focus_bounds[11].y,a->manager_focus_bounds[11].w,a->manager_focus_bounds[11].h);
        const struct nk_user_font *button_font=&a->manager_fonts[4];const char *button_label=manager_button_label(a,7);
        struct nk_rect next=a->manager_focus_bounds[7],second=a->manager_focus_bounds[12];
        printf("{\"kind\":\"project_wizard_controls\",\"next\":[%.4f,%.4f,%.4f,%.4f],\"next_text_width\":%.4f,\"second_card\":[%.4f,%.4f,%.4f,%.4f],\"font_roles\":[",next.x,next.y,next.w,next.h,button_font->width(button_font->userdata,button_font->height,button_label,(int)strlen(button_label)),second.x,second.y,second.w,second.h);
        for(unsigned i=0;i<12;i++){bool strong=i<=3 || i==11,mono=i==7;float em=mono?a->workbench_code_em:strong?a->workbench_bold_em:a->workbench_regular_em;printf("%s%.4f",i?",":"",a->manager_fonts[i].height/(em*f));}printf("]}\n");
        *capture=ok?"project-wizard.png":"project-wizard-before.png";
        test_window_key_mod(a->window,SDLK_TAB,SDL_KMOD_SHIFT);break;
    case 4:if(a->manager_focus_reveal){--*stage;return;}test_mouse(a,a->manager_focus_bounds[7],true);break;
    case 5:test_mouse(a,a->manager_focus_bounds[7],false);break;
    case 6:
        ok=a->manager_step==1 && a->project_manager;
        ok &= a->manager_focus_bounds[2].w>0 && a->manager_focus_bounds[6].w>0;
        test_window_key_mod(a->window,SDLK_TAB,SDL_KMOD_SHIFT);test_window_key_mod(a->window,SDLK_TAB,SDL_KMOD_SHIFT);break;
    case 7:if(a->manager_focus_reveal){--*stage;return;}test_mouse(a,a->manager_focus_bounds[9],true);break;
    case 8:test_mouse(a,a->manager_focus_bounds[9],false);break;
    case 9:
        ok=a->manager_step==0 && !strcmp(original_name,a->manager_name);
        test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 10:ok=!a->project_manager;a->quitting=true;printf("PROJECT WIZARD SELF-TEST: %s\n",ok?"PASSED":"FAILED");break;
    }
    if(!ok){fprintf(stderr,"Project B stage %d: step=%d modal-width=%.3f\n",*stage-1,a->manager_step,a->manager_bounds.w);*code=1;a->quitting=true;}
}
