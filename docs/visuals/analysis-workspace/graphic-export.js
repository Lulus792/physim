/* Working local design example. Exports a stable data snapshot, never an app screenshot. */
(() => {
  'use strict';
  const $=id=>document.getElementById(id),dialog=$('graphic-dialog'),trigger=$('graphic-export');
  const escape=value=>String(value).replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&apos;'}[c]));
  const fmt=(number,digits=2)=>number.toLocaleString('de-DE',{minimumFractionDigits:digits,maximumFractionDigits:digits});
  let snapshot=null,current=null,sequence=0,busy=false;
  const clipboardAvailable=()=>!!(navigator.clipboard?.write&&window.ClipboardItem);
  function build(){
    const format=document.querySelector('#graphic-form input[name=format]:checked').value;
    const scope=$('graphic-scope').value,width=Number($('graphic-size').value),height=width*17/24;
    const [start,end]=scope==='visible'?snapshot.visible:[0,snapshot.total];
    const dark=$('graphic-appearance').value==='current'&&snapshot.theme==='dark';
    const colors=dark?{paper:'#1b1c20',text:'#edeef2',muted:'#a6a9b1',grid:'#3b3d45',curves:['#8abaff','#f5b46c','#8bcca5','#d0a6ec','#f4a9b8','#7dd5df','#ddd48c','#bcbcff']}:{paper:'#ffffff',text:'#20242c',muted:'#555e6c',grid:'#dce1e9',curves:['#17569e','#914800','#00643b','#7844a0','#a52d52','#126875','#6e6410','#4c4f9f']};
    let values=snapshot.runs.flatMap(run=>run.points.filter(p=>p.t>=start&&p.t<=end).map(p=>p.value));
    if(!values.length)values=snapshot.runs.flatMap(run=>run.points.map(p=>p.value));
    let low=Math.min(...values),high=Math.max(...values),padding=(high-low)*.12||Math.max(1,Math.abs(low)*.1);low-=padding;high+=padding;
    const x=t=>116+(t-start)/(end-start)*1032,y=v=>700-(v-low)/(high-low)*470;
    const precision=step=>step>=1?(Number.isInteger(step)?0:1):Math.min(8,Math.ceil(-Math.log10(step)));
    const xd=precision((end-start)/5),yd=Math.max(2,precision((high-low)/4));
    const labels=snapshot.runs.map(r=>'Lauf '+r.id),range=`${fmt(start,3)}–${fmt(end,3)} s`,title=snapshot.channel;
    const metadata={kind:'physim-design-preview',channel:snapshot.channel,unit:snapshot.unit,scope,range:[start,end],runs:snapshot.runs.map(run=>({id:run.id,points:run.points.length})),appearance:dark?'dark':'light'};
    let svg=`<svg xmlns="http://www.w3.org/2000/svg" width="${width}" height="${height}" viewBox="0 0 1200 850" role="img" aria-labelledby="export-title export-desc"><title id="export-title">${escape(title+' · '+snapshot.title)}</title><desc id="export-desc">${escape(labels.join(', ')+'. '+range+'. Einheit '+snapshot.unit+'. Synthetische Beispielmessdaten.')}</desc><metadata>${escape(JSON.stringify(metadata))}</metadata><rect width="1200" height="850" fill="${colors.paper}"/><g font-family="Segoe UI, Arial, sans-serif" fill="${colors.text}"><text x="64" y="58" font-size="13" letter-spacing="2" fill="${colors.muted}">PHYSIM / AUSWERTUNG</text><text x="64" y="104" font-size="34" font-weight="600">${escape(title)}</text><text x="64" y="136" font-size="18" fill="${colors.muted}">${escape(snapshot.title+' · '+range)}</text>`;
    snapshot.runs.forEach((run,i)=>{
      const xx=64+(i%4)*280,yy=173+Math.floor(i/4)*30,dash=i?' stroke-dasharray="'+(i%2?'10 6':'3 6')+'"':'';
      svg+=`<line x1="${xx}" x2="${xx+30}" y1="${yy-5}" y2="${yy-5}" stroke="${colors.curves[i]}" stroke-width="3"${dash}/><text x="${xx+40}" y="${yy}" font-size="17" fill="${colors.muted}">${escape('Lauf '+run.id+(i===0?' · Referenz':''))}</text>`;
    });
    svg+=`<defs><clipPath id="export-clip"><rect x="116" y="230" width="1032" height="470"/></clipPath></defs>`;
    for(let i=0;i<=4;i++){
      const yy=230+i*470/4,value=high-i*(high-low)/4;
      svg+=`<line x1="116" x2="1148" y1="${yy}" y2="${yy}" stroke="${colors.grid}"/><text x="98" y="${yy+7}" text-anchor="end" fill="${colors.muted}" font-size="19">${escape(fmt(value,yd))}</text>`;
    }
    for(let i=0;i<=5;i++){
      const t=start+i*(end-start)/5,xx=x(t);
      svg+=`<line x1="${xx}" x2="${xx}" y1="230" y2="700" stroke="${colors.grid}"/><text x="${xx}" y="734" text-anchor="middle" fill="${colors.muted}" font-size="19">${escape(fmt(t,xd))}</text>`;
    }
    if(low<0&&high>0)svg+=`<line x1="116" x2="1148" y1="${y(0)}" y2="${y(0)}" stroke="${colors.muted}" opacity=".5"/>`;
    svg+=`<text x="116" y="216" fill="${colors.muted}" font-size="19">${escape(snapshot.symbol+' ['+snapshot.unit+']')}</text><text x="1148" y="773" text-anchor="end" fill="${colors.muted}" font-size="19">Zeit [s]</text>`;
    snapshot.runs.forEach((run,i)=>{
      const d=run.points.map((point,j)=>`${j?'L':'M'}${x(point.t).toFixed(3)},${y(point.value).toFixed(3)}`).join(' ');
      svg+=`<path class="export-curve" data-run="${escape(run.id)}" d="${d}" clip-path="url(#export-clip)" fill="none" stroke="${colors.curves[i]}" stroke-width="2.5"${i?' stroke-dasharray="'+(i%2?'10 6':'3 6')+'"':''}/>`;
    });
    svg+=`<text x="64" y="814" fill="${colors.muted}" font-size="14">${escape('Beispielmessdaten · '+(scope==='visible'?'Sichtbarer Ausschnitt':'Gesamtes Diagramm')+' · '+snapshot.runs.length+' '+(snapshot.runs.length===1?'Lauf':'Läufe'))}</text></g></svg>`;
    const channel=({Auslenkung:'auslenkung',Winkelgeschwindigkeit:'winkelgeschwindigkeit','Mechanische Energie':'energie'})[snapshot.channel]||'diagramm';
    return{svg,width,height,format,scope,range,metadata,filename:`physim-${channel}-${snapshot.runs.map(r=>r.id).join('-')}${scope==='visible'?'-ausschnitt':''}.${format}`};
  }
  function render(){
    current=build();$('graphic-preview').innerHTML=current.svg;
    $('graphic-dimensions').textContent=current.format==='png'?`${current.width} × ${current.height} px`:'SVG · frei skalierbar';
    $('graphic-summary').textContent=`${snapshot.runs.length} ${snapshot.runs.length===1?'Lauf':'Läufe'} · ${snapshot.unit} · ${current.range} · ${current.scope==='visible'?'sichtbarer Ausschnitt':'gesamtes Diagramm'}`;
    $('graphic-filename').textContent=current.filename;$('graphic-size').disabled=current.format==='svg';
    $('graphic-size-hint').textContent=current.format==='svg'?'SVG bleibt ohne feste Pixelgröße skalierbar.':'Mehr Bildpixel, dieselben Messdaten.';
    $('graphic-save').textContent=current.format.toUpperCase()+' speichern…';
    $('graphic-copy').disabled=!clipboardAvailable();
    $('graphic-copy').title=clipboardAvailable()?'Als PNG in die Zwischenablage kopieren':'In diesem Browser nicht verfügbar; PNG oder SVG speichern.';
  }
  function open(){
    if(busy)return;let next=null;document.dispatchEvent(new CustomEvent('physim-export-request',{detail:{respond:value=>next=value}}));
    if(!next)return;snapshot=next;++sequence;
    const option=$('graphic-scope').querySelector('[value=visible]');option.disabled=snapshot.visible[0]===0&&snapshot.visible[1]===snapshot.total;
    if(option.disabled)$('graphic-scope').value='full';
    $('graphic-feedback').textContent=clipboardAvailable()?'PNG oder SVG direkt aus dieser Vorschau speichern.':'Bild kopieren ist hier nicht verfügbar. PNG und SVG kannst du speichern.';
    render();dialog.showModal();$('graphic-title').tabIndex=-1;$('graphic-title').focus();
  }
  async function png(item){
    const url=URL.createObjectURL(new Blob([item.svg],{type:'image/svg+xml;charset=utf-8'}));
    try{
      const image=new Image();image.src=url;await image.decode();
      const canvas=document.createElement('canvas');canvas.width=item.width;canvas.height=item.height;
      const context=canvas.getContext('2d');if(!context)throw Error('Kein Bildrenderer verfügbar.');context.drawImage(image,0,0,item.width,item.height);
      return await new Promise((resolve,reject)=>canvas.toBlob(blob=>blob?resolve(blob):reject(Error('PNG konnte nicht erzeugt werden.')),'image/png'));
    }finally{URL.revokeObjectURL(url);}
  }
  function lock(value){busy=value;for(const input of document.querySelectorAll('#graphic-form input,#graphic-form select'))input.disabled=value;for(const id of ['graphic-copy','graphic-save'])$(id).disabled=value;if(!value&&snapshot)render();}
  async function output(copy){
    if(busy||!current||!dialog.open)return;
    const item=current,token=sequence;lock(true);$('graphic-feedback').textContent=copy?'Bild wird kopiert …':'Grafik wird vorbereitet …';
    try{
      if(copy){if(!clipboardAvailable())throw Error('Bild kopieren ist in diesem Browser nicht verfügbar.');await navigator.clipboard.write([new ClipboardItem({'image/png':png(item)})]);}
      else{
        const blob=item.format==='png'?await png(item):new Blob([item.svg],{type:'image/svg+xml;charset=utf-8'});
        if(token!==sequence||!dialog.open)return;
        const url=URL.createObjectURL(blob),a=document.createElement('a');a.href=url;a.download=item.filename;a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);
      }
      if(token===sequence&&dialog.open)$('graphic-feedback').textContent=copy?'Bild als PNG in die Zwischenablage kopiert.':`${item.filename} wurde zum Speichern an den Browser übergeben.`;
    }catch(error){if(token===sequence&&dialog.open)$('graphic-feedback').textContent=copy?'Kopieren nicht möglich. Nutze „PNG speichern…“ oder prüfe die Browserberechtigung.':`Export fehlgeschlagen: ${error.message}`;}
    finally{lock(false);}
  }
  trigger.addEventListener('click',open);$('graphic-close').onclick=()=>dialog.close();
  $('graphic-form').onsubmit=e=>e.preventDefault();$('graphic-form').onchange=()=>{if(!busy&&snapshot)render();};
  $('graphic-save').onclick=()=>output(false);$('graphic-copy').onclick=()=>output(true);
  dialog.addEventListener('close',()=>{++sequence;if(trigger.getClientRects().length)trigger.focus();});
  document.addEventListener('keydown',event=>{
    if(!dialog.open)return;
    if(event.key==='Escape'){event.stopImmediatePropagation();return;}
    if(event.key==='F5'||((event.ctrlKey||event.metaKey)&&['s','f'].includes(event.key.toLowerCase()))||(event.altKey&&['1','2','3'].includes(event.key))){
      event.preventDefault();event.stopImmediatePropagation();if((event.ctrlKey||event.metaKey)&&event.key.toLowerCase()==='s')$('graphic-save').click();
    }
  },true);
  document.addEventListener('physim-preview-command',event=>{if(event.detail.type==='visible'&&!event.detail.visible&&dialog.open)dialog.close();});
  if(new URLSearchParams(location.search).get('example')==='export'&&parent===window)open();
})();
