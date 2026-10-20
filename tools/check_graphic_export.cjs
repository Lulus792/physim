// Export example: inspect actual SVG/PNG downloads, snapshot semantics and dialog interactions.
const {chromium}=require('playwright'),sharp=require('sharp'),assert=require('node:assert/strict'),fs=require('node:fs'),path=require('node:path'),{pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..'),out=path.join(root,'build/graphic-export-review');fs.mkdirSync(out,{recursive:true});
(async()=>{
 const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
 const report={date:new Date().toISOString(),platform:process.platform,browser:await browser.version(),checks:[],layouts:[],errors:[]};
 const page=await browser.newPage({viewport:{width:1440,height:1100}});page.setDefaultTimeout(10000);page.on('pageerror',e=>report.errors.push(e.message));
 try{
  const url=pathToFileURL(path.join(root,'docs/visuals/analysis-workspace/index.html')).href;
  await page.goto(url+'?example=export');assert(await page.locator('#graphic-dialog').isVisible());
  assert.equal(await page.locator('#graphic-preview .export-curve').count(),2);assert.match(await page.locator('#graphic-preview').innerText(),/Referenz/);
  await page.screenshot({path:path.join(out,'dialog-dark.png')});
  async function download(){const event=page.waitForEvent('download');await page.locator('#graphic-save').click();const d=await event;return{data:fs.readFileSync(await d.path()),name:d.suggestedFilename()};}
  let png=await download(),info=await sharp(png.data).metadata();assert.equal(info.width,2400);assert.equal(info.height,1700);
  assert.match(png.name,/024-023\.png$/);const pixel=await sharp(png.data).extract({left:0,top:0,width:1,height:1}).removeAlpha().raw().toBuffer();assert.deepEqual([...pixel],[255,255,255]);
  fs.writeFileSync(path.join(out,'example.png'),png.data);
  await page.locator('#graphic-size').selectOption('1200');png=await download();info=await sharp(png.data).metadata();assert.equal(info.width,1200);assert.equal(info.height,850);
  await page.locator('#graphic-size').selectOption('4800');png=await download();info=await sharp(png.data).metadata();assert.equal(info.width,4800);assert.equal(info.height,3400);
  report.checks.push('Real PNG downloads at 1200x850, 2400x1700 and 4800x3400; default white background');
  await page.locator('input[name=format][value=svg]').check();assert(await page.locator('#graphic-size').isDisabled());
  let svg=await download(),source=svg.data.toString('utf8');assert.match(source,/<svg xmlns=/);assert.equal((source.match(/class="export-curve"/g)||[]).length,2);assert(!source.includes('crosshair'));assert(!source.includes('Alles zeigen'));assert.match(source,/Radiant|rad/);assert.match(source,/2400|4800/);
  const svgInfo=await sharp(svg.data).metadata();assert(svgInfo.width>0);fs.writeFileSync(path.join(out,'example.svg'),svg.data);
  await page.keyboard.press('Escape');assert(await page.locator('#graphic-export').evaluate(el=>el===document.activeElement));
  const stats=await page.locator('#statistics').innerText();await page.locator('#zoom-in').click();await page.locator('#graphic-export').click();
  await page.locator('#graphic-scope').selectOption('visible');svg=await download();source=svg.data.toString('utf8');assert.match(source,/Sichtbarer Ausschnitt/);assert.match(source,/\[5,15\]/);assert.match(svg.name,/-ausschnitt.svg$/);
  assert.equal(await page.locator('#statistics').innerText(),stats);
  const before=await page.locator('#graphic-preview').innerHTML();
  // Underlying selection changes cannot silently alter the already opened export.
  await page.evaluate(()=>{const e=document.querySelector('#unit');e.value='deg';e.dispatchEvent(new Event('change'));});
  assert.equal(await page.locator('#graphic-preview').innerHTML(),before);
  await page.keyboard.press('Escape');await page.locator('#graphic-export').click();assert.match(await page.locator('#graphic-preview').innerText(),/deg/);
  report.checks.push('SVG preserves units, comparison legend, explicit full/crop range; open export snapshot survives source changes');
  await page.locator('#graphic-appearance').selectOption('current');svg=await download();assert.match(svg.data.toString('utf8'),/fill="#1b1c20"/);
  await page.locator('#graphic-close').click();await page.locator('button[data-theme=light]').click();await page.locator('#graphic-export').click();
  await page.locator('input[name=format][value=png]').check();await page.locator('#graphic-size').selectOption('2400');await page.screenshot({path:path.join(out,'dialog-light.png')});
  for(const width of [1440,900,500,390]){
   await page.setViewportSize({width,height:1000});const r=await page.locator('#graphic-dialog').boundingBox();assert(r.x>=0&&r.width<=width);assert(await page.locator('#graphic-dialog').evaluate(el=>el.scrollWidth<=el.clientWidth+1));
   await page.locator('#graphic-save').scrollIntoViewIfNeeded();assert(await page.locator('#graphic-save').isVisible());report.layouts.push({width,height:1000});
  }
  await page.setViewportSize({width:1440,height:1100});await page.locator('#graphic-close').click();
  // Exercise clipboard payload and denial paths without changing the user's OS clipboard.
  await page.evaluate(()=>{
   window.ClipboardItem=class{constructor(items){this.items=items;}};
   Object.defineProperty(navigator,'clipboard',{configurable:true,value:{write:async items=>{const blob=await items[0].items['image/png'];window.clipboardProof={type:blob.type,size:blob.size};}}});
  });
  await page.locator('#graphic-export').click();await page.locator('#graphic-copy').click();await page.waitForFunction(()=>document.querySelector('#graphic-feedback').textContent.includes('Zwischenablage kopiert'));
  const clip=await page.evaluate(()=>window.clipboardProof);assert.equal(clip.type,'image/png');assert(clip.size>1000);
  await page.evaluate(()=>{navigator.clipboard.write=()=>Promise.reject(Error('denied'));});await page.locator('#graphic-copy').click();await page.waitForFunction(()=>document.querySelector('#graphic-feedback').textContent.includes('Kopieren nicht möglich'));
  await page.keyboard.press('Escape');await page.locator('#scenario').selectOption('empty');assert(await page.locator('#graphic-export').isDisabled());
  report.checks.push('Clipboard receives generated PNG / denial fallback tested with isolated adapter; actual OS clipboard untouched; empty state disabled; Escape focus return and four widths');
  await page.goto(pathToFileURL(path.join(root,'docs/visuals/unified-workspace/index.html')).href+'#simulate');
  const sim=page.frameLocator('#frame-simulate'),analysis=page.frameLocator('#frame-analyze');await sim.locator('body[data-unified]').waitFor();
  await sim.locator('#step-button').click();await sim.locator('#shared-export').click();await analysis.locator('#graphic-export').click();assert(await analysis.locator('#graphic-dialog').isVisible());assert.equal(await analysis.locator('#graphic-preview .export-curve').count(),1);
  await analysis.locator('#graphic-close').click();assert(await analysis.locator('#graphic-export').evaluate(el=>el===document.activeElement));
  report.checks.push('Export dialog integrated with actual transferred simulation samples in unified workspace');
  assert.deepEqual(report.errors,[]);report.passed=true;
 }catch(error){await page.screenshot({path:path.join(out,'failure.png')});throw error;}
 finally{fs.writeFileSync(path.join(out,'results.json'),JSON.stringify(report,null,2));await browser.close();}
 console.log(JSON.stringify(report,null,2));
})().catch(error=>{console.error(error);process.exitCode=1;});
