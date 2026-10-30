'use strict';
(()=>{
 const $=x=>document.querySelector(x),query=new URLSearchParams(location.search),data=window.PHYSIM_COMPLETION_DATA.symbols;
 const variants={a:['A · KOMPAKT','Wenig Fläche. Klarer Vorschlag.'],b:['B · LISTE & KURZINFO','Name und Bedeutung nebeneinander.'],c:['C · INFO IN DER ZEILE','Die aktive Zeile erzählt mehr.']};
 const scenarios=['normal','long','parameters','member','static','type','none','comment','readonly','reference-error','reference-loading','edge'];
 const s={variant:variants[query.get('variant')]?query.get('variant'):'a',scenario:scenarios.includes(query.get('scenario'))?query.get('scenario'):'normal',theme:['dark','light','contrast'].includes(query.get('theme'))?query.get('theme'):'dark',docs:{},file:'experiment',open:false,matches:[],index:0,snapshot:null,serial:0,details:false,reference:'ready'};
 const ed=$('#editor'),popup=$('#popup'),frame=$('#frame'),list=$('#choices'),doc=()=>s.docs[s.file],esc=x=>String(x).replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
 const word=c=>c!==undefined&&/[a-zA-Z0-9_]/.test(c),utf8=x=>new TextEncoder().encode(x).length;
 function say(text){$('#feedback').textContent=text;}
 function sync(){const u=new URL(location.href);for(const[k,v]of Object.entries({variant:s.variant,scenario:s.scenario,theme:s.theme}))u.searchParams.set(k,v);history.replaceState(null,'',u);}
 function prepare(text,cursor,language){
  let start=cursor,end=cursor;while(start&&word(text[start-1]))start--;while(end<text.length&&word(text[end]))end++;
  if(start&&text.codePointAt(start-1)>=128)return {error:'Kein unterstützter API-Bezeichner an dieser Position.'};
  if(start<cursor&&/[0-9]/.test(text[start]))return {error:'Zahlen sind keine API-Bezeichner.'};
  const prefix=text.slice(start,cursor);if(utf8(prefix)>=128)return {error:'Der Präfix ist länger als 127 UTF-8-Bytes.'};
  let block=0,line=false,quote='';
  for(let i=0;i<start;i++){const c=text[i],n=text[i+1];if(line){if(c==='\n')line=false;continue;}if(block){if(c==='*'&&n==='/'){block--;i++;}else if(language&&c==='/'&&n==='*'){block++;i++;}continue;}if(quote){if(c==='\\'&&n)i++;else if(c===quote)quote='';continue;}if(c==='/'&&n==='/'){line=true;i++;}else if(c==='/'&&n==='*'){block=1;i++;}else if(c==='"'||(!language&&c==="'"))quote=c;}
  if(block||line||quote)return {error:'In Kommentaren und Zeichenketten werden keine API-Namen ergänzt.'};
  let member=false,receiver='';if(start&&text[start-1]==='.'){
   if(!language)return {error:'C-Member werden von der vorhandenen API-Vervollständigung nicht aufgelöst.'};member=true;let begin=start-1;while(begin&&word(text[begin-1]))begin--;receiver=text.slice(begin,start-1);if(utf8(receiver)>=128)return {error:'Der Empfänger ist zu lang.'};
  }else if(!language&&text.slice(start-2,start)==='->')return {error:'C-Member werden von der vorhandenen API-Vervollständigung nicht aufgelöst.'};
  return {start,end,prefix,member,receiver,language};
 }
 function insertion(symbol,q){return q.member?symbol.name.slice(symbol.name.lastIndexOf('.')+1):symbol.name;}
 function matching(q){
  const known=q.member&&!!q.receiver&&data.some(x=>x.language&&x.name===q.receiver&&(x.kind==='type'||/^[A-Z]/.test(x.name)));
  return data.filter(x=>x.language===q.language&&(!q.language||q.member===x.name.includes('.'))&&(!known||x.name.startsWith(q.receiver+'.'))&&(!q.language||!q.member||x.instance!==known)&&insertion(x,q).startsWith(q.prefix));
 }
 function mirror(){
  const text=doc().text,cursor=ed.selectionEnd,q=s.snapshot?.query;
  const start=s.open&&q?q.start:cursor;
  $('#mirror').innerHTML=esc(text.slice(0,start))+(s.open&&q?'<span class="completion-prefix">'+esc(text.slice(start,cursor))+'</span>':'')+'<span id="caret" class="caret-marker">&#8203;</span>'+esc(text.slice(cursor))+'\n';
  $('#mirror').scrollTop=ed.scrollTop;$('#mirror').scrollLeft=ed.scrollLeft;
  $('#position').textContent=`Zeile ${text.slice(0,cursor).split('\n').length} · ${doc().language?'Physim':'C'}`;
 }
 function updateChrome(){
  document.body.dataset.variant=s.variant;document.documentElement.dataset.theme=s.theme;$('#direction').textContent=variants[s.variant][0];$('#direction-title').textContent=variants[s.variant][1];$('#theme').textContent='Darstellung: '+{dark:'Hell',light:'Hoher Kontrast',contrast:'Dunkel'}[s.theme];
  document.querySelectorAll('button[data-variant]').forEach(b=>b.setAttribute('aria-pressed',String(b.dataset.variant===s.variant)));
  document.querySelectorAll('[data-file]').forEach(b=>{const active=b.dataset.file===s.file;b.setAttribute('aria-selected',String(active));b.tabIndex=active?0:-1;b.textContent=(b.dataset.file==='experiment'?(doc().language?'pendel.phys':'pendel.c'):(doc().language?'auswertung.phys':'auswertung.c'))+(s.docs[b.dataset.file].dirty?' ·':'');});
  $('#source-label').textContent='experiments / pendel / '+(s.file==='experiment'?'pendel':'auswertung')+(doc().language?'.phys':'.c');$('#language').textContent=(doc().language?'Physim':'C')+' · UTF-8';$('#document-state').textContent=doc().readonly?'Nur lesen · Einfügen gesperrt':doc().dirty?'Ungespeicherte Beispieländerungen':'Gespeicherter Beispielstand';ed.readOnly=doc().readonly;$('#undo').disabled=!doc().history.length||doc().readonly;$('#open').disabled=doc().readonly;$('#finish-load').disabled=s.reference==='ready';sync();
 }
 function close(message='',focus=false){s.open=false;s.snapshot=null;s.serial++;popup.hidden=true;ed.removeAttribute('aria-activedescendant');mirror();if(focus)ed.focus({preventScroll:true});if(message)say(message);}
 function info(symbol,compact=false,allowAction=true){
  const section=document.createElement('section');section.className='description';section.setAttribute('aria-label','Information zu '+symbol.name);
  section.innerHTML=`<p class="eyebrow">${symbol.kind==='function'?'FUNKTION':symbol.kind==='type'?'TYP':'WERT'} / ${esc(symbol.module)}</p><pre class="signature">${esc(symbol.signature)}</pre>`;
  const details=document.createElement(compact?'details':'div');if(compact){details.open=s.details;const summary=document.createElement('summary');summary.textContent='Kurzinfo und Referenz';details.append(summary);details.addEventListener('toggle',()=>{s.details=details.open;position();});}
  if(s.reference==='loading'||s.reference==='error'){
   const note=document.createElement('p');note.className='reference-state '+(s.reference==='loading'?'loading':'');note.textContent=s.reference==='loading'?'Referenz wird geladen… · Zustandsentwurf. Der Name kann weiterhin eingefügt werden.':'Referenz nicht verfügbar. Signatur und Einfügen bleiben verfügbar; die bisherige API-Karte bleibt erhalten.';details.append(note);
  }else{
   const p=document.createElement('p');p.textContent=symbol.summary||'Für diesen Katalogeintrag liegt keine Kurzbeschreibung vor.';details.append(p);
   if(symbol.parameters.length){const dl=document.createElement('dl');dl.className='parameter-list';for(const[name,description]of symbol.parameters){const dt=document.createElement('dt'),dd=document.createElement('dd');dt.textContent=name;dd.textContent=description;dl.append(dt,dd);}details.append(dl);}
  }
  const button=document.createElement('button');button.type='button';button.className='reference';button.textContent='Referenz öffnen ↗';button.disabled=s.reference!=='ready'||!symbol.documented;button.setAttribute('aria-label','Referenz für '+symbol.name+' öffnen');
  button.title='Referenz öffnen · Umschalt+F1';const captured=s.snapshot;button.addEventListener('click',()=>reference(symbol,captured));if(allowAction)details.append(button);section.append(details);return section;
 }
 function renderPopup(){
  updateChrome();popup.hidden=!s.open;list.replaceChildren();$('#description').replaceChildren();if(!s.open){mirror();return;}
  const captured=s.snapshot;$('#popup-context').textContent=(doc().language?'PHYSIM':'C-API')+' / '+(captured.query.member?captured.query.receiver+'.':'')+captured.query.prefix;$('#count').textContent=`${s.index+1} von ${s.matches.length}`;$('.popup-footer .reference')?.remove();
  s.matches.forEach((symbol,i)=>{
   const row=document.createElement('div');row.className='choice';row.id='choice-'+i;row.setAttribute('role','option');row.setAttribute('aria-selected',String(i===s.index));row.dataset.name=symbol.name;
   const leaf=insertion(symbol,captured.query),prefix=captured.query.prefix;row.innerHTML=`<span class="symbol-icon" aria-hidden="true">${symbol.kind==='function'?'ƒ':symbol.kind==='type'?'T':'#'}</span><span class="choice-text"><span class="choice-name"><mark>${esc(leaf.slice(0,prefix.length))}</mark>${esc(leaf.slice(prefix.length))}</span><span class="choice-module">${esc(symbol.module)} · ${symbol.kind==='function'?'Funktion':symbol.kind==='type'?'Typ':'Wert'}</span></span>`;
   row.setAttribute('aria-label',symbol.name+', '+symbol.signature);
   if(i===s.index&&s.variant==='c'){row.append(info(symbol,false,false));row.setAttribute('aria-description',symbol.summary||'Keine Kurzbeschreibung vorhanden.');}
   row.addEventListener('pointermove',()=>{if(s.variant!=='c'&&s.index!==i&&s.open){s.index=i;renderPopup();}});
   row.addEventListener('mousedown',e=>{if(!e.target.closest('button,summary'))e.preventDefault();});
   row.addEventListener('click',e=>{if(e.target.closest('button,summary,details,.description'))return;commit(symbol,captured);});list.append(row);
  });
  const selected=s.matches[s.index];if(s.variant!=='c')$('#description').replaceWith(Object.assign(info(selected,s.variant==='a'),{id:'description'}));
  else {const button=document.createElement('button');button.className='reference';button.textContent='Referenz öffnen ↗';button.title='Referenz öffnen · Umschalt+F1';button.setAttribute('aria-label','Referenz für '+selected.name+' öffnen');button.disabled=s.reference!=='ready'||!selected.documented;button.addEventListener('click',()=>reference(selected,captured));$('.popup-footer').append(button);}
  ed.setAttribute('aria-activedescendant','choice-'+s.index);mirror();position();
  const active=list.children[s.index];if(active){const top=active.offsetTop-list.offsetTop,bottom=top+active.offsetHeight;if(active.offsetHeight>list.clientHeight||top<list.scrollTop)list.scrollTop=top;else if(bottom>list.scrollTop+list.clientHeight)list.scrollTop=bottom-list.clientHeight;}
 }
 function position(){
  if(!s.open||popup.hidden)return;const marker=$('#caret');if(!marker)return;
  const f=frame.getBoundingClientRect(),c=marker.getBoundingClientRect(),x=c.left-f.left,y=c.top-f.top,bottom=c.bottom-f.top;
  popup.style.maxHeight=Math.min(frame.clientHeight-16,440,innerHeight-20)+'px';
  const natural=popup.getBoundingClientRect().height,below=frame.clientHeight-bottom-8,above=y-8;
  const up=natural>below&&above>below,available=Math.max(100,up?above:below);popup.style.maxHeight=Math.min(440,available,innerHeight-20)+'px';
  const size=popup.getBoundingClientRect();popup.style.left=Math.max(8,Math.min(x,frame.clientWidth-size.width-8))+'px';popup.style.top=Math.max(8,up?y-size.height-5:bottom+5)+'px';popup.dataset.placement=up?'above':'below';
 }
 function open(){
  $('#empty-state').hidden=true;if(doc().readonly){close();$('#empty-state').textContent='Nur lesen: API-Vorschläge können hier keinen Text einfügen.';$('#empty-state').hidden=false;return;}
  if(ed.selectionStart!==ed.selectionEnd){close('Auswahl zuerst aufheben. Vorschläge ergänzen einen Bezeichner am Cursor.');return;}
  const q=prepare(doc().text,ed.selectionEnd,doc().language);if(q.error){close();$('#empty-state').textContent=q.error;$('#empty-state').hidden=false;return;}
  const previous=s.matches[s.index]?.name;s.matches=matching(q);if(!s.matches.length){close();$('#empty-state').textContent='Keine API-Vorschläge für „'+q.prefix+'“. Weiter tippen oder den Präfix prüfen.';$('#empty-state').hidden=false;return;}
  s.index=Math.max(0,s.matches.findIndex(x=>x.name===previous));s.open=true;s.snapshot={file:s.file,revision:doc().revision,text:doc().text,cursor:ed.selectionEnd,scrollX:ed.scrollLeft,scrollY:ed.scrollTop,query:q,serial:++s.serial};renderPopup();
 }
 function valid(captured){return !!captured&&s.open&&captured===s.snapshot&&captured.file===s.file&&captured.revision===doc().revision&&captured.text===doc().text&&ed.selectionStart===ed.selectionEnd&&captured.cursor===ed.selectionEnd&&!doc().readonly;}
 function commit(symbol,captured){
  if(!valid(captured)){say('Veralteter Vorschlag verworfen. Beispieltext unverändert.');return;}
  const q=captured.query,insert=insertion(symbol,q),old=doc().text,position=q.start+insert.length;
  if(old.slice(q.start,q.end)!==insert){doc().history.push({text:old,cursor:ed.selectionEnd,dirty:doc().dirty});doc().text=old.slice(0,q.start)+insert+old.slice(q.end);doc().dirty=true;doc().revision++;}
  ed.value=doc().text;ed.setSelectionRange(position,position);close('„'+insert+'“ als Name eingefügt. Kein Aufruf, kein Speichern oder Kompilieren.',true);updateChrome();
 }
 function reference(symbol,captured){
  if(!valid(captured)||s.reference!=='ready'||!symbol.documented)return;
  $('#reference-result').hidden=false;$('#reference-name').textContent=symbol.name;$('#reference-source').textContent=symbol.reference+' · '+symbol.signature;
  close('Referenz für „'+symbol.name+'“ angefordert. Der Beispielcode bleibt unverändert.',true);
 }
 function undo(){if(!doc().history.length||doc().readonly)return;const before=doc().history.pop();doc().text=before.text;doc().dirty=before.dirty;doc().revision++;ed.value=before.text;ed.setSelectionRange(before.cursor,before.cursor);close('Letzte Beispieländerung rückgängig gemacht.',true);updateChrome();}
 function selectFile(id){close();s.file=id;ed.value=doc().text;ed.setSelectionRange(doc().text.length,doc().text.length);$('#empty-state').hidden=true;updateChrome();mirror();say('Dokument gewechselt. Vorherige Vorschläge verworfen; andere Entwürfe bleiben erhalten.');}
 function reset(){
  const language=['member','static'].includes(s.scenario);let line='ps_vOLD',prefix='ps_v';
  if(s.scenario==='long'){line='ps_contact_worldOLD';prefix='ps_contact_world';}if(s.scenario==='parameters'){line='ps_channel_aOLD';prefix='ps_channel_a';}if(s.scenario==='type'){line='ps_vecOLD';prefix='ps_vec';}if(s.scenario==='member'){line='direction.normOLD';prefix='direction.norm';}if(s.scenario==='static'){line='Mat4.transOLD';prefix='Mat4.trans';}if(s.scenario==='none'){line='ps_nothing_matchesOLD';prefix='ps_nothing_matches';}if(s.scenario==='comment'){line='// ps_vOLD';prefix='// ps_v';}
  const before='// Pendelstudie · Präfix α 😀 bleibt erhalten\n'+(language?'let direction = Vec3(0.0, 1.0, 0.0)\n':'#include "physim/core.h"\n')+'\n'+(s.scenario==='edge'?'\n'.repeat(11):'');
  const text=before+line+'\n',cursor=before.length+prefix.length;
  s.docs={experiment:{text,language,readonly:s.scenario==='readonly',dirty:false,history:[],revision:1},analysis:{text:'// Eigener Analysestand\n'+(language?'Mat4.trans':'ps_vl')+'\n',language,readonly:false,dirty:false,history:[],revision:1}};
  s.file='experiment';s.open=false;s.snapshot=null;s.matches=[];s.index=0;s.details=['reference-error','reference-loading'].includes(s.scenario);s.reference=s.scenario==='reference-error'?'error':s.scenario==='reference-loading'?'loading':'ready';$('#reference-result').hidden=true;ed.value=text;ed.setSelectionRange(cursor,cursor);ed.scrollTop=0;ed.scrollLeft=0;$('#scenario').value=s.scenario;updateChrome();open();mirror();say('Beispielzustand geöffnet. Einfügen ändert ausschließlich den Bezeichner im Beispieltext.');
 }
 document.querySelectorAll('button[data-variant]').forEach(b=>b.addEventListener('click',()=>{s.variant=b.dataset.variant;renderPopup();}));
 $('#scenario').addEventListener('change',e=>{s.scenario=e.target.value;reset();});$('#theme').addEventListener('click',()=>{s.theme={dark:'light',light:'contrast',contrast:'dark'}[s.theme];renderPopup();});$('#open').addEventListener('click',()=>{ed.focus({preventScroll:true});open();});$('#close').addEventListener('click',()=>close('Vorschläge geschlossen. Text unverändert.',true));$('#reset').addEventListener('click',reset);$('#undo').addEventListener('click',undo);
 $('#invalidate').addEventListener('click',()=>{doc().revision++;close('Dokumentstand geändert. Offenes Popup und alte Aktionen verworfen.');updateChrome();});$('#finish-load').addEventListener('click',()=>{s.reference='ready';renderPopup();say('Beispiel-Referenz verfügbar. Kein nativer Ladevorgang ausgeführt.');});
 const tabs=[...document.querySelectorAll('[data-file]')];tabs.forEach((b,i)=>{b.addEventListener('click',()=>selectFile(b.dataset.file));b.addEventListener('keydown',e=>{if(!['ArrowLeft','ArrowRight','Home','End'].includes(e.key))return;e.preventDefault();const next=e.key==='Home'?0:e.key==='End'?tabs.length-1:1-i;selectFile(tabs[next].dataset.file);tabs[next].focus();});});
 ed.addEventListener('input',()=>{if(ed.value===doc().text)return;const wasOpen=s.open;doc().history.push({text:doc().text,cursor:ed.selectionEnd,dirty:doc().dirty});doc().text=ed.value;doc().dirty=true;doc().revision++;updateChrome();if(wasOpen)open();else mirror();});
 ed.addEventListener('click',()=>{if(s.open)close('Cursor versetzt. Vorschläge mit Strg+Leertaste erneut öffnen.');mirror();});ed.addEventListener('select',()=>{if(s.open&&(ed.selectionStart!==ed.selectionEnd||ed.selectionEnd!==s.snapshot?.cursor))close('Cursor oder Textauswahl geändert. Vorschläge geschlossen.');mirror();});ed.addEventListener('scroll',()=>{if(s.open&&(ed.scrollLeft!==s.snapshot?.scrollX||ed.scrollTop!==s.snapshot?.scrollY))close('Code gescrollt. Vorschläge geschlossen.');mirror();});
 ed.addEventListener('keydown',e=>{
  if(e.isComposing)return;if(e.ctrlKey&&e.code==='Space'&&!e.altKey&&!e.metaKey){e.preventDefault();open();return;}
  if((e.ctrlKey||e.metaKey)&&!e.shiftKey&&e.key.toLowerCase()==='z'){e.preventDefault();undo();return;}
  if(!s.open)return;if(e.key==='F1'&&e.shiftKey){e.preventDefault();const symbol=s.matches[s.index];if(s.reference==='ready'&&symbol.documented)reference(symbol,s.snapshot);else say('Referenz momentan nicht verfügbar. Der Beispielcode bleibt erhalten.');return;}
  if(['ArrowLeft','ArrowRight','PageUp','PageDown'].includes(e.key)){close('Cursor versetzt. Vorschläge geschlossen.');return;}
  if((e.ctrlKey||e.metaKey||e.altKey)||/^F\d+$/.test(e.key)){close();return;}
  if(['ArrowDown','ArrowUp','Home','End'].includes(e.key)){e.preventDefault();s.index=e.key==='Home'?0:e.key==='End'?s.matches.length-1:(s.index+(e.key==='ArrowDown'?1:-1)+s.matches.length)%s.matches.length;renderPopup();}
  else if(e.key==='Enter'||e.key==='Tab'){e.preventDefault();if(!e.repeat)commit(s.matches[s.index],s.snapshot);}
  else if(e.key==='Escape'){e.preventDefault();close('Vorschläge geschlossen. Text unverändert.',true);}
 });
 document.addEventListener('keydown',e=>{if(e.key==='Escape'&&s.open&&popup.contains(e.target)){e.preventDefault();close('Vorschläge geschlossen.',true);}});
 document.addEventListener('pointerdown',e=>{if(s.open&&!popup.contains(e.target)&&e.target!==ed&&!e.target.closest('.variants,.controls,#theme,#open,#finish-load'))close();},true);
 window.addEventListener('resize',position);window.addEventListener('blur',()=>{if(s.open)close('Fensterfokus gewechselt. Vorschläge geschlossen.');});new ResizeObserver(position).observe(frame);
 reset();
})();
