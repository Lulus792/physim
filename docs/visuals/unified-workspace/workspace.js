/* Local preview coordinator. No native build, runner, network or filesystem writes. */
(() => {
  'use strict';
  const $=id=>document.getElementById(id),areas=['develop','simulate','analyze'];
  const state={area:'develop',theme:'dark',ready:new Set(),build:'ready',dirty:false,files:null,model:null,runs:[],runId:0};
  const labels={develop:'Werkbank',simulate:'Messlabor',analyze:'Auswertung'};
  function send(area,type,payload={}){if(state.ready.has(area))$('frame-'+area).contentWindow.postMessage({protocol:'physim-preview-v1',type,...payload},'*');}
  function announce(message){$('global-status').textContent=message;}
  function navigate(area,focus=false){
    if(!areas.includes(area))area='develop';state.area=area;
    for(const name of areas){const active=name===area;$('panel-'+name).hidden=!active;$('tab-'+name).setAttribute('aria-selected',String(active));$('tab-'+name).tabIndex=active?0:-1;send(name,'visible',{visible:active});}
    $('area-label').textContent=labels[area];
    if(location.hash!=='#'+area)history.replaceState(null,'','#'+area);
    send(area,'resize');if(focus)$('tab-'+area).focus();render();
  }
  function render(){
    $('dirty-mark').textContent=state.dirty?'●':'';
    $('run-count').hidden=!state.runs.length;$('run-count').textContent=state.runs.length;
    $('workflow-state').textContent=state.build==='building'?'Vorschau-Build läuft …':state.build==='error'?'Build prüfen · Entwickeln':state.build!=='ready'?'Modell geändert · Vorschau neu bauen':`${state.runs.length} Messläufe · ${state.model?'Modell bereit':'Wird vorbereitet'}`;
    const next=$('next-action');next.disabled=!state.ready.has(state.area);
    next.textContent=state.area==='develop'?(state.build==='ready'?'Zur Simulation →':'Vorschau bauen →'):state.area==='simulate'?'Messlauf auswerten →':'Zum Modell →';
    for(const area of ['simulate'])send(area,'availability',{ready:state.build==='ready'});
  }
  function build(){send('develop','build');announce('Vorschau-Build angefordert · bekannte Modellkonstanten werden geprüft');}
  function action(name){
    if(name==='protocol'){window.PhysimProtocol.open();return;}
    if(name==='help'){$('help').showModal();return;}
    if(name==='save'){send('develop','save');announce('Demodateien für diese Sitzung gespeichert');return;}
    if(name==='build'){build();return;}
    if(name==='cancel'){send('develop','cancel');return;}
    if(name==='search'||name==='analysis-code'){navigate('develop');send('develop',name==='search'?'search':'open-analysis');return;}
    if(name==='curves'){navigate('simulate');send('simulate','curves');return;}
    if(['left','right'].includes(name)){send(state.area,name);return;}
    if(name==='download'){
      if(!state.files){announce('Die Werkbank wird noch geladen.');return;}
      const blob=new Blob([JSON.stringify({format:'physim-design-preview-v1',files:state.files,model:state.model,runs:state.runs},null,2)],{type:'application/json'}),url=URL.createObjectURL(blob),a=document.createElement('a');
      a.href=url;a.download='pendelstudie-designvorschau.json';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);announce('Projektkopie mit Demodateien und Messläufen heruntergeladen');
    }
  }
  window.addEventListener('message',event=>{
    const area=areas.find(name=>$('frame-'+name).contentWindow===event.source),m=event.data;
    if(!area||!m||m.protocol!=='physim-preview-v1')return;
    if(m.type==='ready'){
      state.ready.add(area);send(area,'theme',{theme:state.theme});send(area,'visible',{visible:state.area===area});
      send(area,'protocol-visible',{value:window.PhysimProtocol.isOpen()});
      if(area==='simulate'&&state.model)send(area,'model',{model:state.model});
      if(area==='analyze')send(area,'runs',{runs:state.runs,select:null});
      if(area==='develop')send(area,'runs',{runs:state.runs});
      render();return;
    }
    if(m.type==='develop-state'&&area==='develop'){
      window.PhysimProtocol.recordBuild(m);
      const previous=state.build;state.build=m.scenario;state.dirty=m.dirty;state.files=m.files;
      if(m.scenario==='ready'&&m.model){
        const changed=JSON.stringify(state.model)!==JSON.stringify(m.model);state.model=m.model;
        if(changed||previous!=='ready')send('simulate','model',{model:m.model});
      }
      if(m.scenario==='error')announce(m.error||'Vorschau-Build fehlgeschlagen · Diagnose in Entwickeln');
      else if(previous==='building'&&m.scenario==='ready')announce('Vorschau gebaut · neues Modell gilt ab dem nächsten Lauf');
      render();return;
    }
    if(m.type==='inspector-model'&&area==='simulate'){
      send('develop','edit-model',{model:m.model});announce('Modellwerte in main.c übernommen · Vorschau wird gebaut');return;
    }
    if(m.type==='run'&&area==='simulate'){
      const run=m.run;if(!run||!Array.isArray(run.data)||run.data.length<2||run.data.length>1001)return;
      if(state.runs.length>=8){announce('Acht Messläufe gespeichert. Projektkopie herunterladen; Neuladen beginnt eine neue Sitzung.');return;}
      run.id=String(++state.runId).padStart(3,'0');state.runs.push(run);
      window.PhysimProtocol.recordRun(run);
      send('develop','runs',{runs:state.runs});
      send('analyze','runs',{runs:state.runs,select:run.id});navigate('analyze');announce(`Lauf ${run.id} übernommen · ${run.data.length} Messpunkte aus der Simulation`);return;
    }
    if(m.type==='navigate'&&areas.includes(m.area)){
      navigate(m.area);if(m.file==='analysis.c')send('develop','open-analysis');
      if(m.area==='analyze'&&state.runs.some(r=>r.id===m.run))send('analyze','runs',{runs:state.runs,select:m.run});return;
    }
    if(m.type==='action')action(m.action);
    if(m.type==='status'&&typeof m.message==='string')announce(m.message);
    if(m.type==='shortcut'&&['save','search','build'].includes(m.action))action(m.action);
  });
  document.querySelectorAll('[data-area]').forEach(b=>b.onclick=()=>navigate(b.dataset.area));
  document.querySelector('.workspace-tabs').addEventListener('keydown',event=>{
    const at=areas.indexOf(state.area);let next;
    if(event.key==='ArrowRight')next=areas[(at+1)%3];if(event.key==='ArrowLeft')next=areas[(at+2)%3];if(event.key==='Home')next=areas[0];if(event.key==='End')next=areas[2];
    if(next){event.preventDefault();navigate(next,true);}
  });
  $('next-action').onclick=()=>{if(state.area==='develop'){if(state.build!=='ready')build();else navigate('simulate');}else if(state.area==='simulate')send('simulate','export');else navigate('develop');};
  $('theme').onclick=()=>{
    state.theme=state.theme==='dark'?'light':'dark';document.documentElement.dataset.theme=state.theme;
    $('theme').innerHTML=state.theme==='dark'?'☀ <span>Hell</span>':'☾ <span>Dunkel</span>';$('theme').setAttribute('aria-label',state.theme==='dark'?'Helle Darstellung aktivieren':'Dunkle Darstellung aktivieren');
    for(const area of areas)send(area,'theme',{theme:state.theme});
  };
  const menuButtons=[...document.querySelectorAll('.menu>button')];
  function closeMenus(restore=false){for(const b of menuButtons){const was=b.getAttribute('aria-expanded')==='true';b.setAttribute('aria-expanded','false');$(b.getAttribute('aria-controls')).hidden=true;if(restore&&was)b.focus();}}
  function openMenu(b){closeMenus();b.setAttribute('aria-expanded','true');$(b.getAttribute('aria-controls')).hidden=false;}
  for(const b of menuButtons){b.onclick=()=>{const open=b.getAttribute('aria-expanded')==='true';closeMenus();if(!open)openMenu(b);};b.onpointerenter=()=>{if(menuButtons.some(p=>p.getAttribute('aria-expanded')==='true'))openMenu(b);};b.onkeydown=e=>{if(e.key==='ArrowDown'){e.preventDefault();openMenu(b);$(b.getAttribute('aria-controls')).querySelector('button').focus();}};}
  document.querySelectorAll('.popup').forEach(p=>p.onkeydown=e=>{
    const list=[...p.querySelectorAll('button')],i=list.indexOf(document.activeElement);let n;
    if(e.key==='ArrowDown')n=(i+1)%list.length;if(e.key==='ArrowUp')n=(i+list.length-1)%list.length;if(e.key==='Home')n=0;if(e.key==='End')n=list.length-1;
    if(n!==undefined){e.preventDefault();list[n].focus();}
  });
  document.addEventListener('click',e=>{const target=e.target.closest('[data-action]');if(target){closeMenus(true);action(target.dataset.action);}else if(!e.target.closest('.menu'))closeMenus();});
  function shortcut(e){
    if(e.key==='Escape')closeMenus(true);
    if($('help').open)return;
    if(e.altKey&&['1','2','3'].includes(e.key)){e.preventDefault();navigate(areas[Number(e.key)-1],true);}
    if(e.key==='F5'){e.preventDefault();build();}
    if((e.ctrlKey||e.metaKey)&&['s','f'].includes(e.key.toLowerCase())){e.preventDefault();action(e.key.toLowerCase()==='s'?'save':'search');}
  }
  document.addEventListener('keydown',shortcut);
  $('help-close').onclick=()=>$('help').close();$('help').addEventListener('close',()=>document.querySelector('[data-action=help]').focus());
  $('opaque').onchange=()=>{for(const area of areas)send(area,'opaque',{value:$('opaque').checked});};
  addEventListener('hashchange',()=>navigate(location.hash.slice(1)));navigate(location.hash.slice(1));
})();
