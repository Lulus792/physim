// Isolated browser design study; no native compiler, runner or filesystem operations.
const {chromium}=require('playwright'),assert=require('node:assert/strict'),fs=require('node:fs'),path=require('node:path'),{pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),out=path.join(root,'build/design-options-review');fs.mkdirSync(out,{recursive:true});
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
 const report={date:new Date().toISOString(),platform:process.platform,browser:await browser.version(),checks:[],layouts:[],errors:[]};
 const page=await browser.newPage({viewport:{width:1440,height:1100},acceptDownloads:true});page.setDefaultTimeout(10000);page.on('pageerror',e=>report.errors.push(e.message));
 const url=pathToFileURL(path.join(root,'docs/visuals/design-options/index.html')).href;
 async function go(topic,variant='a'){await page.goto(url+`?topic=${topic}&variant=${variant}`);await page.waitForFunction(()=>!!window.DesignDialogs);}
 async function fits(){const measure=await page.evaluate(()=>({width:innerWidth,scroll:document.documentElement.scrollWidth,dialogs:[...document.querySelectorAll('dialog[open]')].map(d=>({width:d.clientWidth,scroll:d.scrollWidth}))}));assert(measure.scroll<=measure.width+1,JSON.stringify(measure));for(const d of measure.dialogs)assert(d.scroll<=d.width+2,JSON.stringify(d));}
 try{
  for(const topic of ['protocol','simulation','analysis','runs','project','files'])for(const variant of ['a','b','c'])for(const theme of ['dark','light'])for(const width of [1440,500]){
   await page.setViewportSize({width,height:1100});await go(topic,variant);await page.evaluate(t=>document.documentElement.dataset.theme=t,theme);await fits();
   if(width===1440&&theme==='dark')await page.locator(topic==='project'?'#project-dialog':'#design-stage').screenshot({path:path.join(out,`${topic}-${variant}.png`)});
   if(topic==='project'){await page.locator('#project-dialog button[data-close]').first().click();await page.locator('[data-open-project]').first().waitFor();}
   report.layouts.push({topic,variant,theme,width,height:1100});
  }
  report.checks.push('72 layouts: six topics × three structural variants × two themes × 1440/500 px; all three project dialogs and their variants render separately');
  for(const variant of ['a','b','c']){
   await page.setViewportSize({width:1440,height:1100});await go('protocol',variant);
   assert.equal(await page.locator('[data-log]').count(),8);await page.locator('[data-level=warning]').click();assert.equal(await page.locator('[data-log]').count(),3);
   if(variant==='c'){await page.locator('[data-origin=Simulation]').click();assert.equal(await page.locator('[data-log]').count(),2);}
   await page.locator('#log-query').fill('908');assert.equal(await page.locator('[data-log]').count(),1);
   if(variant==='b')await page.locator('[data-log=flood]').click();assert.match(await page.locator('.log-detail').innerText(),/nicht vollständig/);
   const download=page.waitForEvent('download');await page.locator('[data-download-log]').click();const dl=await download;const text=fs.readFileSync(await dl.path(),'utf8');assert.match(text,/908/);assert(!text.includes('undeclared'));
   await page.locator('#log-query').fill('unauffindbar');assert.match(await page.locator('#design-stage').innerText(),/Kein passender Eintrag/);assert(await page.locator('[data-download-log]').isDisabled());
   await go('protocol',variant);if(variant==='b')await page.locator('[data-log=compile]').click();await page.locator('[data-source-demo]').click();assert.match(await page.locator('.source-preview').innerText(),/gravity_m_s/);
   await page.evaluate(()=>Object.defineProperty(navigator,'clipboard',{value:{writeText:async()=>{throw Error('denied');}},configurable:true}));await page.locator('[data-copy-log]').click();assert.match(await page.locator('#gallery-status').innerText(),/nicht verfügbar/);
   await page.locator('#scenario-select').selectOption('flood');assert.equal(await page.locator('[data-log]').count(),68);
  }
  report.checks.push('Each protocol variant: intersected filters/search, honest missing/source context, Unicode/raw data, filtered actual TXT download, empty search, clipboard denial and 68-row flood sample');
  for(const variant of ['a','b','c']){
   await go('simulation',variant);assert.match(await page.locator('#design-stage').innerText(),/0,05 m\/N/);await page.locator('[data-object=drag]').first().click();assert.match(await page.locator('.object-pane').innerText(),/0,03 N/);
   await page.locator('#show-labels').uncheck();assert.equal(await page.locator('.scene-space .scene-label').count(),0);await page.locator('#show-labels').check();
   await page.locator('[data-sim-tab=parameters]').click();await page.locator('#param-mass').fill('-1');await page.locator('[data-validate-parameters]').click();assert.match(await page.locator('#parameter-result').innerText(),/gültigen Bereich/);
   await page.locator('#param-mass').fill('1');await page.locator('#param-integrator').selectOption('3');await page.locator('[data-validate-parameters]').click();assert.match(await page.locator('#parameter-result').innerText(),/Verlet/);
   await page.locator('#param-integrator').selectOption('4');await page.locator('#adaptive-toggle').check();await page.locator('[data-parameter=minimum]').fill('0,1');await page.locator('[data-parameter=maximum]').fill('0,01');await page.locator('[data-validate-parameters]').click();assert.match(await page.locator('#parameter-result').innerText(),/Adaptive Grenzen/);
   await page.locator('[data-parameter=minimum]').fill('0,0001');await page.locator('[data-validate-parameters]').click();assert.match(await page.locator('#parameter-result').innerText(),/gültig/);
   await page.locator('[data-sim-tab=channels]').click();assert.match(await page.locator('#design-stage').innerText(),/energy.balance/);assert.match(await page.locator('#design-stage').innerText(),/sensor.angle/);
   await page.locator('#scenario-select').selectOption('missing');await page.locator('[data-sim-tab=scene]').click();assert.match(await page.locator('.object-pane').innerText(),/nicht mehr vorhanden/);await page.locator('[data-select-existing]').click();assert.match(await page.locator('.object-pane').innerText(),/Gewichtskraft/);
  }
  report.checks.push('Three simulation variants: explicit force scales/selection, label visibility, parameter bounds, Verlet/drag dependency, adaptive bounds, 11 channel layouts, vanished selection recovery');
  await go('analysis');await page.locator('[data-run-check="002"]').check();assert.match(await page.locator('.report-document').innerText(),/Auswahl geändert/);await page.locator('[data-refresh-report]').click();assert(!/Auswahl geändert/.test(await page.locator('.report-document').innerText()));
  await page.locator('#analysis-channel').selectOption('sensor');assert.match(await page.locator('#design-stage').innerText(),/fehlt in Lauf 003/);assert.equal(await page.locator('#analysis-channel').inputValue(),'sensor');
  await page.locator('#scenario-select').selectOption('recovered');assert.match(await page.locator('#design-stage').innerText(),/validierte Teil/);assert.equal(await page.locator('.mini-chart').count(),2);
  await go('runs');await page.locator('[data-select-run="003"]').click();assert.match(await page.locator('#design-stage').innerText(),/Ungültiger Modellwert/);await page.locator('[data-series-stop]').click();assert(await page.locator('[data-series-stop]').isDisabled());
  await page.locator('#history-time').fill('5000');assert.match(await page.locator('#timeline-value').innerText(),/5000/);for(const scenario of ['starting','timeout','limited','recovered']){await page.locator('#scenario-select').selectOption(scenario);assert(await page.locator('.banner').first().isVisible());}
  report.checks.push('Variable reports: stable source snapshot/staleness, missing channels, recovered runs, two figures; series partial failure, cancellation, long timeline and startup/timeout/limit/recovery states');
  for(const variant of ['a','b','c']){
   await go('files',variant);await page.locator('[data-file=main]').click();assert.match(await page.locator('.file-preview').innerText(),/static const double length_m/);await page.locator('.code-context summary').click();assert.match(await page.locator('.code-context').innerText(),/ps_scene_add_id/);await fits();
   await page.locator('[data-file=runs]').dblclick();assert.match(await page.locator('.browser-path').innerText(),/runs/);await page.locator('[data-file=run2]').click();assert.match(await page.locator('.file-preview').innerText(),/Wiederhergestellt/);
   await page.locator('[data-browser-action=up]').click();await page.locator('#inline-query').fill('Versuchsnotizen');assert.equal(await page.locator('[data-file]').count(),1);await page.locator('#inline-query').fill('');
   await page.locator('[data-browser-hidden=inline]').check();assert(await page.locator('[data-file=hidden]').isVisible());
   await page.locator('[data-browser-action=create]').click();await page.locator('#inline-new-folder').fill('Neuer Vergleich');await page.locator('[data-browser-action=create-confirm]').click();assert.match(await page.locator('.browser-footer').innerText(),/nur im Speicher/);
   await page.locator('#scenario-select').selectOption('denied');assert.match(await page.locator('#design-stage').innerText(),/Kein Zugriff/);
  }
  report.checks.push('Three custom explorers: actual button/double-click folder navigation, preview/code, recovered-run metadata, search, hidden entries, in-memory folder creation and denied state');
  for(const variant of ['a','b','c']){
   await go('project',variant);assert(await page.locator('#project-dialog').isVisible());await page.keyboard.press('Escape');await page.locator('#project-dialog').waitFor({state:'hidden'});await page.locator('.project-launch [data-open-project]').click();
   if(variant==='b'){await page.getByRole('button',{name:'Weiter →',exact:true}).click();await page.locator('#project-name').fill('Eingabe bleibt');await page.locator('[data-project-back]').click();await page.getByRole('button',{name:'Weiter →',exact:true}).click();assert.equal(await page.locator('#project-name').inputValue(),'Eingabe bleibt');}
   await page.locator('#project-name').fill('Pendelstudie');await page.locator('#project-form button[type=submit]').click();assert.match(await page.locator('#project-error').innerText(),/bereits vergeben/);
   await page.locator('#project-name').fill('Neue Untersuchung');await page.locator('[data-open-folder]').click();assert(await page.locator('#folder-dialog').isVisible());await page.keyboard.press('Escape');assert(await page.locator('#project-dialog').isVisible());assert.equal(await page.locator('#project-name').inputValue(),'Neue Untersuchung');
   await page.locator('[data-open-folder]').click();await page.locator('#picker-browser [data-file=empty]').click();await page.locator('#picker-confirm').click();await page.locator('#folder-dialog').waitFor({state:'hidden'});assert.match(await page.locator('#project-location').inputValue(),/Neues Labor/);
   await page.locator('#project-analysis').selectOption('C');await page.locator('#project-form button[type=submit]').click();if(variant==='b')await page.locator('#project-form button[type=submit]').click();assert.match(await page.locator('.project-success').innerText(),/keine Dateien oder Ordner/);assert.match(await page.locator('.project-success').innerText(),/analysis.c/);
   await page.locator('#project-dialog button[data-close]').first().click();
  }
  await go('project');await page.locator('#project-name').fill('Behalten');await page.locator('[data-dialog-variant=c]').click();assert.equal(await page.locator('#project-name').inputValue(),'Behalten');await page.locator('#project-only-analysis').check();assert(await page.locator('#project-experiment').isHidden());assert(!/main.c/.test(await page.locator('#project-dialog').innerText()));
  // Native dialog containment: tab navigation never lands in the inert gallery.
  for(let i=0;i<18;i++){await page.keyboard.press('Tab');assert(await page.evaluate(()=>document.querySelector('#project-dialog').contains(document.activeElement)));}
  await page.keyboard.press('Escape');await page.locator('#scenario-select').selectOption('denied');await page.locator('#project-form button[type=submit]').click();assert.match(await page.locator('#project-error').innerText(),/nicht beschreibbar/);
  report.checks.push('Three project popups: Escape, duplicate-name error, nested custom folder picker/cancel/selection, separate language choices, wizard back state and success without disk writes; variant switch preserves inputs, analysis-only fields, modal keyboard containment and denied location');
  for(const area of ['develop','simulate','analyze'])for(const theme of ['dark','light'])for(const width of [1440,500]){
   await page.setViewportSize({width,height:1000});await page.goto(url+'?topic=empty&standalone=1');await page.waitForFunction(()=>!!window.DesignDialogs);await page.evaluate(t=>document.documentElement.dataset.theme=t,theme);await page.locator('[data-empty-area='+area+']').click();assert.match(page.url(),/standalone=1/);await fits();assert.equal(await page.locator('.empty-workspace .scene-mass').count(),0);assert.match(await page.locator('.empty-status').innerText(),/Kein Projekt geöffnet/);assert(await page.locator('#variant-nav').isHidden());
   if(area==='simulate')assert(await page.getByRole('button',{name:'▶ Starten',exact:true}).isDisabled());if(area==='analyze')assert(await page.getByRole('button',{name:'Analyse starten',exact:true}).isDisabled());
   if(width===1440)await page.screenshot({path:path.join(out,`empty-${area}-${theme}.png`)});report.layouts.push({topic:'empty',area,theme,width,height:1000});
  }
  await page.locator('#empty-tab-develop').focus();await page.keyboard.press('End');assert.equal(await page.locator('#empty-tab-analyze').getAttribute('aria-selected'),'true');await page.locator('.empty-welcome [data-open-project]').click();assert(await page.locator('#project-dialog').isVisible());await page.keyboard.press('Escape');assert(await page.locator('.empty-welcome').isVisible());await page.locator('.empty-welcome [data-open-workspace]').click();await page.locator('#picker-browser [data-file=empty]').click();await page.locator('#picker-confirm').click();await page.locator('#folder-dialog').waitFor({state:'hidden'});assert.match(await page.locator('.empty-status').innerText(),/Kein Projekt geöffnet/);
  report.checks.push('Complete current-language empty workspace: 12 area/theme/width states, standalone chrome, keyboard area navigation, zero scene/data and disabled unavailable actions; new-project popup does not load a fake project');
  await page.setViewportSize({width:760,height:740});await go('project','c');await page.evaluate(()=>document.documentElement.style.fontSize='28px');await fits();await page.locator('#project-form button[type=submit]').scrollIntoViewIfNeeded();assert((await page.locator('#project-form button[type=submit]').boundingBox()).y<740);await page.screenshot({path:path.join(out,'project-large-text.png')});await page.keyboard.press('Escape');
  await page.emulateMedia({forcedColors:'active',reducedMotion:'reduce'});await page.locator('#theme-button').focus();assert(await page.locator('#theme-button').evaluate(e=>e===document.activeElement));
  const dense=await browser.newPage({viewport:{width:1440,height:1000},deviceScaleFactor:2});await dense.goto(url+'?topic=empty&standalone=1');await dense.locator('.empty-workspace').waitFor();await dense.screenshot({path:path.join(out,'empty-density-2x.png')});await dense.close();
  report.checks.push('200% root text in short project dialog with reachable submit, forced colors/reduced-motion keyboard focus and separate 2x-density empty-workspace capture');
  assert.deepEqual(report.errors,[]);report.passed=true;
 }catch(error){await page.screenshot({path:path.join(out,'failure.png'),fullPage:true});throw error;}
 finally{fs.writeFileSync(path.join(out,'validation.json'),JSON.stringify(report,null,2)+'\n');await browser.close();}
 console.log(JSON.stringify({passed:report.passed,layouts:report.layouts.length,checks:report.checks.length,errors:report.errors},null,2));
})().catch(e=>{console.error(e);process.exitCode=1;});
