/* Opt-in adapter: standalone references keep their original behavior. */
(() => {
  'use strict';
  const area=new URLSearchParams(location.search).get('unified');
  if(parent===window||!['develop','simulate','analyze'].includes(area))return;
  const $=id=>document.getElementById(id),app=document.querySelector('.app-window');
  document.body.dataset.unified=area;
  const link=document.createElement('link');link.rel='stylesheet';link.href='../unified-workspace/embed.css';document.head.append(link);
  // Shared semantic palette; scene and curve-specific colors remain in the selected designs.
  const palette=document.createElement('style');palette.textContent=`:root{--text:#edeef2;--muted:#a6a9b1;--window:#1e1f23;--header:#25262b;--border:#3b3d45;--edit:#191a1e;--button:#35363e;--hover:#42444e;--accent:#8abaff;--primary:#1965ce;--on-primary:#fff;--canvas:#141518;--panel:#1e1f23;--surface:#303136;--selection:#2b3d59;--selected:#2b3d59;--good:#8bcca5;--error:#ffaaa2}:root[data-theme=light]{--text:#20242c;--muted:#555e6c;--window:#f6f7f9;--header:#edeff3;--border:#bec3ce;--edit:#fff;--button:#e0e3ea;--hover:#d5dae4;--accent:#17569e;--primary:#1556ac;--on-primary:#fff;--canvas:#e6e9ee;--panel:#f6f7f9;--surface:#e0e3ea;--selection:#d9e6f8;--selected:#d9e6f8;--good:#286240;--error:#a52925}`;document.head.append(palette);
  const emit=(type,payload={})=>parent.postMessage({protocol:'physim-preview-v1',type,...payload},'*');
  const command=detail=>document.dispatchEvent(new CustomEvent('physim-preview-command',{detail}));
  function click(id){$(id)?.click();}
  let available=true;
  window.addEventListener('message',event=>{
    const m=event.data;if(event.source!==parent||!m||m.protocol!=='physim-preview-v1')return;
    if(m.type==='theme')document.documentElement.dataset.theme=m.theme==='light'?'light':'dark';
    if(m.type==='visible')command(m);
    if(m.type==='resize')window.dispatchEvent(new Event('resize'));
    if(m.type==='protocol-visible')document.body.classList.toggle('global-protocol-open',!!m.value);
    if(m.type==='diagnostic-source'&&area==='develop'){
      if(!Object.hasOwn(files,m.file))return;
      openFile(m.file,true);const at=editor.value.indexOf(m.token);
      if(at>=0){editor.setSelectionRange(at,at+m.token.length);editor.scrollTop=Math.max(0,(editor.value.slice(0,at).split('\n').length-4)*parseFloat(getComputedStyle(editor).lineHeight));syncScroll();cursor();}
    }
    if(m.type==='availability')available=!!m.ready;
    if(m.type==='opaque'&&area==='develop'){$('reduce-transparency').checked=m.value;$('reduce-transparency').dispatchEvent(new Event('change'));}
    if(area==='develop'){
      if(m.type==='save')save();if(m.type==='build')build();if(m.type==='cancel')click('cancel');if(m.type==='search')search(true);if(m.type==='open-analysis')openFile('analysis.c',true);
      if(m.type==='left')click('toggle-tree');if(m.type==='right')click('toggle-api');
      if(m.type==='runs'){
        const container=[...document.querySelectorAll('.file-tree details')].find(e=>e.querySelector('summary').textContent.trim()==='runs');
        if(container){container.querySelectorAll('.tree-note,.session-run').forEach(e=>e.remove());for(const run of m.runs){const button=document.createElement('button');button.className='file session-run';button.textContent='Lauf '+run.id;button.onclick=()=>emit('navigate',{area:'analyze',run:run.id});container.append(button);}}
      }
      if(m.type==='edit-model'){
        const length=Number(m.model?.length),mass=Number(m.model?.mass);
        if(!Number.isFinite(length)||length<.1||length>5||!Number.isFinite(mass)||mass<.01||mass>10)return;
        let source=files['main.c'].replace(/(static\s+const\s+double\s+length_m\s*=\s*)[^;]+;/,(_,prefix)=>prefix+length+';');
        if(/static\s+const\s+double\s+mass_kg\s*=/.test(source))source=source.replace(/(static\s+const\s+double\s+mass_kg\s*=\s*)[^;]+;/,(_,prefix)=>prefix+mass+';');
        else source=source.replace('static const double length_m',`static const double mass_kg = ${mass};\nstatic const double length_m`);
        files['main.c']=source;openFile('main.c');editor.value=source;edited();save();build();
      }
    }else{
      if(m.type==='left')click(area==='simulate'?'tree-toggle':'toggle-left');
      if(m.type==='right')click(area==='simulate'?'inspector-toggle':'toggle-right');
      if(m.type==='curves')click('curves-toggle');
      if(['model','runs','export'].includes(m.type))command(m);
    }
  });
  document.addEventListener('physim-preview-output',e=>emit(e.detail.type,e.detail));
  document.addEventListener('keydown',e=>{
    if(e.altKey&&['1','2','3'].includes(e.key)){e.preventDefault();e.stopImmediatePropagation();emit('navigate',{area:['develop','simulate','analyze'][Number(e.key)-1]});}
    if(e.key==='F5'||((e.ctrlKey||e.metaKey)&&['s','f'].includes(e.key.toLowerCase()))){e.preventDefault();e.stopImmediatePropagation();emit('shortcut',{action:e.key==='F5'?'build':e.key.toLowerCase()==='s'?'save':'search'});}
  },true);
  if(area==='develop'){
    function model(){
      const source=files['main.c'].replace(/\/\*[\s\S]*?\*\/|\/\/[^\n]*/g,'');
      function constant(name,fallback){const matches=[...source.matchAll(new RegExp('static\\s+const\\s+double\\s+'+name+'\\s*=\\s*([+-]?(?:\\d+(?:\\.\\d*)?|\\.\\d+)(?:e[+-]?\\d+)?)\\s*;','gi'))];return matches.length===1?Number(matches[0][1]):matches.length===0&&fallback!==undefined&&!new RegExp('\\b'+name+'\\b').test(source)?fallback:NaN;}
      const value={length:constant('length_m'),mass:constant('mass_kg',1),gravity:constant('gravity_m_s2'),amplitude:constant('initial_angle_rad')};
      return Number.isFinite(value.length)&&value.length>=.1&&value.length<=5&&Number.isFinite(value.mass)&&value.mass>=.01&&value.mass<=10&&Number.isFinite(value.gravity)&&value.gravity>0&&value.gravity<=30&&Number.isFinite(value.amplitude)&&Math.abs(value.amplitude)<=1?value:null;
    }
    let last='';
    function publish(){
      const current=model();
      // This validates only the explicit example contract, never arbitrary C.
      if(scenario==='ready'&&!current){scenario='error';updateBuildUI();$('result-detail').textContent='Vorschau: Modellkonstanten als einzelne Zahlen angeben. Länge 0,1–5 m; Masse 0,01–10 kg; 0 < g ≤ 30; |Winkel| ≤ 1 rad.';}
      const payload={scenario,dirty:dirty(),files:{...files},model:current,error:scenario==='error'?$('result-detail').textContent:null};
      const key=JSON.stringify(payload);if(key!==last){last=key;emit('develop-state',payload);}
    }
    new MutationObserver(publish).observe($('build-status'),{childList:true,subtree:true});
    new MutationObserver(publish).observe($('save-status'),{childList:true,subtree:true});
    editor.addEventListener('input',publish);$('save').addEventListener('click',publish);
    // Original buttons and keyboard paths also keep the coordinator up to date.
    document.addEventListener('click',()=>queueMicrotask(publish));
    emit('ready');publish();
  }
  if(area==='simulate'){
    document.querySelector('.experiment-name small').textContent='Pendelstudie / Vorschau-Lauf';
    const button=document.createElement('button');button.id='shared-export';button.className='shared-export';button.textContent='Messlauf auswerten →';button.onclick=()=>command({type:'export'});document.querySelector('.panel-toolbar').append(button);
    document.addEventListener('click',e=>{
      if(e.target.closest('#open-log')){e.preventDefault();e.stopImmediatePropagation();emit('action',{action:'protocol'});return;}
      if(e.target.closest('#run-button,#step-button')&&app.dataset.state==='ready'&&!available){e.preventDefault();e.stopImmediatePropagation();emit('status',{message:'Zuerst die geänderte Vorschau in Entwickeln bauen.'});}
      if(e.target.closest('#compile-model')){
        e.preventDefault();e.stopImmediatePropagation();
        const model={length:Number($('model-length').value.replace(',','.')),mass:Number($('model-mass').value.replace(',','.'))};
        if(!Number.isFinite(model.length)||model.length<.1||model.length>5||!Number.isFinite(model.mass)||model.mass<.01||model.mass>10){emit('status',{message:'Modellwerte prüfen: Länge 0,1–5 m, Masse 0,01–10 kg.'});return;}
        if(app.dataset.state==='running'||app.dataset.state==='paused')click('stop-button');
        emit('inspector-model',{model});
      }
    },true);
    emit('ready');
  }
  if(area==='analyze'){
    $('code-open').onclick=()=>emit('navigate',{area:'develop',file:'analysis.c'});
    $('import').onclick=()=>emit('navigate',{area:'simulate'});$('import').textContent='Zur Simulation →';
    document.querySelector('.list-caption').textContent='DIESE SITZUNG';
    const source=document.querySelectorAll('.inspector-section')[2];source.querySelectorAll('dd')[0].textContent='Einfaches Pendel';source.querySelectorAll('dd')[1].textContent='C · Vorschau';source.querySelectorAll('dd')[2].textContent='analysis.c';
    document.addEventListener('click',e=>{if(e.target.closest('[data-action=load]')){e.preventDefault();e.stopImmediatePropagation();emit('navigate',{area:'simulate'});}},true);
    emit('ready');
  }
})();
