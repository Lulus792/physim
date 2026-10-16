// Browser-only review of the accepted F1 design; no native application claims.
// Dependencies: playwright, sharp. NODE_PATH can point to the bundled runtime.
const {chromium} = require('playwright');
const sharp = require('sharp');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const {pathToFileURL} = require('node:url');
const root = path.resolve(__dirname,'..');
const design = path.join(root,'docs/visuals/f1-layouts');
const output = path.join(root,'build/f1-h-review');
const exporting = process.argv.includes('--export');
const states = [
  {left:false,right:false,name:'open'},
  {left:true,right:false,name:'left-closed'},
  {left:false,right:true,name:'right-closed'},
  {left:true,right:true,name:'both-closed'}
];
const themes = ['light','dark'];
fs.mkdirSync(output,{recursive:true});
const url = pathToFileURL(path.join(design,'index.html')).href;
function ratio(a,b) {
  const luminance = hex => {
    const values = hex.trim().slice(1).match(/../g).map(v=>parseInt(v,16)/255).map(v=>v<=.04045?v/12.92:((v+.055)/1.055)**2.4);
    return values[0]*.2126+values[1]*.7152+values[2]*.0722;
  };
  const x=luminance(a),y=luminance(b);
  return (Math.max(x,y)+.05)/(Math.min(x,y)+.05);
}
(async()=>{
 const browser = await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER ? {executablePath:process.env.PHYSIM_BROWSER} : process.platform==='win32' ? {channel:'msedge'} : {})});
 try {
  const page=await browser.newPage({viewport:{width:1400,height:1250},deviceScaleFactor:1});
  page.setDefaultTimeout(8000);
  const errors=[];page.on('pageerror',e=>errors.push(e.message));
  const report={browser:await browser.version(),platform:process.platform,states:[],contrast:[],errors};
  const geometry=()=>page.evaluate(()=>Object.fromEntries(['navigation','reading-pane','context-panel'].map(name=>{
    const element=document.querySelector('.'+name),r=element.getBoundingClientRect();
    return [name,{x:r.x,y:r.y,width:r.width,height:r.height,column:getComputedStyle(element).gridColumnStart}];
  })));
  async function setPanel(side,closed) {
    const button=page.locator(`[data-panel="${side}"]:visible`).first();
    if ((await button.getAttribute('aria-expanded')==='true')===closed) await button.click();
  }
  for(const theme of themes) {
   await page.goto(url+'?theme='+theme);
   const tokens=await page.evaluate(()=>{
     const css=getComputedStyle(document.documentElement);
     return Object.fromEntries(['ink','muted','paper','panel','selection','selection-text','accent'].map(k=>[k,css.getPropertyValue('--'+k).trim()]));
   });
   for(const pair of [['ink','paper'],['muted','paper'],['ink','panel'],['muted','panel'],['selection-text','selection']]) {
     const contrast=ratio(tokens[pair[0]],tokens[pair[1]]);
     assert(contrast>=4.5,`${theme}: contrast ${pair.join('/')} = ${contrast}`);
     report.contrast.push({theme,pair,ratio:+contrast.toFixed(2)});
   }
   for(const width of [1080,760]) for(const font of [16,32]) {
    await page.locator(`[data-width="${width}"]`).click();await page.locator(`[data-font="${font}"]`).click();
    for(const state of states) {
     await setPanel('left',state.left);await setPanel('right',state.right);
     const g=await geometry();
     assert(g['reading-pane'].width>=300,`Article disappeared or became unusable: ${JSON.stringify({theme,width,font,state,g})}`);
     assert.equal(g['reading-pane'].column,'2');
     assert.equal(await page.locator('#nav-content').isVisible(),!state.left);
     assert.equal(await page.locator('#context-content').isVisible(),!state.right);
     if(!state.right) {
       assert(g['context-panel'].width>=210);
       assert(g['context-panel'].x>=g['reading-pane'].x+g['reading-pane'].width-1,'Right pane overlaps reading pane');
     }
     if(!state.left) assert(g.navigation.x+g.navigation.width<=g['reading-pane'].x+1,'Navigation overlaps article');
     const overflow=await page.evaluate(()=>['.window','.workspace','.article-scroll','.nav-content','.context-content'].filter(s=>{
       const e=document.querySelector(s),r=e.getBoundingClientRect();return r.width>0&&r.height>0&&e.scrollWidth>e.clientWidth+1;
     }));
     assert.deepEqual(overflow,[]);
     report.states.push({theme,width,font,...state,articleWidth:g['reading-pane'].width});
    }
   }
   // Regression: not just aria-expanded; the other panel must keep its geometry.
   await page.goto(url+'?theme='+theme);const original=await geometry();
   await setPanel('left',true);let g=await geometry();
   assert(g['reading-pane'].width>original['reading-pane'].width);
   assert.equal(g['context-panel'].x,original['context-panel'].x);
   assert.equal(g['context-panel'].width,original['context-panel'].width);
   await setPanel('left',false);await setPanel('right',true);g=await geometry();
   assert.equal(g.navigation.x,original.navigation.x);assert.equal(g.navigation.width,original.navigation.width);
   await setPanel('right',false);
   await page.locator('[data-jump="workflow"]').click();const top=await page.locator('#article-scroll').evaluate(e=>e.scrollTop);
   for(const side of ['left','right']) await setPanel(side,true);
   for(const side of ['right','left']) await setPanel(side,false);
   assert(Math.abs(await page.locator('#article-scroll').evaluate(e=>e.scrollTop)-top)<2);
   await page.locator('#tab-example').click();await page.locator('[data-language="physim"]').click();
   assert((await page.locator('#workflow h2').textContent()).includes('Physim'));
   assert((await page.locator('#example-code').textContent()).includes('pendulumDerivative'));
   assert.equal(await page.locator('[data-language]').count(),2);
   await setPanel('left',true);await page.locator(`button[data-theme="${theme==='light'?'dark':'light'}"]`).click();
   assert.equal(await page.locator('.toolbar-left').getAttribute('aria-expanded'),'false');
   assert.equal(await page.locator('.toolbar-right').getAttribute('aria-expanded'),'true');
   assert.equal(await page.locator('#tab-example').getAttribute('aria-selected'),'true');
   assert.equal(await page.locator('[data-language="physim"]').getAttribute('aria-pressed'),'true');
   await page.locator('.toolbar-right').focus();await page.keyboard.press('Enter');
   assert.equal(await page.locator('.toolbar-right').getAttribute('aria-expanded'),'false');await page.keyboard.press('Space');
   assert.equal(await page.locator('.toolbar-right').getAttribute('aria-expanded'),'true');
   await page.locator('#tab-model').click();await page.keyboard.press('ArrowRight');
   assert.equal(await page.locator('#tab-example').getAttribute('aria-selected'),'true');
   await page.locator('#search').fill('not-found-123');assert(await page.locator('.empty').isVisible());
   assert.equal(await page.locator('.toolbar-left').getAttribute('aria-expanded'),'true');
   await page.locator('#search').fill('');
  }
  for(const width of [390,320]) {
   await page.setViewportSize({width,height:1000});await page.goto(url);
   assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
  }
  await page.setViewportSize({width:1400,height:1250});
  for(const theme of themes) for(const state of states) {
   await page.goto(url+`?theme=${theme}${state.left?'&left=closed':''}${state.right?'&right=closed':''}`);
   await page.evaluate(()=>Promise.all([...document.images].map(i=>i.decode())));
   const file=`h-${theme}${state.name==='open'?'':'-'+state.name}.png`;
   await page.locator('#window').screenshot({path:path.join(exporting?design:output,file)});
  }
  assert.deepEqual(errors,[]);
  report.result='PASS';
  report.behavior='32 layouts with positive article geometry; independent side anchoring; reading position; global language; theme retention; keyboard; search; narrow gallery';
  fs.writeFileSync(path.join(output,'result.json'),JSON.stringify(report,null,2));
  if(exporting) {
   const entries=[['h-light.png','H · Hell','Beide Seiten geöffnet'],['h-dark.png','H · Dunkel','Identisches Layout und identische Bedienung'],['h-dark-left-closed.png','Nur links geschlossen','Artikel wächst; rechter Modellbereich bleibt rechts'],['h-dark-right-closed.png','Nur rechts geschlossen','Artikel wächst; linke Navigation bleibt links']];
   const xml=s=>s.replaceAll('&','&amp;');
   const svg=`<svg xmlns="http://www.w3.org/2000/svg" width="1600" height="1460"><rect width="100%" height="100%" fill="#141518"/><g fill="#e1e3e9" font-family="Segoe UI,Arial,sans-serif"><text x="40" y="38" font-size="13" letter-spacing="2">PHYSIM / F1 / AUSGEWÄHLTES DESIGN H</text><text x="40" y="85" font-size="32" font-weight="600">Hell und Dunkel. Beide Seiten unabhängig.</text>${entries.map((e,i)=>{const x=40+i%2*792,y=140+Math.floor(i/2)*640;return `<text x="${x}" y="${y}" font-size="22" font-weight="600">${xml(e[1])}</text><text x="${x}" y="${y+25}" font-size="14" fill="#a6a9b1">${xml(e[2])}</text>`}).join('')}<text x="40" y="1435" font-size="13" fill="#a6a9b1">Feste Spalten: Navigation / Artikel / Kontext · Kleine getrennte Paneel-Icons · Globale C-/Physim-Wahl</text></g></svg>`;
   const images=[];for(let i=0;i<entries.length;i++) images.push({input:await sharp(path.join(design,entries[i][0])).resize({width:752}).png().toBuffer(),left:40+i%2*792,top:190+Math.floor(i/2)*640});
   await sharp(Buffer.from(svg)).composite(images).png().toFile(path.join(design,'h-final-overview.png'));
  }
  console.log(JSON.stringify({result:report.result,browser:report.browser,layoutStates:report.states.length,contrast:report.contrast,errors}));
 } finally {await browser.close();}
})().catch(error=>{console.error(error);process.exit(1)});
