// Real pointer/keyboard tasks against the shared splitter engine in all three areas.
const {chromium}=require('playwright'),assert=require('node:assert/strict'),fs=require('node:fs'),path=require('node:path'),{pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),output=path.join(root,'build/unified-workspace-review/resizing');fs.mkdirSync(output,{recursive:true});
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
 const report={date:new Date().toISOString(),platform:process.platform,browser:await browser.version(),handles:[],layouts:[],errors:[]};
 const page=await browser.newPage({viewport:{width:1440,height:1000}});page.setDefaultTimeout(10000);page.on('pageerror',e=>report.errors.push(e.message));
 try{
  await page.goto(pathToFileURL(path.join(root,'docs/visuals/unified-workspace/index.html')).href);
  const f=area=>page.frameLocator('#frame-'+area),go=async area=>{await page.locator('#tab-'+area).click();await f(area).locator('body[data-unified]').waitFor();};
  await go('simulate');await f('simulate').locator('#step-button').click();await f('simulate').locator('#shared-export').click();await f('analyze').locator('#statistics tr').waitFor();
  const stats=await f('analyze').locator('#statistics').innerText(),source=await f('develop').locator('#editor').inputValue();
  async function check(area,id){
   const frame=f(area),h=frame.locator('#'+id);await h.scrollIntoViewIfNeeded();
   const orientation=await h.getAttribute('aria-orientation'),vertical=orientation==='vertical';
   const value=()=>h.evaluate(el=>Number(el.getAttribute('aria-valuenow')));
   const baseline=await value();await h.focus();await h.press('Enter');const reset=await value();
   const plotBefore=id==='resize-analysis-chart'?Number(await frame.locator('#plot-clip rect').getAttribute('height')):null;
   // First grow, then reverse, using the same physical direction conventions.
   const negative=['resize-api','resize-log','inspector-resize','measure-resize','timeline-resize','resize-analysis-right'].includes(id);
   const grow=vertical?(negative?'ArrowLeft':'ArrowRight'):(negative?'ArrowUp':'ArrowDown');
   await h.press(grow);const after10=await value();assert(Math.abs(after10-reset-10)<=1,`${area}/${id}: expected 10 pixels, got ${after10-reset}`);
   if(plotBefore!==null)assert(Number(await frame.locator('#plot-clip rect').getAttribute('height'))>plotBefore,'Chart renderer did not receive the new height');
   await h.press('Shift+'+grow);assert(Math.abs(await value()-after10-40)<=1,`${area}/${id}: Shift must add 40 pixels`);
   await h.press('Home');assert(Math.abs(await value()-Number(await h.getAttribute('aria-valuemin')))<=1,`${id}: Home minimum`);
   await h.press('End');assert(Math.abs(await value()-Number(await h.getAttribute('aria-valuemax')))<=1,`${id}: End maximum`);
   await h.press('Enter');assert(Math.abs(await value()-reset)<=1,`${id}: Enter reset`);
   const drag=async(cancel=false)=>{
    await h.scrollIntoViewIfNeeded();const b=await h.boundingBox(),x=b.x+b.width/2,y=b.y+b.height/2;
    await page.mouse.move(x,y);await page.mouse.down();await page.mouse.move(x+(vertical?(negative?-35:35):0),y+(!vertical?(negative?-35:35):0),{steps:5});
    if(cancel)await page.keyboard.press('Escape');await page.mouse.up();
   };
   await drag(true);assert(Math.abs(await value()-reset)<=1,`${id}: Escape failed to restore drag start`);
   await drag();assert(Math.abs(await value()-reset-35)<=2,`${id}: pointer drag did not resize`);
   await h.dblclick();assert(Math.abs(await value()-reset)<=1,`${id}: double click reset`);
   assert(await h.evaluate(el=>el===document.activeElement),`${id}: splitter focus lost`);
   const paint=await h.evaluate(el=>({line:getComputedStyle(el,'::before').backgroundColor,after:getComputedStyle(el,'::after').content}));assert.notEqual(paint.line,'rgba(0, 0, 0, 0)');assert(['none','normal'].includes(paint.after),`${id}: old pill handle remained`);
   report.handles.push({area,id,orientation,baseline,reset,keyboard:'10/40, Home/End, Enter',pointer:'drag, Escape rollback, double-click reset'});
  }
  await go('develop');for(const id of ['resize-tree','resize-api','resize-log'])await check('develop',id);
  await go('simulate');await f('simulate').locator('#tree-toggle').click();await f('simulate').locator('#curves-toggle').click();
  const time=await f('simulate').locator('#time').inputValue();
  for(const id of ['tree-resize','inspector-resize','measure-resize','timeline-resize','plot-resize'])await check('simulate',id);
  assert.equal(await f('simulate').locator('#time').inputValue(),time,'Resizing changed simulation time');
  await go('analyze');for(const id of ['resize-analysis-left','resize-analysis-right','resize-analysis-chart'])await check('analyze',id);
  assert.equal(await f('analyze').locator('#statistics').innerText(),stats,'Resizing changed statistics');
  await f('analyze').locator('#resize-analysis-left').press('ArrowRight');const retained=await f('analyze').locator('#run-panel').evaluate(el=>el.clientWidth);
  await go('develop');assert.equal(await f('develop').locator('#editor').inputValue(),source);await go('analyze');assert.equal(await f('analyze').locator('#run-panel').evaluate(el=>el.clientWidth),retained);
  await f('analyze').locator('#toggle-left').click();assert(await f('analyze').locator('#resize-analysis-left').isHidden());await f('analyze').locator('#toggle-left').click();assert.equal(await f('analyze').locator('#run-panel').evaluate(el=>el.clientWidth),retained);
  for(const theme of ['dark','light']){
   if(await page.locator('html').getAttribute('data-theme')!==theme)await page.locator('#theme').click();
   for(const width of [1440,1080,760,500]){
    await page.setViewportSize({width,height:1000});
    for(const area of ['develop','simulate','analyze']){
     await go(area);const frame=f(area);assert(await frame.locator('body').evaluate(()=>document.documentElement.scrollWidth<=innerWidth+1),`${area}/${width} overflow`);
     const center=await frame.locator(area==='develop'?'#editor':area==='simulate'?'#scene':'.analysis-pane').boundingBox();assert(center&&center.width>=200,`${area}/${width} central area lost`);
     report.layouts.push({area,width,theme});
     if(width===1440)await page.screenshot({path:path.join(output,area+'-'+theme+'.png')});
    }
   }
  }
  assert.deepEqual(report.errors,[]);report.passed=true;
 }catch(error){await page.screenshot({path:path.join(output,'failure.png')});throw error;}
 finally{fs.writeFileSync(path.join(output,'results.json'),JSON.stringify(report,null,2));await browser.close();}
 console.log(JSON.stringify({passed:report.passed,handles:report.handles.length,layouts:report.layouts.length,errors:report.errors}));
})().catch(error=>{console.error(error);process.exitCode=1;});
