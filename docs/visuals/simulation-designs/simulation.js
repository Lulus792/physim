/* Local, illustrative design interactions. This is not the Physim runner. */
(() => {
  'use strict';
  const $ = id => document.getElementById(id);
  const app = document.querySelector('.app-window');
  const descriptions = {
    studio: ['01 / Studio','Ein vertrauter Arbeitsraum für Modell und Bewegung.','Alles an seinem Platz.','Links die Szenenstruktur, rechts der ausgewählte Körper. Kurven lassen sich bei Bedarf direkt unter den Messwerten aufklappen; zugeklappt erhält die Szene mehr Platz.'],
    labor: ['02 / Messlabor','Bewegung und Messwerte in einem gemeinsamen Blick.','Sehen, was die Bewegung bedeutet.','Zwei Signalverläufe verbinden Messwerte und Szene über einen gemeinsamen Zeitzeiger. Die Kurven lassen sich zuklappen, um der Szene mehr Raum zu geben.'],
    focus: ['03 / Fokus','Mehr Raum für die Szene, Details genau bei Bedarf.','Das Experiment im Mittelpunkt.','Beide Seiten sind zunächst eingeklappt. Auswahl, Messwerte und Zeit bleiben sichtbar; Szenenbaum und Inspektor lassen sich unabhängig wieder einblenden.']
  };
  const objects = {mass:['Pendelkörper','Kugel · ID 3'],support:['Aufhängung','Box · ID 1'],rod:['Pendelstange','Linie · ID 2'],velocity:['Geschwindigkeit','Vektor · ID 4'],trail:['Bahnkurve','Pfad · ID 5']};
  let mode='paused', time=2.4, liveTime=2.4, browsing=false, replay=false, selected='mass', last=0;
  const curvesByDesign={studio:false,labor:true,focus:false};
  const fmt = (n,d=2) => n.toLocaleString('de-DE',{minimumFractionDigits:d,maximumFractionDigits:d});
  const angle = t => .5*Math.cos(Math.sqrt(9.81/1.5)*t);
  const speed = t => -1.5*.5*Math.sqrt(9.81/1.5)*Math.sin(Math.sqrt(9.81/1.5)*t);
  function panels() {
    for(const [id,cls] of [['tree','tree-closed'],['inspector','inspector-closed']]) $(id+'-toggle').setAttribute('aria-expanded',String(!app.classList.contains(cls)));
  }
  function curves() {
    const open=curvesByDesign[app.dataset.design];
    $('curves').hidden=!open;
    app.classList.toggle('curves-open',open);
    $('curves-toggle').setAttribute('aria-expanded',String(open));
    $('curves-toggle').textContent=open?'⌃ Kurven zuklappen':'⌄ Kurven aufklappen';
  }
  function design(value) {
    app.dataset.design=value;
    curves();
    document.querySelectorAll('.variants [data-design]').forEach(b=>b.setAttribute('aria-pressed',String(b.dataset.design===value)));
    app.classList.toggle('tree-closed',value==='focus');app.classList.toggle('inspector-closed',value==='focus');panels();
    const [caption,description,title,note]=descriptions[value];
    $('design-caption').textContent=caption;$('design-description').textContent=description;$('note-number').textContent=caption.slice(0,2);$('note-title').textContent=title;$('note-text').textContent=note;
  }
  function object(value) {
    selected=value;
    document.querySelectorAll('[data-object]').forEach(b=>{b.classList.toggle('active',b.dataset.object===value);b.setAttribute('aria-pressed',String(b.dataset.object===value));});
    document.querySelectorAll('.selected-name').forEach(e=>e.textContent=objects[value][0]);$('selected-type').textContent=objects[value][1];
    const props={mass:[['Masse','1,00','kg'],['Radius','0,08','m'],['Position x','0','m'],['Position y','0','m']],support:[['Breite','0,40','m'],['Höhe','0,08','m'],['Position x','0,00','m'],['Position y','0,00','m']],rod:[['Länge','1,50','m'],['Von','ID 1',''],['Zu','ID 3','']],velocity:[['Zuordnung','ID 3',''],['Größe',fmt(Math.abs(speed(time))),'m/s']],trail:[['Zuordnung','ID 3',''],['Darstellung','Gestrichelt','']]}[value];
    document.querySelector('.focus-note > span').textContent=value==='mass'?'m = 1,00 kg · l = 1,50 m':props.slice(0,2).map(([k,v,u])=>k+': '+v+(u?' '+u:'')).join(' · ');
    $('object-properties').replaceChildren(...props.map(([k,v,u])=>{const row=document.createElement('div'),dt=document.createElement('dt'),dd=document.createElement('dd'),unit=document.createElement('span');dt.textContent=k;dd.textContent=v+' ';unit.textContent=u;dd.append(unit);row.append(dt,dd);if(k==='Position x'&&value==='mass')dd.id='position-value';if(k==='Position y'&&value==='mass')dd.id='position-y';return row;}));
    $('selection-ring').style.visibility=value==='mass'?'visible':'hidden';render();
  }
  function status() {
    const unavailable=mode==='empty'||mode==='error';
    app.dataset.state=mode;$('state-select').value=mode;
    $('run-status').textContent={paused:'Ⅱ Pausiert',running:'● Läuft',ready:'○ Bereit',empty:'○ Kein Projekt',error:'! Lauf fehlgeschlagen'}[mode];
    $('scene-state').textContent=browsing?'Aufzeichnung':{paused:'Pausiert',running:'Läuft',ready:'Bereit',empty:'Kein Projekt',error:'Fehler'}[mode];
    $('run-button').textContent=mode==='running'?'Ⅱ Pause':mode==='ready'?'▶ Neuer Lauf':'▶ Fortsetzen';
    for(const id of ['run-button','step-button','stop-button','reset-button','speed','time','replay-button','live-button']) $(id).disabled=unavailable;
    $('step-button').disabled=unavailable||mode==='running';$('stop-button').disabled=unavailable||mode==='ready';
    $('state-overlay').hidden=!unavailable;$('state-overlay').classList.toggle('error',mode==='error');
    $('overlay-title').textContent=mode==='error'?'Der Lauf wurde angehalten.':'Dein nächstes Experiment.';
    $('overlay-message').textContent=mode==='error'?'Das Modell hat einen ungültigen Wert geliefert. Bisher aufgezeichnete Messdaten bleiben erhalten. Details findest du im Protokoll.':'Öffne einen Projektordner, um dein Modell zu simulieren.';
    $('overlay-action').textContent=mode==='error'?'Beispiel zurücksetzen':'Beispiel anzeigen';
    $('status-detail').textContent=mode==='error'?'! Ungültiger Modellwert bei t = '+fmt(time)+' s':mode==='empty'?'Kein Physim-Projekt geöffnet':'✓ Modell gebaut';
    $('history-label').textContent=browsing?'Aufzeichnung · Lauf bleibt unverändert':'Aktuelle Szene';
    $('live-button').setAttribute('aria-pressed',String(!browsing));$('replay-button').textContent=replay?'Ⅱ Anhalten':'▷ Abspielen';
    $('footer-context').textContent=browsing?'Aufzeichnung · Live bei '+fmt(liveTime)+' s':'Lauf 004 · Seed 42';
  }
  function render() {
    const a=angle(time),v=speed(time),h=1.5*(1-Math.cos(a)),deg=a*180/Math.PI;
    const displayAngle=-Math.atan2(117,226)*180/Math.PI+deg;
    $('pendulum').setAttribute('transform',`rotate(${-displayAngle} 450 96)`);
    $('ball-shadow').setAttribute('cx',450+250*Math.sin(a));
    $('angle-value').innerHTML=fmt(deg)+' <small>°</small>';$('velocity-value').innerHTML=fmt(v)+' <small>m/s</small>';$('height-value').innerHTML=fmt(h)+' <small>m</small>';
    if($('position-value'))$('position-value').innerHTML=fmt(1.5*Math.sin(a))+' <span>m</span>';
    if($('position-y'))$('position-y').innerHTML=fmt(-1.5*Math.cos(a))+' <span>m</span>';
    $('measurement-time').textContent='bei t = '+fmt(time)+' s';$('time-value').innerHTML=fmt(time)+' <span>/ 6,00 s</span>';$('time').value=time;
    document.querySelectorAll('.chart-cursor').forEach(e=>{e.setAttribute('x1',32+time/6*416);e.setAttribute('x2',32+time/6*416);});
  }
  document.querySelectorAll('.variants [data-design]').forEach(b=>b.addEventListener('click',()=>design(b.dataset.design)));
  document.querySelectorAll('.segmented [data-theme]').forEach(b=>b.addEventListener('click',()=>{document.documentElement.dataset.theme=b.dataset.theme;document.querySelectorAll('.segmented [data-theme]').forEach(c=>c.setAttribute('aria-pressed',String(c===b)));}));
  $('tree-toggle').addEventListener('click',()=>{app.classList.toggle('tree-closed');panels();});$('inspector-toggle').addEventListener('click',()=>{app.classList.toggle('inspector-closed');panels();});
  $('curves-toggle').addEventListener('click',()=>{curvesByDesign[app.dataset.design]=!curvesByDesign[app.dataset.design];curves();});
  document.querySelectorAll('[data-object]').forEach(b=>b.addEventListener('click',()=>object(b.dataset.object)));
  for(const [id,target] of [['show-trail','trail'],['show-vectors','velocity'],['show-grid','floor-grid']]) $(id).addEventListener('change',()=>{(target==='floor-grid'?document.querySelector('.floor-grid'):$(target)).style.visibility=$(id).checked?'visible':'hidden';});
  $('state-select').addEventListener('change',()=>{mode=$('state-select').value;replay=false;browsing=false;if(mode==='ready'){time=0;liveTime=0;}status();render();});
  $('run-button').addEventListener('click',()=>{if(mode==='ready'){liveTime=time=0;browsing=false;}mode=mode==='running'?'paused':'running';replay=false;status();render();});
  $('step-button').addEventListener('click',()=>{liveTime=Math.min(6,liveTime+.001);if(!browsing)time=liveTime;status();render();});
  $('stop-button').addEventListener('click',()=>{mode='ready';replay=false;status();});
  $('reset-button').addEventListener('click',()=>{mode='paused';time=liveTime=0;browsing=replay=false;status();render();});
  $('overlay-action').addEventListener('click',()=>{mode='paused';time=liveTime=2.4;browsing=replay=false;status();render();});
  $('time').addEventListener('input',()=>{time=Number($('time').value);browsing=true;replay=false;status();render();});
  $('live-button').addEventListener('click',()=>{time=liveTime;browsing=replay=false;status();render();});
  $('replay-button').addEventListener('click',()=>{replay=!replay;if(replay){browsing=true;if(time>=6)time=0;}status();});
  function camera() {
    const c=$('camera').value;
    // Illustrative 2D projection only; full orbit belongs to the native scene renderer.
    const transforms={perspective:'',front:'translate(0 15) scale(1 .96)',side:'translate(225 0) scale(.5 1)',top:'translate(0 125) scale(1 .48)'};
    document.querySelector('.physics-scene').style.transform=transforms[c];
    $('view-label').textContent=$('camera').selectedOptions[0].text+' · Meter';
    document.querySelector('.scene-hint').textContent='Kameraansichten: illustrative Projektion';
  }
  $('camera').addEventListener('change',camera);$('camera-reset').addEventListener('click',()=>{$('camera').value='perspective';camera();});
  for(const [id,fn,scale] of [['angle-curve',angle,.5],['velocity-curve',speed,1.92]]) {let d='';for(let i=0;i<=160;i++){const t=i/160*6;d+=(i?'L':'M')+(32+i/160*416).toFixed(2)+' '+(52-fn(t)/scale*35).toFixed(2)+' ';}$(id).setAttribute('d',d);}
  function tick(now) {
    const dt=last?Math.min((now-last)/1000,.1):0;last=now;
    if(mode==='running') {const multiplier=$('speed').value==='offline'?16:Number($('speed').value);liveTime=Math.min(6,liveTime+dt*multiplier);if(!browsing)time=liveTime;if(liveTime>=6){mode='ready';status();}render();}
    if(replay){time=Math.min(6,time+dt);if(time>=6){replay=false;status();}render();}
    requestAnimationFrame(tick);
  }
  const params=new URLSearchParams(location.search);if(params.get('theme')==='light')document.querySelector('[data-theme="light"]').click();
  design(descriptions[params.get('design')]?params.get('design'):'studio');object('mass');status();render();requestAnimationFrame(tick);
})();
