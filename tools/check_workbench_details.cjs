// Compare workbench chrome alternatives, preserving the original studies as a baseline.
const {chromium}=require('playwright');
const sharp=require('sharp');
const assert=require('node:assert/strict');
const fs=require('node:fs');
const path=require('node:path');
const {pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),design=path.join(root,'docs/visuals/develop-designs');
const output=path.join(root,'build/workbench-detail-review');
const exporting=process.argv.includes('--export');fs.mkdirSync(output,{recursive:true});
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
 try {
  const page=await browser.newPage({viewport:{width:1440,height:1100}});page.setDefaultTimeout(8000);
  const errors=[];page.on('pageerror',e=>errors.push(e.message));
  const report={browser:await browser.version(),platform:process.platform,layouts:[],combinations:0,space:[],interactions:[],errors};
  const open=name=>page.goto(pathToFileURL(path.join(design,name||'index.html')).href);
  const geometry=()=>page.evaluate(()=>{
   const app=document.querySelector('.app-window').getBoundingClientRect(),code=document.querySelector('#code-area').getBoundingClientRect();
   return {document:document.documentElement.scrollWidth,viewport:innerWidth,editor:document.querySelector('#editor').getBoundingClientRect().width,codeTop:code.top-app.top};
  });
  await open('original.html');const baseline=await geometry();
  await open();
  assert.equal(await page.locator('#detail-controls').inputValue(),'headers');
  assert.equal(await page.locator('#detail-tabs').inputValue(),'pills');
  assert.equal(await page.locator('#detail-tree').inputValue(),'guides');
  assert.equal(await page.locator('#selected-workbench').getAttribute('aria-pressed'),'true');
  await page.locator('[data-detail="edges"]').click();
  await page.locator('#selected-workbench').click();
  assert.equal(await page.locator('.app-window').getAttribute('data-controls'),'headers');
  assert.equal(await page.locator('.app-window').getAttribute('data-tabs'),'pills');
  assert.equal(await page.locator('.app-window').getAttribute('data-tree'),'guides');
  for(const theme of ['dark','light']){
   await page.locator(`button[data-theme="${theme}"]`).click();
   await page.locator('.app-window').screenshot({path:path.join(exporting?design:output,`selected-workbench-${theme}.png`)});
  }
  report.interactions.push('selected workbench defaults and restore action: panel headers, compact tabs, path guides');
  for(const width of [1440,1080,760,500])for(const theme of ['dark','light'])for(const preset of ['edges','headers','dock']){
   await page.setViewportSize({width,height:1100});
   await page.locator(`button[data-theme="${theme}"]`).click();await page.locator(`[data-detail="${preset}"]`).click();
   const g=await geometry();assert(g.document<=width+1,`Overflow ${width}/${theme}/${preset}: ${JSON.stringify(g)}`);assert(g.editor>=200,`Editor too narrow ${JSON.stringify(g)}`);
   assert.equal(await page.locator('.project-bar').isVisible(),false);assert.equal(await page.locator('.panel-controls').isVisible(),false);
   for(const id of ['toggle-tree','toggle-api','search-open','save'])assert(await page.locator('#'+id).isVisible(),`Hidden action ${id}/${preset}`);
   for(const [control,panel]of[['tree','explorer'],['api','inspector']]){
    await page.locator('#toggle-'+control).click();assert.equal(await page.locator('#'+panel).isVisible(),false);
    assert(await page.locator('#toggle-'+control).isVisible(),'Reopen action disappeared');
    assert(await page.locator('#toggle-'+control).evaluate(el=>el===document.activeElement),'Focus was lost when control moved');
    await page.locator('#toggle-'+control).press('Enter');assert(await page.locator('#'+panel).isVisible());
   }
   const folders=await page.locator('.file-tree details>summary>span').allTextContents();assert.deepEqual(folders,['assets','runs','build']);
   const tabWidths=await page.locator('.workspace-tabs>span').evaluateAll(els=>els.map(el=>el.getBoundingClientRect().width));
   assert(Math.max(...tabWidths)-Math.min(...tabWidths)<1,'Workspace tabs no longer equal');
   if(width===1440&&theme==='dark'){const saved=baseline.codeTop-g.codeTop;assert(saved>=100,`Too little vertical space saved: ${saved}`);report.space.push({preset,old:baseline.codeTop,new:g.codeTop,saved});}
   report.layouts.push({width,theme,preset,...g});
  }
  await page.setViewportSize({width:1440,height:1100});
  for(const theme of ['dark','light'])for(const controls of ['edges','headers','dock'])for(const tabs of ['line','cards','pills'])for(const tree of ['quiet','guides','surfaces']){
   await page.locator(`button[data-theme="${theme}"]`).click();
   await page.locator('#detail-controls').selectOption(controls);await page.locator('#detail-tabs').selectOption(tabs);await page.locator('#detail-tree').selectOption(tree);
   const g=await geometry();assert(g.document<=1441);assert(g.editor>400);
   report.combinations++;
  }
  await page.locator('[data-detail="headers"]').click();
  const original=await page.locator('#editor').inputValue();await page.locator('#editor').fill(original+'\n// Detailprüfung');
  for(const preset of ['edges','dock','headers'])await page.locator(`[data-detail="${preset}"]`).click();
  assert.match(await page.locator('#editor').inputValue(),/Detailprüfung/);
  await page.locator('#search-open').click();await page.locator('#query').fill('length_m');await page.locator('#find-next').click();
  assert.equal(await page.locator('#editor').evaluate(el=>el.value.slice(el.selectionStart,el.selectionEnd)),'length_m');
  await page.keyboard.press('Escape');assert(await page.locator('#search-open').evaluate(el=>el===document.activeElement));
  await page.locator('#save').click();assert.match(await page.locator('#save-status').innerText(),/Alle Änderungen/);
  await page.keyboard.press('F5');await page.waitForFunction(()=>document.querySelector('#build-status').textContent.includes('erfolgreich'));
  assert(await page.locator('#build-status').isVisible());
  await page.locator('#scenario').selectOption('error');await page.locator('#diagnostic-link').click();
  assert.equal(await page.locator('#editor').evaluate(el=>el.value.slice(el.selectionStart,el.selectionEnd)),'gravity_m_s');
  await page.locator('#scenario').selectOption('empty');assert.equal(await page.locator('.workspace-shell').isVisible(),false);
  await page.locator('#scenario').selectOption('ready');
  await page.locator('.file-tree details').first().locator('summary').click();assert(await page.locator('.tree-note').first().isVisible());
  report.interactions.push('all panel triggers remain visible, keyboard-operable and focused after relocation','folder order and equal workspace tabs preserved','source edits survive presets','search and save from tab row','build and diagnostics after footer relocation','empty state hides edge rails','folder disclosure');
  await page.emulateMedia({forcedColors:'active',reducedMotion:'reduce'});assert.equal(await page.locator('#highlight').evaluate(el=>getComputedStyle(el).visibility),'hidden');
  await page.emulateMedia({forcedColors:'none'});
  for(const scale of [1.5,2]){
   const ctx=await browser.newContext({viewport:{width:1440,height:1100},deviceScaleFactor:scale});const scaled=await ctx.newPage();await scaled.goto(pathToFileURL(path.join(design,'index.html')).href);
   assert((await scaled.locator('#editor').boundingBox()).width>400);await ctx.close();
  }
  report.interactions.push('forced-colors text fallback, reduced-motion, 1.5x/2x browser layout smoke checks');
  await open();
  for(const preset of ['edges','headers','dock'])for(const theme of ['dark','light']){
   await page.locator(`button[data-detail="${preset}"]`).click();await page.locator(`button[data-theme="${theme}"]`).click();
   await page.locator('.app-window').screenshot({path:path.join(exporting?design:output,`detail-${preset}-${theme}.png`)});
  }
  if(exporting){
   const tiles=[];let row=0;
   for(const [preset,label] of [['edges','A / Randgriffe'],['headers','B / Panelköpfe'],['dock','C / Statusdock']]){
    const heading=Buffer.from(`<svg width="1440" height="40"><rect width="100%" height="100%" fill="#141518"/><text x="8" y="27" fill="#edeef2" font-family="Segoe UI,sans-serif" font-size="19">${label}</text><text x="1270" y="27" fill="#a6a9b1" font-family="Segoe UI,sans-serif" font-size="12">Dunkel / Hell</text></svg>`);
    tiles.push({input:heading,left:0,top:row*490});let col=0;
    for(const theme of ['dark','light']){const input=await sharp(path.join(design,`detail-${preset}-${theme}.png`)).resize({width:710,height:438,fit:'contain',background:'#141518'}).png().toBuffer();tiles.push({input,left:col*730,top:row*490+44});col++;}row++;
   }
   await sharp({create:{width:1440,height:1470,channels:4,background:'#141518'}}).composite(tiles).png().toFile(path.join(design,'detail-overview.png'));
  }
  assert.deepEqual(errors,[]);fs.writeFileSync(path.join(output,'review.json'),JSON.stringify(report,null,2)+'\n');
  console.log(JSON.stringify({...report,layouts:report.layouts.length},null,2));
 }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
