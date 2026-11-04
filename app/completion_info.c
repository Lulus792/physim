#include "completion_info.h"
#include "autosave.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
static bool valid(const ps_completion_info_key *key){
    return key && key->editor && (key->text || !key->bytes) && key->bytes<=PS_SOURCE_MAX_BYTES &&
        key->cursor<=key->bytes && isfinite(key->scroll_x) && isfinite(key->scroll_y);
}
void ps_completion_info_cancel(ps_completion_info *info){
    if(info){free(info->text);memset(info,0,sizeof *info);}
}
ps_result ps_completion_info_request(ps_completion_info *info,const ps_completion_info_key *key,double now,bool immediate){
    if(!info || !key)return PS_INVALID;
    if(key->bytes>PS_SOURCE_MAX_BYTES)return PS_LIMIT;
    if(!valid(key) || !isfinite(now) || now<0)return PS_INVALID;
    double due=immediate?0:now+PS_COMPLETION_INFO_DELAY;if(!isfinite(due))return PS_INVALID;
    bool same=info->text && info->bytes==key->bytes && (!key->bytes || !memcmp(info->text,key->text,key->bytes));
    char *text=info->text;
    if(!same){
        text=malloc(key->bytes+1);if(!text)return PS_MEMORY;
        if(key->bytes)memcpy(text,key->text,key->bytes);
        text[key->bytes]=0;free(info->text);
    }
    *info=(ps_completion_info){.mode=immediate?PS_COMPLETION_INFO_VISIBLE:PS_COMPLETION_INFO_PENDING,
        .editor=key->editor,.candidate=key->candidate,.cursor=key->cursor,.bytes=key->bytes,.text=text,
        .scroll_x=key->scroll_x,.scroll_y=key->scroll_y,.due=due};return PS_OK;
}
bool ps_completion_info_update(ps_completion_info *info,const ps_completion_info_key *key,double now){
    if(!info || info->mode==PS_COMPLETION_INFO_HIDDEN)return false;
    if(!valid(key) || !isfinite(now) || now<0 || info->editor!=key->editor || info->candidate!=key->candidate ||
       info->cursor!=key->cursor || info->bytes!=key->bytes || info->scroll_x!=key->scroll_x || info->scroll_y!=key->scroll_y ||
       (key->bytes && memcmp(info->text,key->text,key->bytes))){ps_completion_info_cancel(info);return false;}
    if(info->mode==PS_COMPLETION_INFO_PENDING && now>=info->due){info->mode=PS_COMPLETION_INFO_VISIBLE;info->due=0;}
    return info->mode==PS_COMPLETION_INFO_VISIBLE;
}
