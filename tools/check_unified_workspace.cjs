// Cross-workspace user tasks, numeric provenance and layout checks. Browser evidence only.
const {chromium}=require('playwright'),assert=require('node:assert/strict');
const fs=require('node:fs'),path=require('node:path'),{pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),out=path.join(root,'build/unified-workspace-review');fs.mkdirSync(out,{recursive:true});
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
 const report={date:new Date().toISOString(),platform:process.platform,browser:await browser.version(),checks:[],layouts:[],errors:[]};
 try{
  const page=await browser.newPage({viewport:{width:1440,height:1000}});page.setDefaultTimeout(10000);
  page.on('pageerror',e=>report.errors.push(e.message));
  await page.goto(pathToFileURL(path.join(root,'docs/visuals/unified-workspace/index.html')).href);
  const frame=area=>page.frameLocator('#frame-'+area),dev=frame('develop'),sim=frame('simulate'),analysis=frame('analyze');
  await dev.locator('body[data-unified=develop]').waitFor({state:'attached'});await sim.locator('body[data-unified=simulate]').waitFor({state:'attached'});await analysis.locator('body[data-unified=analyze]').waitFor({state:'attached'});
  const go=async area=>{await page.locator('#tab-'+area).click();await page.waitForFunction(a=>document.querySelector('#tab-'+a).getAttribute('aria-selected')==='true',area);};
  await go('analyze');assert(await analysis.locator('#state-panel').isVisible());assert.equal(await analysis.locator('#run-list input').count(),0);
  await go('develop');
  await dev.locator('#search-close').click();
  const code=await dev.locator('#editor').inputValue();
  const modified=code.replace('length_m = 1.5','length_m = 2.0').replace('initial_angle_rad = 0.45','initial_angle_rad = 0.3');
  await dev.locator('#editor').fill(modified);await page.waitForFunction(()=>document.querySelector('#dirty-mark').textContent==='●');
  await dev.locator('#editor').press('Control+s');await page.waitForFunction(()=>document.querySelector('#dirty-mark').textContent==='');
  await dev.locator('#editor').press('F5');await dev.locator('#build-status').filter({hasText:'Build erfolgreich'}).waitFor({state:'attached'});
  await page.waitForFunction(()=>document.querySelector('#workflow-state').textContent.includes('Modell bereit'));
  await go('simulate');assert.equal(await sim.locator('#model-length').inputValue(),'2,00');
  assert.equal(await sim.locator('#tree-toggle').getAttribute('aria-expanded'),'false');assert.equal(await sim.locator('#curves-toggle').getAttribute('aria-checked'),'false');
  await sim.locator('#speed').selectOption('16');await sim.locator('#run-button').click();
  await sim.locator('.app-window[data-state=ready]').waitFor();
  await sim.locator('#shared-export').click();await page.waitForFunction(()=>document.querySelector('#tab-analyze').getAttribute('aria-selected')==='true');
  await analysis.locator('#run-list input').first().waitFor();assert.equal(await analysis.locator('#run-list input').count(),1);
  assert.match(await analysis.locator('#run-details').innerText(),/2,00 m/);assert.match(await analysis.locator('#run-details').innerText(),/0,300 rad/);
  const downloadEvent=page.waitForEvent('download');await analysis.locator('#export-csv').click();const download=await downloadEvent;
  const csv=fs.readFileSync(await download.path(),'utf8'),rows=csv.trim().split(/\r?\n/);assert.equal(rows[0],'run,time_s,theta_rad');
  const first=rows[1].split(',').map(Number);assert(Math.abs(first[2]-.3)<1e-12,'Simulation initial value not exported');
  const last=rows.at(-1).split(',').map(Number);assert.equal(last[1],6);assert.equal(rows.length,302);assert(Math.abs(last[2]-.3*Math.cos(Math.sqrt(9.80665/2)*last[1]))<1e-12);
  report.checks.push('Edited/saved source → build → simulation → exact observed samples → CSV with matching model');
  await analysis.locator('#analyze').click();await analysis.locator('#report-panel').getByText('Nachvollziehbare Herkunft',{exact:true}).waitFor();
  assert.match(await analysis.locator('#report-panel').innerText(),/Lauf 001/);
  await analysis.locator('#code-open').click();await page.waitForFunction(()=>document.querySelector('#tab-develop').getAttribute('aria-selected')==='true');
  assert.equal(await dev.locator('#active-file').innerText(),'analysis.c');
  await dev.locator('.file-tabs [data-file="main.c"]').click();assert.equal(await dev.locator('#editor').inputValue(),modified);
  await go('simulate');assert.equal(await sim.locator('#run-status').innerText(),'○ Bereit');
  await sim.locator('#tree-toggle').click();await sim.locator('#curves-toggle').click();
  await go('develop');await go('simulate');assert.equal(await sim.locator('#tree-toggle').getAttribute('aria-expanded'),'true');assert.equal(await sim.locator('#curves-toggle').getAttribute('aria-checked'),'true');
  await sim.locator('#source-model-group').evaluate(el=>el.open=true);await sim.locator('#model-length').fill('1,20');
  await sim.locator('#compile-model').click();await page.waitForFunction(()=>document.querySelector('#workflow-state').textContent.includes('Vorschau-Build'));
  await page.waitForFunction(()=>document.querySelector('#workflow-state').textContent.includes('Modell bereit'));
  await sim.locator('#run-button').click();await sim.locator('.app-window[data-state=ready]').waitFor();await sim.locator('#shared-export').click();
  await page.waitForFunction(()=>document.querySelector('#run-count').textContent==='2');
  await analysis.locator('input[value="001"]').check();assert.equal(await analysis.locator('#chart .curve').count(),2);
  assert.equal(await analysis.locator('#statistics tr').count(),2);assert.match(await analysis.locator('#run-details').innerText(),/2,00 m/);
  await analysis.locator('#tab-report').click();assert.match(await analysis.locator('#report-panel').innerText(),/Laufauswahl wurde geändert/);
  await analysis.locator('#tab-data').click();await analysis.locator('#unit').selectOption('deg');assert(!/NaN|Infinity/.test(await analysis.locator('#statistics').innerText()));
  report.checks.push('Analysis code opens editor; pane/search state retained; inspector uses same source; historical snapshots and stale report provenance preserved');
  // Error handling leaves prior measured runs usable.
  await go('develop');await dev.locator('#editor').fill(modified.replace('length_m = 2.0','length_m = -2.0'));await dev.locator('#editor').press('F5');
  await page.waitForFunction(()=>document.querySelector('#workflow-state').textContent.includes('Build prüfen'));
  await go('analyze');assert.equal(await analysis.locator('#run-list input').count(),2);
  await go('develop');await dev.locator('#editor').fill(modified);await dev.locator('#editor').press('Control+s');await dev.locator('#editor').press('F5');await page.waitForFunction(()=>document.querySelector('#workflow-state').textContent.includes('Modell bereit'));
  await dev.locator('#editor').press('Control+f');
  await go('simulate');await sim.locator('#tree-toggle').click();await sim.locator('#curves-toggle').click();
  // One palette, no duplicate chrome, no horizontal overflow at representative sizes.
  for(const width of [1440,1080,760,500])for(const theme of ['dark','light']){
   await page.setViewportSize({width,height:1000});
   if(await page.locator('html').getAttribute('data-theme')!==theme)await page.locator('#theme').click();
   for(const area of ['develop','simulate','analyze']){
    await go(area);const f=frame(area);await f.locator(`html[data-theme=${theme}]`).waitFor();
    assert(await f.locator('.app-window > .workspace-tabs').isHidden());
    const content=await f.locator('body').evaluate(()=>({scroll:document.documentElement.scrollWidth,width:innerWidth}));assert(content.scroll<=content.width+1,`${area} overflow at ${width}: ${JSON.stringify(content)}`);
    assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth+1));
    const text=await f.locator('body').evaluate(el=>getComputedStyle(el).color),shell=await page.locator('body').evaluate(el=>getComputedStyle(el).color);assert.equal(text,shell);
    report.layouts.push({area,width,height:1000,theme});
    if(width===1440){await page.screenshot({path:path.join(out,area+'-'+theme+'.png')});if(process.argv.includes('--export')){fs.copyFileSync(path.join(out,area+'-'+theme+'.png'),path.join(root,'docs/visuals/unified-workspace',area+'-'+theme+'.png'));}}
   }
  }
  await page.setViewportSize({width:1440,height:1000});await go('develop');await page.locator('#tab-develop').focus();await page.keyboard.press('ArrowRight');assert.equal(await page.locator('#tab-simulate').getAttribute('aria-selected'),'true');await page.keyboard.press('End');assert.equal(await page.locator('#tab-analyze').getAttribute('aria-selected'),'true');
  await go('simulate');
  for(let mask=0;mask<8;mask++){
   for(const [i,id] of ['tree-toggle','inspector-toggle','curves-toggle'].entries()){
    const attribute=id==='curves-toggle'?'aria-checked':'aria-expanded',open=(mask&(1<<i))!==0;
    if((await sim.locator('#'+id).getAttribute(attribute))!==String(open))await sim.locator('#'+id).click();
   }
   assert((await sim.locator('#scene').boundingBox()).width>200);
  }
  await go('analyze');
  for(const side of ['left','right','left','right']){await analysis.locator('#toggle-'+side).click();assert((await analysis.locator('.analysis-pane').boundingBox()).width>200);}
  await go('simulate');await sim.locator('#reset-button').click();await sim.locator('#step-button').click();await sim.locator('#shared-export').click();
  await page.waitForFunction(()=>document.querySelector('#run-count').textContent==='3');
  const ticks=await analysis.locator('#chart text[y="296"]').allTextContents();assert.equal(new Set(ticks).size,6,'Short runs need distinct time-axis ticks');
  for(let i=0;i<3;i++)await analysis.locator('#zoom-in').click();assert.equal(await analysis.locator('#chart .curve').count(),1);assert(!/NaN|Infinity/.test(await analysis.locator('#chart').innerHTML()));
  const dense=await browser.newPage({viewport:{width:1080,height:1000},deviceScaleFactor:2});
  await dense.goto(page.url());await dense.frameLocator('#frame-develop').locator('body[data-unified]').waitFor({state:'attached'});
  await dense.locator('#tab-simulate').click();await dense.frameLocator('#frame-simulate').locator('#step-button').click();await dense.frameLocator('#frame-simulate').locator('#shared-export').click();
  await dense.frameLocator('#frame-analyze').locator('#run-list input').waitFor();assert.equal(await dense.locator('#run-count').innerText(),'1','Initial single step must export the compiled model');
  assert(await dense.evaluate(()=>document.documentElement.scrollWidth<=innerWidth+1));await dense.close();
  await page.emulateMedia({forcedColors:'active',reducedMotion:'reduce'});await page.getByRole('button',{name:'Hilfe',exact:true}).click();await page.locator('#opaque').check();await page.locator('#help-close').click();
  await go('develop');assert(await dev.locator('#reduce-transparency').isChecked());await page.emulateMedia({forcedColors:'none'});
  await page.getByRole('button',{name:'Hilfe',exact:true}).click();assert(await page.locator('#help').isVisible());await page.keyboard.press('Escape');assert(await page.getByRole('button',{name:'Hilfe',exact:true}).evaluate(el=>el===document.activeElement));
  await page.getByRole('button',{name:'Datei',exact:true}).focus();await page.keyboard.press('ArrowDown');await page.keyboard.press('Escape');assert(await page.getByRole('button',{name:'Datei',exact:true}).evaluate(el=>el===document.activeElement));
  report.checks.push('24 layouts, shared palette, 8 simulation/4 analysis panel states, short-run zoom/ticks, 2x density, forced colors, reduced motion/transparency, keyboard tabs/menus, dialog focus return');
  assert.deepEqual(report.errors,[]);report.passed=true;
 }finally{fs.writeFileSync(path.join(out,'results.json'),JSON.stringify(report,null,2));await browser.close();}
 console.log(JSON.stringify(report,null,2));
})().catch(e=>{console.error(e);process.exitCode=1;});
