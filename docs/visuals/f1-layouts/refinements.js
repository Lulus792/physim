const $ = (selector) => document.querySelector(selector);
const $$ = (selector) => [...document.querySelectorAll(selector)];
const html = (text) => text.replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
let mode = 'balanced', language = 'c', selected = 'pendulum', contextTab = 'model';
const closed = {left:false, right:false};
const panelScroll = {left:0, right:0};
let readingBookmark = null;
const concepts = {
  balanced: ['H','Schalter an den äußeren Enden','Links die Navigation von E/G, rechts der Modellbereich von C. Zwei kleine Paneel-Icons sitzen jeweils am passenden Ende der Kopfleiste.','Beide Schalter bleiben auch bei eingeklappten Seiten sichtbar. Die mittlere Lesefläche wächst mit jeder geschlossenen Seite.'],
  rails: ['I','Schmale Streifen als Anker','Die Paneele tragen ihren Schalter direkt im eigenen Kopf. Eingeklappt bleibt je ein schmaler Streifen mit dem Icon zum Wiederöffnen.','Links und rechts lassen sich getrennt auf einen 40-px-Streifen reduzieren. Die Position des jeweiligen Schalters bleibt an der zugehörigen Seite.'],
  controls: ['J','Beide Layout-Icons an einem Ort','Themen und Abschnittssprünge bleiben links. Modell und Beispiel stehen rechts. Die beiden Paneel-Icons sind kompakt oben rechts zusammengefasst.','Zwei nebeneinanderliegende Icons steuern unabhängig die linke und rechte Fläche. Beide Seiten können ganz verschwinden.']
};
const track = () => language === 'c' ? 'C' : 'Physim';
const part = () => language === 'c' ? 'Teil I · C' : 'Teil II · Physim';
const source = () => language === 'c' ? 'docs/pendulum-tutorial.md · C-Ausschnitt' : 'examples/language/pendulum_integrator.phys';
function announce(text) { $('#state-message').textContent = text; }

function panelIcon(side) {
  const x = side === 'left' ? 3 : 15;
  const divider = side === 'left' ? 9 : 15;
  return `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" aria-hidden="true"><rect class="panel-fill" x="${x}" y="4" width="6" height="16" fill="currentColor" stroke="none"/><rect x="3" y="4" width="18" height="16" rx="2"/><path d="M${divider} 4v16"/></svg>`;
}

function workflowMarkup() {
  return language === 'c'
    ? '<section id="workflow"><h2>Den Versuch in C vorbereiten</h2><p>Definiere den Modellzustand und seine Ableitung in C. Die C-Funktion schreibt Winkeltempo und Winkelbeschleunigung in den Ausgabepuffer.</p><ol><li>Lege das Experiment im C-Lernweg an.</li><li>Implementiere die Zustandsableitung und baue das C-Projekt.</li><li>Speichere je einen Euler- und RK4-Lauf mit denselben Anfangswerten.</li></ol><p>Die Erläuterung von Funktionszeigern, Puffern und Zustandsdaten gehört zum C-Lernweg.</p></section>'
    : '<section id="workflow"><h2>Den Versuch in Physim vorbereiten</h2><p>Beschreibe die Ableitung als Physim-Funktion mit typisierten Parametern und einem zurückgegebenen Array. Bearbeite die <code>.phys</code>-Quelle; erzeugter C-Code ist ein Buildartefakt.</p><ol><li>Lege das Experiment im Physim-Lernweg an.</li><li>Definiere Parameter und die Funktion <code>pendulumDerivative</code>.</li><li>Speichere je einen Euler- und RK4-Lauf mit denselben Anfangswerten.</li></ol><p>Syntax, Typinferenz und Werte werden im Physim-Lernweg erklärt.</p></section>';
}

function documentData() {
  if (selected === 'guide') return {
    title: language === 'c' ? 'C: Einstieg in Experimente' : 'Physim: Einstieg in Experimente',
    lead: language === 'c' ? 'Modellzustand, C-Funktionen und Puffer bilden den Einstieg in die C-Schnittstelle.' : 'Syntax, typisierte Werte und Funktionen bilden den Einstieg in die Physim-Sprache.',
    body: workflowMarkup() + `<section id="source"><h2>${language === 'c' ? 'Zustandsableitung als C-Funktion' : 'Zustandsableitung als Physim-Funktion'}</h2><pre>${html(codeSamples[language])}</pre></section>`
  };
  const doc = docs.find(d => d.id === selected) || docs[0];
  if (selected === 'pendulum') return {
    title:doc.title,
    lead: language === 'c' ? 'Berechne das Pendel im C-Lernweg. Vergleiche die numerischen Verfahren über dieselben Messwerte und Energiefehler.' : 'Berechne das Pendel im Physim-Lernweg. Vergleiche die numerischen Verfahren über dieselben Messwerte und Energiefehler.',
    body:modelMarkup + workflowMarkup() + methodsMarkup + compareMarkup
  };
  if (selected === 'numerics') return {...doc, lead:`Numerische Verfahren im ${track()}-Lernweg auswählen und ihre Genauigkeit prüfen.`, body:methodsMarkup + workflowMarkup() + compareMarkup};
  return doc;
}

function sections() {
  return [...$('#article-content').querySelectorAll('h2')].map((heading,i) => ({
    id:heading.closest('section')?.id || (heading.id ||= `heading-${i}`), title:heading.textContent
  }));
}
function sectionMarkup() {
  return `<div class="section-links"><p class="nav-label">In diesem Artikel</p>${sections().map(section => `<a class="section-link" href="#${section.id}" data-jump="${section.id}">${html(section.title)}</a>`).join('')}</div>`;
}
function docButton(id, title) {
  return `<button class="nav-row" data-document="${id}"${id === selected ? ' aria-current="page"' : ''}>${html(title)}</button>${id === selected ? sectionMarkup() : ''}`;
}
function navItems() {
  return [{id:'guide',title:language === 'c' ? 'C: Einstieg in Experimente' : 'Physim: Einstieg in Experimente',kind:part()},
    {id:'pendulum',title:'Pendel & Integratoren',kind:'Mechanik'}, {id:'spring',title:'Feder–Masse–Dämpfer',kind:'Mechanik'},
    {id:'projectile',title:'Wurfparabel',kind:'Mechanik'}, {id:'numerics',title:'Numerische Verfahren',kind:'Nachschlagen'}, {id:'trouble',title:'Fehler finden',kind:'Nachschlagen'}];
}
function renderNavigation() {
  const query = $('#search').value.trim().toLocaleLowerCase('de');
  if (query) {
    const matches = navItems().filter(item => `${item.title} ${item.kind} ${item.id === 'pendulum' ? 'Euler RK4 Energie' : ''}`.toLocaleLowerCase('de').includes(query));
    $('#nav-content').innerHTML = `<p class="nav-label">Suche · ${track()}</p><p class="search-count">${matches.length} Treffer</p>${matches.length ? matches.map(item => docButton(item.id,item.title)).join('') : '<p class="empty">Keine Treffer. Probiere „Pendel“, „Euler“ oder den gewählten Sprachnamen.</p>'}`;
  } else {
    $('#nav-content').innerHTML = `<div class="nav-group"><p class="nav-label">${part()}</p>${docButton('guide',language === 'c' ? 'C: Einstieg in Experimente' : 'Physim: Einstieg in Experimente')}</div><div class="nav-group"><p class="nav-label">Mechanik</p>${docButton('pendulum','Pendel & Integratoren')}${docButton('spring','Feder–Masse–Dämpfer')}${docButton('projectile','Wurfparabel')}</div><div class="nav-group"><p class="nav-label">Nachschlagen</p>${docButton('numerics','Numerische Verfahren')}${docButton('trouble','Fehler finden')}</div>`;
  }
  updateActiveSection();
}

function renderContext() {
  const relevant = ['pendulum','guide','numerics','trouble'].includes(selected);
  $$('[data-tab]').forEach(button => {
    button.setAttribute('aria-selected',button.dataset.tab === contextTab);
    button.tabIndex = button.dataset.tab === contextTab ? 0 : -1;
  });
  $('#context-content').setAttribute('aria-labelledby',contextTab === 'model' ? 'tab-model' : 'tab-example');
  if (!relevant) {
    $('#context-content').innerHTML = '<h2>Kein Modellkontext hinterlegt</h2><p>Dieser verkürzte Layoutartikel zeigt nur den Navigationszustand. Wechsle zum Pendel, um Modell und Beispiel zu vergleichen.</p>';
  } else if (contextTab === 'model') {
    $('#context-content').innerHTML = `<h2>Das Versuchsmodell</h2><img class="diagram" src="model.svg" alt="Ideales Pendel mit Masse, Stangenlänge und Winkel zur Senkrechten"><table><caption>Ausgangswerte · Vakuum</caption><tbody><tr><th>Masse</th><td>1 kg</td></tr><tr><th>Länge</th><td>1,5 m</td></tr><tr><th>Startwinkel</th><td>0,45 rad</td></tr><tr><th>Starttempo</th><td>0 rad/s</td></tr></tbody></table><div class="notice"><strong>Worauf achten?</strong>Ohne Widerstand bleibt die mechanische Energie im idealen Modell konstant.</div><p class="source">${part()} · Pendel-Lernpfad</p>`;
  } else {
    $('#context-content').innerHTML = `<h2>Die Zustandsableitung</h2><div class="code-head"><span>${track()}-Lernweg</span><button data-copy="example-code">Kopieren</button></div><pre id="example-code">${html(codeSamples[language])}</pre><p>Ausschnitt des gewählten Sprachlernwegs. Kein vollständiges Programm.</p><p class="source">${source()}</p><div class="notice"><strong>Einmal oben wählen</strong>Die Dokumentationssprache gilt auch für diesen Ausschnitt.</div>`;
  }
}

function renderArticle() {
  readingBookmark = null;
  const doc = documentData();
  $('#article-title').textContent = doc.title;
  $('#article-lead').textContent = doc.lead;
  $('#article-content').innerHTML = doc.body;
  $('#article-path').textContent = `${part()} / ${selected === 'guide' ? 'Einstieg' : selected === 'pendulum' ? 'Mechanik / Pendel-Lernpfad' : doc.title}`;
  $('#footer-language').textContent = `${part()} · Offline-Handbuch`;
  $('#search').placeholder = `${track()}-Handbuch durchsuchen …`;
  $$('[data-language]').forEach(button => button.setAttribute('aria-pressed',button.dataset.language === language));
  $('#article-scroll').scrollTop = 0;
  renderNavigation();
  renderContext();
}
function captureBookmark() {
  const top = $('#article-scroll').getBoundingClientRect().top;
  const elements = [...$('.article').querySelectorAll('h1,h2,h3,p,pre,.equation')];
  const element = elements.reduce((a,b) => Math.abs(a.getBoundingClientRect().top-top) < Math.abs(b.getBoundingClientRect().top-top) ? a : b);
  return {element, offset:element.getBoundingClientRect().top-top};
}
function restoreBookmark() {
  if (!readingBookmark?.element.isConnected) return;
  const scroller = $('#article-scroll');
  scroller.scrollTop += readingBookmark.element.getBoundingClientRect().top - scroller.getBoundingClientRect().top - readingBookmark.offset;
}
function applyPanels() {
  for (const side of ['left','right']) {
    $('#window').classList.toggle(`${side}-closed`,closed[side]);
    const content = side === 'left' ? $('#nav-content') : $('#context-content');
    content.hidden = closed[side];
    if (side === 'right') $('.context-tabs').hidden = closed.right;
    $$(`[data-panel="${side}"]`).forEach(button => {
      const label = `${side === 'left' ? 'Themennavigation' : 'Modellbereich'} ${closed[side] ? 'einblenden' : 'ausblenden'}`;
      button.setAttribute('aria-label',label);
      button.title = label;
      button.setAttribute('aria-expanded',!closed[side]);
    });
  }
}
function setPanel(side, next) {
  if (!readingBookmark) readingBookmark = captureBookmark();
  const content = side === 'left' ? $('#nav-content') : $('#context-content');
  if (next) panelScroll[side] = content.scrollTop;
  closed[side] = next;
  applyPanels();
  if (!next) content.scrollTop = panelScroll[side];
  restoreBookmark();
  announce(`${side === 'left' ? 'Themennavigation' : 'Modellbereich'} ${next ? 'ausgeblendet' : 'eingeblendet'}.`);
}
function setMode(next, notify = true) {
  const bookmark = captureBookmark();
  mode = Object.hasOwn(concepts,next) ? next : 'balanced';
  for (const key of Object.keys(concepts)) $('#window').classList.toggle(`mode-${key}`,key === mode);
  $$('[data-mode]').forEach(button => button.setAttribute('aria-pressed',button.dataset.mode === mode));
  const c = concepts[mode];
  $('#concept-number').textContent = `KONZEPT ${c[0]} / RUNDE 3`;
  $('#concept-title').textContent = c[1];
  $('#concept-description').textContent = c[2];
  $('#concept-controls').textContent = c[3];
  $('#footer-label').textContent = `${c[0]} · ${c[1]}`;
  applyPanels();
  readingBookmark = bookmark;
  restoreBookmark();
  if (notify) announce(`Entwurf ${c[0]} geöffnet. Die Dokumentationssprache bleibt ${track()}.`);
}
function updateActiveSection() {
  const list = sections(), top = $('#article-scroll').getBoundingClientRect().top;
  let active = list[0]?.id;
  for (const section of list) if (document.getElementById(section.id).getBoundingClientRect().top <= top+85) active = section.id;
  $$('[data-jump]').forEach(link => { if (link.dataset.jump === active) link.setAttribute('aria-current','location'); else link.removeAttribute('aria-current'); });
}
function jump(id) {
  readingBookmark = null;
  const target = document.getElementById(id), scroller = $('#article-scroll');
  if (!target) return;
  scroller.scrollTop += target.getBoundingClientRect().top - scroller.getBoundingClientRect().top - 16;
  const heading = target.matches('h2') ? target : target.querySelector('h2');
  if (heading) { heading.tabIndex = -1; heading.focus({preventScroll:true}); }
  updateActiveSection();
}
document.addEventListener('click',async event => {
  const button = event.target.closest('button,a');
  if (!button) return;
  const d = button.dataset;
  if (d.mode) setMode(d.mode);
  if (d.panel) setPanel(d.panel,!closed[d.panel]);
  if (d.language) { language = d.language; renderArticle(); announce(`Gesamte Dokumentation auf ${track()} umgestellt: Lernweg, Anleitung, Suche und Beispiele.`); }
  if (d.document) { selected = d.document; renderArticle(); announce(`Artikel „${$('#article-title').textContent}“ geöffnet.`); }
  if (d.jump) { event.preventDefault(); jump(d.jump); }
  if (d.tab) { contextTab = d.tab; renderContext(); announce(`Rechter Bereich: ${contextTab === 'model' ? 'Modell' : 'Beispiel'} im ${track()}-Lernweg.`); }
  if (d.width) { $$('[data-width]').forEach(item => item.setAttribute('aria-pressed',item === button)); $('#window').classList.toggle('narrow',d.width === '760'); }
  if (d.font) { $$('[data-font]').forEach(item => item.setAttribute('aria-pressed',item === button)); $('#window').style.setProperty('--reading-size',d.font+'px'); $('#window').classList.toggle('large-text',d.font === '32'); }
  if (d.copy) { try { await navigator.clipboard.writeText(document.getElementById(d.copy).textContent); announce('Beispielausschnitt kopiert.'); } catch { announce('Browser-Zwischenablage nicht verfügbar. Den Ausschnitt direkt markieren.'); } }
});
function search() {
  if (closed.left) setPanel('left',false);
  renderNavigation();
  announce($('.search-count')?.textContent || `Themen im ${track()}-Lernweg.`);
}
$('#search').addEventListener('input',search);
$('#search-form').addEventListener('submit',event => { event.preventDefault(); search(); });
$('.context-tabs').addEventListener('keydown',event => {
  if (!['ArrowLeft','ArrowRight','Home','End'].includes(event.key)) return;
  event.preventDefault();
  contextTab = event.key === 'Home' ? 'model' : event.key === 'End' ? 'example' : contextTab === 'model' ? 'example' : 'model';
  renderContext();
  $(`[data-tab="${contextTab}"]`).focus();
});
$('#article-scroll').addEventListener('scroll',updateActiveSection,{passive:true});
for (const event of ['wheel','pointerdown','keydown','touchstart']) $('#article-scroll').addEventListener(event,() => { readingBookmark = null; },{passive:true});
$$('[data-panel]').forEach(button => { button.innerHTML = panelIcon(button.dataset.panel); });
const initial = new URLSearchParams(location.search);
language = initial.get('language') === 'physim' ? 'physim' : 'c';
renderArticle();
setMode(initial.get('layout') || 'balanced',false);
if (initial.get('width') === '760') $('[data-width="760"]').click();
if (initial.get('font') === '32') $('[data-font="32"]').click();
if (initial.get('left') === 'closed') setPanel('left',true);
if (initial.get('right') === 'closed') setPanel('right',true);
