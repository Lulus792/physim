const {chromium}=require('playwright'),assert=require('node:assert/strict'),fs=require('node:fs'),path=require('node:path'),{pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),out=path.join(root,'build/file-safety-options-review');fs.mkdirSync(out,{recursive:true});
(async()=>{
 const browser=await chromium.launch({channel:'msedge',headless:true}),page=await browser.newPage({viewport:{width:1440,height:1100}});
 const report={date:new Date().toISOString(),browser:await browser.version(),checks:[],errors:[]};page.on('pageerror',e=>report.errors.push(e.message));
 const base=pathToFileURL(path.join(root,'docs/visuals/file-safety-options/index.html')).href;
 const go=async(topic='save',extra='')=>{await page.goto(base+`?variant=b2&topic=${topic}${extra}`);await page.locator('#preview-host .sheet').waitFor();};
 const rows=()=>page.locator('#preview-host .diff-line').evaluateAll(es=>es.map(e=>({kind:e.dataset.kind,text:e.querySelector('.diff-content').textContent,old:e.dataset.old,new:e.dataset.new})));
 const sides=async()=>{const r=await rows();return {before:r.filter(x=>x.kind!=='add').map(x=>x.text).join('\n'),after:r.filter(x=>x.kind!=='delete').map(x=>x.text).join('\n')};};
 const act=id=>page.locator(`#preview-host [data-act=${id}]`).click();
 try{
  for(const topic of ['save','conflict','recovery'])for(const theme of ['dark','light']){
   await go(topic,'&theme='+theme);assert.equal(await page.locator('#preview-host .unified-diff').count(),1);assert.equal(await page.locator('#preview-host .versions').count(),0);assert.match(await page.locator('.diff-note').innerText(),/keine zusammengeführte Datei/);
   await page.locator('#preview-host .sheet').screenshot({path:path.join(out,`b2-${topic}-${theme}.png`)});
  }
  await go();assert.deepEqual(await sides(),{before:'// Pendel\nconst double gravity = 9.81;\nconst double length = 1.0;\nstep(dt);',after:'// Pendel\nconst double gravity = 9.80665;\nconst double length = 1.5;\nstep(dt);'});assert.equal(await page.locator('.diff-counts').innerText(),'+2\n−2');
  await page.locator('#preview-host [data-file=analysis]').click();assert.deepEqual(await sides(),{before:'let angle = run.channel("angle");\nplot(angle);',after:'let angle = run.channel("angle");\nplot(angle, title: "Winkelverlauf");\nprint(mean(angle));'});
  await page.locator('#preview-host [data-file=notes]').click();assert.match((await sides()).after,/Länge: 1,5 m/);assert.match((await sides()).before,/Länge: 1,0 m/);
  await page.locator('[data-variant=b]').click();assert.equal(await page.locator('#preview-host .versions').count(),1);assert.equal(await page.locator('#preview-host .unified-diff').count(),0);await page.locator('[data-variant=b2]').click();assert.match((await sides()).after,/Energiebilanz prüfen/);
  report.checks.push('One common diff in all topics/themes; exact saved/editor texts reconstruct from DOM rows; line changes follow selected C/Physim/Markdown file; original B stays side-by-side and switching retains selection');
  await go('conflict','&scenario=deleted');assert.equal((await rows()).filter(x=>x.kind==='delete').length,0);assert((await rows()).every(x=>x.kind==='add'));assert.match(await page.locator('.diff-legend').innerText(),/Datei fehlt/);
  await go('conflict','&scenario=changedagain');await page.locator('#preview-host input[value=external]').check();await act('resolve');await act('confirm-commit');assert.match((await sides()).before,/length = 2.5/);assert.match((await sides()).after,/length = 1.5/);assert.equal(await page.locator('#preview-host .result').count(),0);
  await go('recovery','&scope=document');assert.equal(await page.locator('#preview-host [data-file]').count(),1);assert.match(await page.locator('.diff-legend').innerText(),/Automatische Sicherung/);
  for(const scenario of ['corrupt','none']){await go('recovery','&scenario='+scenario);assert.equal(await page.locator('#preview-host .unified-diff').count(),0);}
  report.checks.push('Deleted original has only added lines; new external revision updates the exact comparison and blocks completion; recovery compares correct document; missing/corrupt backup never invents a diff');
  await go('save');await page.setViewportSize({width:500,height:850});await page.evaluate(()=>document.documentElement.style.fontSize='28px');assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth+1));await page.locator('#popup-open').click();assert(await page.locator('#live-dialog .unified-diff').isVisible());assert(await page.locator('#live-dialog .sheet').evaluate(e=>e.scrollWidth<=e.clientWidth+1));await page.keyboard.press('Escape');
  await page.emulateMedia({forcedColors:'active'});await go();assert(await page.locator('.diff-sign').first().isVisible());assert((await rows()).some(r=>r.kind==='delete'));assert((await rows()).some(r=>r.kind==='add'));report.checks.push('Small 500px / 200% UI, actual popup and forced colors retain readable common view and explicit plus/minus distinction');
  await page.emulateMedia({forcedColors:'none'});await page.setViewportSize({width:1440,height:1100});
  await page.goto(base);assert.equal(await page.locator('body').getAttribute('data-variant'),'b2');
  for(const theme of ['dark','light']){
   await page.goto(base+`?approved=1&variant=a&theme=${theme}`);assert(await page.locator('.variants').isHidden());assert.equal(await page.locator('body').getAttribute('data-variant'),'b2');assert.match(page.url(),/variant=b2/);
   for(const topic of ['conflict','recovery','save']){await page.locator(`[data-topic=${topic}]`).click();assert.equal(await page.locator('body').getAttribute('data-variant'),'b2');assert.equal(await page.locator('#preview-host .unified-diff').count(),1);assert.match(page.url(),/approved=1/);}
   await page.locator('#scenario').selectOption('partial');assert.match(page.url(),/approved=1/);await page.reload();assert.equal(await page.locator('body').getAttribute('data-variant'),'b2');assert(await page.locator('.variants').isHidden());
  }
  await page.locator('#reference-mode').click();assert(await page.locator('.variants').isVisible());await page.locator('[data-variant=c]').click();assert.equal(await page.locator('body').getAttribute('data-variant'),'c');await page.locator('#reference-mode').click();assert.equal(await page.locator('body').getAttribute('data-variant'),'b2');
  await page.locator('#preview-host .sheet').screenshot({path:path.join(out,'b2-approved.png')});report.checks.push('Default B2 and approved mode: hidden variant chooser, conflicting variant ignored, all topics/themes/scenario/reload retain B2; history comparison does not alter approval');
  assert.deepEqual(report.errors,[]);report.passed=true;
 }catch(e){report.failure=e.stack;await page.screenshot({path:path.join(out,'b2-failure.png'),fullPage:true});throw e;}
 finally{fs.writeFileSync(path.join(out,'b2-validation.json'),JSON.stringify(report,null,2)+'\n');await browser.close();}
 console.log(JSON.stringify(report));
})().catch(e=>{console.error(e);process.exitCode=1;});
