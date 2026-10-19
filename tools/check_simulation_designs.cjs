// Browser review of design studies, not a native Physim/platform acceptance test.
const {chromium}=require('playwright');
const sharp=require('sharp');
const assert=require('node:assert/strict');
const fs=require('node:fs');
const path=require('node:path');
const {pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..');
const design=path.join(root,'docs/visuals/simulation-designs');
const output=path.join(root,'build/simulation-design-review');
const exporting=process.argv.includes('--export');
fs.mkdirSync(output,{recursive:true});
const url=pathToFileURL(path.join(design,'index.html')).href;
function contrast(a,b){
 const lum=c=>{let hex=c.trim().slice(1);if(hex.length===3)hex=hex.split('').map(v=>v+v).join('');const v=hex.match(/../g).map(v=>parseInt(v,16)/255).map(v=>v<=.04045?v/12.92:((v+.055)/1.055)**2.4);return v[0]*.2126+v[1]*.7152+v[2]*.0722;};
 const x=lum(a),y=lum(b);return(Math.max(x,y)+.05)/(Math.min(x,y)+.05);
}
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
const report={platform:process.platform,browser:await browser.version(),layouts:[],curves:[],contrast:[],interactions:[],errors:[]};
 try{
 const page=await browser.newPage({viewport:{width:1440,height:1250},deviceScaleFactor:1});
 page.on('pageerror',e=>report.errors.push(e.message));
 for(const theme of ['dark','light']){
  for(const variant of ['studio','labor','focus']){
   await page.goto(url+`?theme=${theme}&design=${variant}`);
   assert.equal(await page.locator('.app-window').getAttribute('data-design'),variant);
   assert.equal(await page.locator('html').getAttribute('data-theme'),theme);
   for(const width of [1440,1080,760,390]){
    await page.setViewportSize({width,height:1250});
    const geometry=await page.evaluate(()=>{
     const r=document.querySelector('.scene').getBoundingClientRect();
     return{overflow:document.documentElement.scrollWidth>innerWidth,sceneWidth:r.width,sceneHeight:r.height};
    });
    assert(!geometry.overflow,`${theme}/${variant}/${width}: document overflow`);
    assert(geometry.sceneWidth>=200,`${theme}/${variant}/${width}: scene collapsed`);
    assert(geometry.sceneHeight>=250);
    for(const s of ['.scene-tree','.inspector','.measurements','.transport','.timeline']){
     const e=page.locator(s);if(await e.isVisible())assert(await e.evaluate(e=>e.scrollWidth<=e.clientWidth+1),`${theme}/${variant}/${width}: overflow in ${s}`);
    }
    report.layouts.push({theme,variant,width,...geometry});
    if(variant!=='focus'){
     const open=await page.locator('#curves-toggle').getAttribute('aria-expanded')==='true';
     assert.equal(open,variant==='labor');
     const before=await page.locator('#scene').evaluate(e=>e.getBoundingClientRect().height);
     const time=await page.locator('#time').inputValue();
     const tree=await page.locator('#tree-toggle').getAttribute('aria-expanded');
     const inspector=await page.locator('#inspector-toggle').getAttribute('aria-expanded');
     await page.locator('#curves-toggle').focus();await page.keyboard.press('Enter');
     assert.equal(await page.locator('#curves').isVisible(),!open);
     assert.equal(await page.locator('#curves-toggle').getAttribute('aria-expanded'),String(!open));
     assert.equal(await page.locator('#time').inputValue(),time);
     assert.equal(await page.locator('#tree-toggle').getAttribute('aria-expanded'),tree);
     assert.equal(await page.locator('#inspector-toggle').getAttribute('aria-expanded'),inspector);
     const after=await page.locator('#scene').evaluate(e=>e.getBoundingClientRect().height);
     if(width>=760)assert(open?after>before:after<before,`${variant}/${width}: scene should receive curve space when collapsed (${before}/${after})`);
     assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
     await page.locator('#curves-toggle').focus();await page.keyboard.press('Space');
     assert.equal(await page.locator('#curves').isVisible(),open);
     report.curves.push({theme,variant,width,sceneBefore:before,sceneAfter:after});
    }
   }
   await page.setViewportSize({width:1440,height:1250});
   await page.screenshot({path:path.join(output,`${variant}-${theme}.png`),fullPage:true});
   if(exporting)await page.locator('.app-window').screenshot({path:path.join(design,`${variant}-${theme}.png`)});
   if(exporting&&variant!=='focus'){
    await page.locator('#curves-toggle').click();
    await page.locator('.app-window').screenshot({path:path.join(design,`${variant}-${theme}-curves-${variant==='studio'?'open':'closed'}.png`)});
    // Keep independent disclosure choices while switching designs and themes.
    await page.locator('.variants [data-design="focus"]').click();
    await page.locator(`.variants [data-design="${variant}"]`).click();
    assert.equal(await page.locator('#curves').isVisible(),variant==='studio');
    await page.locator(`.segmented [data-theme="${theme==='dark'?'light':'dark'}"]`).click();
    assert.equal(await page.locator('#curves').isVisible(),variant==='studio');
    await page.locator(`.segmented [data-theme="${theme}"]`).click();
    await page.locator('#curves-toggle').click();
   }
   // Each side is independent, including its fixed grid column.
   const initial=await page.locator('#inspector-toggle').getAttribute('aria-expanded');
   await page.locator('#tree-toggle').click();assert.equal(await page.locator('#inspector-toggle').getAttribute('aria-expanded'),initial);
   await page.locator('#tree-toggle').click();
   const left=await page.locator('#tree-toggle').getAttribute('aria-expanded');
   await page.locator('#inspector-toggle').focus();await page.keyboard.press('Enter');
   assert.equal(await page.locator('#tree-toggle').getAttribute('aria-expanded'),left);
   await page.keyboard.press('Space');
   await page.locator('#time').fill('1.25');assert.equal(await page.locator('#live-button').getAttribute('aria-pressed'),'false');
   assert((await page.locator('#measurement-time').textContent()).includes('1,25'));
   await page.locator('#live-button').click();assert((await page.locator('#time-value').textContent()).includes('2,40'));
   await page.locator('#run-button').click();assert.equal(await page.locator('.app-window').getAttribute('data-state'),'running');
   await page.waitForFunction(()=>Number(document.getElementById('time').value)>2.4);
   await page.locator('#run-button').click();assert.equal(await page.locator('.app-window').getAttribute('data-state'),'paused');
   await page.locator('#reset-button').click();assert.equal(await page.locator('#time').inputValue(),'0');
   await page.locator('#step-button').click();assert.equal(await page.locator('#time').inputValue(),'0.001');
   await page.locator('#stop-button').click();await page.locator('#run-button').click();
   assert.equal(await page.locator('.app-window').getAttribute('data-state'),'running');await page.locator('#run-button').click();
   for(const state of ['empty','error']){
    await page.locator('#state-select').selectOption(state);assert(await page.locator('#state-overlay').isVisible());assert(await page.locator('#run-button').isDisabled());
    await page.locator('#overlay-action').click();assert(await page.locator('#state-overlay').isHidden());
   }
   // Theme changes preserve independent panel and timeline state.
   await page.locator('#tree-toggle').click();const tree=await page.locator('#tree-toggle').getAttribute('aria-expanded');
   await page.locator(`.segmented [data-theme="${theme==='dark'?'light':'dark'}"]`).click();
   assert.equal(await page.locator('#tree-toggle').getAttribute('aria-expanded'),tree);
   await page.locator(`.segmented [data-theme="${theme}"]`).click();
   if(await page.locator('#tree-toggle').getAttribute('aria-expanded')==='false')await page.locator('#tree-toggle').click();
   await page.locator('[data-object="rod"]').click();assert.equal(await page.locator('#selected-type').textContent(),'Linie · ID 2');
   await page.locator('#show-vectors').uncheck();assert.equal(await page.locator('#velocity').evaluate(e=>getComputedStyle(e).visibility),'hidden');
   await page.locator('#camera').selectOption('front');assert((await page.locator('#view-label').textContent()).includes('Vorne'));
   await page.locator('#camera-reset').click();assert.equal(await page.locator('#camera').inputValue(),'perspective');
   report.interactions.push({theme,variant,result:'panels, keyboard, history/live, run/pause, step/reset, restart, empty/error, theme, selection, visibility, camera passed'});
  }
  const css=await page.evaluate(()=>{const s=getComputedStyle(document.documentElement);return Object.fromEntries(['text','muted','window','panel','canvas','selected','accent','primary','on-primary'].map(k=>[k,s.getPropertyValue('--'+k).trim()]));});
  for(const pair of [['text','window'],['muted','panel'],['muted','canvas'],['text','selected'],['accent','selected'],['on-primary','primary']]){
   const ratio=contrast(css[pair[0]],css[pair[1]]);assert(ratio>=4.5,`${theme}: ${pair} contrast ${ratio}`);report.contrast.push({theme,pair,ratio:+ratio.toFixed(2)});
  }
 }
 // Browser text scaling and 2x rasterization; no physical monitor/DPI claim.
 await page.setViewportSize({width:1080,height:1250});await page.goto(url);
 await page.addStyleTag({content:'body{font-size:28px}button,select{font-size:inherit!important}'});
 assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
 for(const s of ['.scene-tree','.inspector','.transport','.timeline'])assert(await page.locator(s).evaluate(e=>e.scrollWidth<=e.clientWidth+1),`200% controls: overflow in ${s}`);
 await page.screenshot({path:path.join(output,'text-scale-200.png'),fullPage:true});
 const hidpi=await browser.newPage({viewport:{width:1440,height:1250},deviceScaleFactor:2,reducedMotion:'reduce'});
 await hidpi.goto(url);await hidpi.screenshot({path:path.join(output,'studio-dark-2x.png'),fullPage:true});await hidpi.close();
 assert.deepEqual(report.errors,[]);
 if(exporting){
  const tiles=[];for(const theme of ['dark','light'])for(const variant of ['studio','labor','focus'])tiles.push(await sharp(path.join(design,`${variant}-${theme}.png`)).resize({width:680}).png().toBuffer());
  const sizes=await Promise.all(tiles.map(t=>sharp(t).metadata()));const rowHeight=Math.max(...sizes.map(s=>s.height))+18;
  await sharp({create:{width:1384,height:rowHeight*3+18,channels:4,background:'#17181b'}}).composite(tiles.map((input,i)=>({input,left:12+Math.floor(i/3)*692,top:12+(i%3)*rowHeight}))).png().toFile(path.join(design,'overview.png'));
 }
 fs.writeFileSync(path.join(output,'review.json'),JSON.stringify(report,null,2));
 console.log(JSON.stringify({browser:report.browser,layouts:report.layouts.length,curveLayouts:report.curves.length,interactions:report.interactions.length,contrast:report.contrast,errors:report.errors,output},null,2));
 }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
