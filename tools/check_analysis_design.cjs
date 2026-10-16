// Browser review of a design proposal; no native-app or screenreader certification.
const {chromium} = require('playwright');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const {pathToFileURL} = require('node:url');
const root=path.resolve(__dirname,'..');
const design=path.join(root,'docs/visuals/analysis-workspace');
const output=path.join(root,'build/analysis-design-review');
const exporting=process.argv.includes('--export');
fs.mkdirSync(output,{recursive:true});
(async()=>{
  const browser=await chromium.launch({headless:true,...(process.env.PHYSIM_BROWSER?{executablePath:process.env.PHYSIM_BROWSER}:process.platform==='win32'?{channel:'msedge'}:{})});
  try {
    const page=await browser.newPage({viewport:{width:1440,height:1100}});
    page.setDefaultTimeout(8000);
    const errors=[];page.on('pageerror',e=>errors.push(e.message));
    const result={browser:await browser.version(),platform:process.platform,layouts:[],contrast:[],interactions:[],errors};
    const open=()=>page.goto(pathToFileURL(path.join(design,'index.html')).href);
    await open();
    for(const width of [1440,1080,760,500]) for(const theme of ['dark','light']) {
      await page.setViewportSize({width,height:1100});
      await page.locator(`button[data-theme="${theme}"]`).click();
      assert.equal(await page.locator(`button[data-theme="${theme}"]`).getAttribute('aria-pressed'),'true');
      const colors=await page.evaluate(()=>{
        const style=getComputedStyle(document.documentElement),get=name=>style.getPropertyValue('--'+name).trim();
        return {text:get('text'),muted:get('muted'),window:get('window'),plot:get('plot'),header:get('header'),primary:get('primary'),onPrimary:get('on-primary')};
      });
      const luminance=hex=>{let h=hex.slice(1);if(h.length===3)h=h.split('').map(c=>c+c).join('');const c=h.match(/../g).map(v=>parseInt(v,16)/255).map(v=>v<=.04045?v/12.92:((v+.055)/1.055)**2.4);return c[0]*.2126+c[1]*.7152+c[2]*.0722;};
      const contrast=(a,b)=>{const x=luminance(a),y=luminance(b);return (Math.max(x,y)+.05)/(Math.min(x,y)+.05);};
      if(width===1440) for(const [fg,bg] of [['text','window'],['muted','header'],['muted','plot'],['onPrimary','primary']]) {
        const ratio=contrast(colors[fg],colors[bg]);assert(ratio>=4.5,`Low contrast ${theme} ${fg}/${bg}: ${ratio}`);result.contrast.push({theme,fg,bg,ratio});
      }
      const geometry=await page.evaluate(()=>({
        documentWidth:document.documentElement.scrollWidth,
        viewport:innerWidth,
        chart:document.querySelector('#chart').getBoundingClientRect().width,
        content:document.querySelector('.analysis-pane').getBoundingClientRect().width
      }));
      assert(geometry.documentWidth<=width+1,`Page overflow at ${width} ${theme}: ${JSON.stringify(geometry)}`);
      assert(geometry.chart>250,`Chart too narrow at ${width}`);
      result.layouts.push({width,theme,...geometry});
    }
    await page.setViewportSize({width:1440,height:1100});
    await page.locator('button[data-theme="dark"]').click();
    const stats=await page.locator('#statistics').innerText();
    const fullPath=await page.locator('#chart .curve').getAttribute('d');
    await page.locator('#zoom-in').click();
    assert.equal(await page.locator('#statistics').innerText(),stats,'Zoom changed full-run statistics');
    assert.notEqual(await page.locator('#chart .curve').getAttribute('d'),fullPath);
    await page.locator('#zoom-reset').click();
    await page.locator('#unit').selectOption('deg');
    assert.match(await page.locator('#statistics-unit').innerText(),/in deg/);
    assert.notEqual(await page.locator('#statistics').innerText(),stats);
    await page.locator('#unit').selectOption('rad');
    await page.locator('#run-list input[value="023"]').check();
    assert.equal(await page.evaluate(()=>document.activeElement.value),'023','Run selection lost keyboard focus');
    assert.equal(await page.locator('#chart .curve').count(),2);
    assert.equal(await page.locator('#statistics tr').count(),2);
    await page.locator('#analyze').click();
    await page.waitForFunction(()=>document.querySelector('#analyze').textContent==='2 Läufe vergleichen');
    assert.match(await page.locator('#report-panel').innerText(),/Ergebnisse aller ausgewählten Läufe/);
    await page.locator('#run-list input[value="023"]').uncheck();
    assert.match(await page.locator('#report-panel').innerText(),/Laufauswahl wurde geändert/);
    await page.locator('#tab-data').click();
    await page.locator('#tab-data').press('ArrowRight');
    assert.equal(await page.locator('#tab-report').getAttribute('aria-selected'),'true');
    await page.locator('#tab-report').press('Home');
    assert.equal(await page.locator('#tab-data').getAttribute('aria-selected'),'true');
    await page.locator('#code-open').click();
    assert(await page.locator('#code-dialog').isVisible());
    await page.keyboard.press('Escape');
    assert.equal(await page.evaluate(()=>document.activeElement.id),'code-open');
    for(const theme of ['light','dark']) {
      await page.locator(`button[data-theme="${theme}"]`).click();
      for(const [left,right] of [[false,false],[true,false],[true,true],[false,true]]) {
        for(const [side,closed] of [['left',left],['right',right]]) {
          const toggle=page.locator('#toggle-'+side);
          if((await toggle.getAttribute('aria-expanded')==='false')!==closed)await toggle.click();
        }
        assert.equal(await page.locator('#run-panel').isVisible(),!left);
        assert.equal(await page.locator('#inspector').isVisible(),!right);
        assert((await page.locator('.analysis-pane').boundingBox()).width>500);
      }
    }
    result.interactions.push('zoom preserves full-run statistics','unit conversion updates curve and statistics','run comparison','analysis completion','report preserves source selection','keyboard tabs','dialog Escape and focus return','independent side panels');
    await open();
    const download=page.waitForEvent('download');await page.locator('#export-csv').click();
    const downloaded=await download;const downloadPath=await downloaded.path();
    const csv=fs.readFileSync(downloadPath,'utf8');assert.equal(csv.split('\r\n').length,1002);assert.match(csv,/run,time_s,theta_rad/);
    result.interactions.push('CSV contains all 1001 sample rows');
    for(const channel of ['theta','omega','energy']) {
      await page.locator('#channel').selectOption(channel);assert.equal(await page.locator('#chart .curve').count(),1);
      assert(!/NaN|Infinity/.test(await page.locator('#statistics').innerText()));
    }
    await page.locator('#channel').selectOption('theta');
    for(const name of ['empty','loading','error']) {
      await page.locator('#scenario').selectOption(name);
      assert(await page.locator('#state-panel').isVisible());assert(await page.locator('#analyze').isDisabled());
      await page.locator('#tab-report').click();assert(!(await page.locator('#report-panel').isVisible()),'Report appeared over unavailable data');
      await page.locator('#tab-data').click();assert(!(await page.locator('#data-panel').isVisible()),'Stale data appeared over state panel');
      await page.locator('#state-panel button').click();assert(await page.locator('#data-panel').isVisible());
    }
    await page.locator('#search-runs').fill('999');assert.match(await page.locator('#run-list').innerText(),/Kein passender/);
    await page.locator('#search-runs').fill('');
    await page.locator('#analyze').click();await page.locator('#analyze').click();
    assert.match(await page.locator('#status').innerText(),/abgebrochen/);
    result.interactions.push('all channels','empty/loading/error recovery','run search without results','analysis cancellation');
    // Simulate browser text enlargement without changing viewport geometry.
    await page.addStyleTag({content:'body{zoom:2}'});
    assert(await page.locator('#analyze').isVisible());
    await open();
    for(const theme of ['dark','light']) {
      await page.locator(`button[data-theme="${theme}"]`).click();
      await page.locator('#app').screenshot({path:path.join(exporting?design:output,`analysis-${theme}.png`)});
    }
    await page.locator('button[data-theme="dark"]').click();
    await page.locator('#scenario').selectOption('compare');
    await page.locator('#app').screenshot({path:path.join(exporting?design:output,'analysis-comparison.png')});
    await page.locator('#analyze').click();
    await page.waitForFunction(()=>document.querySelector('#analyze').textContent==='2 Läufe vergleichen');
    await page.locator('#app').screenshot({path:path.join(exporting?design:output,'analysis-report.png')});
    await page.setViewportSize({width:760,height:1100});
    await page.locator('#toggle-right').click();
    assert(await page.locator('#inspector').isVisible());
    await page.locator('#toggle-left').click();
    assert(await page.locator('#run-panel').isVisible());
    await page.locator('#toggle-left').click();
    assert(await page.locator('.analysis-pane').isVisible());
    await page.setViewportSize({width:1440,height:1100});
    for(const side of ['left','right']) if(await page.locator('#toggle-'+side).getAttribute('aria-expanded')==='true')await page.locator('#toggle-'+side).click();
    await page.setViewportSize({width:760,height:1100});
    await page.locator('#toggle-right').click();
    assert((await page.locator('#inspector').boundingBox()).width>200,'Details lost after resizing with closed panels');
    await page.locator('#toggle-left').click();
    assert((await page.locator('#run-panel').boundingBox()).width>200,'Run list lost after resizing with closed panels');
    result.interactions.push('compact panel navigation','200% enlargement smoke check');
    assert.deepEqual(errors,[]);
    fs.writeFileSync(path.join(output,'review.json'),JSON.stringify(result,null,2)+'\n');
    console.log(JSON.stringify(result,null,2));
  } finally {await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
