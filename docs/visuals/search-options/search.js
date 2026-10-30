'use strict';
(()=>{
 const $=s=>document.querySelector(s),q=new URLSearchParams(location.search);
 const variants={a:['A · SUCHINSEL','Nah am Code. Leicht erreichbar.'],b:['B · DOKUMENTLEISTE','Alles an einem festen Platz.'],c:['C · TREFFERBEREICH','Jede Fundstelle im Blick.']};
 const cases=['matches','none','empty','readonly','unicode','long','loading','error'];
 const originals={experiment:'// Pendelstudie · θ, ω und Energie\n#include "physim.h"\n\nvoid setup(void) {\n    double damping = 0.02;\n    double angle = 0.33;\n    // damping steuert die Abklingrate.\n    pendulum_set_damping(damping);\n}\n\nvoid step(double dt) {\n    // damping bleibt zwischen Schritten konstant.\n    pendulum_integrate(dt);\n}\n',analysis:'// Auswertung · eigener Dokumentstand\nvoid report(void) {\n    const char *channel = "damping";\n    plot_series(channel);\n}\n',notes:'Pendelstudie\n\nDämpfung vergleichen.\ndamping = 0.02 im ersten Versuch.\nAuswertung und Quelle getrennt speichern.\n'};
 const names={experiment:'pendel.c',analysis:'auswertung.c',notes:'notizen.txt'};
 const s={variant:variants[q.get('variant')]?q.get('variant'):'a',theme:['dark','light','contrast'].includes(q.get('theme'))?q.get('theme'):'dark',scenario:cases.includes(q.get('scenario'))?q.get('scenario'):'matches',file:'experiment',docs:{},query:'damping',replacement:'friction',hits:[],active:-1,selection:[0,0],expanded:q.get('replace')==='1',open:true,pending:null,phase:'ready'};
 const escape=x=>String(x).replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
 const doc=()=>s.docs[s.file],ed=$('#editor'),search=$('#search');
 const bytes=x=>new TextEncoder().encode(x).length;
 function say(text){$('#feedback').textContent=text;}
 function sync(){const url=new URL(location.href);for(const[k,v]of Object.entries({variant:s.variant,scenario:s.scenario,theme:s.theme,replace:s.expanded?'1':'0'}))url.searchParams.set(k,v);history.replaceState(null,'',url);}
 function cancelPending(message){if(!s.pending)return;s.pending=null;$('#confirm').hidden=true;document.body.dataset.confirm='false';if(message)say(message);}
 function invalid(){return bytes(s.query)>=128?'Suchtext ist länger als 127 UTF-8-Bytes. Bitte kürzen.':bytes(s.replacement)>=128&&s.expanded?'Ersetzung ist länger als 127 UTF-8-Bytes. Bitte kürzen.':'';}
 function matches(){const result=[];if(!s.query||bytes(s.query)>=128)return result;let at=0;while((at=doc().text.indexOf(s.query,at))!==-1){result.push([at,at+s.query.length]);at+=s.query.length;}return result;}
 function mirror(){
  const text=doc().text,points=new Set([0,text.length,...s.hits.flat(),...s.selection]);
  const bounds=[...points].filter(x=>x>=0&&x<=text.length).sort((a,b)=>a-b);let html='';
  for(let i=0;i<bounds.length-1;i++){
   const begin=bounds[i],end=bounds[i+1];let part=escape(text.slice(begin,end));
   if(s.selection[0]<=begin&&s.selection[1]>=end&&s.selection[0]!==s.selection[1])part='<span class="selected-range">'+part+'</span>';
   const hit=s.hits.findIndex(h=>h[0]<=begin&&h[1]>=end);
   if(hit>=0)part='<mark class="'+(s.active===hit?'active':'')+'">'+part+'</mark>';
   html+=part;
  }
  $('#mirror').innerHTML=html+'\n';$('#mirror').scrollTop=ed.scrollTop;$('#mirror').scrollLeft=ed.scrollLeft;
  $('#selection-state').textContent=s.selection[1]>s.selection[0]?`${[...text.slice(...s.selection)].length} Zeichen ausgewählt`:'Keine Textauswahl';
 }
 function results(){
  const list=$('#results');list.replaceChildren();const heading=document.createElement('h3');heading.textContent='Fundstellen · '+names[s.file];list.append(heading);
  if(s.phase!=='ready'||!s.query||!s.hits.length){const p=document.createElement('p');p.className='results-empty';p.textContent=s.phase!=='ready'?'Die Quelle ist noch nicht durchsuchbar.':!s.query?'Suchtext eingeben, um Fundstellen zu sehen.':'Keine Fundstelle. Suchtext prüfen oder eine andere Datei öffnen.';list.append(p);return;}
  s.hits.forEach((hit,i)=>{
   const line=doc().text.slice(0,hit[0]).split('\n').length,start=doc().text.lastIndexOf('\n',hit[0]-1)+1;
   let end=doc().text.indexOf('\n',hit[1]);if(end<0)end=doc().text.length;
   const left=Math.max(start,hit[0]-42),right=Math.min(end,hit[1]+65),button=document.createElement('button');button.className='result';button.dataset.hit=i;button.setAttribute('aria-current',String(s.active===i));
   button.setAttribute('aria-label',`Treffer ${i+1}, Zeile ${line}, ${names[s.file]}`);
   button.innerHTML=`<span>Zeile ${line} · ${i+1} von ${s.hits.length}</span><code>${left>start?'…':''}${escape(doc().text.slice(left,hit[0]))}<mark>${escape(doc().text.slice(...hit))}</mark>${escape(doc().text.slice(hit[1],right))}${right<end?'…':''}</code>`;
   button.addEventListener('click',()=>activate(i,true));list.append(button);
  });
 }
 function render(){
  s.hits=matches();if(s.active>=s.hits.length)s.active=-1;
  document.body.dataset.variant=s.variant;document.body.dataset.expanded=String(s.expanded);document.body.dataset.blocked=String(s.phase!=='ready'||!!invalid());document.body.dataset.confirm=String(!!s.pending);document.documentElement.dataset.theme=s.theme;
  $('#direction').textContent=variants[s.variant][0];$('#direction-title').textContent=variants[s.variant][1];$('#theme').textContent='Darstellung: '+{dark:'Hell',light:'Hoher Kontrast',contrast:'Dunkel'}[s.theme];
  document.querySelectorAll('button[data-variant]').forEach(b=>b.setAttribute('aria-pressed',String(b.dataset.variant===s.variant)));
  document.querySelectorAll('[data-file]').forEach(b=>{b.setAttribute('aria-selected',String(b.dataset.file===s.file));b.tabIndex=b.dataset.file===s.file?0:-1;b.textContent=names[b.dataset.file]+(s.docs[b.dataset.file].dirty?' ·':'');});
  $('#file-path').textContent=s.scenario==='long'?'experiments / vergleichsreihen / sehr-lange-bezeichnung-der-pendelstudie-mit-unterschiedlichen-daempfungskoeffizienten / '+names[s.file]:'experiments / pendel / '+names[s.file];
  $('#document-label').textContent=names[s.file]+(doc().readonly?' · Nur lesen':'');$('#search-file').textContent=names[s.file];search.setAttribute('aria-label','Suche in '+names[s.file]);search.hidden=!s.open;
  $('#replacement').hidden=!s.expanded;$('#expand').textContent=s.expanded?'⌄':'›';$('#expand').setAttribute('aria-expanded',String(s.expanded));$('#expand').setAttribute('aria-label',s.expanded?'Ersetzung einklappen':'Ersetzung aufklappen');$('#expand').title=$('#expand').getAttribute('aria-label');
  const unavailable=s.phase!=='ready'||!!invalid(),none=!s.hits.length;
  for(const id of ['previous','next'])$('#'+id).disabled=unavailable||none;
  for(const id of ['replace-one','replace-all'])$('#'+id).disabled=unavailable||none||doc().readonly;
  $('#replacement-input').readOnly=doc().readonly;ed.readOnly=doc().readonly||s.phase!=='ready';ed.setAttribute('aria-label',(ed.readOnly?'Nur lesbarer':'Bearbeitbarer')+' Beispielcode · '+names[s.file]);
  $('#undo').disabled=!doc().history.length||s.phase!=='ready'||doc().readonly;
  $('#dirty-state').textContent=doc().dirty?'Ungespeicherte Beispieländerungen':doc().readonly?'Nur lesen · Suchen bleibt möglich':'Gespeicherter Beispielstand';$('#dirty-state').dataset.dirty=String(doc().dirty);
  const status=s.phase==='loading'?'Quelle wird geladen…':s.phase==='error'?'Quelle nicht verfügbar':invalid()?'Eingabe prüfen':!s.query?'Suchtext eingeben':none?'Keine Treffer':`${s.active<0?'–':s.active+1} von ${s.hits.length} Treffern`;
  $('#search-status').textContent=status;$('#query').setAttribute('aria-invalid',String(bytes(s.query)>=128));$('#replacement-input').setAttribute('aria-invalid',String(bytes(s.replacement)>=128));
  $('#problem').hidden=!unavailable;$('#problem-text').textContent=s.phase==='loading'?'Der Beispielquellstand wird noch geladen. Ersetzen ist bis dahin gesperrt.':s.phase==='error'?'Der Quellstand konnte nicht gelesen werden. Der vorhandene Beispieltext bleibt erhalten.':invalid();
  $('#retry').hidden=s.phase==='ready';$('#retry').textContent=s.phase==='loading'?'Beispielladen abschließen':'Erneut prüfen';$('#cancel-load').hidden=s.phase!=='loading';
  $('#confirm').hidden=!s.pending;results();mirror();sync();
 }
 function activate(index,focusEditor=false){
  if(s.phase!=='ready'||!s.hits.length)return;s.active=index;const hit=s.hits[index];s.selection=[...hit];ed.setSelectionRange(...hit);
  const style=getComputedStyle(ed),lineHeight=parseFloat(style.lineHeight);ed.scrollTop=Math.max(0,(doc().text.slice(0,hit[0]).split('\n').length-4)*lineHeight);
  const lineStart=doc().text.lastIndexOf('\n',hit[0]-1)+1,measure=document.createElement('canvas').getContext('2d');measure.font=style.font;
  ed.scrollLeft=Math.max(0,measure.measureText(doc().text.slice(lineStart,hit[1])).width-ed.clientWidth+30);
  if(focusEditor)ed.focus({preventScroll:true});render();
 }
 function move(direction){
  if(s.phase!=='ready'||invalid()||!s.hits.length)return;
  const next=s.active<0?(direction>0?0:s.hits.length-1):(s.active+direction+s.hits.length)%s.hits.length;
  const wrap=s.active>=0&&((direction>0&&next===0)||(direction<0&&next===s.hits.length-1));activate(next);say(wrap?(direction>0?'Am Dateiende weiter zum ersten Treffer.':'Am Dateianfang weiter zum letzten Treffer.'):`Treffer ${next+1} in ${names[s.file]}.`);
 }
 function record(){doc().history.push({text:doc().text,selection:[...s.selection],dirty:doc().dirty});}
 function changed(text,selection){doc().text=text;doc().dirty=true;doc().revision++;ed.value=text;s.selection=selection;ed.setSelectionRange(...selection);cancelPending();s.active=-1;render();}
 function replaceOne(){
  if($('#replace-one').disabled)return;
  const selected=s.hits.findIndex(h=>h[0]===s.selection[0]&&h[1]===s.selection[1]);
  const nextIndex=selected>=0?selected:s.hits.findIndex(h=>h[0]>=s.selection[1]);activate(nextIndex<0?0:nextIndex);
  const hit=s.hits[s.active];if(!hit)return;record();const next=doc().text.slice(0,hit[0])+s.replacement+doc().text.slice(hit[1]);changed(next,[hit[0],hit[0]+s.replacement.length]);say(`Ein Treffer in ${names[s.file]} ersetzt. Ungespeichert · Rückgängig verfügbar.`);
 }
 function askAll(){
  if($('#replace-all').disabled)return;
  s.pending={file:s.file,revision:doc().revision,query:s.query,replacement:s.replacement,count:s.hits.length};
  $('#confirm-title').textContent=`${s.hits.length} Treffer in ${names[s.file]} ersetzen?`;
  $('#confirm-description').textContent=`„${s.query}“ → ${s.replacement?'„'+s.replacement+'“':'leerer Text (Treffer löschen)'}. Nur diese Datei. Eine gemeinsame rückgängig machbare Änderung; es wird nichts gespeichert.`;
  render();$('#confirm-replace').focus({preventScroll:true});
 }
 function replaceAll(){
  const pending=s.pending;if(!pending)return;
  if(pending.file!==s.file||pending.revision!==doc().revision||pending.query!==s.query||pending.replacement!==s.replacement||doc().readonly||s.phase!=='ready'||invalid()) {cancelPending('Der Stand hat sich geändert. Ersetzungen bitte neu prüfen.');render();return;}
  record();const text=doc().text.split(s.query).join(s.replacement),count=s.hits.length;changed(text,[0,0]);say(`${count} Treffer nur in ${names[s.file]} ersetzt. Ungespeichert · als ein Schritt rückgängig.`);$('#replacement-input').focus({preventScroll:true});
 }
 function undo(){if($('#undo').disabled)return;const before=doc().history.pop();doc().text=before.text;doc().dirty=before.dirty;doc().revision++;ed.value=before.text;s.selection=before.selection;ed.setSelectionRange(...s.selection);s.active=-1;cancelPending();render();say('Letzte Beispieländerung rückgängig gemacht.');}
 function close(){cancelPending();s.open=false;render();ed.focus({preventScroll:true});say('Suche geschlossen. Text und Auswahl bleiben erhalten.');}
 function open(){s.open=true;render();$('#query').focus({preventScroll:true});$('#query').select();}
 function selectFile(id){cancelPending('Datei gewechselt. Vorherige Ersetzungsprüfung verworfen.');s.file=id;s.active=-1;s.selection=[0,0];ed.value=doc().text;ed.setSelectionRange(0,0);render();say(`Suche jetzt nur in ${names[id]}. Andere Dateiänderungen bleiben erhalten.`);}
 function reset(){
  s.docs=Object.fromEntries(Object.entries(originals).map(([id,text])=>[id,{text,dirty:false,history:[],revision:1,readonly:s.scenario==='readonly'}]));
  s.file='experiment';s.open=true;s.phase=['loading','error'].includes(s.scenario)?s.scenario:'ready';s.pending=null;s.query=s.scenario==='none'?'air_resistance':s.scenario==='empty'?'':s.scenario==='unicode'?'α😀':'damping';s.replacement=s.scenario==='unicode'?'β🌙':'friction';
  if(s.scenario==='unicode')doc().text='// UTF-8 · Präfix ä und 🌍\nconst char *label = "α😀";\n// α😀 bleibt vollständig.\n// α😀\n';
  if(s.scenario==='long')doc().text='// Lange Zeilen und Fundstellen außerhalb des Anfangs\n'+Array.from({length:45},(_,i)=>i%7===0?'    '+('context_'.repeat(30))+' damping = 0.02;':`// Zeile ${i+2}: Messlauf und Auswertung`).join('\n')+'\n';
  $('#query').value=s.query;$('#replacement-input').value=s.replacement;ed.value=doc().text;s.selection=[0,0];s.active=-1;render();
  if(s.hits.length&&s.phase==='ready')activate(0);say('Beispiel bereit. Nur die aktuelle Datei wird durchsucht.');
 }
 $('#query').addEventListener('input',e=>{s.query=e.target.value;s.active=-1;cancelPending('Suchtext geändert. Vorherige Ersetzungsprüfung verworfen.');render();});
 $('#replacement-input').addEventListener('input',e=>{s.replacement=e.target.value;cancelPending('Ersetzung geändert. Vorherige Prüfung verworfen.');render();});
 $('#expand').addEventListener('click',()=>{s.expanded=!s.expanded;cancelPending();render();if(s.expanded)$('#replacement-input').focus({preventScroll:true});else $('#query').focus({preventScroll:true});});
 $('#previous').addEventListener('click',()=>move(-1));$('#next').addEventListener('click',()=>move(1));$('#replace-one').addEventListener('click',replaceOne);$('#replace-all').addEventListener('click',askAll);$('#confirm-replace').addEventListener('click',replaceAll);
 $('#cancel-replace').addEventListener('click',()=>{cancelPending();render();$('#replace-all').focus({preventScroll:true});say('Alle Ersetzungen abgebrochen. Beispieltext unverändert.');});
 $('#close').addEventListener('click',close);$('#open-search').addEventListener('click',open);$('#undo').addEventListener('click',undo);$('#reset').addEventListener('click',reset);
 $('#retry').addEventListener('click',()=>{s.phase='ready';cancelPending();render();say('Beispielquelle wieder verfügbar. Kein Dateizugriff ausgeführt.');$('#query').focus({preventScroll:true});});$('#cancel-load').addEventListener('click',()=>{s.phase='ready';close();say('Beispielladen abgebrochen. Vorhandener Text erhalten.');});
 ed.addEventListener('input',()=>{if(ed.value===doc().text)return;record();doc().text=ed.value;doc().dirty=true;doc().revision++;s.selection=[ed.selectionStart,ed.selectionEnd];s.active=-1;cancelPending('Textstand geändert. Vorherige Ersetzungsprüfung verworfen.');render();});
 const selection=()=>{s.selection=[ed.selectionStart,ed.selectionEnd];mirror();};ed.addEventListener('select',selection);ed.addEventListener('scroll',mirror);ed.addEventListener('focus',()=>{document.body.dataset.editorFocus='true';selection();});ed.addEventListener('blur',()=>{document.body.dataset.editorFocus='false';selection();});document.body.dataset.editorFocus='false';
 document.querySelectorAll('[data-variant]').forEach(b=>b.addEventListener('click',()=>{s.variant=b.dataset.variant;render();}));
 const tabs=[...document.querySelectorAll('[data-file]')];tabs.forEach((b,i)=>{b.addEventListener('click',()=>selectFile(b.dataset.file));b.addEventListener('keydown',e=>{if(!['ArrowLeft','ArrowRight','Home','End'].includes(e.key))return;e.preventDefault();const next=e.key==='Home'?0:e.key==='End'?tabs.length-1:(i+(e.key==='ArrowRight'?1:-1)+tabs.length)%tabs.length;selectFile(tabs[next].dataset.file);tabs[next].focus();});});
 $('#scenario').value=s.scenario;$('#scenario').addEventListener('change',e=>{s.scenario=e.target.value;reset();});$('#theme').addEventListener('click',()=>{s.theme={dark:'light',light:'contrast',contrast:'dark'}[s.theme];render();});$('#opaque').addEventListener('change',e=>document.body.dataset.opaque=String(e.target.checked));
 document.addEventListener('keydown',e=>{
  if(e.isComposing)return;
  if((e.ctrlKey||e.metaKey)&&e.key.toLowerCase()==='f'){e.preventDefault();open();}
  else if((e.ctrlKey||e.metaKey)&&!e.shiftKey&&e.key.toLowerCase()==='z'&&e.target===ed){e.preventDefault();undo();}
  else if(e.key==='Escape'&&s.open&&(search.contains(e.target)||e.target===ed)){e.preventDefault();if(s.pending){cancelPending();render();$('#replace-all').focus({preventScroll:true});say('Ersetzungsprüfung abgebrochen.');}else close();}
  else if(e.key==='Enter'&&(e.target===$('#query')||e.target===$('#replacement-input'))){e.preventDefault();move(e.shiftKey?-1:1);}
 });
 new ResizeObserver(()=>document.querySelector('.editor-layout').style.setProperty('--search-height',search.getBoundingClientRect().height+'px')).observe(search);
 reset();
})();
