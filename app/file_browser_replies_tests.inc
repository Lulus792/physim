/* Real B/A widgets and application completion routing. A retired reply is
 * injected deliberately; this is not a claim about driving an OS file chooser. */
static void browser_replies_test_frame(app *a,int *stage,int *code) {
    static unsigned settle;static ps_dialog_ticket old,current;static char target[4096];
    if(settle++<2)return;settle=0;bool okay=true;browser_dialog *d=a->browser;
    if(d && (browser_busy(d) || (d->visible && d->cache_dirty)))return;
#define REPLY_MANAGER(id) do{if(!manager_test_focus(a,id))return;}while(0)
#define REPLY_BROWSER(id) do{if(!browser_test_focus(a,id))return;}while(0)
    switch(*stage){
    case 0:
        okay=ps_browser_create_directory(browser_test_root,"Neuer Auftrag λ",target)==PS_BROWSER_CREATED;
        open_project_manager(a,"Datei");break;
    case 1:REPLY_MANAGER(7);manager_test_key(a,SDLK_RETURN,0);break;
    case 2:okay=a->manager_step==1;REPLY_MANAGER(1);manager_test_key(a,SDLK_RETURN,0);break;
    case 3:
        okay=browser_active(a) && d->mode==PS_DIALOG_MANAGER_PARENT;
        if(okay){old=d->ticket;okay=ps_dialog_request_find(&a->dialog_requests,old)!=NULL;test_window_key_mod(browser_test_window(a),SDLK_ESCAPE,0);}break;
    case 4:okay=!browser_active(a) && !a->dialog_pending;manager_test_key(a,SDLK_ESCAPE,0);break;
    case 5:okay=!a->project_manager;open_project_manager(a,"Datei");break;
    case 6:REPLY_MANAGER(7);manager_test_key(a,SDLK_RETURN,0);break;
    case 7:REPLY_MANAGER(0);manager_test_key(a,SDLK_A,PS_UI_COMMAND_MOD);break;
    case 8:manager_test_text(a,target);break;
    case 9:okay=!strcmp(a->manager_parent,target);REPLY_MANAGER(1);manager_test_key(a,SDLK_RETURN,0);break;
    case 10:
        okay=browser_active(a) && !strcmp(d->location,target);
        if(okay){current=d->ticket;okay=old.route==current.route && old.version<current.version;
            browser_complete(a,old,browser_test_root);}break;
    case 11:
        okay=browser_active(a) && a->dialog_pending && !strcmp(a->manager_parent,target) &&
            ps_dialog_request_find(&a->dialog_requests,current) && !ps_dialog_request_find(&a->dialog_requests,old);
        if(okay){REPLY_BROWSER(20);test_window_key_mod(browser_test_window(a),SDLK_RETURN,0);}break;
    case 12:
        okay=!browser_active(a) && !a->dialog_pending && !strcmp(a->manager_parent,target) && !ps_dialog_requests_pending(&a->dialog_requests);
        printf("{\"kind\":\"dialog_reply_ownership\",\"route\":%u,\"retired_version\":%llu,\"current_version\":%llu,\"stale_reply_ignored\":%s}\n",current.route,(unsigned long long)old.version,(unsigned long long)current.version,okay?"true":"false");
        manager_test_key(a,SDLK_ESCAPE,0);break;
    case 13:{
        okay=!a->project_manager && !browser_active(a) && !ps_dialog_requests_pending(&a->dialog_requests) && !a->loaded && !a->workspace_open;
        ps_dialog_request *request=NULL;okay &= ps_dialog_request_begin(&a->dialog_requests,PS_DIALOG_ADD_FOLDER,&request)==PS_OK;
        if(okay){a->dialog_pending=true;workspace_dialog_callback(ps_dialog_request_retain(request),NULL,0);}break;
    }
    case 14:{
        okay=!a->dialog_pending && !ps_dialog_requests_pending(&a->dialog_requests) && strstr(a->status,"Dateiauswahl fehlgeschlagen") &&
            strstr(a->status,ps_result_string(PS_IO)) && !strstr(a->status,"%s");
        const unsigned routes[]={PS_DIALOG_OPEN_FOLDER,PS_DIALOG_ADD_FILE,PS_DIALOG_ADD_FOLDER,PS_DIALOG_IMPORT_RUN,PS_DIALOG_RESUME_BATCH,PS_DIALOG_BROWSE};
        ps_dialog_request *held[6]={0},*parent=NULL;ps_dialog_ticket tickets[6]={{0}};
        for(unsigned i=0;i<6;i++){
            ps_dialog_request *request=NULL;bool begun=ps_dialog_request_begin(&a->dialog_requests,routes[i],&request)==PS_OK;
            okay &= begun;if(begun){held[i]=ps_dialog_request_retain(request);tickets[i]=ps_dialog_request_ticket(request);}
        }
        okay &= ps_dialog_request_begin(&a->dialog_requests,PS_DIALOG_MANAGER_PARENT,&parent)==PS_OK;
        ps_dialog_ticket independent=ps_dialog_request_ticket(parent);
        if(okay)okay=open_workspace_path(a,target) && a->workspace_open && !strcmp(a->workspace,target);
        for(unsigned i=0;i<6;i++){
            okay &= ps_dialog_request_find(&a->dialog_requests,tickets[i])==NULL;
            const char *files[]={browser_test_root,NULL};if(held[i])workspace_dialog_callback(held[i],files,0);
        }
        okay &= ps_dialog_request_find(&a->dialog_requests,independent)==parent && parent!=NULL;
        ps_dialog_request_cancel(&a->dialog_requests,independent);a->dialog_pending=ps_dialog_requests_pending(&a->dialog_requests);
        break;
    }
    default:
        okay=!a->dialog_pending && !ps_dialog_requests_pending(&a->dialog_requests) && a->workspace_open &&
            !strcmp(a->workspace,target) && !a->workspace_addition_count && !a->loaded && !a->runner.running;
        printf("{\"kind\":\"dialog_project_change\",\"retired_routes\":6,\"late_callbacks_ignored\":%s}\n",okay?"true":"false");
        puts(okay?"DIALOG REPLY OWNERSHIP SELF-TEST: PASSED":"DIALOG REPLY OWNERSHIP SELF-TEST: FAILED");a->quitting=true;break;
    }
    if(!okay){fprintf(stderr,"Dialog reply stage%d failed: manager=%d picker=%d pending=%d\n",*stage,a->project_manager,browser_active(a),a->dialog_pending);*code=1;a->quitting=true;}
    (*stage)++;
#undef REPLY_MANAGER
#undef REPLY_BROWSER
}
