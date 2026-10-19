const {chromium}=require('playwright');
const sharp=require('sharp');
const assert=require('node:assert/strict');
const fs=require('node:fs');
const path=require('node:path');
const {pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),design=path.join(root,'docs/visuals/develop-designs'),output=path.join(root,'build/workbench-search-review');
const exporting=process.argv.includes('--export');fs.mkdirSync(output,{recursive:true});
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
 try{
  const page=await browser.newPage({viewport:{width:1440,height:1200}});page.setDefaultTimeout(8000);
  const errors=[];page.on('pageerror',e=>errors.push(e.message));
  const report={browser:await browser.version(),platform:process.platform,layouts:[],interactions:[],errors};
  const open=()=>page.goto(pathToFileURL(path.join(design,'index.html')).href);
  const geometry=()=>page.locator('#code-area').evaluate(el=>{const r=el.getBoundingClientRect();return {x:r.x,y:r.y,width:r.width,height:r.height};});
  const same=(a,b)=>{for(const key of Object.keys(a))assert(Math.abs(a[key]-b[key])<.1,`Code moved: ${JSON.stringify({a,b})}`);};
  await open();
  for(const width of [1440,1080,760,500])for(const theme of ['dark','light'])for(const mode of ['island','card']){
   await page.setViewportSize({width,height:1200});await page.locator(`button[data-theme="${theme}"]`).click();
   await page.keyboard.press('Escape');const before=await geometry();const scroll=await page.locator('#editor').evaluate(el=>el.scrollTop);
   await page.locator(`[data-search-design="${mode}"]`).click();same(before,await geometry());
   assert.equal(await page.locator('#editor').evaluate(el=>el.scrollTop),scroll);
   assert(await page.locator('#query').evaluate(el=>el===document.activeElement));
   let box=await page.locator('#search-bar').boundingBox();const code=await page.locator('#code-area').boundingBox();
   assert(box.x>=code.x&&box.x+box.width<=code.x+code.width+1&&box.y+box.height<=code.y+code.height);
   if(await page.locator('#replace-toggle').getAttribute('aria-expanded')==='false')await page.locator('#replace-toggle').click();
   same(before,await geometry());box=await page.locator('#search-bar').boundingBox();assert(box.y+box.height<=code.y+code.height);
   assert(await page.locator('#replacement').isVisible());
   const g=await page.evaluate(()=>({width:innerWidth,document:document.documentElement.scrollWidth}));assert(g.document<=width+1);
   report.layouts.push({width,theme,mode,code:before});
  }
  await page.setViewportSize({width:1440,height:1200});await open();
  await page.locator('[data-search-design="island"]').click();await page.locator('#query').fill('length_m');await page.locator('#find-next').click();
  const first=await page.locator('#editor').evaluate(el=>el.selectionStart);await page.locator('#find-next').click();const next=await page.locator('#editor').evaluate(el=>el.selectionStart);assert(next>first);
  await page.locator('#find-previous').click();assert.equal(await page.locator('#editor').evaluate(el=>el.selectionStart),first);
  await page.locator('#query').focus();await page.keyboard.press('Shift+Enter');assert((await page.locator('#editor').evaluate(el=>el.selectionStart))>first);
  await page.locator('#replace-toggle').click();await page.locator('#replacement').fill('pendulum_length');await page.locator('#replace').click();assert.match(await page.locator('#editor').inputValue(),/pendulum_length/);
  await page.locator('#query').fill('absent_symbol_123');assert.match(await page.locator('#search-result').innerText(),/0 Treffer/);
  await page.keyboard.press('Escape');assert(await page.locator('#search-open').evaluate(el=>el===document.activeElement));
  await page.keyboard.press('Control+f');assert(await page.locator('#query').evaluate(el=>el===document.activeElement));await page.keyboard.press('Escape');
  report.interactions.push('search and replacement overlay preserve exact code geometry and scroll','forward/backward search including Shift Enter wrap','replace, no matches, Ctrl F and Escape focus return');
  const measure=async id=>(await page.locator(id).boundingBox());
  async function dragHandle(key,dx,dy){const h=await measure('#resize-'+key);await page.mouse.move(h.x+h.width/2,h.y+h.height/2);await page.mouse.down();await page.mouse.move(h.x+h.width/2+dx,h.y+h.height/2+dy,{steps:8});await page.mouse.up();}
  let tree=await measure('#explorer');await dragHandle('tree',70,0);let changed=await measure('#explorer');assert(changed.width>tree.width+50);
  await page.locator('#toggle-tree').click();await page.locator('#toggle-tree').click();assert.equal((await measure('#explorer')).width,changed.width);
  let api=await measure('#inspector');await dragHandle('api',-60,0);assert((await measure('#inspector')).width>api.width+40);
  let log=await measure('#log-body'),code=await geometry();await dragHandle('log',0,-60);assert((await measure('#log-body')).height>log.height+40);assert((await geometry()).height<code.height-40);
  const scrollState=await page.evaluate(()=>{
   const editor=document.querySelector('#editor');editor.scrollTop=editor.scrollHeight;editor.dispatchEvent(new Event('scroll'));
   return ['#editor','#highlight','#line-numbers'].map(id=>({id,top:document.querySelector(id).scrollTop,height:document.querySelector(id).scrollHeight,client:document.querySelector(id).clientHeight}));
  });
  for(const item of scrollState)assert(Math.abs(item.top-scrollState[0].top)<1,`Resize desynchronized code and line numbers: ${JSON.stringify(scrollState)}`);
  await page.locator('#editor').evaluate(el=>{el.scrollTop=0;el.dispatchEvent(new Event('scroll'));});
  for(const theme of ['dark','light']){
   await page.locator(`button[data-theme="${theme}"]`).click();
   await page.locator('.app-window').screenshot({path:path.join(exporting?design:output,`workbench-resized-${theme}.png`)});
  }
  const h=page.locator('#resize-tree');await h.focus();tree=await measure('#explorer');await h.press('ArrowLeft');assert((await measure('#explorer')).width<tree.width);
  await h.press('Home');assert.equal((await measure('#explorer')).width,140);await h.press('Enter');assert.equal((await measure('#explorer')).width,218);
  const handleBox=await measure('#resize-tree');await page.mouse.move(handleBox.x+4,handleBox.y+30);await page.mouse.down();await page.mouse.move(handleBox.x+84,handleBox.y+30);await page.keyboard.press('Escape');await page.mouse.up();assert.equal((await measure('#explorer')).width,218);
  await page.locator('#reset-panels').click();assert.equal((await measure('#inspector')).width,255);assert.equal((await measure('#log-body')).height,86);
  report.interactions.push('mouse resizing of workspace and context, log height takes space from code','panel size survives close/reopen','keyboard arrows, minimum, Enter reset','Escape cancels resize and restores size','reset all sizes');
  await page.setViewportSize({width:760,height:1200});await page.locator('#resize-api').focus();api=await measure('#inspector');await page.locator('#resize-api').press('ArrowDown');assert((await measure('#inspector')).height>api.height);
  await page.setViewportSize({width:500,height:1200});await page.locator('#resize-tree').focus();tree=await measure('#explorer');await page.locator('#resize-tree').press('ArrowDown');assert((await measure('#explorer')).height>tree.height);
  assert((await page.locator('#editor').boundingBox()).width>200);
  report.interactions.push('stacked panels switch to vertical resizing at narrow widths');
  await page.setViewportSize({width:1440,height:1200});await open();
  await page.emulateMedia({forcedColors:'active',reducedMotion:'reduce'});await page.locator('[data-search-design="card"]').click();assert(await page.locator('#query').isVisible());await page.emulateMedia({forcedColors:'none'});
  await open();
  for(const mode of ['island','card'])for(const theme of ['dark','light']){
   await page.locator(`button[data-theme="${theme}"]`).click();await page.locator(`[data-search-design="${mode}"]`).click();await page.locator('#query').fill('length_m');
   if(mode==='card'&&await page.locator('#replace-toggle').getAttribute('aria-expanded')==='false')await page.locator('#replace-toggle').click();
   if(mode==='island'&&await page.locator('#replace-toggle').getAttribute('aria-expanded')==='true')await page.locator('#replace-toggle').click();
   await page.locator('.app-window').screenshot({path:path.join(exporting?design:output,`search-${mode}-${theme}.png`)});
  }
  if(exporting){
   const tiles=[];let row=0;for(const [mode,label]of [['island','01 / Suchinsel'],['card','02 / Suchkarte']]){
    tiles.push({input:Buffer.from(`<svg width="1440" height="40"><rect width="100%" height="100%" fill="#141518"/><text x="10" y="27" fill="#edeef2" font-family="Segoe UI,sans-serif" font-size="19">${label}</text><text x="1270" y="27" fill="#a6a9b1" font-family="Segoe UI,sans-serif" font-size="12">Dunkel / Hell</text></svg>`),left:0,top:row*490});
    let col=0;for(const theme of ['dark','light']){tiles.push({input:await sharp(path.join(design,`search-${mode}-${theme}.png`)).resize({width:710,height:438,fit:'contain',background:'#141518'}).png().toBuffer(),left:col*730,top:row*490+44});col++;}row++;
   }
   await sharp({create:{width:1440,height:980,channels:4,background:'#141518'}}).composite(tiles).png().toFile(path.join(design,'search-overview.png'));
  }
  assert.deepEqual(errors,[]);fs.writeFileSync(path.join(output,'review.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({...report,layouts:report.layouts.length},null,2));
 }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
