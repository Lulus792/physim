/* Inspectable design interactions only. Native contracts are in IMPLEMENTATION.md. */
(() => {
  'use strict';
  const $=id=>document.getElementById(id),app=document.querySelector('.app-window');
  const feedback=text=>$('inspector-feedback').textContent=text;
  const number=id=>Number($(id).value);
  const validNumber=(id,min,max)=>$(id).value.trim()!==''&&Number.isFinite(number(id))&&number(id)>=min&&number(id)<=max;
  const mark=(id,valid)=>$(id).setAttribute('aria-invalid',String(!valid));
  let values=Object.fromEntries(['angle','velocity','height'].map(k=>[k,parseFloat($(k+'-value').textContent.replace(',','.'))]));
  function validate(){
    const p=['parameter-length','parameter-mass'].map(id=>{const e=$(id),ok=validNumber(id,Number(e.min),Number(e.max));mark(id,ok);return ok;}).every(Boolean);
    $('parameter-validation').textContent=p?'':'Endliche Werte innerhalb des angegebenen Bereichs eingeben.';
    let dt=validNumber('run-dt',Number.MIN_VALUE,1);
    if($('adaptive-steps').checked){
      const low=validNumber('minimum-dt',Number.MIN_VALUE,1),high=validNumber('maximum-dt',Number.MIN_VALUE,1);
      const order=low&&high&&number('minimum-dt')<=number('run-dt')&&number('run-dt')<=number('maximum-dt');
      mark('minimum-dt',low&&order);mark('maximum-dt',high&&order);dt=dt&&order;
    }else{mark('minimum-dt',true);mark('maximum-dt',true);}
    mark('run-dt',dt);
    const seedText=$('run-seed').value,seed=/^\d+$/.test(seedText)&&BigInt(seedText)<=18446744073709551615n;
    mark('run-seed',seed);
    const bp=$('step-breakpoints').value.trim(),parts=bp?bp.split(',').map(v=>v.trim()):[];
    const breaks=parts.length<=32&&parts.every(v=>/^\d+$/.test(v)&&Number(v)<=1000000000)&&new Set(parts.map(Number)).size===parts.length;
    mark('step-breakpoints',breaks);
    $('run-validation').textContent=!dt?'0 < Minimum ≤ Startschritt ≤ Maximum ≤ 1 s wählen.':!seed?'Seed: ganze Zahl von 0 bis 18446744073709551615.':!breaks?'Bis zu 32 verschiedene Schrittzahlen von 0 bis 1000000000, mit Kommas getrennt.':'';
    const memory=validNumber('memory-limit',0,16384)&&Number.isInteger(number('memory-limit')),wall=validNumber('time-limit',0,86400);
    mark('memory-limit',memory);mark('time-limit',wall);$('limit-validation').textContent=memory&&wall?'':'Speicher: 0–16384 ganze MiB; Zeit: 0–86400 s.';
    return p&&dt&&seed&&breaks&&memory&&wall;
  }
  function syncLocks(){
    const mode=app.dataset.state,locked=mode!=='ready';
    for(const id of ['parameter-fields','run-fields','limit-fields'])$(id).disabled=locked;
    $('settings-note').textContent=locked?'Während eines aktiven oder pausierten Laufs gesperrt. Erst Stop drücken. Kein Runner angebunden.':'Einstellungen für den nächsten Lauf. Nur Designvorschau; keine Datei wird verändert.';
    $('save-display').disabled=mode==='empty';
  }
  for(const id of ['parameter-fields','run-fields','limit-fields'])$(id).addEventListener('input',()=>{validate();feedback('Beispielwerte geändert · noch nicht ausgeführt');});
  $('adaptive-steps').addEventListener('change',()=>{$('adaptive-fields').hidden=!$('adaptive-steps').checked;validate();});
  document.querySelectorAll('[data-default-for]').forEach(b=>b.addEventListener('click',()=>{const e=$(b.dataset.defaultFor);e.value=e.dataset.default;validate();feedback('Beispielparameter auf Standard zurückgesetzt');}));
  document.addEventListener('click',e=>{
    if(e.target.closest('#run-button')&&app.dataset.state==='ready'&&!validate()){
      e.preventDefault();e.stopImmediatePropagation();
      const input=document.querySelector('#inspector [aria-invalid=true]');
      input.closest('details').open=true;
      if(app.classList.contains('inspector-closed'))$('inspector-toggle').click();
      input.focus();feedback('Eingaben vor dem nächsten Lauf korrigieren');
    }
  },true);
  const units={angle:[['deg','°',1],['rad','rad',Math.PI/180]],velocity:[['mps','m/s',1],['cmps','cm/s',100]],height:[['m','m',1],['cm','cm',100]]};
  function measurement(){
    const k=$('inspector-channel').value,u=units[k].find(v=>v[0]===$('inspector-unit').value)||units[k][0];
    $('inspector-measurement').textContent=(values[k]*u[2]).toLocaleString('de-DE',{minimumFractionDigits:2,maximumFractionDigits:3})+' '+u[1];
    const selected=document.querySelector('.tree-row.active')?.dataset.object||'mass';
    const a=values.angle*Math.PI/180,moving=selected==='mass'||selected==='velocity';
    const length=values.length||1.5,xyz=moving?[length*Math.sin(a),-length*Math.cos(a),0]:[0,0,0];
    const coordinates=xyz.map(v=>v.toLocaleString('de-DE',{minimumFractionDigits:2,maximumFractionDigits:2})).join(' / ');
    $('world-position').textContent=coordinates;$('local-position').textContent=coordinates;
  }
  function channel(){
    $('inspector-unit').replaceChildren(...units[$('inspector-channel').value].map(([value,label])=>new Option(label,value)));measurement();
  }
  $('inspector-channel').addEventListener('change',channel);$('inspector-unit').addEventListener('change',measurement);
  document.addEventListener('physim-demo-values',e=>{values=e.detail;measurement();});channel();
  $('open-log').addEventListener('click',()=>$('inspector-log').showModal());
  const hiddenObjects=new Set();
  const shapes={mass:'#mass,#selection-ring,#object-label',rod:'#pendulum > line',support:'path[fill="var(--support-top)"],path[fill="var(--support-side)"],path[fill="var(--support-front)"]',velocity:'#velocity',trail:'#trail'};
  $('hide-object').addEventListener('click',()=>{
    const selected=document.querySelector('.tree-row.active')?.dataset.object||'mass';
    document.querySelectorAll(shapes[selected]).forEach(e=>{e.style.visibility='hidden';hiddenObjects.add(e);});
    feedback('Auswahl in der Beispielansicht ausgeblendet');
  });
  $('show-objects').addEventListener('click',()=>{hiddenObjects.forEach(e=>e.style.visibility='');hiddenObjects.clear();$('inspector-show-labels').dispatchEvent(new Event('change'));for(const id of ['show-trail','show-vectors','show-grid'])$(id).dispatchEvent(new Event('change'));feedback('Alle Beispielobjekte wieder eingeblendet');});
  document.querySelectorAll('[data-mirror]').forEach(e=>{
    const source=$(e.dataset.mirror);
    e.addEventListener('change',()=>{source.checked=e.checked;source.dispatchEvent(new Event('change'));});
    source.addEventListener('change',()=>e.checked=source.checked);
  });
  $('inspector-show-labels').addEventListener('change',()=>{$('object-label').style.visibility=$('inspector-show-labels').checked?'':'hidden';});
  $('show-points').addEventListener('change',()=>feedback('Punktfilter geändert · das Beispiel enthält keine Punktobjekte'));
  $('inspector-camera').addEventListener('change',()=>{$('camera').value=$('inspector-camera').value;$('camera').dispatchEvent(new Event('change'));});
  $('camera').addEventListener('change',()=>{$('inspector-camera').value=$('camera').value;});
  $('camera-reset').addEventListener('click',()=>{$('inspector-camera').value='perspective';$('camera-yaw').value='0';$('camera-pitch').value='0';$('camera-distance').value='6';$('camera-2d').checked=false;$('camera-orthographic').checked=false;$('camera-plane').disabled=true;$('camera-yaw').disabled=false;$('camera-pitch').disabled=false;cameraSliders();});
  function cameraSliders(){
    const yaw=number('camera-yaw'),pitch=number('camera-pitch'),distance=number('camera-distance');
    $('yaw-output').textContent=yaw.toLocaleString('de-DE',{minimumFractionDigits:2})+' rad';$('pitch-output').textContent=pitch.toLocaleString('de-DE',{minimumFractionDigits:2})+' rad';$('distance-output').textContent=distance.toLocaleString('de-DE',{minimumFractionDigits:1})+' m';
    const svg=document.querySelector('.physics-scene');svg.style.scale=String(6/distance);svg.style.rotate=String(yaw*.08)+'rad';
    svg.style.transformOrigin='50% 50%';
    $('display-feedback').textContent='Kameraregler sind in dieser Vorschau illustrativ. Nativ echte 2D/3D-Kamera verwenden.';
  }
  for(const id of ['camera-yaw','camera-pitch','camera-distance'])$(id).addEventListener('input',cameraSliders);
  $('camera-2d').addEventListener('change',()=>{
    const planar=$('camera-2d').checked;$('camera-plane').disabled=!planar;$('camera-yaw').disabled=planar;$('camera-pitch').disabled=planar;
    $('inspector-camera').value=planar?'front':'perspective';$('inspector-camera').dispatchEvent(new Event('change'));
  });
  $('camera-plane').addEventListener('change',()=>{$('inspector-camera').value=['front','side','top'][$('camera-plane').selectedIndex];$('inspector-camera').dispatchEvent(new Event('change'));});
  $('camera-orthographic').addEventListener('change',()=>feedback('Projektion für native Umsetzung gewählt · Browseransicht ist illustrativ'));
  $('save-display').addEventListener('click',()=>{$('display-feedback').textContent='Beispielvorgabe übernommen · nur in dieser Vorschau, keine Projektdatei geschrieben.';});
  new MutationObserver(syncLocks).observe(app,{attributes:true,attributeFilter:['data-state']});
  validate();syncLocks();
})();
