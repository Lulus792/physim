/* Protocol design reference. Examples never alter the model or a real run. */
(() => {
  'use strict';
  const $=id=>document.getElementById(id),levels={error:['⊗','Fehler'],warning:['△','Warnung'],info:['ⓘ','Info'],debug:['◇','Debug']};
  const examples=[
    {id:'ex-build',level:'error',origin:'Build',title:'Bezeichner „gravity_m_s“ ist nicht definiert',context:'main.c · Modell bauen',time:'10:42:08',source:'main.c:13:14',token:'gravity_m_s2',file:'main.c',message:'Der Compiler kennt den verwendeten Namen nicht. Prüfe die Schreibweise der Gravitationskonstante.',impact:'Build fehlgeschlagen. Ein vorhandenes Experimentmodul wird dadurch nicht ersetzt.',raw:"main.c:13:14: error: 'gravity_m_s' undeclared\n  dy[1] = -gravity_m_s / length_m * sin(y[0]);\n           ^~~~~~~~~~~\n  did you mean 'gravity_m_s2'?",jump:'Beispielstelle öffnen'},
    {id:'ex-warning',level:'warning',origin:'Build',title:'Variable „energy“ wird nicht verwendet',context:'main.c · Modell bauen',time:'10:42:08',source:'main.c:24:12',message:'Eine lokale Variable wird berechnet, aber anschließend nicht gelesen.',impact:'Diese Warnung verhindert allein keinen erfolgreichen Build. Prüfe, ob die Berechnung benötigt wird.',raw:"main.c:24:12: warning: unused variable 'energy' [-Wunused-variable]"},
    {id:'ex-runtime',level:'error',origin:'Simulation',title:'Modellschritt liefert keinen endlichen Wert',context:'Lauf 004 · Schritt',time:'10:43:12',logical:'t = 2,40 s',message:'Eine Berechnung hat NaN geliefert. Prüfe Eingabewerte und Divisionen im Modell.',impact:'Beispielsituation: Der Lauf endet mit Fehler. Bereits bestätigte Messdaten bleiben zur Untersuchung verfügbar; die Vollständigkeit muss am Lauf geprüft werden.',raw:'[Beispiel] step: non-finite model result\nZeit: 2.40 s\nKeine Quellposition übermittelt.'},
    {id:'ex-model',level:'warning',origin:'Simulation',title:'Auslenkung außerhalb der Kleinwinkelnäherung',context:'Lauf 004 · Modellmeldung',time:'10:43:10',logical:'t = 0,00 s',message:'Das Modell meldet: „Die Näherung ist für diese Auslenkung nur eingeschränkt geeignet.“\nDies ist eine Beispielmeldung des Modells, keine automatische Physim-Prüfung.',impact:'Der Lauf kann weiterlaufen. Ob Ergebnisse geeignet sind, hängt von der Modellannahme ab.',raw:'Experiment [warning, t=0 s]: Die Näherung ist für diese Auslenkung nur eingeschränkt geeignet.'},
    {id:'ex-analysis',level:'error',origin:'Auswertung',title:'Messlauf kann nicht geöffnet werden',context:'Lauf 003 · Daten laden',time:'10:44:01',message:'Die angeforderte Laufdatei ist nicht erreichbar. Prüfe den Speicherort und die Zugriffsrechte.',impact:'Dieser Lauf kann nicht ausgewertet werden. Bereits geladene andere Läufe bleiben ausgewählt.',raw:'[Beispiel] open run: file unavailable\nruns/003/run.psrun'},
    {id:'ex-info',level:'info',origin:'Simulation',title:'Messlauf wurde gespeichert',context:'Lauf 004 · Aufzeichnung',time:'10:44:02',logical:'t = 6,00 s',message:'Die Aufzeichnung ist abgeschlossen und kann in Auswerten untersucht werden.',impact:'Beispiel für eine informative Abschlussmeldung.',raw:'[Beispiel] Recording completed: run 004',success:true},
    {id:'ex-debug',level:'debug',origin:'Simulation',title:'Modell zurückgesetzt',context:'Lauf 004 · Experiment',time:'10:43:09',logical:'t = 0,00 s',message:'Reset „Pendelstudie“\nSeed: 42\tSchrittweite: 0,01 s\nUnicode und mehrzeilige Meldungen bleiben lesbar.',impact:'Debugmeldungen unterstützen die Fehlersuche und sind gesondert filterbar.',raw:'Experiment [debug, t=0 s]: Reset "Pendelstudie"\nSeed: 42\tSchrittweite: 0.01 s'},
    {id:'ex-overflow',level:'warning',origin:'Simulation',title:'Weitere Meldungen wurden unterdrückt',context:'Lauf 004 · Logger',time:'10:43:13',logical:'t = 2,50 s',message:'908 zusätzliche Meldungen wurden vom Logger unterdrückt.',impact:'Das Protokoll enthält an dieser Stelle nicht alle Meldungen. Die Anzahl bleibt sichtbar; keine erfundenen Einzelmeldungen ergänzen.',raw:'[Beispiel] Logger suppressed 908 messages'}
  ];
  let session=[],mode='session',filter='all',selected=null,previousBuild=null,serial=0,opener=null,height=340;
  const panel=document.createElement('section');panel.id='protocol-panel';panel.hidden=true;panel.setAttribute('aria-labelledby','protocol-title');
  panel.innerHTML=`<div id="protocol-resize" class="resize-handle" tabindex="0" role="separator" aria-label="Protokollhöhe ändern" aria-controls="protocol-panel" aria-orientation="horizontal" aria-describedby="protocol-resize-help"></div>
    <div class="protocol-heading"><h2 id="protocol-title">Protokoll</h2><span class="protocol-summary" id="protocol-summary"></span><div class="protocol-mode" role="group" aria-label="Meldungsquelle"><button id="protocol-session" aria-pressed="true">Diese Sitzung</button><button id="protocol-examples" aria-pressed="false">Designbeispiele</button></div><button id="protocol-close" aria-label="Protokoll schließen">✕</button></div>
    <div class="protocol-tools"><div class="protocol-filters" role="group" aria-label="Meldungen nach Schweregrad filtern"></div><label class="protocol-search">Suchen <input id="protocol-query" type="search" placeholder="Meldung oder Quelle …"></label><label><span class="protocol-notice">Bereich </span><select id="protocol-origin" aria-label="Meldungen nach Bereich filtern"><option>Alle Bereiche</option><option>Build</option><option>Simulation</option><option>Auswertung</option></select></label></div>
    <div class="protocol-body"><div class="protocol-list" id="protocol-list" aria-label="Protokollmeldungen"></div><article class="protocol-detail" id="protocol-detail" aria-label="Meldungsdetails"></article></div>
    <div class="protocol-foot"><span id="protocol-notice"></span><span id="protocol-feedback" role="status" aria-live="polite"></span><button id="protocol-download">Protokoll herunterladen ↓</button></div><span id="protocol-resize-help" hidden>Pfeile ändern um 10 Pixel, Umschalt um 40. Pos1 Minimum, Ende Maximum. Enter oder Doppelklick setzt zurück. Escape bricht Ziehen ab.</span>`;
  $('panels').after(panel);
  const viewButton=document.createElement('button');viewButton.dataset.action='protocol';viewButton.textContent='Protokoll anzeigen';$('view-menu').append(viewButton);
  const records=()=>mode==='examples'?examples:session;
  const clock=()=>new Date().toLocaleTimeString('de-DE',{hour12:false});
  function el(tag,text,className){const e=document.createElement(tag);if(text!==undefined)e.textContent=text;if(className)e.className=className;return e;}
  function symbol(r){const s=el('span',r.success?'✓':levels[r.level][0],'protocol-symbol '+(r.success?'success':r.level));s.setAttribute('aria-hidden','true');return s;}
  function summary(){const rows=records(),errors=rows.filter(r=>r.level==='error').length,warnings=rows.filter(r=>r.level==='warning').length;return `${errors} Fehler · ${warnings} Warnungen · ${rows.length} Meldungen`;}
  function detail(record){
    const target=$('protocol-detail');target.replaceChildren();delete target.dataset.level;
    if(!record){target.append(el('p','Wähle eine Meldung aus. Die Details erscheinen hier.','protocol-empty'));return;}
    target.dataset.level=record.level;
    const back=el('button','← Alle Meldungen','protocol-back');back.onclick=()=>{panel.dataset.detailOpen='false';$('protocol-list').querySelector('[aria-current=true]')?.focus();};target.append(back);
    const kicker=el('div',undefined,'protocol-kicker');kicker.append(symbol(record),el('span',`${levels[record.level][1]} · ${record.origin}`));if(mode==='examples')kicker.append(el('span','Designbeispiel','protocol-example-tag'));
    target.append(kicker,el('h3',record.title),el('p',record.message),el('p',record.impact,'protocol-impact'));
    const meta=el('dl');for(const [label,value]of [['Kontext',record.context],['Quellstelle',record.source||'Nicht übermittelt'],['Modellzeit',record.logical||'Nicht übermittelt'],['Uhrzeit',record.time]])meta.append(el('dt',label),el('dd',value));target.append(meta);
    const raw=el('details');raw.append(el('summary','Originalmeldung anzeigen'),el('pre',record.raw));target.append(raw);
    const actions=el('div',undefined,'protocol-actions');
    if(record.file&&record.token){const jump=el('button',(record.jump||'Zur Quellstelle')+' →','source-action');jump.onclick=()=>{
      $('tab-develop').click();$('frame-develop').contentWindow.postMessage({protocol:'physim-preview-v1',type:'diagnostic-source',file:record.file,token:record.token},'*');
      $('protocol-feedback').textContent=mode==='examples'?'Beispielstelle markiert · Quelltext unverändert':'Quellstelle markiert';
    };actions.append(jump);}
    const copy=el('button','Meldung kopieren');copy.onclick=async()=>{try{await navigator.clipboard.writeText(format(record));$('protocol-feedback').textContent='Meldung kopiert';}catch{$('protocol-feedback').textContent='Kopieren nicht verfügbar. Protokoll herunterladen oder Originaltext auswählen.';}};actions.append(copy);target.append(actions);
  }
  function render(){
    panel.dataset.detailOpen='false';
    const rows=records(),q=$('protocol-query').value.toLocaleLowerCase('de'),origin=$('protocol-origin').value;
    const shown=rows.filter(r=>(filter==='all'||r.level===filter)&&(origin==='Alle Bereiche'||r.origin===origin)&&[r.title,r.message,r.source,r.context,r.raw].join(' ').toLocaleLowerCase('de').includes(q));
    $('protocol-summary').textContent=summary();$('protocol-count').textContent=mode==='examples'?'Beispiele':`${session.filter(r=>r.level==='error').length} Fehler`;
    $('protocol-session').setAttribute('aria-pressed',String(mode==='session'));$('protocol-examples').setAttribute('aria-pressed',String(mode==='examples'));
    for(const b of document.querySelectorAll('[data-log-filter]')){b.setAttribute('aria-pressed',String(filter===b.dataset.logFilter));b.querySelector('.filter-count').textContent=b.dataset.logFilter==='all'?rows.length:rows.filter(r=>r.level===b.dataset.logFilter).length;}
    if(!shown.some(r=>r.id===selected))selected=shown[0]?.id||null;
    const list=$('protocol-list');list.replaceChildren();
    for(const r of shown){const button=el('button',undefined,'protocol-row');button.dataset.record=r.id;button.setAttribute('aria-current',String(r.id===selected));button.setAttribute('aria-label',`${levels[r.level][1]}: ${r.title}. ${r.context}`);const content=el('span');content.append(el('strong',r.title),el('small',`${levels[r.level][1]} · ${r.context}${r.logical?' · '+r.logical:''}`));button.append(symbol(r),content,el('time',r.time));button.onclick=()=>{selected=r.id;list.querySelectorAll('button').forEach(b=>b.setAttribute('aria-current',String(b.dataset.record===selected)));detail(r);panel.dataset.detailOpen='true';if(innerWidth<=900)targetBack();};list.append(button);}
    if(!shown.length)list.append(el('p',rows.length?'Keine Meldung passt zu diesen Filtern.':'Noch keine Meldungen. Baue die Vorschau oder öffne die Designbeispiele.','protocol-empty'));
    detail(shown.find(r=>r.id===selected));
    $('protocol-notice').textContent=mode==='examples'?`Unabhängige Beispielsituationen · ${shown.length} von ${rows.length} sichtbar · keine aktuellen Fehler`:`Lokale Vorschau · ${shown.length} von ${rows.length} sichtbar · letzte 200 Meldungen`;
    $('protocol-download').disabled=!shown.length;
  }
  function targetBack(){$('protocol-detail').querySelector('.protocol-back').focus();panel.querySelector('.protocol-body').scrollTop=0;}
  for(const [key,label]of [['all','Alle'],['error','Fehler'],['warning','Warnungen'],['info','Info'],['debug','Debug']]){const b=el('button');b.dataset.logFilter=key;b.setAttribute('aria-pressed',String(key==='all'));if(key!=='all')b.append(symbol({level:key}));b.append(el('span',label),el('span','0','filter-count'));b.onclick=()=>{filter=key;render();};panel.querySelector('.protocol-filters').append(b);}
  function setMode(next){mode=next;filter='all';selected=null;$('protocol-query').value='';$('protocol-origin').selectedIndex=0;$('protocol-feedback').textContent='';render();}
  $('protocol-session').onclick=()=>setMode('session');$('protocol-examples').onclick=()=>setMode('examples');$('protocol-query').oninput=render;$('protocol-origin').onchange=render;
  function visible(value){panel.hidden=!value;$('protocol-toggle').setAttribute('aria-expanded',String(value));for(const frame of document.querySelectorAll('#panels iframe'))frame.contentWindow.postMessage({protocol:'physim-preview-v1',type:'protocol-visible',value},'*');if(value)resize.sync();}
  function open(){if(panel.hidden)opener=document.activeElement;visible(true);render();}
  $('protocol-toggle').onclick=()=>{if(panel.hidden){open();$('protocol-close').focus();}else close();};
  function close(){visible(false);if(opener?.isConnected&&opener.offsetParent!==null)opener.focus();else $('protocol-toggle').focus();}
  $('protocol-close').onclick=close;
  function format(r){return `[${levels[r.level][1]}] ${r.time} · ${r.context}${r.logical?' · '+r.logical:''}\n${r.title}\n${r.message}\n${r.impact}\n${r.source||'Keine Quellposition'}\n${r.raw}`;}
  $('protocol-download').onclick=()=>{const shownIds=[...$('protocol-list').querySelectorAll('[data-record]')].map(e=>e.dataset.record),content=(mode==='examples'?'Physim · unabhängige Designbeispiele, keine aktuellen Fehler':'Physim · lokale Designvorschau')+'\n\n'+records().filter(r=>shownIds.includes(r.id)).map(format).join('\n\n');const url=URL.createObjectURL(new Blob([content],{type:'text/plain;charset=utf-8'})),a=el('a');a.href=url;a.download=mode==='examples'?'physim-protokoll-beispiele.txt':'physim-vorschau-protokoll.txt';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);$('protocol-feedback').textContent='Sichtbare Meldungen heruntergeladen';};
  function limits(){return[150,Math.max(150,Math.min(500,Math.floor(innerHeight*.55)))];}
  function setHeight(value){const[min,max]=limits();height=Math.max(min,Math.min(max,value));panel.style.setProperty('--protocol-height',height+'px');resize.sync();}
  const resize=PhysimResize.attach($('protocol-resize'),{axis:()=>'y',sign:()=>-1,limits,value:()=>height,set:setHeight,reset:()=>setHeight(340),snapshot:()=>height,restore:setHeight});
  // When controls consume the dock (large text/short height), scroll the whole
  // content rather than leaving an invisible, zero-height message viewport.
  function reflow(){if(panel.hidden)return;const fixed=['.protocol-heading','.protocol-tools','.protocol-foot'].reduce((sum,s)=>sum+panel.querySelector(s).getBoundingClientRect().height,0);panel.classList.toggle('protocol-scroll-all',height-fixed<100);}
  const observer=new ResizeObserver(reflow);for(const e of [panel,...panel.querySelectorAll('.protocol-heading,.protocol-tools,.protocol-foot')])observer.observe(e);
  addEventListener('resize',()=>{setHeight(height);resize.sync();});setHeight(height);
  function append(record){session.push({id:'session-'+(++serial),time:clock(),...record});if(session.length>200)session=session.slice(-200);if(mode==='session')render();}
  window.PhysimProtocol={open,isOpen:()=>!panel.hidden,recordBuild(m){
    if(m.scenario===previousBuild)return;const before=previousBuild;previousBuild=m.scenario;
    if(m.scenario==='building')append({level:'info',origin:'Build',title:'Vorschau-Build gestartet',context:'main.c · Demo-Prüfung',message:'Die bekannten Modellkonstanten werden geprüft.',impact:'C-Code wird in dieser Vorschau nicht kompiliert.',raw:'[Demo] Build started'});
    if(m.scenario==='error'){const location=/\bgravity_m_s\b/.test(m.files?.['main.c']||'')?{source:'main.c · gravity_m_s',file:'main.c',token:'gravity_m_s'}:{};append({level:'error',origin:'Build',title:'Vorschau-Build fehlgeschlagen',context:'main.c · Demo-Prüfung',...location,message:m.error||'Die Modellkonstanten konnten nicht übernommen werden.',impact:'Behebe den Fehler und baue die Vorschau erneut.',raw:m.error||'[Demo] Modellprüfung fehlgeschlagen'});if(mode==='session'){filter='all';$('protocol-query').value='';$('protocol-origin').selectedIndex=0;selected=session.at(-1).id;open();}}
    if(m.scenario==='ready'&&(before==='building'||before===null))append({level:'info',origin:'Build',title:before===null?'Vorschaumodell bereit':'Vorschau-Build erfolgreich',context:'main.c · Demo-Prüfung',success:true,message:'Die Modellkonstanten sind für die Pendelvorschau verfügbar.',impact:'Du kannst in Simulieren einen neuen Messlauf starten.',raw:'[Demo] Model ready'});
    if(before==='building'&&!['building','ready','error'].includes(m.scenario))append({level:'info',origin:'Build',title:'Vorschau-Build unterbrochen',context:'main.c · Demo-Prüfung',message:'Der Build wurde vor dem Abschluss beendet.',impact:'Es wurde kein neues Vorschaumodell übernommen.',raw:'[Demo] Build interrupted'});
  },recordRun(run){append({level:'info',origin:'Simulation',title:`Messlauf ${run.id} übernommen`,context:`Lauf ${run.id} · Vorschau`,success:true,message:`${run.data.length} Messpunkte wurden an Auswerten übergeben.`,impact:'Die Daten stammen aus der Pendelvorschau mit Kleinwinkelnäherung.',raw:`[Demo] Run ${run.id}: ${run.data.length} samples`});}};
  render();if(new URLSearchParams(location.search).get('example')==='protocol'){setMode('examples');open();}
})();
