/* Detail studies on the selected workbench. The original demo actions stay shared. */
(() => {
  const svg = name => {
    const paths = {
      search:'<circle cx="7" cy="7" r="4.5"/><path d="m10.5 10.5 4 4"/>',
      save:'<path d="M3 2h9l3 3v10H2V2h1Zm2 0v5h6V2M5 15V9h7v6"/>',
      left:'<rect x="1.5" y="2.5" width="14" height="12" rx="2"/><path d="M6 3v11m5-8-2 2.5 2 2.5"/>',
      right:'<rect x="1.5" y="2.5" width="14" height="12" rx="2"/><path d="M11 3v11M6 6l2 2.5L6 11"/>',
      folder:'<path d="M1.5 5V3h5l2 2h7v9h-14V5Z"/>',
    };
    return `<svg viewBox="0 0 17 17" aria-hidden="true" fill="none" stroke="currentColor" stroke-width="1.2" stroke-linecap="round" stroke-linejoin="round">${paths[name]}</svg>`;
  };
  const presets = {
    edges:{title:'A / Randgriffe',tabs:'line',tree:'quiet',description:'Feste Griffe links und rechts. Projekt und Kontext bleiben auch geschlossen mit einem Klick erreichbar.',note:'Die zusätzliche Werkzeugleiste entfällt. Schmale Randgriffe schalten die Seitenbereiche; Suche und Speichern sitzen direkt an den Dateitabs. Eine Linie markiert das aktive Dokument.'},
    headers:{title:'B / Panelköpfe',tabs:'cards',tree:'guides',description:'Schalter dort, wo sie wirken. Ohne dauerhafte Seitenleisten am Rand.',note:'Geöffnete Bereiche tragen ihren Schalter im Kopf. Nach dem Zuklappen erscheint er direkt neben den Dateitabs wieder. Kartentabs und feine Pfadlinien geben der Werkbank Struktur.'},
    dock:{title:'C / Statusdock',tabs:'pills',tree:'surfaces',description:'Beschriftete Schalter unten. Oben gehört der Platz den Dateien.',note:'Projekt und Kontext werden über die vorhandene Statuszeile geschaltet. Kompakte Tabflächen und sanfte Zeilenflächen im Workspace schaffen Orientierung ohne eigene Werkzeugzeile.'},
  };
  const states = {};
  const selected={controls:'headers',tabs:'pills',tree:'guides'};
  let current='edges';
  app.classList.add('detail-study');
  // The project is identified in the window title and tree. Build status/profile use the existing footer.
  const profile=$('#profile').closest('label'), buildStatus=$('#build-status');
  const footer=$('.app-status');
  const footerTools=document.createElement('div');footerTools.className='footer-tools';
  footerTools.append(profile,buildStatus);footer.prepend(footerTools);
  const dock=document.createElement('div');dock.className='panel-dock';footer.prepend(dock);
  const fileTabs=$('.file-tabs'), editorPane=$('.editor-pane');
  const top=document.createElement('div');top.className='editor-topline';editorPane.insertBefore(top,fileTabs);top.append(fileTabs);
  const reopenLeft=document.createElement('div');reopenLeft.className='reopen-left';top.prepend(reopenLeft);
  const tools=document.createElement('div');tools.className='editor-tools';top.append(tools);
  for(const [id,icon,label] of [['search-open','search','Im Dokument suchen · Ctrl/Command F'],['save','save','Demodateien speichern · Ctrl/Command S']]) {
    const button=$('#'+id);button.innerHTML=svg(icon);button.setAttribute('aria-label',label);button.title=label;tools.append(button);
  }
  const reopenRight=document.createElement('div');reopenRight.className='reopen-right';tools.prepend(reopenRight);
  const workspace=$('.workspace');
  const shell=document.createElement('div');shell.className='workspace-shell';workspace.before(shell);
  const leftRail=document.createElement('div');leftRail.className='panel-rail left-rail';
  const rightRail=document.createElement('div');rightRail.className='panel-rail right-rail';
  shell.append(leftRail,workspace,rightRail);
  const treeTitle=$('#explorer > .pane-title');treeTitle.innerHTML='<span>Workspace</span>';
  const contextTitle=$('#inspector > .pane-title');
  $('#explorer > details > summary').title='Pendelstudie · geöffneter Workspace';
  $$('.file-tree details > summary').forEach(summary=>{
    const name=summary.textContent;summary.innerHTML=`${svg('folder')}<span>${name}</span>`;
    summary.title=name==='build'?'Generierte Builddateien':name==='runs'?'Gespeicherte Messläufe':'Projektassets';
  });
  function labelButtons() {
    for(const [key,icon,text] of [['tree','left','Projekt'],['api','right','Kontext']]) {
      const button=$('#toggle-'+key),open=button.getAttribute('aria-expanded')==='true';
      button.innerHTML=`${svg(icon)}<span>${text}</span>`;
      button.setAttribute('aria-label',`${text} ${open?'zuklappen':'aufklappen'}`);
      button.title=`${text} ${open?'zuklappen':'aufklappen'}`;
    }
  }
  function placeControls() {
    const mode=$('#detail-controls').value;app.dataset.controls=mode;
    const tree=$('#toggle-tree'),api=$('#toggle-api');
    if(mode==='edges'){leftRail.append(tree);rightRail.append(api);}
    else if(mode==='dock')dock.append(tree,api);
    else {
      (tree.getAttribute('aria-expanded')==='true'?treeTitle:reopenLeft).append(tree);
      (api.getAttribute('aria-expanded')==='true'?contextTitle:reopenRight).append(api);
    }
    labelButtons();
    $('#design-subtitle').textContent=Object.entries(selected).every(([key,value])=>$('#detail-'+key).value===value)?'Panelköpfe · kompakte Tabs · Pfadlinien':presets[current].description;
  }
  function measure() {
    $('#space-saved').textContent='2 Leisten weniger über dem Code';
    $('#space-saved').title='Projektname im Baum, Profil und Buildstatus unten. Suche und Speichern sitzen an den Dateitabs.';
  }
  function mix() {
    app.dataset.tabs=$('#detail-tabs').value;app.dataset.tree=$('#detail-tree').value;
    placeControls();measure();
    $$('[data-detail]').forEach(button=>{
      const p=presets[button.dataset.detail];
      const match=button.dataset.detail===$('#detail-controls').value&&p.tabs===$('#detail-tabs').value&&p.tree===$('#detail-tree').value;
      button.setAttribute('aria-pressed',String(match));
    });
    const exact=presets[$('#detail-controls').value];
    const mixed=exact.tabs!==$('#detail-tabs').value||exact.tree!==$('#detail-tree').value;
    const isSelected=Object.entries(selected).every(([key,value])=>$('#detail-'+key).value===value);
    $('#selected-workbench').setAttribute('aria-pressed',String(isSelected));
    $('#design-title').textContent=isSelected?'Werkbank / Ausgewählt':mixed?'Werkbank / Eigene Kombination':presets[current].title;
    $('#note-kicker').textContent=isSelected?'WERKBANK / AUSGEWÄHLT':mixed?'WERKBANK / DETAILS KOMBINIERT':presets[current].title.toUpperCase();
    $('#design-note').textContent=isSelected?'Ausgewählt: Schalter im Panelkopf, kompakte Tabs aus dem Statusdock-Entwurf und Pfadlinien im Workspace. Die zusätzliche Werkzeugleiste entfällt.':mixed?'Schalter, Tabs und Workspace sind unabhängig kombiniert. Die Dateireihenfolge und Einrückung der Unterordner bleiben erhalten.':presets[current].note;
    if(isSelected)$('#design-subtitle').textContent='Panelköpfe · kompakte Tabs · Pfadlinien';
  }
  function choose(name) {
    states[current]={tree:$('#toggle-tree').getAttribute('aria-expanded')==='true',api:$('#toggle-api').getAttribute('aria-expanded')==='true'};
    current=name;const p=presets[name];
    $('#detail-controls').value=name;$('#detail-tabs').value=p.tabs;$('#detail-tree').value=p.tree;
    const state=states[name]||{tree:true,api:true};
    setPanel('tree',state.tree);setPanel('api',state.api);mix();
  }
  $$('[data-detail]').forEach(button=>button.addEventListener('click',()=>choose(button.dataset.detail)));
  function restoreSelection() {
    choose(selected.controls);
    $('#detail-tabs').value=selected.tabs;$('#detail-tree').value=selected.tree;mix();
  }
  $('#selected-workbench').addEventListener('click',restoreSelection);
  for(const id of ['detail-controls','detail-tabs','detail-tree'])$('#'+id).addEventListener('change',()=>{
    current=$('#detail-controls').value;mix();
  });
  for(const key of ['tree','api'])$('#toggle-'+key).addEventListener('click',e=>{
    // The shared listener has already updated the panel. Move its trigger before restoring focus.
    placeControls();e.currentTarget.focus();
  });
  const note=$('.design-note > p');note.innerHTML='Werkbank bleibt die Grundlage. Die Unterordner behalten ihre Position.<br><a href="original.html">Frühere Gesamtentwürfe</a> · <a href="DETAILS.md">Details und Prüfbericht</a> · lokale Demo ohne Dateizugriff.';
  window.addEventListener('resize',measure);
  restoreSelection();
})();
