// Review of the inspector design alternatives, splitter interaction and staged build demo.
const {chromium}=require('playwright');
const sharp=require('sharp');
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
const {pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),design=path.join(root,'docs/visuals/simulation-designs'),out=path.join(root,'build/inspector-variants-review');
const exporting=process.argv.includes('--export');fs.mkdirSync(out,{recursive:true});
const url=pathToFileURL(path.join(design,'index.html')).href;
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
 const report={browser:await browser.version(),platform:process.platform,layouts:[],splitters:[],builds:[],errors:[]};
 try{
  const page=await browser.newPage({viewport:{width:1440,height:1550}});page.setDefaultTimeout(10000);page.on('pageerror',e=>report.errors.push(e.message));
  for(const theme of ['dark','light'])for(const style of ['rows','fields','compact']){
   await page.goto(url+`?design=labor&theme=${theme}&inspectorStyle=${style}`);
   assert.equal(await page.locator('.app-window').getAttribute('data-inspector-style'),style);
   assert.equal(await page.locator('#inspector details').count(),9);
   for(const width of [1440,1080,760,390]){
    await page.setViewportSize({width,height:1550});
    assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth),`${style}/${theme}/${width}: page overflow`);
    for(const selector of ['#inspector','.inspector-body','.measurements','.timeline'])assert(await page.locator(selector).evaluate(e=>e.scrollWidth<=e.clientWidth+1),`${style}/${theme}/${width}: ${selector} overflow`);
    report.layouts.push({style,theme,width});
   }
   await page.setViewportSize({width:1440,height:1550});
   await page.locator('#object-group').evaluate(e=>e.open=false);await page.locator('#source-model-group').evaluate(e=>e.open=true);
   await page.locator('#parameter-group').evaluate(e=>e.open=true);await page.locator('#run-group').evaluate(e=>e.open=true);
   await page.locator('.inspector-body').evaluate(e=>e.scrollTop=0);
   await page.locator('#inspector').screenshot({path:path.join(exporting?design:out,`inspector-${style}-${theme}.png`)});
  }
  for(const theme of ['dark','light']){
   await page.goto(url+`?design=labor&theme=${theme}`);
   assert.equal(await page.locator('.app-window').getAttribute('data-inspector-style'),'fields');
   const elementWidth=selector=>page.locator(selector).evaluate(e=>e.getBoundingClientRect().width);
   const elementHeight=selector=>page.locator(selector).evaluate(e=>e.getBoundingClientRect().height);
   const initialWidth=await elementWidth('#inspector');
   await page.locator('#inspector-resize').focus();await page.keyboard.press('ArrowLeft');assert((await elementWidth('#inspector'))>initialWidth);
   await page.keyboard.press('Home');assert.equal(await elementWidth('#inspector'),210);await page.keyboard.press('Enter');assert.equal(await elementWidth('#inspector'),initialWidth);
   // Pointer drag, including capture outside the narrow grip.
   const box=await page.locator('#inspector-resize').boundingBox();await page.mouse.move(box.x+box.width/2,box.y+100);await page.mouse.down();await page.mouse.move(box.x-60,box.y+100,{steps:6});await page.mouse.up();
   assert((await elementWidth('#inspector'))>initialWidth+50);assert(!(await page.locator('.app-window').getAttribute('class')).includes('resizing'));
   await page.locator('#tree-toggle').click();const left=await elementWidth('#scene-tree');await page.locator('#tree-resize').focus();await page.keyboard.press('ArrowRight');assert((await elementWidth('#scene-tree'))>left);
   const beforeMeasure=await elementHeight('#measurements');await page.locator('#measure-resize').focus();await page.keyboard.press('ArrowUp');assert((await elementHeight('#measurements'))>beforeMeasure);
   const beforeTimeline=await elementHeight('.timeline');await page.locator('#timeline-resize').focus();await page.keyboard.press('ArrowUp');assert((await elementHeight('.timeline'))>beforeTimeline);
   const beforeWorkspace=await elementHeight('.workspace');await page.locator('#workspace-resize').focus();await page.keyboard.press('ArrowDown');assert((await elementHeight('.workspace'))>beforeWorkspace);
   await page.locator('#curves-toggle').click();const firstCurve=await elementWidth('#curves .chart:first-child');await page.locator('#plot-resize').focus();await page.keyboard.press('ArrowRight');assert((await elementWidth('#curves .chart:first-child'))>firstCurve);
   const time=await page.locator('#time').inputValue(),selection=await page.locator('#selected-type').textContent();
   await page.locator('button[data-inspector-style="compact"]').click();await page.locator('button[data-inspector-style="fields"]').click();assert.equal(await page.locator('#time').inputValue(),time);assert.equal(await page.locator('#selected-type').textContent(),selection);assert.equal(await page.locator('#curves-toggle').getAttribute('aria-expanded'),'true');
   await page.locator('.app-window').screenshot({path:path.join(exporting?design:out,`messlabor-resizable-${theme}.png`)});
   report.splitters.push({theme,result:'all six splitters, pointer capture, keyboard, reset and state retention passed'});
   // Staged editing must not change the paused model before a build and new run.
   await page.goto(url+`?design=labor&theme=${theme}`);
   const oldAngle=await page.locator('#angle-value').textContent(),oldTime=await page.locator('#time').inputValue();
   await page.locator('#model-length').fill('1,234567');await page.locator('#model-mass').focus();assert.equal(await page.locator('#model-length').inputValue(),'1,234567');await page.locator('#discard-model').click();
   await page.locator('#model-length').fill('2,20');assert(await page.locator('#model-change-bar').isVisible());
   assert.equal(await page.locator('#angle-value').textContent(),oldAngle);assert.equal(await page.locator('#time').inputValue(),oldTime);
   assert.equal(await page.locator('#compile-model').textContent(),'Stoppen & neu kompilieren');
   await page.locator('.app-window').screenshot({path:path.join(exporting?design:out,`model-change-${theme}.png`)});
   await page.locator('#compile-model').click();assert.equal(await page.locator('.app-window').getAttribute('data-state'),'ready');
   await page.waitForFunction(()=>document.getElementById('compile-model').textContent==='Neuer Lauf');
   assert.equal(await page.locator('#angle-value').textContent(),oldAngle);
   await page.evaluate(()=>document.addEventListener('physim-demo-values',e=>window.__reviewModel=e.detail));
   await page.locator('#compile-model').click();assert.equal(await page.locator('.app-window').getAttribute('data-state'),'running');
   assert.equal(await page.evaluate(()=>window.__reviewModel.length),2.2);
   assert.equal(await page.locator('.chart:nth-child(2) .chart-axis text').first().textContent(),'2,3');
   await page.locator('#run-button').click();
   await page.locator('#model-length').fill('2.5');await page.locator('#discard-model').click();assert.equal(await page.locator('#model-length').inputValue(),'2,20');assert(await page.locator('#model-change-bar').isHidden());
   await page.locator('#model-length').fill('9');assert(await page.locator('#compile-model').isDisabled());assert.equal(await page.locator('#model-length').getAttribute('aria-invalid'),'true');
   await page.locator('#stop-button').click();await page.locator('#run-button').click();assert.equal(await page.locator('.app-window').getAttribute('data-state'),'ready');
   report.builds.push({theme,result:'pending edit, explicit stop/build, apply only on new run, discard and invalid values passed'});
  }
  assert.deepEqual(report.errors,[]);
  if(exporting){
   const captions=['A · Ruhige Zeilen','B · Wertefelder','C · Kompakt'];
   const images=[];for(const theme of ['dark','light'])for(const [i,style]of ['rows','fields','compact'].entries()){
    const input=await sharp(path.join(design,`inspector-${style}-${theme}.png`)).resize({width:320}).png().toBuffer();images.push({input,left:12+i*332,top:45+(theme==='light'?720:0)});
   }
   const header=Buffer.from(`<svg width="1008" height="36"><style>text{font:14px sans-serif;fill:#edeef2}</style>${captions.map((s,i)=>`<text x="${18+i*332}" y="25">${s}</text>`).join('')}</svg>`);
   for(const [index,theme] of ['dark','light'].entries())await sharp({create:{width:1008,height:720,channels:4,background:'#17181b'}}).composite([{input:header,left:0,top:0},...images.slice(index*3,index*3+3).map(v=>({...v,top:45}))]).png().toFile(path.join(design,`inspector-styles-${theme}.png`));
   await sharp({create:{width:1008,height:1450,channels:4,background:'#17181b'}}).composite([{input:header,left:0,top:0},...images]).png().toFile(path.join(design,'inspector-styles-comparison.png'));
  }
  fs.writeFileSync(path.join(out,'review.json'),JSON.stringify(report,null,2));console.log(JSON.stringify(report,null,2));
 }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
