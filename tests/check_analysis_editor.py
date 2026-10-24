"""Analysis source uses the selected native workbench and returns to unchanged analysis data."""
import argparse,json,pathlib,re,sys
from check_display_workflow import Workflow
from check_graphic_raster import png
from check_transport_design import pixels
from check_inspector_tail import rect

def inspect(folder,out,only):
    records=[]
    for line in out.splitlines():
        if not line.startswith('ANALYSIS EDITOR kind='):continue
        assert 'area=0 active=1 target=1 preserved=1' in line,line
        assert 'status-name=1' in line,line
        r=dict(kind=re.search(r'kind=(\w+)',line)[1],variant=re.search(r'variant=(\w+)',line)[1],size=int(re.search(r'size=(\d+)',line)[1]))
        for k in ('strip','tab','tree','editor','frame','search','nav','context'):r[k]=rect(line,k)
        records.append(r)
    assert len(records)==(2 if only else 8),len(records)
    assert 'ANALYSIS EDITOR ACTIONS code-button=1 primary-return=1 active-file=1 search=1 undo-save=1 data-report-source=1 area-persistence=1 native-focus-yield=1 menu=1' in out
    for r in records:
        f=r['size']/16;im=png(folder/f"analysis-editor-{r['variant']}-{r['kind']}.png")
        assert im[:2]==((1440,1000) if r['size']==16 else (1080,740)),r
        assert abs(r['tab'][3]-28*f)<.03 and abs(r['strip'][3]-40*f)<.03,r
        assert r['editor'][2]>100 and r['editor'][3]>100 and r['context'][2]>=180,r
        ink=(255,255,255) if r['variant']=='contrast' else (32,36,44) if r['variant'] in ('light','large') else (237,238,242)
        assert pixels(im,r['tab'],ink,50),('missing active analysis filename',r)
        selected=(0,0,110) if r['variant']=='contrast' else (217,230,248) if r['variant'] in ('light','large') else (43,61,89)
        assert len(pixels(im,r['tree'],selected,4))>=500,('analysis source not selected in workspace tree',r)
        if r['variant']!='contrast':
            button=(224,227,234) if r['variant'] in ('light','large') else (53,54,62)
            assert len(pixels(im,r['nav'],button,4))>=300,('wrong primary workspace',r)
        if r['kind']=='search':
            island,frame=r['search'],r['frame'];assert abs(island[1]-frame[1]-4)<.03 and abs(island[0]+island[2]-(frame[0]+frame[2]-8))<.03,r
        else:assert r['search'][2]==0,r
    assert (folder/'analysis-editor-returned.png').exists()
    return records

def main():
    for s in (sys.stdout,sys.stderr):
        if hasattr(s,'reconfigure'):s.reconfigure(encoding='utf-8',errors='replace')
    p=argparse.ArgumentParser();p.add_argument('--app',type=pathlib.Path,required=True);p.add_argument('--root',type=pathlib.Path,required=True);p.add_argument('--work',type=pathlib.Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
    flow=Workflow(a.app,a.work,a.root,False);flow.env['SDL_FORCE_RAISEWINDOW']='1';checks=[]
    for font in ('system','bundled'):
        if font=='bundled':flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
        else:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        for language in ('c','phys'):
            folder=a.work/(font+'-'+language);folder.mkdir();flow.run('--workspace-state-test',folder,'analysis-editor-'+language,timeout=190,marker='ANALYSIS EDITOR SELF-TEST: PASSED')
            checks.append(dict(font=font,language=language,only=False,records=inspect(folder,flow.steps[-1]['stdout'],False)))
        language='c' if font=='system' else 'phys';folder=a.work/(font+'-only-'+language);folder.mkdir();flow.run('--workspace-state-test',folder,'analysis-editor-only-'+language,timeout=190,marker='ANALYSIS EDITOR SELF-TEST: PASSED')
        checks.append(dict(font=font,language=language,only=True,records=inspect(folder,flow.steps[-1]['stdout'],True)))
    (a.work/'validation.json').write_text(json.dumps(dict(scope=__doc__,checks=checks),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('ANALYSIS EDITOR: PASSED')
if __name__=='__main__':main()
