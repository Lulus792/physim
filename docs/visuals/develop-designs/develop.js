'use strict';
const $ = s => document.querySelector(s);
const $$ = s => [...document.querySelectorAll(s)];
const app = $('.app-window'), editor = $('#editor');
// Intentionally abbreviated excerpts. They are editable design content, not complete experiments.
const files = {
  'main.c': `#include "physim/experiment.h"
#include <math.h>

/* Ideales Pendel · Modell ohne Reibung */
static const double length_m = 1.5;
static const double gravity_m_s2 = 9.80665;
static const double initial_angle_rad = 0.45;

static void derivative(double t, const double *y,
                       double *dy, void *user) {
    (void)t;
    dy[0] = y[1];
    dy[1] = -gravity_m_s2 / length_m * sin(y[0]);
}

static ps_result create(ps_context *ctx) {
    ps_channel_add(ctx, "angle", PS_RADIAN,
                   "Winkel des Pendels");
    return PS_OK;
}

/* Designausschnitt · weitere Callbacks ausgeblendet */`,
  'analysis.c': `#include "physim/analysis.h"

/* Analysequelle · gekürzte Designansicht */
/* Hier wird der aufgezeichnete Lauf ausgewertet. */

/* Messgröße: angle [rad]
   Ergebnis: Winkelverlauf und Periodendauer
   Die vollständige Analyse bleibt im Projekt. */`,
  'physim.project': `# Physim-Projekt · illustrative Zusammenfassung
name = Pendelstudie
experiment = main.c
analysis = analysis.c
profile = Debug

# Buildprodukte bleiben unter build/.
# Kein Parser für das native Projektformat angebunden.`
};
const saved = {...files};
let active = 'main.c', scenario = 'ready', timer, buildRevision = 0;
let revision = 0, builtRevision = 0;
let folderEditing = false;
const esc = s => s.replaceAll('&','&amp;').replaceAll('<','&lt;').replaceAll('>','&gt;');
function highlight(s) {
  // Tokenize before escaping to prevent user-entered HTML becoming markup.
  return s.replace(/\/\*[\s\S]*?\*\/|\/\/[^\n]*|"(?:\\.|[^"\\])*"|\b(?:static|const|double|void|return|include|typedef)\b|\b\d+(?:\.\d+)?\b|\b(?:ps_\w+|PS_\w+|sin|derivative|create)\b|[^]/g, token => {
    const cls = token.startsWith('/*') || token.startsWith('//') ? 'comment' : token.startsWith('"') ? 'str' : /^(static|const|double|void|return|include|typedef)$/.test(token) ? 'kw' : /^\d/.test(token) ? 'num' : /^(ps_|PS_|sin$|derivative$|create$)/.test(token) ? 'fn' : '';
    return cls ? `<span class="${cls}">${esc(token)}</span>` : esc(token);
  });
}
function dirty() { return Object.keys(files).some(name => files[name] !== saved[name]); }
function syncScroll() {
  $('#highlight').scrollTop = editor.scrollTop; $('#highlight').scrollLeft = editor.scrollLeft;
  $('#line-numbers').scrollTop = editor.scrollTop;
}
function renderCode() {
  $('#highlight').innerHTML = highlight(editor.value) + '\n';
  $('#line-numbers').textContent = Array.from({length:editor.value.split('\n').length}, (_,i) => i+1).join('\n');
  syncScroll(); cursor();
}
function cursor() {
  const before = editor.value.slice(0,editor.selectionStart).split('\n');
  $('#cursor').textContent = `Zeile ${before.length}, Spalte ${before.at(-1).length+1}`;
}
function dirtyUI() {
  $$('.dirty-dot').forEach(el => {const name=el.closest('[data-file]').dataset.file;el.hidden=files[name]===saved[name];});
  $$('[data-file]').forEach(el => {
    const name = el.dataset.file;
    el.setAttribute('aria-label', `${name}${files[name] !== saved[name] ? ', ungespeichert' : ''}`);
  });
  $('#save-status').textContent = dirty() ? '● Ungespeicherte Änderungen · Demo' : 'Alle Änderungen gespeichert · Demo';
  $('#window-title').textContent = `Pendelstudie${dirty() ? ' ●' : ''} — Physim`;
}
function openFile(name, focus=false) {
  active=name; editor.value=files[name]; editor.setSelectionRange(0,0);editor.scrollTop=0; editor.scrollLeft=0;
  editor.setAttribute('aria-label',`Quellcode der Demodatei ${name}`);
  $('#active-file').textContent=name;
  $$('[data-file]').forEach(el=>{const selected=el.dataset.file===name;el.classList.toggle('selected',selected);el.setAttribute('aria-pressed',String(selected));});
  renderCode(); dirtyUI(); if(focus)editor.focus();
}
function setPanel(key, visible) {
  const mapping={tree:['explorer','toggle-tree','tree-closed'],api:['inspector','toggle-api','api-closed'],log:['log-body','toggle-log',null]};
  const [id,toggle,cls]=mapping[key]; $('#'+id).hidden=!visible;
  $('#'+toggle).setAttribute('aria-expanded',String(visible));
  if(cls)app.classList.toggle(cls,!visible);
  if(key==='log')$('#toggle-log').textContent=`${visible?'⌄':'›'} Protokoll`;
}
function menu(open, restore=false) {
  $('#build-popup').hidden=!open;$('#menu-build').setAttribute('aria-expanded',String(open));
  if(open)$('#build').focus();else if(restore)$('#menu-build').focus();
}
function updateBuildUI() {
  const noProject=['empty','folder'].includes(scenario), running=scenario==='building', failed=scenario==='error';
  $('#scenario').value=scenario;
  $('.workspace').hidden=noProject&&!folderEditing;$('#empty-state').hidden=!noProject||folderEditing;
  $('#empty-title').textContent=scenario==='folder'?'Dein Ordner ist geöffnet.':'Raum für eine neue Idee.';
  $('#empty-detail').textContent=scenario==='folder'?'Kein physim.project gefunden. Dateien lassen sich unabhängig bearbeiten.':'Öffne einen Ordner oder lege ein Physim-Projekt an.';
  $('#open-folder').textContent=scenario==='folder'?'Datei im Editor öffnen':'Ordner öffnen';
  $('#new-project').textContent=scenario==='folder'?'Projektmanager ansehen':'Neues Projekt';
  const text={ready:'✓ Build erfolgreich',dirty:'● Änderungen · Build veraltet',unbuilt:'○ Noch nicht gebaut',building:'◌ Build läuft …',error:'! Build fehlgeschlagen',folder:'○ Kein Physim-Projekt',empty:'○ Kein Projekt'};
  $('#build-status').textContent=text[scenario];$('#build-status').classList.toggle('error',failed);
  $('#build').disabled=noProject||running;$('#cancel').disabled=!running;
  $('#profile').disabled=noProject||running;$('#save').disabled=(noProject&&!folderEditing)||running;editor.readOnly=running;
  $('#insert-example').disabled=noProject||running;$('#search-open').disabled=noProject&&!folderEditing;
  $('#result-icon').textContent=failed?'!':running?'◌':scenario==='ready'?'✓':'○';
  $('#result-icon').style.color=failed?'var(--error)':'var(--good)';
  $('#result-title').textContent=failed?'Eine Stelle braucht Aufmerksamkeit.':running?'Experiment wird gebaut …':scenario==='ready'?'Experiment bereit.':scenario==='dirty'?'Der Code wurde geändert.':'Bereit für den ersten Build.';
  $('#result-detail').textContent=failed?'Der Demo-Compiler meldet einen unbekannten Bezeichner.':running?'Kompilieren und Modul prüfen · Demoablauf':scenario==='ready'?'Der letzte Demo-Build war erfolgreich. Weiter im Bereich Simulieren.':'Projekt über Build → Projekt bauen oder F5 bauen.';
  $('#result-time').textContent=scenario==='ready'?'0,84 s':'';
  $('#problem-count').textContent=failed?'1 Fehler · 0 Warnungen':running?'Wird geprüft …':'0 Fehler · 0 Warnungen';
  $('#diagnostic-link').hidden=!failed;$('#raw-log').hidden=!running;
  $('#raw-log').textContent=`[Demo] ${$('#profile').value} · main.c\n[1/2] Quelltext kompilieren …\n[2/2] Experimentmodul prüfen …`;
  $('#log-profile').textContent=$('#profile').value;
  if(failed||running)setPanel('log',true);
}
function setScenario(value) {
  clearTimeout(timer);++buildRevision; scenario=value;folderEditing=false;
  if(value==='dirty') { files['main.c']=saved['main.c']+'\n// Ein neuer Gedanke';++revision;openFile('main.c'); }
  if(value==='error') { files['main.c']=saved['main.c'].replace('-gravity_m_s2','-gravity_m_s');++revision;openFile('main.c'); }
  if(value==='ready')builtRevision=revision;
  dirtyUI();updateBuildUI();
}
function build() {
  if(['empty','folder','building'].includes(scenario))return;
  scenario='building';const run=++buildRevision;menu(false);updateBuildUI();
  // A demonstrative outcome based on the known sample error; no C compiler is attached.
  timer=setTimeout(()=>{
    if(run!==buildRevision)return;
    scenario=/\bgravity_m_s\b/.test(files['main.c'])?'error':'ready';
    if(scenario==='ready')builtRevision=revision;
    updateBuildUI();
  },1000);
}
function edited() {
  files[active]=editor.value;++revision;renderCode();dirtyUI();
  if(!['empty','folder'].includes(scenario)){scenario='dirty';updateBuildUI();}
}
function save() { if($('#save').disabled)return;for(const name of Object.keys(files))saved[name]=files[name];dirtyUI(); }
function search(open, focus=true) {
  $('#search-bar').hidden=!open;$('#search-open').setAttribute('aria-expanded',String(open));
  if(focus)(open?$('#query'):$('#search-open')).focus();
}
function findNext() {
  const query=$('#query').value;
  if(!query){$('#search-result').textContent='Suchtext eingeben.';return false;}
  let at=editor.value.indexOf(query,editor.selectionEnd);
  if(at<0)at=editor.value.indexOf(query);
  if(at<0){$('#search-result').textContent='Keine Treffer';return false;}
  editor.focus();editor.setSelectionRange(at,at+query.length);
  const height=parseFloat(getComputedStyle(editor).lineHeight);
  editor.scrollTop=Math.max(0,(editor.value.slice(0,at).split('\n').length-4)*height);syncScroll();cursor();
  $('#search-result').textContent=`${editor.value.split(query).length-1} Treffer`;return true;
}
$$('button[data-theme]').forEach(el=>el.addEventListener('click',()=>{document.documentElement.dataset.theme=el.dataset.theme;$$('button[data-theme]').forEach(b=>b.setAttribute('aria-pressed',String(b===el)));}));
$$('[data-file]').forEach(el=>el.addEventListener('click',()=>openFile(el.dataset.file)));
for(const key of ['tree','api','log'])$('#toggle-'+key).addEventListener('click',()=>setPanel(key,$('#toggle-'+key).getAttribute('aria-expanded')!=='true'));
$('#menu-build').addEventListener('click',()=>menu($('#build-popup').hidden));
document.addEventListener('click',e=>{if(!e.target.closest('.build-menu'))menu(false);});
$('#build').addEventListener('click',build);
$('#cancel').addEventListener('click',()=>{clearTimeout(timer);++buildRevision;scenario=builtRevision===revision?'ready':'unbuilt';menu(false,true);updateBuildUI();$('#result-title').textContent='Build abgebrochen.';});
$('#scenario').addEventListener('change',e=>setScenario(e.target.value));
$('#profile').addEventListener('change',()=>{++revision;scenario='unbuilt';updateBuildUI();});
$('#save').addEventListener('click',save);
editor.addEventListener('input',edited);editor.addEventListener('scroll',syncScroll);
for(const event of ['click','keyup','select'])editor.addEventListener(event,cursor);
$('#search-open').addEventListener('click',()=>search($('#search-bar').hidden));
$('#search-close').addEventListener('click',()=>search(false));
$('#find-next').addEventListener('click',findNext);
$('#query').addEventListener('keydown',e=>{if(e.key==='Enter'){e.preventDefault();findNext();}});
$('#replace').addEventListener('click',()=>{
  if(editor.readOnly)return;
  const query=$('#query').value;if(!query)return;
  if(editor.value.slice(editor.selectionStart,editor.selectionEnd)!==query&&!findNext())return;
  editor.setRangeText($('#replacement').value,editor.selectionStart,editor.selectionEnd,'select');edited();editor.focus();
});
$('#diagnostic-link').addEventListener('click',()=>{
  openFile('main.c',true);const at=editor.value.indexOf('gravity_m_s /');
  editor.setSelectionRange(Math.max(0,at),Math.max(0,at)+11);editor.scrollTop=150;syncScroll();cursor();
});
$$('[data-symbol]').forEach(el=>el.addEventListener('click',()=>{
  openFile('main.c',true);const at=editor.value.indexOf(el.dataset.symbol);editor.setSelectionRange(Math.max(0,at),Math.max(0,at)+el.dataset.symbol.length);cursor();
}));
$('#insert-example').addEventListener('click',()=>{
  openFile('main.c');editor.focus();const at=editor.value.indexOf('    return PS_OK;');
  const end=at<0?editor.value.length:at;editor.setRangeText('    ps_channel_add(ctx, "angle.demo", PS_RADIAN, "Demokanal");\n',end,end,'end');edited();
});
$('#open-folder').addEventListener('click',()=>{
  if(scenario==='folder'){folderEditing=true;updateBuildUI();openFile('main.c',true);return;}
  setScenario('folder');
});
$('#new-project').addEventListener('click',()=>{setScenario('unbuilt');$('#result-detail').textContent='Vorhandenes Demo-Projekt geöffnet. Die native App verwendet hier den separaten Projektmanager.';});
document.addEventListener('keydown',e=>{
  if(e.key==='Escape'){if(!$('#build-popup').hidden){menu(false,true);e.preventDefault();}else if(!$('#search-bar').hidden){search(false);e.preventDefault();}}
  if(e.key==='F5'){e.preventDefault();build();}
  if((e.ctrlKey||e.metaKey)&&e.key.toLowerCase()==='s'){e.preventDefault();save();}
  if((e.ctrlKey||e.metaKey)&&e.key.toLowerCase()==='f'&&!$('#search-open').disabled){e.preventDefault();search(true);}
});
openFile(active);updateBuildUI();
