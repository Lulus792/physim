/* Final cool glass search island and resizable regions. Browser reference only. */
(() => {
  app.classList.add('resizable');
  const options=document.createElement('div');options.className='material-options';options.innerHTML='<label><input type="checkbox" id="reduce-transparency"> Transparenz reduzieren</label><button id="reset-panels">Panelgrößen zurücksetzen</button>';
  $('.preview-heading').before(options);
  app.dataset.glass='cool';
  const help=document.createElement('p');help.className='resize-help';help.id='resize-help';help.textContent='Trennlinien ziehen oder mit Tab fokussieren und Pfeiltasten nutzen. Umschalt = größere Schritte · Doppelklick oder Enter = Standardgröße.';
  options.after(help);
  const bar=$('#search-bar'),code=$('#code-area');code.append(bar);
  bar.setAttribute('role','search');bar.setAttribute('aria-label','Suche im aktiven Dokument');
  const queryLabel=$('#query').closest('label'),replaceLabel=$('#replacement').closest('label');
  queryLabel.firstChild.textContent='';replaceLabel.firstChild.textContent='';
  $('#query').setAttribute('aria-label','Suchtext');$('#query').placeholder='Im Dokument suchen';
  $('#replacement').setAttribute('aria-label','Ersetzung');$('#replacement').placeholder='Ersetzen durch';
  const icon=name=>{
    const paths={search:'<circle cx="7" cy="7" r="4.5"/><path d="m10.4 10.4 3.6 3.6"/>',expand:'<path d="m6 4 4 4-4 4"/>',collapse:'<path d="m4 6 4 4 4-4"/>',previous:'<path d="m4 10 4-4 4 4"/>',next:'<path d="m4 6 4 4 4-4"/>',close:'<path d="m4.5 4.5 7 7m0-7-7 7"/>',replace:'<path d="M3 5h9l-2-2m2 2-2 2M13 11H4l2 2m-2-2 2-2"/>'};
    return `<svg viewBox="0 0 16 16" aria-hidden="true" focusable="false" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">${paths[name]}</svg>`;
  };
  queryLabel.classList.add('search-field');const magnifier=document.createElement('span');magnifier.className='search-field-icon';magnifier.innerHTML=icon('search');queryLabel.prepend(magnifier);
  const row=document.createElement('div');row.className='floating-search-row';
  const replacement=document.createElement('div');replacement.className='floating-search-row floating-search-replace';replacement.id='floating-replacement';replacement.hidden=true;
  const disclose=document.createElement('button');disclose.id='replace-toggle';disclose.innerHTML=icon('expand');disclose.setAttribute('aria-label','Ersetzen einblenden');disclose.title='Ersetzen einblenden';disclose.setAttribute('aria-expanded','false');disclose.setAttribute('aria-controls',replacement.id);
  const previous=document.createElement('button');previous.id='find-previous';previous.innerHTML=icon('previous');previous.title='Vorheriger Treffer · Umschalt Enter';previous.setAttribute('aria-label','Vorheriger Treffer');
  $('#find-next').innerHTML=icon('next');$('#find-next').title='Nächster Treffer · Enter';$('#find-next').setAttribute('aria-label','Nächster Treffer');
  $('#search-close').innerHTML=icon('close');$('#search-close').title='Suche schließen · Escape';
  $('#replace').innerHTML=icon('replace')+'<span>Ersetzen</span>';
  const navigation=document.createElement('div');navigation.className='search-navigation';navigation.setAttribute('role','group');navigation.setAttribute('aria-label','Treffernavigation');navigation.append(previous,$('#find-next'));
  const footer=document.createElement('div');footer.className='floating-search-footer';footer.append($('#search-result'));const scope=document.createElement('small');scope.textContent='Aktuelle Datei · Escape schließt';footer.append(scope);
  row.append(disclose,queryLabel,navigation,$('#search-close'));replacement.append(replaceLabel,$('#replace'));
  bar.replaceChildren(row,replacement,footer);
  function counts() {
    const q=$('#query').value,n=q?editor.value.split(q).length-1:0;
    $('#search-result').textContent=q?`${n} Treffer`:'Suchtext eingeben';
  }
  disclose.addEventListener('click',()=>{replacement.hidden=!replacement.hidden;disclose.setAttribute('aria-expanded',String(!replacement.hidden));disclose.innerHTML=icon(replacement.hidden?'expand':'collapse');disclose.setAttribute('aria-label',replacement.hidden?'Ersetzen einblenden':'Ersetzen ausblenden');disclose.title=disclose.getAttribute('aria-label');});
  previous.addEventListener('click',()=>{
    const q=$('#query').value;if(!q){counts();return;}
    const start=editor.selectionStart-1;let at=start>=0?editor.value.lastIndexOf(q,start):-1;if(at<0)at=editor.value.lastIndexOf(q);
    if(at<0){counts();return;}
    editor.focus();editor.setSelectionRange(at,at+q.length);editor.scrollTop=Math.max(0,(editor.value.slice(0,at).split('\n').length-4)*parseFloat(getComputedStyle(editor).lineHeight));syncScroll();cursor();counts();
  });
  $('#query').addEventListener('keydown',event=>{if(event.key==='Enter'&&event.shiftKey){event.preventDefault();event.stopImmediatePropagation();previous.click();}},true);
  for(const event of ['input','change'])$('#query').addEventListener(event,counts);
  editor.addEventListener('input',counts);$('#replace').addEventListener('click',counts);$('#search-open').addEventListener('click',counts);
  $('#query').addEventListener('focus',counts);$('#insert-example').addEventListener('click',counts);
  new MutationObserver(counts).observe($('#active-file'),{childList:true,subtree:true,characterData:true});
  $('#reduce-transparency').addEventListener('change',event=>app.classList.toggle('reduced-transparency',event.target.checked));
  counts();

  const workspace=$('.workspace'),panes={tree:$('#explorer'),api:$('#inspector'),log:$('#diagnostics')};
  const defaults={tree:218,api:255,treeHeight:230,apiHeight:320,log:86},sizes={...defaults};
  const clamp=(v,min,max)=>Math.max(min,Math.min(max,v));
  const handles={},splitters={};
  function axis(key){return key==='log'||(key==='api'&&innerWidth<=950)||(key==='tree'&&innerWidth<=620)?'y':'x';}
  function sizeKey(key){return axis(key)==='y'&&key!=='log'?key+'Height':key;}
  function limits(key){
    if(axis(key)==='y')return key==='log'?[60,Math.min(300,$('.editor-pane').clientHeight-280)]:[140,540];
    const other=key==='tree'?'api':'tree';
    const otherSize=innerWidth>950&&!panes[other].hidden?panes[other].getBoundingClientRect().width:0;
    const min=key==='tree'?140:180;
    return [min,Math.max(min,Math.min(480,workspace.clientWidth-otherSize-300))];
  }
  function render(){
    let left=sizes.tree,right=sizes.api;
    const available=workspace.clientWidth;
    if(innerWidth>950){
      const l=panes.tree.hidden?0:left,r=panes.api.hidden?0:right;
      if(l+r>available-300){
        const lmin=panes.tree.hidden?0:140,rmin=panes.api.hidden?0:180;
        const excess=Math.max(0,available-300-lmin-rmin),wanted=Math.max(1,l+r-lmin-rmin);
        left=lmin+excess*Math.max(0,l-lmin)/wanted;right=rmin+excess*Math.max(0,r-rmin)/wanted;
      }
    }else left=Math.min(left,Math.max(140,available-300));
    const values={'--tree-size':left,'--api-size':right,'--tree-height':sizes.treeHeight,'--api-height':sizes.apiHeight,'--log-size':sizes.log};
    for(const [name,value]of Object.entries(values)){const text=Math.round(value)+'px';if(app.style.getPropertyValue(name)!==text)app.style.setProperty(name,text);}
  }
  function position(){
    render();const bounds=workspace.getBoundingClientRect();
    for(const key of ['tree','api']){
      const h=handles[key],pane=panes[key],r=pane.getBoundingClientRect(),vertical=axis(key)==='x';if(h.hidden!==pane.hidden)h.hidden=pane.hidden;
      if(vertical){h.style.cssText=`left:${(key==='tree'?r.right:r.left)-bounds.left-4}px;top:${r.top-bounds.top}px;width:9px;height:${r.height}px`;}
      else{h.style.cssText=`left:${r.left-bounds.left}px;top:${r.top-bounds.top-4}px;width:${r.width}px;height:9px`;}
    }
    for(const key of ['tree','api','log']){
      const h=handles[key],a=axis(key),[min,max]=limits(key);
      h.setAttribute('aria-orientation',a==='x'?'vertical':'horizontal');h.setAttribute('aria-valuemin',String(Math.round(min)));h.setAttribute('aria-valuemax',String(Math.round(max)));
      const r=panes[key].getBoundingClientRect(),value=key==='log'?$('#log-body').clientHeight:a==='x'?r.width:r.height;
      h.setAttribute('aria-valuenow',String(Math.round(value)));h.setAttribute('aria-valuetext',Math.round(value)+' Pixel');
      h.setAttribute('aria-label',`${key==='tree'?'Workspace':key==='api'?'Codekontext':'Protokoll'}${a==='x'?'breite':'höhe'} ändern`);
      splitters[key]?.sync();
    }
    syncScroll();
  }
  function setSize(key,value){const [min,max]=limits(key);sizes[sizeKey(key)]=clamp(value,min,max);position();}
  function reset(key){setSize(key,defaults[sizeKey(key)]);}
  for(const key of ['tree','api','log']){
    const h=document.createElement('div');handles[key]=h;h.id='resize-'+key;h.className='resize-handle resize-'+key;h.tabIndex=0;
    h.setAttribute('role','separator');h.setAttribute('aria-controls',key==='log'?'log-body':panes[key].id);h.setAttribute('aria-describedby','resize-help');
    (key==='log'?panes.log:workspace).append(h);
    splitters[key]=PhysimResize.attach(h,{
      axis:()=>axis(key),limits:()=>limits(key),value:()=>{const r=panes[key].getBoundingClientRect();return key==='log'?$('#log-body').clientHeight:axis(key)==='x'?r.width:r.height;},
      sign:()=>key==='api'&&axis(key)==='x'||key==='log'?-1:1,
      set:value=>setSize(key,value),reset:()=>reset(key),snapshot:()=>({...sizes}),restore:snapshot=>{Object.assign(sizes,snapshot);position();}
    });
  }
  $('#reset-panels').addEventListener('click',()=>{Object.assign(sizes,defaults);position();});
  const observer=new ResizeObserver(position);for(const pane of [workspace,...Object.values(panes),$('#log-body')])observer.observe(pane);
  window.addEventListener('resize',position);
  new MutationObserver(position).observe(workspace,{attributes:true,subtree:true,attributeFilter:['hidden']});
  position();search(true,false);
})();
