// H is the selected design. Themes alter color roles, never panel visibility.
function setTheme(theme, notify = true) {
  const next = theme === 'light' ? 'light' : 'dark';
  document.documentElement.dataset.theme = next;
  document.querySelectorAll('button[data-theme]').forEach(button => button.setAttribute('aria-pressed',button.dataset.theme === next));
  if (notify) announce(`${next === 'dark' ? 'Dunkle' : 'Helle'} Ansicht. Sprache und Seitenzustände bleiben erhalten.`);
}
document.addEventListener('click',event => {
  const button = event.target.closest('button[data-theme]');
  if (button) setTheme(button.dataset.theme);
});
setMode('balanced',false);
setTheme(new URLSearchParams(location.search).get('theme'),false);
$('#concept-number').textContent = 'AUSGEWÄHLTES ZIELBILD · H';
$('#concept-title').textContent = 'Navigation und Kontext unabhängig';
$('#concept-description').textContent = 'Links Themen und Abschnittssprünge aus E/G, rechts Modell und Beispiel aus C. Die kleine linke und rechte Paneelaktion bleiben dauerhaft im Fensterkopf erreichbar.';
$('#concept-controls').textContent = 'Links schaltet ausschließlich die linke Navigation. Rechts schaltet ausschließlich den Modell-/Beispielbereich. Artikel und jeweils andere Seite behalten ihre korrekte Position.';
$('#footer-label').textContent = 'H · Zwei unabhängig schaltbare Seiten';
