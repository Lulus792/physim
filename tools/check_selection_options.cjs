// Browser evidence for the isolated marking study; no native acceptance implied.
const {chromium}=require('playwright'),assert=require('node:assert/strict'),fs=require('node:fs'),path=require('node:path'),{pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),out=path.join(root,'build/selection-options-review');fs.mkdirSync(out,{recursive:true});
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
 const report={date:new Date().toISOString(),platform:process.platform,browser:await browser.version(),sourceCommit:require('node:child_process').execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),layouts:[],checks:[],contrasts:[],errors:[]};
 const page=await browser.newPage({viewport:{width:1440,height:1100}});page.setDefaultTimeout(8000);page.on('pageerror',e=>report.errors.push(e.message));
 const base=pathToFileURL(path.join(root,'docs/visuals/selection-options/index.html')).href;
 const go=async(v='a',c='code',t='dark')=>{await page.goto(base+`?variant=${v}&context=${c}&theme=${t}`);await page.locator('#sample').waitFor();};
 const hidden=async()=>assert(await page.locator('#context-menu').isHidden(),'Left selection must not open actions');
 const bounds=async()=>{const b=await page.locator('#context-menu').boundingBox(),v=page.viewportSize();assert(b&&b.x>=0&&b.y>=0&&b.x+b.width<=v.width+1&&b.y+b.height<=v.height+1,'Context menu must fit viewport');};
 try{
  for(const v of ['a','b','c'])for(const c of ['code','protocol','files','scene'])for(const t of ['dark','light'])for(const width of [1440,500]){
   await page.setViewportSize({width,height:1100});await go(v,c,t);await hidden();
   assert.match(await page.locator('#selection-status').innerText(),/ausgewählt/);assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth+1),'No page overflow');
   const target=page.locator(c==='code'?'.code-editor':c==='scene'?'[data-object=mass]':'[data-row="0"]');await target.focus();await page.keyboard.press('Shift+F10');assert(await page.getByRole('menu').isVisible());await bounds();await page.keyboard.press('Escape');assert(await target.evaluate(e=>e===document.activeElement));
   if(width===1440&&c==='code'){
    const colors=await page.evaluate(()=>{const rgb=s=>s.match(/[\d.]+/g).map(Number),lum=c=>c.slice(0,3).map(x=>{x/=255;return x<=.04045?x/12.92:((x+.055)/1.055)**2.4;}).reduce((s,x,i)=>s+x*[.2126,.7152,.0722][i],0);const editor=document.querySelector('.code-editor'),base=rgb(getComputedStyle(editor).backgroundColor);const bg0=base[3]===0?rgb(getComputedStyle(document.querySelector('.content-pane')).backgroundColor):base;return [editor,...['.kw','.num','.call','.comment'].map(s=>editor.querySelector(s))].map(el=>{const st=getComputedStyle(el,'::selection'),bg=rgb(st.backgroundColor),fg=rgb(st.color),a=bg[3]??1,composite=bg.slice(0,3).map((x,i)=>x*a+bg0[i]*(1-a)),l1=lum(fg),l2=lum(composite);return {role:el.className,foreground:st.color,background:st.backgroundColor,ratio:(Math.max(l1,l2)+.05)/(Math.min(l1,l2)+.05)};});});
    colors.forEach(col=>assert(col.ratio>=4.5,`${v}/${t}/${col.role}: ${col.ratio}`));report.contrasts.push({variant:v,theme:t,colors});
    await page.locator('#sample').click();await page.screenshot({path:path.join(out,`text-${v}-${t}.png`),fullPage:true});
   }
   report.layouts.push({variant:v,context:c,theme:t,width});
  }
  report.checks.push('48 layouts: 3 variants × 4 contexts × 2 themes × 2 widths; no actions on left selection, keyboard menu, bounds, Escape focus return and no horizontal overflow');
  await page.setViewportSize({width:1440,height:1100});await go();
  await page.locator('.code-editor').focus();await page.keyboard.press('Escape');const lines=page.locator('.code-line'),a=await lines.nth(6).boundingBox(),b=await lines.nth(8).boundingBox();
  await page.mouse.move(a.x+15,a.y+a.height/2);await page.mouse.down();await page.mouse.move(b.x+280,b.y+b.height/2,{steps:12});await page.mouse.up();await hidden();
  const text=await page.evaluate(()=>getSelection().toString());assert.match(text,/kinetic/);assert.match(text,/potential/);
  await page.mouse.click(a.x+100,a.y+a.height/2,{button:'right'});await bounds();assert.equal(await page.evaluate(()=>getSelection().toString()),text);
  await page.keyboard.press('End');assert.equal(await page.evaluate(()=>document.activeElement.dataset.action),'all');await page.keyboard.press('Home');assert.equal(await page.evaluate(()=>document.activeElement.dataset.action),'copy');await page.keyboard.press('ArrowDown');assert.equal(await page.evaluate(()=>document.activeElement.dataset.action),'search');await page.keyboard.press('Enter');assert(await page.locator('#details').isVisible());assert((await page.locator('#details').textContent()).includes(text.trim()));await hidden();
  await page.locator('#sample').click();await page.locator('.code-editor').focus();await page.keyboard.press('Shift+F10');
  await page.evaluate(()=>Object.defineProperty(navigator,'clipboard',{configurable:true,value:{writeText:async()=>{throw new Error('permission denied');}}}));await page.getByRole('menuitem',{name:'Kopieren',exact:false}).click();assert.match(await page.locator('#feedback').innerText(),/nicht erlaubt/);
  await page.locator('.code-editor').focus();await page.keyboard.press('Control+a');const all=await page.evaluate(()=>getSelection().toString());assert.match(all,/Energie \[J\]/);assert(!all.includes('SO FUNKTIONIERT'));await page.keyboard.press('ContextMenu');assert(await page.getByRole('menu').isVisible());await page.keyboard.press('Escape');
  report.checks.push('Actual mouse drag selects exact text; right click preserves it; menu arrow/Home/End navigation; search consumes selection; clipboard-denial message; Ctrl+A stays in the editor and Menu key opens actions');
  for(const context of ['protocol','files']){
   await go('a',context);await page.locator('[data-row="0"]').click();await page.locator('[data-row="2"]').click({modifiers:['Control']});await hidden();assert.equal(await page.locator('.selectable-row[aria-pressed=true]').count(),2);
   await page.locator('[data-row="0"]').click({button:'right'});assert.equal(await page.locator('.selectable-row[aria-pressed=true]').count(),2);await page.keyboard.press('Escape');
   await page.locator('[data-row="3"]').click({button:'right'});assert.equal(await page.locator('.selectable-row[aria-pressed=true]').count(),1);assert.equal(await page.locator('[data-row="3"]').getAttribute('aria-pressed'),'true');await bounds();await page.screenshot({path:path.join(out,`${context}-menu.png`)});assert(await page.getByRole('menu').isVisible());await page.keyboard.press('Escape');
   await page.locator('[data-row="0"]').click();await page.locator('[data-row="2"]').click({modifiers:['Shift']});assert.equal(await page.locator('.selectable-row[aria-pressed=true]').count(),3);
   await page.locator('[data-row="2"]').focus();await page.keyboard.press('Shift+F10');await page.getByRole('menuitem',{name:'Alle auswählen'}).click();assert.equal(await page.locator('.selectable-row[aria-pressed=true]').count(),4);
  }
  report.checks.push('Real row clicks: Ctrl/Shift multi-selection; right-click preserves selected set or replaces it for a new target; select-all works for protocol and files');
  await go('b','scene');const svg=page.locator('.scene'),s=await svg.boundingBox();const point=(x,y)=>({x:s.x+x*s.width/700,y:s.y+y*s.height/315});let p1=point(320,105),p2=point(610,300);
  await page.mouse.move(p1.x,p1.y);await page.mouse.down();await page.mouse.move(p2.x,p2.y,{steps:12});await page.mouse.up();await hidden();assert.equal(await page.locator('[data-object][aria-pressed=true]').count(),3);
  await page.locator('[data-object=mass]').click({button:'right'});assert.equal(await page.locator('[data-object][aria-pressed=true]').count(),3);await page.getByRole('menuitem',{name:'Auswahl isolieren'}).click();await hidden();
  const before=await page.locator('[data-object][aria-pressed=true]').evaluateAll(es=>es.map(e=>e.dataset.object));p1=point(20,20);p2=point(80,80);await page.mouse.move(p1.x,p1.y);await page.mouse.down();await page.mouse.move(p2.x,p2.y,{steps:4});await page.keyboard.press('Escape');await page.mouse.up();assert.deepEqual(await page.locator('[data-object][aria-pressed=true]').evaluateAll(es=>es.map(e=>e.dataset.object)),before);
  await page.screenshot({path:path.join(out,'scene-contours.png'),fullPage:true});report.checks.push('Actual marquee selects three objects; right-click preserves set; isolation action; Escape rolls back in-progress drag');
  await go('a','code','light');await page.evaluate(()=>document.documentElement.style.fontSize='28px');assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth+1));await page.locator('.code-editor').focus();await page.keyboard.press('Shift+F10');await bounds();await page.keyboard.press('Escape');
  await page.emulateMedia({forcedColors:'active',reducedMotion:'reduce'});await page.locator('#sample').click();assert.match(await page.locator('.code-editor .kw').first().evaluate(e=>getComputedStyle(e,'::selection').color),/rgb/);await page.locator('.code-editor').focus();await page.keyboard.press('Shift+F10');await bounds();await page.keyboard.press('Escape');
  const hidpi=await browser.newPage({viewport:{width:1440,height:1100},deviceScaleFactor:2});await hidpi.goto(base);assert(await hidpi.evaluate(()=>devicePixelRatio===2&&document.documentElement.scrollWidth<=innerWidth+1));await hidpi.close();
  report.checks.push('200% root text size, forced colors, reduced motion and separate 2× browser pixel density; native OS DPI and screenreader acceptance remain open');
  assert.deepEqual(report.errors,[]);report.passed=true;
 }catch(e){report.failure=e.stack;await page.screenshot({path:path.join(out,'failure.png'),fullPage:true});throw e;}
 finally{fs.writeFileSync(path.join(out,'validation.json'),JSON.stringify(report,null,2)+'\n');await browser.close();}
 console.log(JSON.stringify({passed:report.passed,layouts:report.layouts.length,minimumContrast:Math.min(...report.contrasts.flatMap(x=>x.colors.map(y=>y.ratio))),checks:report.checks}));
})().catch(e=>{console.error(e);process.exitCode=1;});
