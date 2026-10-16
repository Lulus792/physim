/* Standalone design prototype. All input data is generated locally; no native APIs. */
(() => {
  'use strict';
  const $ = id => document.getElementById(id);
  const runs = [
    {id:'024', time:'14:32', damping:0.055, amplitude:0.35},
    {id:'023', time:'14:18', damping:0.025, amplitude:0.35},
    {id:'022', time:'13:56', damping:0.085, amplitude:0.35}
  ];
  const channels = {
    theta:{name:'Auslenkung', symbol:'θ', units:[['rad','Radiant (rad)'],['deg','Grad (°)']]},
    omega:{name:'Winkelgeschwindigkeit', symbol:'ω', units:[['rad/s','Radiant pro Sekunde (rad/s)']]},
    energy:{name:'Mechanische Energie', symbol:'E', units:[['J','Joule (J)']]}
  };
  for (const run of runs) {
    run.data = Array.from({length:1001},(_,i) => {
      const t = i/50, w = Math.sqrt(9.81-run.damping**2), decay = run.amplitude*Math.exp(-run.damping*t);
      const theta = decay*Math.cos(w*t), omega = decay*(-run.damping*Math.cos(w*t)-w*Math.sin(w*t));
      return {t, theta, omega, energy:omega**2/2+9.81*(1-Math.cos(theta))};
    });
  }
  const state = {selected:['024'], focus:'024', scenario:'ready', channel:'theta', unit:'rad', zoom:1, tab:'data', report:null, timer:null};
  const fmt = (v,d=4) => new Intl.NumberFormat('de-DE',{minimumFractionDigits:d,maximumFractionDigits:d}).format(Math.abs(v)<0.5*10**(-d)?0:v);
  const value = point => point[state.channel]*(state.unit==='deg'?180/Math.PI:1);
  const selected = () => runs.filter(r=>state.selected.includes(r.id));
  const signature = () => state.selected.join(',');
  const say = message => { $('status').textContent=message; };
  const available = () => ['ready','compare'].includes(state.scenario) && state.selected.length>0;
  function stats(run) {
    const values=run.data.map(value),mean=values.reduce((a,b)=>a+b,0)/values.length;
    return {mean,std:Math.sqrt(values.reduce((a,b)=>a+(b-mean)**2,0)/(values.length-1)),min:Math.min(...values),max:Math.max(...values)};
  }
  function rows() {
    return selected().map(r=>{const s=stats(r);return `<tr><th scope="row">Lauf ${r.id}</th><td>${fmt(s.mean)}</td><td>${fmt(s.std)}</td><td>${fmt(s.min)}</td><td>${fmt(s.max)}</td></tr>`;}).join('');
  }
  function renderRuns() {
    const query=$('search-runs').value.trim().toLocaleLowerCase('de');
    $('run-list').innerHTML = state.scenario==='empty' ? '<p class="small selection-hint">Noch keine gespeicherten Läufe.</p>' : runs.filter(r=>(`Lauf ${r.id} ${r.time} Dämpfung ${fmt(r.damping,3)}`).toLocaleLowerCase('de').includes(query)).map(r=>`
      <label class="run-row ${state.selected.includes(r.id)?'selected':''}"><input type="checkbox" value="${r.id}" ${state.selected.includes(r.id)?'checked':''} aria-label="Lauf ${r.id} auswählen"><span class="run-copy"><span class="run-title">Lauf ${r.id}<time>${r.time}</time></span><span class="small">Dämpfung ${fmt(r.damping,3)} s⁻¹</span><span class="small">20,0 s · 1.001 Messpunkte</span></span></label>`).join('');
    if (state.scenario!=='empty' && !$('run-list').children.length) $('run-list').innerHTML='<p class="small selection-hint">Kein passender Messlauf.</p>';
    document.querySelector('.count').textContent=state.scenario==='empty'?'0':String(runs.length);
    $('selection-hint').textContent=state.selected.length>1?`${state.selected.length} Läufe für den Vergleich ausgewählt.`:'Weitere Läufe zum Vergleichen auswählen.';
  }
  function renderPlot() {
    const list=selected(),svg=$('chart'),start=10-10/state.zoom,end=10+10/state.zoom;
    const points=list.flatMap(r=>r.data.filter(p=>p.t>=start&&p.t<=end).map(value));
    if(!points.length) return;
    let low=Math.min(...points),high=Math.max(...points),pad=(high-low)*.12||1; low-=pad;high+=pad;
    const x=t=>58+(t-start)/(end-start)*718, y=v=>274-(v-low)/(high-low)*234;
    const c=channels[state.channel];
    let markup=`<title id="plot-title">${c.name} über der Zeit</title><desc id="plot-description">${list.map(r=>'Lauf '+r.id).join(', ')}. Zeitbereich ${fmt(start,1)} bis ${fmt(end,1)} Sekunden. Einheit ${state.unit}. Synthetische Beispieldaten. Die zugehörige Tabelle enthält die Statistik des gesamten Laufs.</desc><defs><clipPath id="plot-clip"><rect x="58" y="40" width="718" height="234"/></clipPath></defs>`;
    for(let i=0;i<=4;i++) {
      const yy=40+i*234/4,v=high-i*(high-low)/4;
      markup+=`<line class="grid" x1="58" x2="776" y1="${yy}" y2="${yy}"/><text x="48" y="${yy+4}" text-anchor="end">${fmt(v,2)}</text>`;
    }
    for(let i=0;i<=5;i++) {
      const t=start+i*(end-start)/5,xx=x(t);
      markup+=`<line class="grid" x1="${xx}" x2="${xx}" y1="40" y2="274"/><text x="${xx}" y="296" text-anchor="middle">${fmt(t,state.zoom>1?1:0)}</text>`;
    }
    if(low<0&&high>0) markup+=`<line class="zero" x1="58" x2="776" y1="${y(0)}" y2="${y(0)}"/>`;
    markup+=`<text x="58" y="20">${c.symbol} [${state.unit}]</text><text x="776" y="320" text-anchor="end">Zeit [s]</text>`;
    list.forEach((r,i)=>{
      const data=r.data.filter(p=>p.t>=start&&p.t<=end);
      const d=data.map((p,j)=>`${j?'L':'M'}${x(p.t).toFixed(2)},${y(value(p)).toFixed(2)}`).join(' ');
      markup+=`<path class="curve series-${i+1}" clip-path="url(#plot-clip)" ${i?`stroke-dasharray="${i===1?'7 4':'2 4'}"`:''} d="${d}"/>`;
    });
    markup+='<line id="crosshair" class="crosshair" x1="0" x2="0" y1="40" y2="274" visibility="hidden"/>';
    svg.innerHTML=markup;
    const resetReadout=()=>{$('cursor-readout').textContent=`Zeitachse ${fmt(start,1)}–${fmt(end,1)} s`;$('crosshair').setAttribute('visibility','hidden');};
    resetReadout();
    svg.onpointermove=e=>{
      const box=svg.getBoundingClientRect(),px=(e.clientX-box.left)/box.width*800;
      if(px<58||px>776) {resetReadout();return;}
      const t=start+(px-58)/718*(end-start),index=Math.max(0,Math.min(1000,Math.round(t*50)));
      $('crosshair').setAttribute('x1',x(index/50));$('crosshair').setAttribute('x2',x(index/50));$('crosshair').setAttribute('visibility','visible');
      $('cursor-readout').textContent=`t = ${fmt(index/50,2)} s · `+list.map(r=>`${r.id}: ${fmt(value(r.data[index]))} ${state.unit}`).join(' · ');
    };
    svg.onpointerleave=resetReadout;
    $('zoom-in').disabled=state.zoom>=8;$('zoom-out').disabled=state.zoom<=1;
  }
  function renderDetails() {
    const run=runs.find(r=>r.id===state.focus)||runs[0];
    $('detail-title').textContent=state.selected.length?`Lauf ${run.id}`:'Kein Lauf ausgewählt';
    $('run-details').innerHTML=state.selected.length?`<div><dt>Dauer</dt><dd>20,0 s</dd></div><div><dt>Messpunkte</dt><dd>1.001</dd></div><div><dt>Abtastrate</dt><dd>50 Hz</dd></div><div><dt>Dämpfung</dt><dd>${fmt(run.damping,3)} s⁻¹</dd></div><div><dt>Datenstatus</dt><dd>Vollständig</dd></div>`:'';
    $('source-file').textContent=state.selected.length?`runs/pendel-${run.id}.psrun`:'—';
    const disabled=!available(); $('channel').disabled=disabled;$('unit').disabled=disabled;
  }
  function emptyState(icon,title,text,action,label,error=false) {
    return `<div class="empty-state ${error?'error-state':''}"><span class="state-icon" aria-hidden="true">${icon}</span><h3>${title}</h3><p>${text}</p>${action?`<button class="primary" data-action="${action}">${label}</button>`:''}</div>`;
  }
  function renderReport() {
    const report=$('report-panel');
    if(state.timer) {report.innerHTML=emptyState('◷','Analyse läuft','Die ausgewählten Läufe werden ausgewertet.','cancel','Abbrechen');return;}
    if(!state.report) {report.innerHTML=emptyState('≋','Bereit für deine Auswertung','Starte den Analysecode für die ausgewählten Messläufe. Das Ergebnis erscheint hier.',available()?'analyze':null,'Analyse starten');return;}
    const r=state.report,stale=r.signature!==signature();
    report.innerHTML=`<div class="report-header"><p class="overline">ANALYSEERGEBNIS · BEISPIEL</p><h3>Gedämpftes Pendel</h3><p>${r.labels} · analysis.phys · ${r.timestamp}</p></div>${stale?'<div class="report-section"><p>Die Laufauswahl wurde geändert. Dieser Bericht bezieht sich weiterhin auf '+r.labels+'.</p><button class="text-button" data-action="analyze">Neue Auswahl auswerten →</button></div>':''}<section class="report-section"><h4>${r.channel} · Lauf ${r.first}</h4><div class="report-stats"><div class="report-stat"><span>Mittelwert [${r.unit}]</span><strong>${fmt(r.stats.mean)}</strong></div><div class="report-stat"><span>Standardabweichung [${r.unit}]</span><strong>${fmt(r.stats.std)}</strong></div><div class="report-stat"><span>Messpunkte</span><strong>1.001</strong></div></div></section><section class="report-section"><h4>Ergebnisse aller ausgewählten Läufe</h4><div class="table-scroll"><table><caption class="sr-only">Ergebnisstatistik in ${r.unit}</caption><thead><tr><th scope="col">Messlauf</th><th scope="col">Mittelwert</th><th scope="col">Std.-Abw.</th><th scope="col">Minimum</th><th scope="col">Maximum</th></tr></thead><tbody>${r.rows}</tbody></table></div><p>${r.channel} in ${r.unit}. Statistik über alle Messpunkte je Lauf; Stichprobenstandardabweichung.</p></section><section class="report-section"><h4>Nachvollziehbare Herkunft</h4><p>Grundlage: ${r.labels}. Der Bericht behält seine Laufauswahl und Anzeigeeinheit auch dann, wenn du anschließend andere Messdaten untersuchst.</p><p class="small">Vorschau aus synthetischen Daten. In der App stammen Inhalte und Methoden aus dem ausgeführten Analysemodul.</p><div class="report-source"><button class="text-button" data-action="data">Zurück zu den Messdaten →</button></div></section>`;
  }
  function setTab(tab,focus=false) {
    state.tab=tab;
    for(const name of ['data','report']) {
      const active=name===tab;
      $('tab-'+name).setAttribute('aria-selected',String(active));$('tab-'+name).tabIndex=active?0:-1;
      $(name+'-panel').hidden=!active;
    }
    if(focus) $('tab-'+tab).focus();
    renderReport();
  }
  function render() {
    renderRuns();renderDetails();renderReport();
    const list=selected(),ready=available(),c=channels[state.channel];
    $('selection-label').textContent='Gedämpftes Pendel / '+(list.length>1?`${list.length} Läufe im Vergleich`:list.length?`Lauf ${list[0].id}`:'Keine Auswahl');
    $('analyze').disabled=!ready&&!state.timer;
    $('analyze').textContent=state.timer?'Abbrechen':list.length>1?`${list.length} Läufe vergleichen`:'Analyse starten';
    $('data-quality').textContent=ready?(list.length>1?`${list.length} Läufe · je 1.001 Punkte`:'Vollständig · 1.001 Messpunkte'):'Keine Datenansicht';
    $('data-panel').hidden=state.tab!=='data'||!ready;$('report-panel').hidden=state.tab!=='report'||!ready;
    $('state-panel').hidden=ready;
    if(!ready) {
      const templates={
        empty:emptyState('⌁','Dein erster Messlauf','Zeichne in der Simulation einen Lauf auf oder importiere eine vorhandene .psrun-Datei.','load','Beispielmesslauf laden'),
        loading:emptyState('◷','Messdaten werden geladen','Die Laufdatei wird geprüft. Sobald sie bereit ist, erscheinen Kurve und Statistik.','load','Laden in Vorschau abschließen'),
        error:emptyState('!','Messlauf konnte nicht geladen werden','Die Laufdatei ist nicht lesbar. Prüfe die Datei oder wähle einen anderen Messlauf.','load','Erneut versuchen',true)
      };
      $('state-panel').innerHTML=templates[state.scenario]||emptyState('⌁','Wähle einen Messlauf','Wähle links einen oder mehrere Läufe aus, um Messdaten zu untersuchen.','load','Letzten Lauf auswählen');
    }
    if(ready) {
      $('chart-title').textContent=c.name;
      $('legend').innerHTML=list.map((r,i)=>`<span class="legend-item"><span class="swatch" aria-hidden="true"></span>Lauf ${r.id}${i===0?' · Referenz':''}</span>`).join('');
      $('statistics').innerHTML=rows();$('statistics-unit').textContent=`${c.name} in ${state.unit} · Stichprobenstandardabweichung`;
      renderPlot();
    }
    document.querySelector('.report-dot').hidden=!state.report;
  }
  function cancelAnalysis(announce=true) {
    if(state.timer) {clearTimeout(state.timer);state.timer=null;if(announce)say('Analyse abgebrochen.');}
  }
  function analyze() {
    if(state.timer) {cancelAnalysis();render();return;}
    if(!available()) return;
    const first=selected()[0];
    const snapshot={signature:signature(),labels:selected().map(r=>'Lauf '+r.id).join(', '),first:first.id,channel:channels[state.channel].name,unit:state.unit,stats:stats(first),rows:rows(),timestamp:'gerade erstellt'};
    state.timer=setTimeout(()=>{state.report=snapshot;state.timer=null;say(`Analyse abgeschlossen · ${snapshot.labels}`);render();},700);
    setTab('report');say('Analyse läuft …');render();
  }
  function scenario(name) {
    cancelAnalysis(false);state.scenario=name;state.selected=name==='empty'?[]:name==='compare'?['024','023']:['024'];state.focus='024';state.zoom=1;state.report=null;state.tab='data';
    $('scenario').value=name;$('search-runs').value='';setTab('data');render();
    say(({ready:'Bereit · Lauf 024 geladen',compare:'Bereit · 2 Läufe im Vergleich',empty:'Noch kein Messlauf vorhanden',loading:'Messdaten werden geladen …',error:'Ladefehler · Messlauf nicht lesbar'})[name]);
  }
  $('scenario').onchange=e=>scenario(e.target.value);
  $('search-runs').oninput=renderRuns;
  $('run-list').onchange=e=>{
    if(e.target.type!=='checkbox')return;
    const restoreFocus=document.activeElement===e.target;
    cancelAnalysis(false);const id=e.target.value;
    state.selected=e.target.checked?[...state.selected,id]:state.selected.filter(x=>x!==id);
    state.focus=e.target.checked?id:state.selected.at(-1)||'024';state.scenario='ready';state.zoom=1;$('scenario').value='ready';
    render();if(restoreFocus)$('run-list').querySelector(`input[value="${id}"]`)?.focus();
    say(`${state.selected.length} ${state.selected.length===1?'Lauf':'Läufe'} ausgewählt.`);
  };
  $('channel').onchange=e=>{
    state.channel=e.target.value;state.unit=channels[state.channel].units[0][0];state.zoom=1;
    $('unit').innerHTML=channels[state.channel].units.map(([v,label])=>`<option value="${v}">${label}</option>`).join('');
    render();say(`${channels[state.channel].name} in ${state.unit}`);
  };
  $('unit').onchange=e=>{state.unit=e.target.value;render();say(`Anzeigeeinheit: ${state.unit}`);};
  $('analyze').onclick=analyze;$('import').onclick=()=>scenario('ready');
  $('zoom-in').onclick=()=>{state.zoom=Math.min(8,state.zoom*2);renderPlot();say(`Zeitachse ${state.zoom}× vergrößert. Statistik bleibt unverändert.`);};
  $('zoom-out').onclick=()=>{state.zoom=Math.max(1,state.zoom/2);renderPlot();};
  $('zoom-reset').onclick=()=>{state.zoom=1;renderPlot();say('Gesamter Zeitbereich angezeigt.');};
  for(const tab of ['data','report']) $('tab-'+tab).onclick=()=>{setTab(tab);render();};
  document.querySelector('.content-tabs').onkeydown=e=>{
    if(['ArrowRight','ArrowLeft','Home','End'].includes(e.key)) {e.preventDefault();setTab(e.key==='Home'?'data':e.key==='End'?'report':state.tab==='data'?'report':'data',true);render();}
  };
  document.querySelector('.analysis-pane').onclick=e=>{
    const action=e.target.closest('[data-action]')?.dataset.action;
    if(action==='load') scenario('ready');if(action==='analyze') analyze();if(action==='cancel'){cancelAnalysis();render();}if(action==='data'){setTab('data');render();}
  };
  for(const side of ['left','right']) $('toggle-'+side).onclick=()=>{
    const workspace=$('workspace'),mobile=matchMedia('(max-width:760px)').matches,compact=matchMedia('(max-width:1000px)').matches;
    if(side==='left'&&mobile) {workspace.classList.toggle('mobile-runs');workspace.classList.remove('details-open');}
    else if(side==='right'&&compact) {workspace.classList.toggle('details-open');workspace.classList.remove('mobile-runs');}
    else workspace.classList.toggle(side+'-closed');
    syncPanels();
  };
  function syncPanels() {
    for(const [side,panel,label] of [['left','run-panel','Laufliste'],['right','inspector','Details']]) {
      const visible=getComputedStyle($(panel)).display!=='none';
      $('toggle-'+side).setAttribute('aria-expanded',String(visible));$('toggle-'+side).setAttribute('aria-label',`${label} ${visible?'ausblenden':'einblenden'}`);
    }
  }
  window.addEventListener('resize',syncPanels);
  for(const button of document.querySelectorAll('button[data-theme]')) button.onclick=()=>{
    document.documentElement.dataset.theme=button.dataset.theme;
    for(const peer of document.querySelectorAll('button[data-theme]'))peer.setAttribute('aria-pressed',String(peer===button));
  };
  $('code-open').onclick=()=>$('code-dialog').showModal();$('code-close').onclick=()=>$('code-dialog').close();
  $('export-csv').onclick=()=>{
    const lines=[`run,time_s,${state.channel}_${state.unit}`];
    for(const run of selected()) for(const p of run.data) lines.push(`${run.id},${p.t},${value(p)}`);
    const blob=new Blob([lines.join('\r\n')],{type:'text/csv;charset=utf-8'}),url=URL.createObjectURL(blob),a=document.createElement('a');
    a.href=url;a.download='physim-beispieldaten.csv';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);say('CSV mit allen synthetischen Messpunkten exportiert.');
  };
  render();syncPanels();
})();
