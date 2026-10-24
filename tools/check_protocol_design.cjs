// Browser-only design acceptance: severity, provenance, source navigation and dock interaction.
const {chromium}=require('playwright'),assert=require('node:assert/strict'),fs=require('node:fs'),path=require('node:path'),{pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),out=path.join(root,'build/protocol-review');fs.mkdirSync(out,{recursive:true});
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
 const report={date:new Date().toISOString(),platform:process.platform,browser:await browser.version(),checks:[],layouts:[],errors:[]};
 const page=await browser.newPage({viewport:{width:1440,height:1000},acceptDownloads:true});page.setDefaultTimeout(15000);page.on('pageerror',e=>report.errors.push(e.message));
 const url=pathToFileURL(path.join(root,'docs/visuals/unified-workspace/index.html')).href;
 const rows=()=>page.locator('.protocol-row');
 try{
  await page.goto(url+'?example=protocol');const dev=page.frameLocator('#frame-develop');await dev.locator('body[data-unified]').waitFor();
  assert.equal(await rows().count(),8);assert.match(await page.locator('#protocol-notice').innerText(),/keine aktuellen Fehler/);
  const source=await dev.locator('#editor').inputValue();await page.locator('.source-action').click();assert.equal(await dev.locator('#editor').inputValue(),source);assert.equal(await dev.locator('#editor').evaluate(e=>e.value.slice(e.selectionStart,e.selectionEnd)),'gravity_m_s2');
  await page.locator('[data-log-filter=warning]').focus();await page.keyboard.press('Enter');assert.equal(await rows().count(),3);assert.match(await page.locator('#protocol-detail').innerText(),/verhindert allein keinen/);
  await page.locator('#protocol-origin').selectOption('Simulation');assert.equal(await rows().count(),2);
  await page.locator('#protocol-query').fill('908');assert.equal(await rows().count(),1);assert.match(await page.locator('#protocol-detail').innerText(),/nicht alle Meldungen/);
  const download=page.waitForEvent('download');await page.locator('#protocol-download').click();const file=await download;const bytes=fs.readFileSync(await file.path(),'utf8');assert.match(bytes,/Logger suppressed 908 messages/);assert(!bytes.includes('gravity_m_s'));assert.match(bytes,/Designbeispiele/);
  await page.locator('#protocol-query').fill('nichtvorhanden');assert.equal(await rows().count(),0);assert(await page.locator('#protocol-download').isDisabled());assert.match(await page.locator('#protocol-list').innerText(),/Keine Meldung/);
  await page.locator('#protocol-examples').click();await page.locator('[data-record=ex-debug]').click();await page.locator('#protocol-detail summary').click();assert.match(await page.locator('#protocol-detail pre').innerText(),/Reset "Pendelstudie"/);
  // Stub only this browser page; never write the operating system clipboard.
  await page.evaluate(()=>Object.defineProperty(navigator,'clipboard',{configurable:true,value:{writeText:async t=>{window.copiedProtocol=t;}}}));
  await page.getByRole('button',{name:'Meldung kopieren',exact:true}).click();assert.match(await page.evaluate(()=>window.copiedProtocol),/Debug/);assert.match(await page.locator('#protocol-feedback').innerText(),/kopiert/);
  await page.evaluate(()=>Object.defineProperty(navigator,'clipboard',{configurable:true,value:{writeText:async()=>{throw new Error('denied');}}}));await page.getByRole('button',{name:'Meldung kopieren',exact:true}).click();assert.match(await page.locator('#protocol-feedback').innerText(),/nicht verfügbar/);
  report.checks.push('Eight labelled independent examples; severity + origin + full-text intersection, empty state, filtered real TXT download, Unicode/raw detail, clipboard success/denial via local stub, source jump preserves source');
  const handle=page.locator('#protocol-resize'),value=()=>handle.getAttribute('aria-valuenow').then(Number);
  await handle.focus();await page.keyboard.press('Enter');assert.equal(await value(),340);await page.keyboard.press('ArrowUp');assert.equal(await value(),350);await page.keyboard.press('Shift+ArrowUp');assert.equal(await value(),390);await page.keyboard.press('Home');assert.equal(await value(),150);await page.keyboard.press('End');assert.equal(await value(),500);await page.keyboard.press('Enter');
  const box=await handle.boundingBox();await page.mouse.move(box.x+box.width/2,box.y+4);await page.mouse.down();await page.mouse.move(box.x+box.width/2,box.y-55,{steps:8});assert((await value())>380);await page.keyboard.press('Escape');await page.mouse.up();assert.equal(await value(),340);
  await page.locator('#protocol-close').click();assert(await page.locator('#protocol-panel').isHidden());await dev.locator('#diagnostics').waitFor({state:'visible'});await page.locator('#protocol-toggle').click();await dev.locator('#diagnostics').waitFor({state:'hidden'});
  await page.locator('#tab-simulate').click();const sim=page.frameLocator('#frame-simulate');await sim.locator('#events-group').evaluate(e=>e.open=true);await page.locator('#protocol-close').click();await sim.locator('#open-log').click();await page.locator('#protocol-panel').waitFor({state:'visible'});assert(await sim.locator('#inspector-log').isHidden());
  report.checks.push('Shared splitter keyboard limits/reset, pointer + Escape rollback; close/restore legacy log; inspector opens same shared dock');
  for(const theme of ['dark','light']){
   if(await page.locator('html').getAttribute('data-theme')!==theme)await page.locator('#theme').click();
   for(const area of ['develop','simulate','analyze']){
    await page.locator('#tab-'+area).click();
    for(const width of [1440,760,500]){
     await page.setViewportSize({width,height:1000});await page.locator('#protocol-examples').click();
     if(width<=900){await page.locator('[data-record=ex-runtime]').click();assert(await page.locator('#protocol-detail').isVisible());await page.locator('.protocol-back').click();assert(await rows().first().isVisible());}
     await page.waitForTimeout(80);const bounds=await page.locator('#protocol-panel').boundingBox();assert(bounds.x>=0&&bounds.x+bounds.width<=width+1&&bounds.y+bounds.height<=1000);
     assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
     await page.screenshot({path:path.join(out,`${area}-${theme}-${width}.png`)});report.layouts.push({theme,area,width,height:1000});
    }
   }
  }
  // 200% text on the new component, a 2x raster context, short viewport and forced colors.
  await page.setViewportSize({width:760,height:740});await page.addStyleTag({content:'#protocol-panel{font-size:24px}#protocol-panel :is(button,input,select,strong,small,p,h2,h3,dt,dd,summary,pre,time,.protocol-kicker,.protocol-summary,.protocol-foot,.protocol-notice){font-size:24px!important}'});await page.locator('#protocol-examples').click();await page.locator('[data-record=ex-runtime]').click();await page.locator('.protocol-back').click();await page.locator('[data-record=ex-overflow]').click();await page.locator('#protocol-detail summary').click();await page.locator('#protocol-download').scrollIntoViewIfNeeded();assert((await page.locator('#protocol-download').boundingBox()).y<740);assert(await page.locator('#protocol-panel').evaluate(e=>e.classList.contains('protocol-scroll-all')));await page.locator('#protocol-detail h3').scrollIntoViewIfNeeded();await page.screenshot({path:path.join(out,'large-text.png')});
  await page.emulateMedia({forcedColors:'active',reducedMotion:'reduce'});await handle.focus();await handle.press('ArrowUp');assert(await handle.evaluate(e=>e===document.activeElement));
  const dense=await browser.newPage({viewport:{width:1440,height:1000},deviceScaleFactor:2});await dense.goto(url+'?example=protocol');assert.equal(await dense.locator('.protocol-row').count(),8);await dense.screenshot({path:path.join(out,'density-2x.png')});await dense.close();
  report.checks.push('18 workspace/theme/width layouts with compact master/detail keyboard return, plus 200% component text, short window, forced colors/reduced motion and 2x browser density');
  await page.setViewportSize({width:1440,height:1000});await page.emulateMedia({forcedColors:'none',reducedMotion:'no-preference'});await page.goto(url);await dev.locator('body[data-unified]').waitFor();assert(await page.locator('#protocol-panel').isHidden());
  await dev.locator('#editor').fill(source.replace('-gravity_m_s2 /','-gravity_m_s /'));await dev.locator('#editor').press('F5');await page.waitForFunction(()=>document.querySelector('#protocol-detail h3')?.textContent.includes('fehlgeschlagen'));
  assert(await page.locator('#protocol-panel').isVisible());await page.locator('.source-action').click();assert.equal(await dev.locator('#editor').evaluate(e=>e.value.slice(e.selectionStart,e.selectionEnd)),'gravity_m_s');
  const count=await rows().count();await page.locator('#tab-simulate').click();await page.locator('#tab-develop').click();assert.equal(await rows().count(),count);
  await dev.locator('#editor').fill(source);await dev.locator('#editor').press('F5');await page.waitForFunction(()=>document.querySelector('#workflow-state').textContent.includes('Modell bereit'));assert.match(await page.locator('#protocol-list').innerText(),/erfolgreich/);
  await page.locator('#protocol-close').click();await page.locator('#tab-simulate').click();await sim.locator('#step-button').click();await sim.locator('#shared-export').click();await page.locator('#protocol-toggle').click();assert.match(await page.locator('#protocol-list').innerText(),/Messlauf 001 übernommen/);
  report.checks.push('Current session: real demo build failure auto-opens with honest source location, no duplicate on navigation, successful recovery, actual run transfer creates message');
  assert.deepEqual(report.errors,[]);report.passed=true;
 }finally{fs.writeFileSync(path.join(out,'validation.json'),JSON.stringify(report,null,2)+'\n');await browser.close();}
 console.log(JSON.stringify(report,null,2));
})().catch(e=>{console.error(e);process.exitCode=1;});
