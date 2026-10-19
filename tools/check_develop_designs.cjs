// Review the browser design studies, not the native Physim application.
const {chromium}=require('playwright');
const sharp=require('sharp');
const assert=require('node:assert/strict');
const fs=require('node:fs');
const path=require('node:path');
const {pathToFileURL}=require('node:url');
const root=path.resolve(__dirname,'..');
const design=path.join(root,'docs/visuals/develop-designs');
const output=path.join(root,'build/develop-design-review');
const exporting=process.argv.includes('--export');
fs.mkdirSync(output,{recursive:true});
const luminance=hex=>{const c=hex.slice(1).match(/../g).map(v=>parseInt(v,16)/255).map(v=>v<=.04045?v/12.92:((v+.055)/1.055)**2.4);return c[0]*.2126+c[1]*.7152+c[2]*.0722;};
const contrast=(a,b)=>{const x=luminance(a),y=luminance(b);return(Math.max(x,y)+.05)/(Math.min(x,y)+.05);};
(async()=>{
  const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
  try {
    const page=await browser.newPage({viewport:{width:1440,height:1100}});
    page.setDefaultTimeout(8000);
    const errors=[];page.on('pageerror',e=>errors.push(e.message));
    const result={browser:await browser.version(),platform:process.platform,layouts:[],contrast:[],interactions:[],errors};
    const open=()=>page.goto(pathToFileURL(path.join(design,'index.html')).href);
    await open();
    for(const width of [1440,1080,760,500]) for(const theme of ['dark','light']) for(const variant of ['workbench','atelier','focus']) {
      await page.setViewportSize({width,height:1100});
      await page.locator(`button[data-theme="${theme}"]`).click();
      await page.locator(`button[data-design="${variant}"]`).click();
      const geometry=await page.evaluate(()=>({viewport:innerWidth,document:document.documentElement.scrollWidth,editor:document.querySelector('#editor').getBoundingClientRect().width}));
      assert(geometry.document<=width+1,`Page overflow: ${width}/${theme}/${variant}: ${JSON.stringify(geometry)}`);
      assert(geometry.editor>=200,`Editor too narrow: ${JSON.stringify(geometry)}`);
      result.layouts.push({width,theme,variant,...geometry});
      if(width===1440&&variant==='workbench') {
        const colors=await page.evaluate(()=>{const s=getComputedStyle(document.documentElement);return Object.fromEntries(['text','muted','edit','window','header','accent','comment','primary','on-primary','keyword','literal','number','selection','good','error'].map(k=>[k,s.getPropertyValue('--'+k).trim()]));});
        for(const [fg,bg] of [['text','edit'],['muted','window'],['muted','header'],['accent','edit'],['comment','edit'],['keyword','edit'],['literal','edit'],['number','edit'],['on-primary','primary'],['text','selection'],['good','window'],['error','window']]) {
          const ratio=contrast(colors[fg],colors[bg]);assert(ratio>=4.5,`Low contrast ${theme}/${fg}/${bg}: ${ratio}`);result.contrast.push({theme,fg,bg,ratio});
        }
      }
    }
    await page.setViewportSize({width:1440,height:1100});
    await open();
    const source=await page.locator('#editor').inputValue();
    await page.locator('.file-tabs [data-file="analysis.c"]').click();
    assert.match(await page.locator('#editor').inputValue(),/Analysequelle/);
    await page.locator('#editor').fill('/* draft */\n<script>alert(1)</script>');
    assert.equal(await page.locator('#highlight script').count(),0,'User text became executable HTML');
    await page.locator('.file-tabs [data-file="main.c"]').click();
    assert.equal(await page.locator('#editor').inputValue(),source);
    await page.locator('.file-tabs [data-file="analysis.c"]').click();
    assert.match(await page.locator('#editor').inputValue(),/draft/);
    await page.keyboard.press('Control+s');
    assert.match(await page.locator('#save-status').innerText(),/Alle Änderungen/);
    result.interactions.push('per-file edits survive navigation; safe syntax highlighting; save clears dirty state');
    await page.locator('.file-tabs [data-file="main.c"]').click();
    await page.keyboard.press('Control+f');
    assert(await page.locator('#query').evaluate(el=>el===document.activeElement));
    await page.locator('#query').fill('length_m');await page.locator('#find-next').click();
    assert.equal(await page.locator('#editor').evaluate(el=>el.value.slice(el.selectionStart,el.selectionEnd)),'length_m');
    await page.locator('#replacement').fill('pendulum_length');await page.locator('#replace').click();
    assert.match(await page.locator('#editor').inputValue(),/pendulum_length/);
    await page.keyboard.press('Escape');assert(await page.locator('#search-open').evaluate(el=>el===document.activeElement));
    result.interactions.push('keyboard search, selection, replace, Escape and focus return');
    await page.locator('#toggle-tree').click();assert(!(await page.locator('#explorer').isVisible()));
    await page.locator('button[data-design="atelier"]').click();assert(await page.locator('#explorer').isVisible());
    await page.locator('button[data-design="workbench"]').click();assert(!(await page.locator('#explorer').isVisible()));
    await page.locator('button[data-design="focus"]').click();
    assert(!(await page.locator('#inspector').isVisible()));assert(!(await page.locator('#log-body').isVisible()));
    await page.locator('#toggle-api').click();await page.locator('#insert-example').click();
    assert.match(await page.locator('#editor').inputValue(),/angle.demo/);
    result.interactions.push('independent per-variant panels and insertion of API example');
    await page.locator('#scenario').selectOption('error');
    await page.locator('#diagnostic-link').click();
    assert(await page.locator('#editor').evaluate(el=>el===document.activeElement));
    assert.equal(await page.locator('#editor').evaluate(el=>el.value.slice(el.selectionStart,el.selectionEnd)),'gravity_m_s');
    await page.locator('#editor').fill((await page.locator('#editor').inputValue()).replace('-gravity_m_s /','-gravity_m_s2 /'));
    await page.keyboard.press('F5');assert.match(await page.locator('#build-status').innerText(),/Build läuft/);
    assert(await page.locator('#editor').evaluate(el=>el.readOnly));
    await page.waitForFunction(()=>document.querySelector('#build-status').textContent.includes('erfolgreich'));
    assert.equal(await page.locator('#editor').evaluate(el=>el.readOnly),false);
    await page.keyboard.press('F5');await page.locator('#menu-build').click();await page.locator('#cancel').click();
    assert.match(await page.locator('#result-title').innerText(),/abgebrochen/);
    await page.locator('#menu-build').click();await page.keyboard.press('Escape');assert(await page.locator('#menu-build').evaluate(el=>el===document.activeElement));
    await page.locator('#profile').selectOption('Release');assert.match(await page.locator('#build-status').innerText(),/Noch nicht gebaut/);
    result.interactions.push('compiler error navigates to token; correction and build; build cancellation; menu Escape focus; profile invalidates build');
    for(const scenario of ['empty','folder']) {
      await page.locator('#scenario').selectOption(scenario);assert(await page.locator('#empty-state').isVisible());
      await page.keyboard.press('F5');assert(await page.locator('#empty-state').isVisible());
    }
    await page.locator('#open-folder').click();assert(await page.locator('#editor').isVisible());
    await page.locator('#editor').fill('ordinary text');assert.match(await page.locator('#build-status').innerText(),/Kein Physim/);
    await page.keyboard.press('F5');assert.match(await page.locator('#build-status').innerText(),/Kein Physim/);
    result.interactions.push('empty workspace and arbitrary folder block project builds while allowing text edits');
    await open();
    await page.emulateMedia({reducedMotion:'reduce',forcedColors:'active'});
    assert.equal(await page.locator('#highlight').evaluate(el=>getComputedStyle(el).visibility),'hidden');
    assert(await page.locator('#editor').isVisible());
    await page.emulateMedia({forcedColors:'none'});
    await page.addStyleTag({content:'button,input,select,summary,.context-body p,.build-result p,.build-result strong{font-size:200%!important}'});
    assert.equal(await page.locator('#build').count(),1);
    assert(await page.locator('#search-open').isVisible());
    assert((await page.locator('#editor').boundingBox()).width>200);
    result.interactions.push('forced-colors editable text fallback; reduced-motion; enlarged control/text smoke check');
    for(const scale of [1.5,2]) {
      const context=await browser.newContext({viewport:{width:1440,height:1100},deviceScaleFactor:scale});const scaled=await context.newPage();
      await scaled.goto(pathToFileURL(path.join(design,'index.html')).href);
      assert((await scaled.locator('#editor').boundingBox()).width>400);
      await context.close();
    }
    result.interactions.push('1.5x and 2x browser device scale layout smoke checks');
    await open();
    for(const variant of ['workbench','atelier','focus'])for(const theme of ['dark','light']) {
      await page.locator(`button[data-design="${variant}"]`).click();await page.locator(`button[data-theme="${theme}"]`).click();
      await page.locator('.app-window').screenshot({path:path.join(exporting?design:output,`${variant}-${theme}.png`)});
    }
    await page.locator('#scenario').selectOption('error');
    await page.locator('.app-window').screenshot({path:path.join(exporting?design:output,'compiler-error.png')});
    if(exporting) {
      const tiles=[];let row=0;
      for(const [variant,label] of [['workbench','01 / Werkbank'],['atelier','02 / API-Atelier'],['focus','03 / Fokus']]) {
        const heading=Buffer.from(`<svg width="1248" height="40"><rect width="100%" height="100%" fill="#141518"/><text x="8" y="26" fill="#edeef2" font-family="Segoe UI, sans-serif" font-size="17">${label}</text><text x="1100" y="26" fill="#a6a9b1" font-family="Segoe UI, sans-serif" font-size="12">Dunkel / Hell</text></svg>`);
        tiles.push({input:heading,left:0,top:row*440});
        let col=0;
        for(const theme of ['dark','light']) {
          const input=await sharp(path.join(design,`${variant}-${theme}.png`)).resize({width:616,height:388,fit:'contain',background:'#141518'}).png().toBuffer();
          tiles.push({input,left:col*632,top:row*440+44});col++;
        }
        row++;
      }
      await sharp({create:{width:1248,height:1320,channels:4,background:'#141518'}}).composite(tiles).png().toFile(path.join(design,'overview.png'));
    }
    assert.deepEqual(errors,[]);
    fs.writeFileSync(path.join(output,'review.json'),JSON.stringify(result,null,2)+'\n');
    console.log(JSON.stringify({browser:result.browser,platform:result.platform,layouts:result.layouts.length,contrasts:result.contrast.length,interactions:result.interactions,errors,exporting},null,2));
  } finally {await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
