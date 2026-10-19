// Current chosen workbench, glass search island and dock resizing. Browser evidence only.
const {chromium}=require('playwright'),sharp=require('sharp'),assert=require('node:assert/strict');
const fs=require('node:fs'),path=require('node:path'),{pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),design=path.join(root,'docs/visuals/develop-designs'),output=path.join(root,'build/develop-design-review');
const exporting=process.argv.includes('--export');fs.mkdirSync(output,{recursive:true});
const rgba=s=>s.match(/[\d.]+/g).map(Number);
const lum=c=>c.map(n=>n/255).map(n=>n<=.04045?n/12.92:((n+.055)/1.055)**2.4).reduce((n,v,i)=>n+v*[.2126,.7152,.0722][i],0);
const contrast=(a,b)=>(Math.max(lum(a),lum(b))+.05)/(Math.min(lum(a),lum(b))+.05);
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
 try{
  const page=await browser.newPage({viewport:{width:1440,height:1200}});page.setDefaultTimeout(8000);
  const errors=[];page.on('pageerror',e=>errors.push(e.message));
  const report={browser:await browser.version(),platform:process.platform,layouts:[],contrasts:[],interactions:[],errors};
  const open=()=>page.goto(pathToFileURL(path.join(design,'index.html')).href);
  const box=id=>page.locator(id).boundingBox();
  const same=(a,b)=>{for(const key of ['x','y','width','height'])assert(Math.abs(a[key]-b[key])<.1,`Code shifted: ${JSON.stringify({a,b})}`);};
  await open();assert.equal(await page.locator('[data-detail],[data-search-design],[data-design]:is(button),.breadcrumbs,#action-source').count(),0);
  for(const width of [1440,1080,760,500])for(const theme of ['dark','light'])for(const glass of ['satin','clear','crystal']){
   await page.setViewportSize({width,height:1200});await page.locator(`button[data-theme="${theme}"]`).click();await page.keyboard.press('Escape');
   const before=await box('#code-area'),scroll=await page.locator('#editor').evaluate(el=>el.scrollTop);
   await page.locator(`button[data-glass="${glass}"]`).click();same(before,await box('#code-area'));assert.equal(await page.locator('#editor').evaluate(el=>el.scrollTop),scroll);
   const popup=await box('#search-bar'),code=await box('#code-area'),tabs=await box('.editor-topline');
   assert(code.y-tabs.y-tabs.height<1);assert(popup.y-code.y<=5);assert(popup.x>=code.x&&popup.x+popup.width<=code.x+code.width+1);
   if(await page.locator('#replace-toggle').getAttribute('aria-expanded')==='false')await page.locator('#replace-toggle').click();same(before,await box('#code-area'));
   assert(await page.locator('#replacement').isVisible());assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth+1));assert((await box('#editor')).width>=200);
   const material=await page.locator('#search-bar').evaluate(el=>({fill:getComputedStyle(el).backgroundColor,filter:getComputedStyle(el).backdropFilter}));
   assert(rgba(material.fill)[3]<1);assert.match(material.filter,/blur/);
   if(width===1440)for(const selector of ['#query','#find-next','#search-result']){
    const c=await page.locator(selector).evaluate(el=>({fg:getComputedStyle(el).color,bg:getComputedStyle(el).backgroundColor}));const fg=rgba(c.fg).slice(0,3),bg=rgba(c.bg),alpha=bg[3]??1;
    const minimum=Math.min(...[0,255].map(base=>contrast(fg,bg.slice(0,3).map(v=>v*alpha+base*(1-alpha)))));assert(minimum>=4.5,`Contrast ${theme}/${glass}/${selector}: ${minimum}`);report.contrasts.push({theme,glass,selector,minimum});
   }
   report.layouts.push({width,theme,glass,codeGap:code.y-tabs.y-tabs.height,searchInset:popup.y-code.y});
  }
  await page.setViewportSize({width:1440,height:1200});await open();
  for(const glass of ['satin','clear','crystal']){
   await page.locator(`button[data-glass="${glass}"]`).click();const g=await box('#code-area');await page.locator('#reduce-transparency').check();same(g,await box('#code-area'));
   assert.equal(await page.locator('#search-bar').evaluate(el=>getComputedStyle(el).backdropFilter),'none');assert.equal(rgba(await page.locator('#search-bar').evaluate(el=>getComputedStyle(el).backgroundColor))[3]??1,1);await page.locator('#reduce-transparency').uncheck();
  }
  await page.locator('#query').fill('length_m');await page.locator('#find-next').click();const first=await page.locator('#editor').evaluate(el=>el.selectionStart);
  await page.locator('#find-next').click();assert((await page.locator('#editor').evaluate(el=>el.selectionStart))>first);await page.locator('#find-previous').click();assert.equal(await page.locator('#editor').evaluate(el=>el.selectionStart),first);
  await page.locator('#query').focus();await page.keyboard.press('Shift+Enter');assert((await page.locator('#editor').evaluate(el=>el.selectionStart))>first);
  await page.locator('#replace-toggle').click();await page.locator('#replacement').fill('pendulum_length');await page.locator('#replace').click();assert.match(await page.locator('#editor').inputValue(),/pendulum_length/);
  await page.keyboard.press('Escape');assert(await page.locator('#search-open').evaluate(el=>el===document.activeElement));
  await page.locator('.file-tabs [data-file="analysis.c"]').click();await page.locator('#editor').fill('/* draft */ <script>alert(1)</script>');assert.equal(await page.locator('#highlight script').count(),0);
  await page.locator('.file-tabs [data-file="main.c"]').click();assert.match(await page.locator('#editor').inputValue(),/pendulum_length/);await page.keyboard.press('Control+s');assert.match(await page.locator('#save-status').innerText(),/Alle Änderungen/);
  await page.keyboard.press('F5');await page.waitForFunction(()=>document.querySelector('#build-status').textContent.includes('erfolgreich'));await page.locator('#scenario').selectOption('error');await page.locator('#diagnostic-link').click();assert.equal(await page.locator('#editor').evaluate(el=>el.value.slice(el.selectionStart,el.selectionEnd)),'gravity_m_s');
  await open();await page.keyboard.press('Escape');
  async function drag(key,dx,dy){const b=await box('#resize-'+key);await page.mouse.move(b.x+b.width/2,b.y+b.height/2);await page.mouse.down();await page.mouse.move(b.x+b.width/2+dx,b.y+b.height/2+dy,{steps:7});await page.mouse.up();}
  let before=await box('#explorer');await drag('tree',60,0);assert((await box('#explorer')).width>before.width+40);const retained=(await box('#explorer')).width;
  await page.locator('#toggle-tree').click();assert(await page.locator('#toggle-tree').isVisible());await page.locator('#toggle-tree').press('Enter');assert.equal((await box('#explorer')).width,retained);
  before=await box('#inspector');await drag('api',-50,0);assert((await box('#inspector')).width>before.width+30);
  before=await box('#log-body');const codeBefore=await box('#code-area');await drag('log',0,-50);assert((await box('#log-body')).height>before.height+30);assert((await box('#code-area')).height<codeBefore.height-30);
  const tops=await page.evaluate(()=>{const el=document.querySelector('#editor');el.scrollTop=el.scrollHeight;el.dispatchEvent(new Event('scroll'));return ['#editor','#highlight','#line-numbers'].map(id=>document.querySelector(id).scrollTop);});assert(tops.every(v=>Math.abs(v-tops[0])<1));
  await page.locator('#resize-tree').focus();await page.keyboard.press('Home');assert.equal((await box('#explorer')).width,140);await page.keyboard.press('Enter');assert.equal((await box('#explorer')).width,218);
  const h=await box('#resize-tree');await page.mouse.move(h.x+4,h.y+30);await page.mouse.down();await page.mouse.move(h.x+64,h.y+30);await page.keyboard.press('Escape');await page.mouse.up();assert.equal((await box('#explorer')).width,218);
  await page.locator('#reset-panels').click();assert.equal((await box('#inspector')).width,255);assert.equal((await box('#log-body')).height,86);
  for(const [width,key,panel]of [[760,'api','#inspector'],[500,'tree','#explorer']]){await page.setViewportSize({width,height:1200});const b=await box(panel);await page.locator('#resize-'+key).focus();await page.keyboard.press('ArrowDown');assert((await box(panel)).height>b.height);}
  await page.setViewportSize({width:1440,height:1200});await open();await page.emulateMedia({forcedColors:'active',reducedMotion:'reduce'});assert.equal(await page.locator('#search-bar').evaluate(el=>getComputedStyle(el).backdropFilter),'none');assert.equal(await page.locator('#highlight').evaluate(el=>getComputedStyle(el).visibility),'hidden');await page.emulateMedia({forcedColors:'none'});
  await page.locator('#scenario').selectOption('empty');assert.equal(await page.locator('.workspace').isVisible(),false);await page.keyboard.press('F5');assert(await page.locator('#empty-state').isVisible());
  report.interactions.push('compact top edge; no code shift on search or replacement','transparent material; conservative control contrast over black/white','opaque fallback without layout change','forward/backward search, replace and Escape focus','file edits, safe highlighting, save, build and diagnostic jump','mouse/keyboard resizing, retained sizes, cancel and reset','scroll sync, narrow panels, forced colors and empty state');
  await page.setViewportSize({width:1280,height:1100});await open();const tiles=[];let row=0;
  for(const [glass,label]of [['satin','01 / Seidenglas'],['clear','02 / Klarglas'],['crystal','03 / Kristallkante']]){
   if(exporting)tiles.push({input:Buffer.from(`<svg width="1440" height="36"><rect width="100%" height="100%" fill="#141518"/><text x="10" y="26" fill="#edeef2" font-family="Segoe UI,sans-serif" font-size="18">${label}</text><text x="1270" y="26" fill="#a6a9b1" font-family="Segoe UI,sans-serif" font-size="12">Dunkel / Hell</text></svg>`),left:0,top:row*280});
   let col=0;for(const theme of ['dark','light']){
    await page.locator(`button[data-theme="${theme}"]`).click();await page.locator(`button[data-glass="${glass}"]`).click();await page.locator('#query').fill('length_m');await page.locator('#query').evaluate(el=>el.blur());
    await page.locator('.app-window').screenshot({path:path.join(exporting?design:output,`glass-${glass}-${theme}.png`)});
    if(exporting){const r=await box('.editor-pane'),crop=await page.screenshot({clip:{x:r.x,y:r.y,width:r.width,height:236}});tiles.push({input:await sharp(crop).resize({width:710,height:236,fit:'contain',background:'#141518'}).png().toBuffer(),left:col*730,top:row*280+40});}col++;
   }row++;
  }
  if(exporting)await sharp({create:{width:1440,height:840,channels:4,background:'#141518'}}).composite(tiles).png().toFile(path.join(design,'glass-overview.png'));
  assert.deepEqual(errors,[]);fs.writeFileSync(path.join(output,'review.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({...report,layouts:report.layouts.length,contrasts:report.contrasts.length},null,2));
 }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
