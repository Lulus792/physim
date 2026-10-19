/* Selected layout only: panel headers, compact tabs, workspace path guides. */
(() => {
  const icon=name=>{
    const paths={search:'<circle cx="7" cy="7" r="4.5"/><path d="m10.5 10.5 4 4"/>',save:'<path d="M3 2h9l3 3v10H2V2h1Zm2 0v5h6V2M5 15V9h7v6"/>',left:'<rect x="1.5" y="2.5" width="14" height="12" rx="2"/><path d="M6 3v11m5-8-2 2.5 2 2.5"/>',right:'<rect x="1.5" y="2.5" width="14" height="12" rx="2"/><path d="M11 3v11M6 6l2 2.5L6 11"/>',folder:'<path d="M1.5 5V3h5l2 2h7v9h-14V5Z"/>'};
    return `<svg viewBox="0 0 17 17" aria-hidden="true" fill="none" stroke="currentColor" stroke-width="1.2" stroke-linecap="round" stroke-linejoin="round">${paths[name]}</svg>`;
  };
  app.classList.add('workbench');
  const tabs=$('.file-tabs'),top=document.createElement('div');top.className='editor-topline';tabs.before(top);top.append(tabs);
  const reopenLeft=document.createElement('div');reopenLeft.className='reopen-left';top.prepend(reopenLeft);
  const tools=document.createElement('div');tools.className='editor-tools';top.append(tools);
  const reopenRight=document.createElement('div');reopenRight.className='reopen-right';tools.append(reopenRight);
  for(const [id,name,label] of [['search-open','search','Im Dokument suchen · Ctrl/Command F'],['save','save','Demodateien speichern · Ctrl/Command S']]){
    const button=$('#'+id);button.innerHTML=icon(name);button.setAttribute('aria-label',label);button.title=label;tools.append(button);
  }
  // The temporary action holder is no longer needed after moving its controls.
  const treeTitle=$('#explorer > .pane-title'),contextTitle=$('#inspector > .pane-title');
  treeTitle.innerHTML='<span>Workspace</span>';
  $$('.file-tree details > summary').forEach(summary=>{const name=summary.textContent;summary.innerHTML=`${icon('folder')}<span>${name}</span>`;});
  function placeControls(){
    for(const [key,name,text,openTarget,closedTarget]of [['tree','left','Projekt',treeTitle,reopenLeft],['api','right','Kontext',contextTitle,reopenRight]]){
      const button=$('#toggle-'+key),open=button.getAttribute('aria-expanded')==='true';
      (open?openTarget:closedTarget).append(button);button.innerHTML=icon(name);
      button.setAttribute('aria-label',`${text} ${open?'zuklappen':'aufklappen'}`);button.title=`${text} ${open?'zuklappen':'aufklappen'}`;
    }
  }
  for(const key of ['tree','api'])$('#toggle-'+key).addEventListener('click',event=>{placeControls();event.currentTarget.focus();});
  placeControls();$('#action-source').remove();
})();
