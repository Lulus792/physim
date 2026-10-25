// Explicit user selection must survive topic changes, reloads and nested dialogs.
const {chromium}=require('playwright'),assert=require('node:assert/strict'),fs=require('node:fs'),path=require('node:path'),{pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),out=path.join(root,'build/approved-design-review');fs.mkdirSync(out,{recursive:true});
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
 const report={date:new Date().toISOString(),platform:process.platform,browser:await browser.version(),layouts:[],checks:[],errors:[]};
 const page=await browser.newPage({viewport:{width:1440,height:1100}});page.setDefaultTimeout(10000);page.on('pageerror',e=>report.errors.push(e.message));
 const base=pathToFileURL(path.join(root,'docs/visuals/design-options/index.html')).href,expected={empty:'a',protocol:'c',simulation:'a',analysis:'a',runs:'a',project:'b',files:'a'};
 const go=async topic=>{await page.goto(base+`?approved=1&topic=${topic}&variant=c`);await page.waitForFunction(()=>!!window.DesignDialogs);};
 try{
  for(const [topic,variant]of Object.entries(expected))for(const width of [1440,500])for(const theme of ['dark','light']){
   await page.setViewportSize({width,height:1100});await go(topic);await page.evaluate(t=>document.documentElement.dataset.theme=t,theme);
   assert.equal(await page.locator('body').getAttribute('data-variant'),variant);assert(await page.locator('#variant-nav').isHidden());assert.match(page.url(),/approved=1/);
   const bounds=await page.evaluate(()=>({viewport:innerWidth,scroll:document.documentElement.scrollWidth,dialogs:[...document.querySelectorAll('dialog[open]')].map(d=>({width:d.clientWidth,scroll:d.scrollWidth}))}));assert(bounds.scroll<=bounds.viewport+1);for(const d of bounds.dialogs)assert(d.scroll<=d.width+2);
   if(topic==='protocol')assert(await page.locator('.log-catalog').isVisible());if(topic==='simulation')assert(await page.locator('.scene-layout').isVisible());if(topic==='analysis')assert(await page.locator('.report-layout').isVisible());if(topic==='files')assert(await page.locator('.browser-classic').isVisible());if(topic==='project'){assert(await page.locator('.wizard-steps').isVisible());assert.equal(await page.locator('[data-dialog-variant]').count(),0);}
   report.layouts.push({topic,variant,width,theme});
  }
  await page.setViewportSize({width:1440,height:1100});await go('protocol');await page.locator('#design-stage').screenshot({path:path.join(out,'approved-protocol-c.png')});
  for(const topic of ['simulation','analysis','runs','files','empty']){await page.locator(`[data-topic=${topic}]`).click();assert.equal(await page.locator('body').getAttribute('data-variant'),expected[topic]);assert.match(page.url(),/approved=1/);}
  await page.reload();assert.equal(await page.locator('body').getAttribute('data-topic'),'empty');assert.equal(await page.locator('body').getAttribute('data-approved'),'true');
  report.checks.push('28 selected topic/theme/width layouts, contradictory variant URL ignored, selected geometry, preserved URL and reload');
  for(const origin of ['protocol','simulation','empty']){
   await go(origin);await page.locator(origin==='empty'?'.empty-welcome [data-open-project]':'.app-actions [data-open-project]').click();assert(await page.locator('.wizard-steps').isVisible());assert.equal(await page.locator('[data-dialog-variant]').count(),0);
   await page.getByRole('button',{name:'Weiter →',exact:true}).click();await page.locator('#project-name').fill('Freigegebene Kombination');
   if(origin==='protocol')await page.locator('#project-dialog').screenshot({path:path.join(out,'approved-project-b.png')});
   await page.locator('[data-open-folder]').click();assert(await page.locator('#folder-dialog .browser-classic').isVisible());assert.equal(await page.locator('#folder-dialog .browser-columns').count(),0);
   if(origin==='protocol')await page.locator('#folder-dialog').screenshot({path:path.join(out,'approved-picker-a.png')});
   await page.locator('#picker-browser [data-file=empty]').click();await page.locator('#picker-confirm').click();await page.locator('#folder-dialog').waitFor({state:'hidden'});assert.equal(await page.locator('#project-name').inputValue(),'Freigegebene Kombination');assert.match(await page.locator('#project-location').inputValue(),/Neues Labor/);
   await page.locator('[data-project-back]').click();await page.getByRole('button',{name:'Weiter →',exact:true}).click();assert.equal(await page.locator('#project-name').inputValue(),'Freigegebene Kombination');
   await page.locator('#project-form button[type=submit]').click();await page.locator('#project-form button[type=submit]').click();assert.match(await page.locator('.project-success').innerText(),/keine Dateien oder Ordner/);await page.keyboard.press('Escape');
   assert.equal(await page.locator('body').getAttribute('data-variant'),expected[origin]);
  }
  report.checks.push('Protocol C / scene A / empty → project B → explorer A, folder choice, preserved inputs/back/submit and unchanged parent layout');
  await go('protocol');await page.locator('#reference-mode').click();await page.locator('[data-variant=b]').click();assert.equal(await page.locator('body').getAttribute('data-variant'),'b');await page.locator('#reference-mode').click();assert.equal(await page.locator('body').getAttribute('data-variant'),'c');
  assert(await page.evaluate(()=>Object.isFrozen(PhysimDesignSelection)&&Object.isFrozen(PhysimDesignSelection.variants)));assert.deepEqual(await page.evaluate(()=>PhysimDesignSelection.variants),{protocol:'c',simulation:'a',analysis:'a',runs:'a',project:'b',files:'a'});
  await page.goto(base+'?approved=1&topic=empty&standalone=1');await page.locator('.empty-workspace').waitFor();assert.match(page.url(),/approved=1/);assert.match(page.url(),/standalone=1/);assert.equal(await page.locator('.empty-workspace .scene-mass').count(),0);
  report.checks.push('Historical comparison cannot overwrite the frozen decision; approved standalone remains empty');
  assert.deepEqual(report.errors,[]);report.passed=true;
 }catch(e){await page.screenshot({path:path.join(out,'failure.png'),fullPage:true});throw e;}
 finally{fs.writeFileSync(path.join(out,'validation.json'),JSON.stringify(report,null,2)+'\n');await browser.close();}
 console.log(JSON.stringify({passed:report.passed,layouts:report.layouts.length,checks:report.checks,errors:report.errors}));
})().catch(e=>{console.error(e);process.exitCode=1;});
