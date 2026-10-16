const $ = (selector) => document.querySelector(selector);
const $$ = (selector) => [...document.querySelectorAll(selector)];
const escapeHtml = (text) => text.replace(/[&<>"']/g, (c) => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
let mode = 'outline';
let selected = 'pendulum';
let step = 0;
let language = 'c';
let category = 'mechanics';
let collapsed = false;
let navigationBookmark = null;

const concepts = {
  outline: ['E', 'Eine gemeinsame Seitenleiste', 'Themen und Abschnitte stehen links in einer zusammenhängenden Hierarchie. Unter dem gewählten Artikel siehst du direkt seine Sprungziele.', 'Eine Navigation, eine Lesefläche. Alle Abschnitte sind ohne Zwischenmenü erreichbar.', 'Die gesamte Leiste lässt sich ausblenden. Auswahl und Leseposition bleiben erhalten.'],
  navigator: ['F', 'Bereich und Kapitel nebeneinander', 'Links wählst du einen Bereich. Direkt daneben stehen dessen Artikel und die Abschnitte der aktuellen Seite. Rechts bleibt der Lesetext.', 'Zwei dauerhaft sichtbare Navigationsspalten statt aufklappbarer Baumknoten.', 'Ein Knopf blendet beide Spalten aus. Bei 760 px stehen die zwei Ebenen untereinander innerhalb derselben Seitenleiste.'],
  reference: ['G', 'Erklärung und Beispiel im Blick', 'Links stehen Themen und Abschnittssprünge. Der Artikel und ein eigener Beispielbereich teilen sich die übrige Fläche.', 'Der Code bleibt beim Lesen sichtbar. C und Physim lassen sich direkt umschalten.', 'Die linke Leiste ist einklappbar. Bei 760 px bleibt das Beispiel als eigener Bereich unter dem Artikel.'],
  classic: ['A', 'Das Handbuch mit direktem Zugriff', 'Links stehen die Themen, rechts die Abschnitte. Bei wenig Breite ziehen die Abschnittslinks in die linke Seitenleiste um.', 'Artikel und Abschnittsübersicht sind direkt erreichbar, ohne aufklappbare Inhaltsmenüs.', 'Der Seitenleistenknopf blendet die linke und rechte Navigation gemeinsam aus.'],
  learn: ['C', 'Schritte neben dem Lerntext', 'Die Lernschritte bleiben links sichtbar. Der aktuelle Text steht mittig, Modell und Ausgangswerte ergänzen ihn rechts.', 'Vier Schritte direkt anwählen oder unten vor- und zurückgehen. Es gibt keine verschachtelten Aufklappmenüs.', 'Die Schrittleiste lässt sich als Ganzes ausblenden. Bei 760 px steht die Modellgrafik im Text.'],
  find: ['D', 'Treffer und Abschnittssprünge', 'Die Suchtreffer stehen links. Der gewählte Artikel erscheint daneben, seine Abschnittslinks bleiben seitlich erreichbar.', 'Suchtreffer und Artikel im Zusammenhang lesen. Die Suche filtert eine lokale Beispielsammlung.', 'Ein Knopf blendet die seitlichen Navigationsflächen aus. Die Auswahl der letzten Treffer bleibt erhalten.']
};
const groups = {
  start: {title: 'Einstieg', ids: ['start','workspace']},
  learning: {title: 'Lernwege', ids: ['c','physim']},
  mechanics: {title: 'Mechanik', ids: ['pendulum','spring','projectile']},
  numerics: {title: 'Numerik', ids: ['numerics']},
  help: {title: 'Hilfe', ids: ['trouble']}
};
const shortTitle = {pendulum: 'Pendel & Integratoren', workspace: 'Arbeitsbereich bedienen', start: 'Start & Lernpfade'};
const lessonTitles = ['Modell verstehen','Versuch vorbereiten','Ergebnisse vergleichen','Selbst untersuchen'];
const lessonSubs = ['Annahmen & Gleichungen','Euler und RK4','Winkel & Energie','Länge verändern'];

function announce(text) { $('#state-message').textContent = text; }

function sectionLinks() {
  return [...$('#article-content').querySelectorAll('h2')].map((heading, index) => {
    const section = heading.closest('section');
    const id = section?.id || (heading.id ||= `heading-${index}`);
    return {id, title: heading.textContent};
  });
}

function sectionsMarkup() {
  const sections = sectionLinks();
  if (!sections.length) return '';
  return `<div class="in-article"><p class="nav-label">In diesem Artikel</p>${sections.map((s, i) => `<a class="section-link" href="#${s.id}" data-jump="${s.id}"${i === 0 ? ' aria-current="location"' : ''}>${escapeHtml(s.title)}</a>`).join('')}</div>`;
}

function docButton(id, withSections = false) {
  const doc = docs.find((d) => d.id === id);
  return `<button class="nav-row" data-document="${id}"${id === selected ? ' aria-current="page"' : ''}>${escapeHtml(shortTitle[id] || doc.title)}</button>${id === selected && withSections ? `<div class="section-links">${sectionsMarkup()}</div>` : ''}`;
}

function topicNavigation(combined) {
  return ['learning','mechanics','numerics','help'].map((key) => `<div class="nav-group"><p class="nav-label">${groups[key].title}</p>${groups[key].ids.map((id) => docButton(id, combined)).join('')}</div>`).join('');
}

function renderNavigation() {
  $('#nav-secondary').replaceChildren();
  $('#toc').innerHTML = sectionsMarkup();
  if (mode === 'navigator') {
    $('#nav-primary').innerHTML = `<p class="nav-label">Bereiche</p>${Object.entries(groups).map(([key, group]) => `<button class="nav-row category" data-category="${key}"${category === key ? ' aria-current="true"' : ''}>${group.title}<span class="category-number">${group.ids.length} Artikel</span></button>`).join('')}<p class="nav-note">Bereich wählen.<br>Kapitel stehen daneben.</p>`;
    $('#nav-secondary').innerHTML = `<p class="nav-label">${groups[category].title}</p>${groups[category].ids.map((id) => docButton(id, false)).join('')}<hr class="nav-divider">${sectionsMarkup()}`;
  } else if (mode === 'learn') {
    $('#nav-primary').innerHTML = `<p class="nav-label">Lernpfad · Mechanik</p><p class="nav-note" style="margin-bottom:20px">Pendel verstehen.<br>Verfahren vergleichen.</p>${lessonTitles.map((title, i) => `<button class="nav-row step" data-step="${i}"${i === step ? ' aria-current="step"' : ''}><span class="count">${i + 1}</span><span class="step-label">${title}<small>${lessonSubs[i]}</small></span></button>`).join('')}<hr class="nav-divider">${sectionsMarkup()}`;
  } else if (mode === 'find') {
    const query = $('#search').value.trim().toLocaleLowerCase('de');
    const matches = query ? docs.filter((doc) => `${doc.title} ${doc.lead} ${doc.snippet}`.toLocaleLowerCase('de').includes(query)) : docs.slice(0,3);
    $('#nav-primary').innerHTML = `<p class="nav-label">Suchergebnisse</p><p class="result-count">${query ? matches.length + ' Treffer' : 'Drei Vorschläge'}</p><div class="result-list">${matches.length ? matches.map((doc) => `<button class="result" data-result="${doc.id}" aria-pressed="${selected === doc.id}"><span class="kind">${escapeHtml(doc.kind)}</span><strong>${escapeHtml(doc.title)}</strong><p>${escapeHtml(doc.snippet)}</p></button>`).join('') : '<p class="empty">Keine Treffer. Probiere „Pendel“, „Euler“ oder „Energie“. Der zuletzt geöffnete Artikel bleibt sichtbar.</p>'}</div><hr class="nav-divider">${sectionsMarkup()}`;
  } else if (mode === 'classic') {
    $('#nav-primary').innerHTML = topicNavigation(false) + `<hr class="nav-divider">${sectionsMarkup()}`;
  } else {
    $('#nav-primary').innerHTML = topicNavigation(true);
  }
  updateActiveSection();
}

function renderContext() {
  const pane = $('#context-pane');
  if (mode === 'reference') {
    pane.innerHTML = `<p class="nav-label">Passend zum Modell</p><h2>Die Zustandsableitung</h2><div class="language" role="group" aria-label="Sprache des Beispiels"><button data-language="c" aria-pressed="${language === 'c'}">C</button><button data-language="physim" aria-pressed="${language === 'physim'}">Physim</button></div><div class="code-head"><span id="context-source"></span><button data-copy="context-code">Kopieren</button></div><pre id="context-code"></pre><p class="source-note">Ausschnitt aus dem vorhandenen Pendelbeispiel. Kein vollständiges Programm.</p><hr class="nav-divider"><h2>Für den Vergleich</h2><table><tbody><tr><th>Masse</th><td>1 kg</td></tr><tr><th>Länge</th><td>1,5 m</td></tr><tr><th>Startwinkel</th><td>0,45 rad</td></tr><tr><th>Starttempo</th><td>0 rad/s</td></tr></tbody></table><p>Euler und RK4 bei gleichen Anfangswerten und gleicher Schrittweite vergleichen.</p>`;
    setLanguage(language);
  } else if (mode === 'learn') {
    pane.innerHTML = `<p class="nav-label">Modellkontext</p><h2>Das Versuchsmodell</h2><img class="diagram" src="model.svg" alt="Pendel mit Masse, Stangenlänge und Winkel zur Senkrechten"><table><tbody><tr><th>Masse</th><td>1 kg</td></tr><tr><th>Länge</th><td>1,5 m</td></tr><tr><th>Startwinkel</th><td>0,45 rad</td></tr><tr><th>Starttempo</th><td>0 rad/s</td></tr></tbody></table><div class="notice"><strong>Worauf achten?</strong>${lessons[step].hint}</div><p class="source-note">Die eigentliche Simulation bleibt im Hauptfenster.</p>`;
  } else {
    pane.replaceChildren();
  }
}

function setLanguage(next) {
  language = next;
  $$('[data-language]').forEach((button) => button.setAttribute('aria-pressed', button.dataset.language === language));
  const code = $('#context-code') || $('#code-block');
  if (code) code.textContent = codeSamples[language];
  const label = $('#context-source') || $('#source-label');
  if (label) label.textContent = language === 'c' ? 'C · Zustandsableitung' : 'Physim · Zustandsableitung';
}

function renderArticle() {
  navigationBookmark = null;
  const doc = docs.find((item) => item.id === selected) || docs[0];
  const learning = mode === 'learn';
  $('#article-title').textContent = learning ? lessons[step].title : doc.title;
  $('#article-lead').textContent = learning ? lessons[step].lead : doc.lead;
  $('#article-content').innerHTML = learning ? lessons[step].body : doc.body;
  $('#lesson-heading').hidden = !learning;
  $('#lesson-heading').textContent = `Schritt ${step + 1} von 4`;
  $('.crumb').hidden = learning;
  $('.step-controls').hidden = !learning;
  $('#previous-step').disabled = step === 0;
  $('#next-step').disabled = step === 3;
  $('#next-step').textContent = step === 3 ? 'Letzter Schritt' : 'Weiter →';
  $('#step-position').textContent = `${step + 1} / 4`;
  $('#article-scroll').scrollTop = 0;
  renderNavigation();
  renderContext();
  if (learning && step === 1) setLanguage(language);
}

function captureBookmark() {
  const top = $('#article-scroll').getBoundingClientRect().top;
  const elements = [...$('#article').querySelectorAll('h1,h2,h3,p,pre,.equation')];
  const element = elements.reduce((nearest, current) => Math.abs(current.getBoundingClientRect().top - top) < Math.abs(nearest.getBoundingClientRect().top - top) ? current : nearest);
  return {element, offset: element.getBoundingClientRect().top - top};
}

function setCollapsed(next, notify = true) {
  const changing = next !== collapsed;
  const bookmark = collapsed && !next && navigationBookmark ? navigationBookmark : captureBookmark();
  if (!collapsed && next) navigationBookmark = bookmark;
  collapsed = next;
  $('#window').classList.toggle('nav-collapsed', collapsed);
  $('#toggle-nav').setAttribute('aria-expanded', !collapsed);
  $('#toggle-nav').setAttribute('aria-controls', 'navigation toc');
  $('#toggle-label').textContent = collapsed ? 'Seitenleiste einblenden' : 'Seitenleiste ausblenden';
  if (changing) {
    const scroller = $('#article-scroll');
    scroller.scrollTop += bookmark.element.getBoundingClientRect().top - scroller.getBoundingClientRect().top - bookmark.offset;
  }
  if (notify) announce(collapsed ? 'Seitenleiste ausgeblendet. Mehr Platz für den Artikel.' : 'Seitenleiste eingeblendet. Themen und Sprungziele direkt sichtbar.');
}

function setMode(next, notify = true) {
  mode = Object.hasOwn(concepts,next) ? next : 'outline';
  if (mode === 'navigator') category = Object.keys(groups).find((key) => groups[key].ids.includes(selected)) || 'mechanics';
  const narrow = $('[data-width="760"]').getAttribute('aria-pressed') === 'true';
  const large = $('[data-font="32"]').getAttribute('aria-pressed') === 'true';
  $('#window').className = `window mode-${mode}${narrow ? ' narrow' : ''}${large ? ' large-text' : ''}`;
  $$('[data-mode]').forEach((button) => button.setAttribute('aria-pressed', button.dataset.mode === mode));
  const c = concepts[mode];
  $('#concept-number').textContent = `KONZEPT ${c[0]} / RUNDE 2`;
  $('#concept-title').textContent = c[1];
  $('#concept-description').textContent = c[2];
  $('#concept-use').textContent = c[3];
  $('#concept-tradeoff').textContent = c[4];
  $('#footer-label').textContent = `${c[0]} · ${c[1]}`;
  if (mode === 'find' && !$('#search').value) $('#search').value = 'Pendel';
  renderArticle();
  setCollapsed(collapsed, false);
  if (notify) announce(`Layout ${c[0]} geöffnet.`);
}

function updateActiveSection() {
  const sections = sectionLinks();
  const top = $('#article-scroll').getBoundingClientRect().top;
  let active = sections[0]?.id;
  for (const section of sections) {
    if (document.getElementById(section.id).getBoundingClientRect().top <= top + 90) active = section.id;
  }
  $$('[data-jump]').forEach((link) => {
    if (link.dataset.jump === active) link.setAttribute('aria-current','location');
    else link.removeAttribute('aria-current');
  });
}

function jump(id) {
  navigationBookmark = null;
  const target = document.getElementById(id);
  if (!target) return;
  const scroller = $('#article-scroll');
  scroller.scrollTop += target.getBoundingClientRect().top - scroller.getBoundingClientRect().top - 18;
  const heading = target.matches('h2') ? target : target.querySelector('h2');
  if (heading) { heading.tabIndex = -1; heading.focus({preventScroll:true}); }
  updateActiveSection();
}

document.addEventListener('click', async (event) => {
  const button = event.target.closest('button,a');
  if (!button) return;
  const data = button.dataset;
  if (data.mode) setMode(data.mode);
  if (data.width) {
    $$('[data-width]').forEach((item) => item.setAttribute('aria-pressed',item === button));
    $('#window').classList.toggle('narrow',data.width === '760');
    announce(`Vorschau: ${data.width} px.`);
  }
  if (data.font) {
    $$('[data-font]').forEach((item) => item.setAttribute('aria-pressed',item === button));
    $('#window').style.setProperty('--body',data.font + 'px');
    $('#window').classList.toggle('large-text',data.font === '32');
    announce(`Artikel und Seitennavigation: ${data.font} px.`);
  }
  if (data.category) {
    category = data.category;
    if (!groups[category].ids.includes(selected)) selected = groups[category].ids[0];
    renderArticle();
  }
  if (data.document || data.result) {
    selected = data.document || data.result;
    renderArticle();
    announce(`Artikel „${$('#article-title').textContent}“ geöffnet.`);
  }
  if (data.step !== undefined) { step = Number(data.step); renderArticle(); }
  if (data.language) setLanguage(data.language);
  if (data.jump) { event.preventDefault(); jump(data.jump); }
  if (button.id === 'toggle-nav') setCollapsed(!collapsed);
  if (button.id === 'previous-step' && step > 0) { step--; renderArticle(); }
  if (button.id === 'next-step' && step < 3) { step++; renderArticle(); }
  if (button.id === 'focus-article') {
    $('#article-title').tabIndex = -1;
    $('#article-title').focus({preventScroll:true});
    $('#article-scroll').scrollTop = 0;
  }
  const copyTarget = data.copy || (button.id === 'copy-code' ? 'code-block' : null);
  if (copyTarget) {
    try { await navigator.clipboard.writeText(document.getElementById(copyTarget).textContent); announce('Beispielausschnitt kopiert.'); }
    catch { announce('Die Browser-Zwischenablage ist nicht verfügbar. Den Ausschnitt direkt im Text markieren.'); }
  }
});

function search() {
  if (mode !== 'find') setMode('find',false);
  setCollapsed(false,false);
  renderNavigation();
  announce($('.result-count').textContent);
}
$('#search-form').addEventListener('submit',(event) => { event.preventDefault(); search(); });
$('#search').addEventListener('input',search);
$('#article-scroll').addEventListener('scroll',updateActiveSection,{passive:true});
for (const event of ['wheel','pointerdown','keydown','touchstart']) {
  $('#article-scroll').addEventListener(event,() => { navigationBookmark = null; },{passive:true});
}

const initial = new URLSearchParams(location.search);
if (initial.get('width') === '760') $('[data-width="760"]').click();
if (initial.get('font') === '32') $('[data-font="32"]').click();
setMode(initial.get('layout') || 'outline',false);
if (initial.get('collapsed') === '1') setCollapsed(true,false);
