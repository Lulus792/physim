// Isolated UI prototypes. Checks virtual effects, not native filesystem behavior.
const {chromium}=require('playwright'),assert=require('node:assert/strict'),fs=require('node:fs'),path=require('node:path'),{pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),out=path.join(root,'build/file-safety-options-review');fs.mkdirSync(out,{recursive:true});
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
 const report={date:new Date().toISOString(),platform:process.platform,browser:await browser.version(),sourceCommit:require('node:child_process').execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),layouts:[],checks:[],errors:[]};
 const page=await browser.newPage({viewport:{width:1440,height:1100}});page.setDefaultTimeout(8000);page.on('pageerror',e=>report.errors.push(e.message));let modal=false;
 const base=pathToFileURL(path.join(root,'docs/visuals/file-safety-options/index.html')).href;
 const host=()=>page.locator(modal?'#modal-host':'#preview-host');
 const act=async id=>host().locator(`[data-act="${id}"]`).first().click();
 const go=async(v='a',topic='save',scenario,scope='project',theme='dark')=>{modal=false;await page.goto(base+`?variant=${v}&topic=${topic}&scope=${scope}&theme=${theme}${scenario?'&scenario='+scenario:''}`);await host().locator('.sheet').waitFor();};
 const ready=async v=>{if(v==='c'&&await host().locator('[data-act=next]').count())await act('next');};
 const states=async()=>host().locator('.file-row').evaluateAll(es=>Object.fromEntries(es.map(e=>[e.querySelector('strong').textContent,e.dataset.state])));
 const result=async re=>{await host().locator('.result h2').filter({hasText:re}).waitFor();};
 const cases={save:['changes','readonly','partial'],conflict:['external','deleted','readonly','changedagain'],recovery:['available','conflict','corrupt','none']};
 try{
  for(const v of ['a','b','c'])for(const [topic,scenarios]of Object.entries(cases))for(const scenario of scenarios)for(const theme of ['dark','light'])for(const width of [1440,500]){
   await page.setViewportSize({width,height:1100});await go(v,topic,scenario,'project',theme);
   assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth+1),'No page overflow');
   assert(await host().locator('.sheet').evaluate(e=>e.scrollWidth<=e.clientWidth+1),'No clipped sheet width');
   if(width===1440&&theme==='dark'&&scenario===scenarios[0])await host().locator('.sheet').screenshot({path:path.join(out,`${topic}-${v}.png`)});
   report.layouts.push({variant:v,topic,scenario,theme,width});
  }
  report.checks.push('132 layouts: 3 directions × 11 topic/scenario states × 2 themes × 1440/500 widths; no horizontal page/sheet overflow');
  await page.setViewportSize({width:1440,height:1100});
  for(const v of ['a','b','c']){
   await go(v);await ready(v);await act('save');await result(/Gespeichert/);assert(Object.values(await states()).every(s=>s==='saved'));assert.match(await page.locator('#workspace-status').innerText(),/abgeschlossen/);
   await go(v,'save','partial');await ready(v);await act('save');await host().locator('[data-act=permission]').waitFor();assert.deepEqual(await states(),{'main.c':'saved','analysis.phys':'error','notes.md':'dirty'});assert.match(await page.locator('#workspace-status').innerText(),/nicht ausgeführt/);
   await act('save');await host().locator('[data-act=permission]').waitFor();assert.deepEqual(await states(),{'main.c':'saved','analysis.phys':'error','notes.md':'dirty'});
   await act('permission');await act('save');await result(/Gespeichert/);assert(Object.values(await states()).every(s=>s==='saved'));
   await go(v,'save','readonly');await ready(v);await act('save');await host().locator('[data-act=permission]').waitFor();assert.equal((await states())['main.c'],'error');assert.equal((await states())['notes.md'],'dirty');
   await act('cancel');await result(/abgebrochen/);assert.equal((await states())['main.c'],'error');
   await go(v);await ready(v);await act('discard');assert(await host().locator('.confirm-panel').isVisible());assert(!await host().locator('.result').count());await act('confirm-back');assert(Object.values(await states()).every(s=>s==='dirty'));await act('discard');await act('confirm-commit');await result(/verworfen/);
   await go(v);await ready(v);await act('save');await act('cancel');await result(/abgebrochen/);const afterCancel=await states();await page.waitForTimeout(700);assert.deepEqual(await states(),afterCancel);assert.match(await page.locator('#workspace-status').innerText(),/Ursprünglicher/);
  }
  report.checks.push('All directions: successful save, blocked read-only save, partial sequential save, repeated failure until explicit fixture permission repair, confirmed discard/back, canceled save rejects late callbacks');
  for(const v of ['a','b','c']){
   await go(v,'conflict');await ready(v);await host().locator('[data-copy-name]').fill('main.c');await act('resolve');assert.match(await host().locator('.banner').innerText(),/neuen Dateinamen/);assert(!await host().locator('.result').count());
   await host().locator('[data-copy-name]').fill('mein-entwurf.c');if(v==='c'){await act('back');await act('next');assert.equal(await host().locator('[data-copy-name]').inputValue(),'mein-entwurf.c');}await act('resolve');await result(/Kopie vorgemerkt/);assert.match(await host().locator('.result').innerText(),/keine echte Datei/);
   await go(v,'conflict','deleted');await ready(v);assert(await host().locator('input[value=external]').isDisabled());
   await go(v,'conflict','changedagain');await ready(v);await host().locator('input[value=external]').check();assert.equal(await host().locator('input[value=external]').evaluate(e=>e===document.activeElement),true);await act('resolve');assert(await host().locator('.confirm-panel').isVisible());await act('confirm-commit');assert.match(await host().locator('.banner').innerText(),/erneut geändert/);assert(!await host().locator('.result').count());await act('resolve');await act('confirm-commit');await result(/Dateistand/);
   await go(v,'conflict','readonly');await ready(v);await host().locator('input[value=keep]').check();await act('resolve');await result(/Entwurf bleibt/);assert.match(await page.locator('#workspace-status').innerText(),/Konflikt offen/);
  }
  report.checks.push('All directions: copy-name protection and wizard back retains input; missing file disables reload; radio keyboard focus retained; new external revision requires fresh confirmation; keep-editing preserves conflict');
  for(const v of ['a','b','c'])for(const scope of ['project','document']){
   await go(v,'recovery','available',scope);const names=Object.keys(await states());assert.equal(names.length,scope==='project'?2:1);if(scope==='project')assert.deepEqual(names,['main.c','analysis.phys']);else assert.deepEqual(names,['notes.md']);await ready(v);await act('recover');await result(/Sicherung im Editor/);assert.match(await host().locator('.result').innerText(),/Noch keine Quelldatei/);await act('save-restored');assert.deepEqual(Object.keys(await states()),names);await ready(v);await act('save');await result(/Entwürfe im Beispiel gespeichert/);
   await go(v,'recovery','available',scope);await ready(v);await host().locator('input[value=disk]').check();await act('recover');assert(await host().locator('.confirm-panel').isVisible());await act('confirm-back');assert(Object.keys(await states()).length===names.length);await host().locator('input[value=disk]').check();await act('recover');await act('confirm-commit');await result(/Gespeicherte Dateien/);
   await go(v,'recovery','corrupt',scope);assert.equal(await host().locator('[data-act=recover]').count(),0);await act('work-disk');await result(/weiterarbeiten/);assert.match(await host().locator('.result').innerText(),/Sicherung bleibt erhalten/);assert(Object.values(await states()).every(s=>s==='saved'));
   await go(v,'recovery','none',scope);assert.equal(await host().locator('[data-act=recover]').count(),0);await act('cancel');assert(Object.values(await states()).every(s=>s==='saved'));
  }
  report.checks.push('Project pair and independent document in all directions: correct restoration scope, load ≠ save, explicit later save, confirmed discard/back, corrupt data never loaded/deleted');
  for(const v of ['a','b','c']){
   await go(v,'recovery','conflict');await ready(v);await act('recover');await act('save-restored');await ready(v);await act('save');await host().locator('[data-act=inspect-conflict]').waitFor();assert.equal((await states())['main.c'],'error');assert.equal((await states())['analysis.phys'],'dirty');await act('inspect-conflict');await ready(v);await host().locator('input[value=external]').check();await act('resolve');await act('confirm-commit');await act('resume-save');await ready(v);await act('save');await host().locator('[data-act=inspect-conflict]').waitFor();assert.equal((await states())['main.c'],'saved');assert.equal((await states())['analysis.phys'],'error');
  }
  report.checks.push('Connected recovery → save → conflict route blocks each externally changed project source and preserves all other drafts; resolving one never authorizes the other');
  for(const v of ['a','b','c']){
   await go(v,'conflict');await page.locator('#popup-open').click();modal=true;assert(await page.locator('#live-dialog').isVisible());assert(await page.evaluate(()=>!!document.activeElement.closest('dialog')));for(let i=0;i<12;i++){await page.keyboard.press('Tab');assert(await page.evaluate(()=>!!document.activeElement.closest('dialog')));}
   await ready(v);await host().locator('input[value=external]').check();await act('resolve');await page.keyboard.press('Escape');assert(await page.locator('#live-dialog').isVisible());assert.equal(await host().locator('.confirm-panel').count(),0);await page.keyboard.press('Escape');await page.locator('#live-dialog').waitFor({state:'hidden'});modal=false;assert.equal(await page.locator('#popup-open').evaluate(e=>e===document.activeElement),true);
  }
  await go('c','recovery','available','project','light');await page.setViewportSize({width:500,height:850});await page.evaluate(()=>document.documentElement.style.fontSize='28px');assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth+1));await page.locator('#popup-open').click();modal=true;await ready('c');assert(await host().locator('.sheet').evaluate(e=>e.scrollWidth<=e.clientWidth+1));await act('recover');await result(/Sicherung im Editor/);await page.keyboard.press('Escape');modal=false;await result(/Sicherung im Editor/);
  await page.emulateMedia({forcedColors:'active',reducedMotion:'reduce'});await go('a','save');await page.locator('#popup-open').click();modal=true;await act('cancel');await page.locator('#live-dialog').waitFor({state:'hidden'});modal=false;
  const dpi=await browser.newPage({viewport:{width:1440,height:1100},deviceScaleFactor:2});await dpi.goto(base);assert(await dpi.evaluate(()=>devicePixelRatio===2&&document.documentElement.scrollWidth<=innerWidth+1));await dpi.close();
  report.checks.push('Actual modals in all directions: contained Tab path, Escape returns from confirmation then cancels, focus returns to launcher; 500px / 200% text, high contrast, reduced motion and 2× browser density');
  assert.deepEqual(report.errors,[]);report.passed=true;
 }catch(e){report.failure=e.stack;await page.screenshot({path:path.join(out,'failure.png'),fullPage:true});throw e;}
 finally{fs.writeFileSync(path.join(out,'validation.json'),JSON.stringify(report,null,2)+'\n');await browser.close();}
 console.log(JSON.stringify({passed:report.passed,layouts:report.layouts.length,checks:report.checks,errors:report.errors}));
})().catch(e=>{console.error(e);process.exitCode=1;});
