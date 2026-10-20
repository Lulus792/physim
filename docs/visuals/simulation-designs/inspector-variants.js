/* Interactive design alternatives; source edits/builds below are local demonstrations. */
(() => {
  'use strict';
  const $=id=>document.getElementById(id),app=document.querySelector('.app-window');
  const fmt=n=>{let s=String(n).replace('.',',');if(!s.includes(','))return s+',00';return s.split(',')[1].length===1?s+'0':s;};
  document.querySelectorAll('#inspector .group-content label[for]').forEach(label=>{
    const input=$(label.htmlFor);if(!input||input.type==='range'||label.closest('.data-field'))return;
    const control=input.closest('.field-with-action')||input;
    if(control.parentElement!==label.parentElement)return;
    const hint=label.nextElementSibling?.matches('small.field-hint')?label.nextElementSibling:null;
    const row=document.createElement('div');row.className='data-field';label.before(row);row.append(label,control);if(hint)row.append(hint);
  });
  $('run-group').open=false;
  function inspectorStyle(style){
    app.dataset.inspectorStyle=style;
    document.querySelectorAll('[data-inspector-style]').forEach(b=>{if(b.tagName==='BUTTON')b.setAttribute('aria-pressed',String(b.dataset.inspectorStyle===style));});
  }
  document.querySelectorAll('button[data-inspector-style]').forEach(b=>b.addEventListener('click',()=>inspectorStyle(b.dataset.inspectorStyle)));
  const requested=new URLSearchParams(location.search).get('inspectorStyle');inspectorStyle(['rows','fields','compact'].includes(requested)?requested:'fields');

  let builtModel={length:1.5,mass:1},building=false,compiled=false;
  for(const id of ['model-length','model-mass']){const e=$(id);e.type='text';e.inputMode='decimal';e.value=fmt(Number(e.value));}
  const globalStatus=document.createElement('span');globalStatus.id='model-global-status';globalStatus.hidden=true;document.querySelector('.app-status').append(globalStatus);
  $('model-change-title').setAttribute('aria-live','polite');
  function sourceValues(){return{length:Number($('model-length').value.replace(',','.')),mass:Number($('model-mass').value.replace(',','.'))};}
  function sourceValid(){
    const a=$('model-length'),b=$('model-mass'),v=sourceValues();
    const okA=a.value.trim()!==''&&Number.isFinite(v.length)&&v.length>=.1&&v.length<=5;
    const okB=b.value.trim()!==''&&Number.isFinite(v.mass)&&v.mass>=.01&&v.mass<=10;
    a.setAttribute('aria-invalid',String(!okA));b.setAttribute('aria-invalid',String(!okB));
    $('source-model-validation').textContent=okA&&okB?'':'Fadenlänge: 0,10–5,00 m. Masse: 0,01–10,00 kg.';
    return okA&&okB;
  }
  function sourceDirty(){const v=sourceValues();return v.length!==builtModel.length||v.mass!==builtModel.mass;}
  function buildState(){
    const valid=sourceValid(),dirty=sourceDirty(),mode=app.dataset.state,active=mode==='running'||mode==='paused';
    const bar=$('model-change-bar');bar.hidden=!(dirty||building||compiled);bar.classList.toggle('compiled',compiled&&!dirty);
    $('source-model-group').classList.toggle('source-pending',dirty);
    $('source-model-fields').disabled=building||mode==='empty'||mode==='error';
    $('compile-model').disabled=building||!valid||mode==='empty'||mode==='error';$('discard-model').disabled=building;$('discard-model').hidden=compiled&&!dirty;
    $('model-change-title').textContent=building?'Wird neu kompiliert …':dirty?'Modellwerte geändert':'Modell ist neu kompiliert';
    $('model-change-description').textContent=building?'Beispielvorschau wird vorbereitet.':dirty?'Die bisherigen Werte gelten bis zum nächsten Build und neuen Lauf.':'Die neuen Werte sind für einen neuen Lauf bereit.';
    $('compile-model').textContent=building?'Kompiliert …':dirty?(active?'Stoppen & neu kompilieren':'Neu kompilieren'):'Neuer Lauf';
    $('model-values-status').textContent='Kompilierter Stand: '+fmt(builtModel.length)+' m · '+fmt(builtModel.mass)+' kg';
    globalStatus.hidden=!(dirty||building||compiled);globalStatus.textContent=building?'Neukompilierung · Vorschau':dirty?'Modell geändert · Neukompilierung nötig':'Modell bereit für neuen Lauf';
  }
  for(const id of ['model-length','model-mass']){$(id).addEventListener('input',()=>{compiled=false;buildState();});$(id).addEventListener('blur',()=>{if(sourceValid()){$(id).value=fmt(sourceValues()[id==='model-length'?'length':'mass']);}});}
  $('discard-model').addEventListener('click',()=>{$('model-length').value=fmt(builtModel.length);$('model-mass').value=fmt(builtModel.mass);compiled=false;buildState();});
  $('compile-model').addEventListener('click',()=>{
    if(!sourceValid()||building)return;
    if(compiled&&!sourceDirty()){$('run-button').click();return;}
    if(['running','paused'].includes(app.dataset.state))$('stop-button').click();
    const snapshot=sourceValues();building=true;buildState();
    setTimeout(()=>{builtModel={...snapshot};building=false;compiled=true;document.dispatchEvent(new CustomEvent('physim-demo-model-built',{detail:{...snapshot}}));buildState();},450);
  });
  document.addEventListener('click',e=>{
    if(e.target.closest('#run-button')&&app.dataset.state==='ready'&&(sourceDirty()||building)){
      e.preventDefault();e.stopImmediatePropagation();if(app.classList.contains('inspector-closed'))$('inspector-toggle').click();$('source-model-group').open=true;
      if(sourceValid())$('compile-model').focus();else document.querySelector('#source-model-fields [aria-invalid=true]').focus();
    }
  },true);
  document.addEventListener('physim-demo-model-active',()=>{compiled=false;buildState();});
  document.addEventListener('physim-preview-model',e=>{
    builtModel={length:e.detail.length,mass:e.detail.mass};building=false;compiled=false;
    $('model-length').value=fmt(builtModel.length);$('model-mass').value=fmt(builtModel.mass);buildState();
  });
  new MutationObserver(buildState).observe(app,{attributes:true,attributeFilter:['data-state']});buildState();

  const sizes={left:190,right:320,workspace:660,timeline:120,measureClosed:150,measureOpen:330,plot:50};
  const initial={...sizes};
  const desktop=()=>innerWidth>650,unified=new URLSearchParams(location.search).get('unified')==='simulate';
  const workspace=document.querySelector('.workspace'),measure=$('measurements'),timeline=document.querySelector('.timeline');
  const handles={},splitters={};
  function geometry(){
    const w=workspace.clientWidth,leftVisible=!app.classList.contains('tree-closed'),rightVisible=!app.classList.contains('inspector-closed');
    let left=leftVisible?Math.min(sizes.left,Math.max(150,w-480)):0;
    let right=rightVisible?Math.min(sizes.right,Math.max(210,w-left-230)):0;
    if(w-left-right<210)left=Math.max(0,w-right-210);
    const height=unified&&workspace.clientHeight?workspace.clientHeight:sizes.workspace;
    return{left,right,w,height,measure:Math.min(app.classList.contains('curves-open')?sizes.measureOpen:sizes.measureClosed,Math.max(130,height-180))};
  }
  function update(){
    const g=geometry();app.style.setProperty('--left-size',g.left+'px');app.style.setProperty('--right-size',g.right+'px');
    app.style.setProperty('--workspace-height',sizes.workspace+'px');app.style.setProperty('--timeline-size',sizes.timeline+'px');app.style.setProperty('--measure-size',g.measure+'px');app.style.setProperty('--plot-split',sizes.plot+'%');
    $('tree-resize').hidden=app.classList.contains('tree-closed');$('inspector-resize').hidden=app.classList.contains('inspector-closed');$('plot-resize').hidden=!app.classList.contains('curves-open');
    for(const splitter of Object.values(splitters))splitter.sync();
  }
  const clamp=(v,min,max)=>Math.min(Math.max(v,min),Math.max(min,max));
  function grip(id,parent,axis,label,target){
    const e=document.createElement('div');e.id=id;handles[id]=e;e.setAttribute('aria-label',label);e.setAttribute('aria-controls',target.id);
    if(id==='measure-resize'||id==='timeline-resize')parent.prepend(e);else parent.append(e);
    function reset(){if(id==='tree-resize')sizes.left=initial.left;if(id==='inspector-resize')sizes.right=initial.right;if(id==='measure-resize'){sizes.measureClosed=initial.measureClosed;sizes.measureOpen=initial.measureOpen;}if(id==='timeline-resize'){sizes.workspace=initial.workspace;sizes.timeline=initial.timeline;}if(id==='workspace-resize')sizes.workspace=initial.workspace;if(id==='plot-resize')sizes.plot=initial.plot;update();}
    function limits(){
      const g=geometry(),plotWidth=Math.max(1,$('curves').clientWidth);
      const ranges={'tree-resize':[150,Math.min(420,g.w-g.right-230)],'inspector-resize':[210,Math.min(540,g.w-g.left-230)],'measure-resize':[130,Math.max(130,g.height-180)],'timeline-resize':[95,Math.min(280,Math.max(95,g.height+timeline.clientHeight-360))],'workspace-resize':[450,1100],'plot-resize':[plotWidth*.25-13,plotWidth*.75-13]};
      const [min,max]=ranges[id];return[min,Math.max(min,max)];
    }
    function value(){return id==='workspace-resize'?sizes.workspace:id==='plot-resize'?$('curves').clientWidth*sizes.plot/100-13:axis==='x'?target.getBoundingClientRect().width:target.getBoundingClientRect().height;}
    function set(requested){
      const [min,max]=limits(),v=clamp(requested,min,max);
      if(id==='tree-resize')sizes.left=v;if(id==='inspector-resize')sizes.right=v;
      if(id==='measure-resize')sizes[app.classList.contains('curves-open')?'measureOpen':'measureClosed']=v;
      if(id==='timeline-resize'){const total=geometry().height+timeline.clientHeight;sizes.timeline=v;sizes.workspace=total-v;}
      if(id==='workspace-resize')sizes.workspace=v;
      if(id==='plot-resize')sizes.plot=(v+13)/Math.max(1,$('curves').clientWidth)*100;
      update();
    }
    splitters[id]=PhysimResize.attach(e,{axis:()=>axis,limits,value,set,reset,enabled:desktop,
      sign:()=>['inspector-resize','measure-resize','timeline-resize'].includes(id)?-1:1,
      snapshot:()=>({...sizes}),restore:snapshot=>{Object.assign(sizes,snapshot);update();}});
  }
  timeline.id='timeline';workspace.id='simulation-workspace';
  $('curves').querySelector('.chart').id='simulation-first-curve';
  grip('tree-resize',workspace,'x','Szenenbaumbreite ändern',$('scene-tree'));grip('inspector-resize',workspace,'x','Inspektorbreite ändern',$('inspector'));
  grip('measure-resize',measure,'y','Messbereichshöhe ändern',measure);grip('timeline-resize',timeline,'y','Zeitleistenhöhe ändern',timeline);grip('plot-resize',$('curves'),'x','Breite der ersten Kurve ändern',$('curves').querySelector('.chart'));grip('workspace-resize',app,'y','Arbeitsflächenhöhe ändern',workspace);
  const note=document.createElement('p');note.className='resize-note';note.textContent='Trennlinien ziehen · Pfeile: 10 px · Umschalt: 40 px · Pos1/Ende: Grenzen · Enter/Doppelklick: Standard · Escape: Abbrechen';app.after(note);
  new MutationObserver(update).observe(app,{attributes:true,attributeFilter:['class','data-design']});
  const observer=new ResizeObserver(update);for(const pane of [workspace,measure,timeline,$('curves')])observer.observe(pane);
  addEventListener('resize',update);update();
})();
