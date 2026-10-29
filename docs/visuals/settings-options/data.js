'use strict';
window.SettingsStudyData=Object.freeze({
 defaults:{theme:'dark',uiSize:16,codeSize:12,autosave:30,vectors:true,paths:true,points:true,labels:true,grid:true,ortho:false,log:false,sidebar:218,inspector:255,logHeight:340},
 initial:{theme:'dark',uiSize:18,codeSize:14,autosave:30,vectors:true,paths:false,points:false,labels:true,grid:true,ortho:true,log:true,sidebar:218,inspector:255,logHeight:340},
 groups:[{id:'appearance',name:'Darstellung',icon:'◐',note:'Theme und lesbare Oberfläche.'},{id:'editor',name:'Editor & Sicherung',icon:'⌘',note:'Code lesen und Änderungen sichern.'},{id:'scene',name:'Simulation',icon:'↗',note:'Sichtbarkeit und Projektion.'},{id:'workspace',name:'Arbeitsfläche',icon:'▥',note:'Persönliche Größen für Entwickeln.'},{id:'units',name:'Anzeigeeinheiten',icon:'m',note:'Eigene Aufgabe je Messkanal.'}],
 fields:[
  {key:'theme',group:'appearance',title:'Darstellung',help:'Für Navigation, Editor, Protokoll und Diagramme. Szenenobjekte behalten ihre Farben.',type:'theme',options:[['dark','Dunkel'],['light','Hell'],['contrast','Hoher Kontrast']]},
  {key:'uiSize',group:'appearance',title:'Schriftgröße der Oberfläche',help:'Unabhängig vom Code. Die Vorschau zeigt Menüs und Beschriftungen in der gewählten Größe.',type:'select',options:[16,18,20,22].map(v=>[v,v+' px']),tags:'ui schrift font lesbarkeit'},
  {key:'codeSize',group:'editor',title:'Schriftgröße im Code-Editor',help:'Gilt für die Projekteditoren und das Protokoll. Ein eigener F1-Lesezoom bleibt unabhängig.',type:'select',options:[12,14,16,18,20,22].map(v=>[v,v+' px']),tags:'font schrift editor'},
  {key:'autosave',group:'editor',title:'Automatische Sicherung',help:'Sichert ungespeicherte Entwürfe. Verändert keine Quelldatei und ersetzt bewusstes Speichern nicht.',type:'select',options:[[10,'Alle 10 Sekunden'],[30,'Alle 30 Sekunden'],[60,'Jede Minute'],[120,'Alle 2 Minuten']],tags:'autosave backup sicherung wiederherstellung'},
  {key:'vectors',group:'scene',title:'Vektoren anzeigen',help:'Pfeile sichtbar machen. Messwerte und Berechnung bleiben unverändert.',type:'boolean'},
  {key:'paths',group:'scene',title:'Flugbahnen anzeigen',help:'Aufgezeichnete Pfade in der Szene darstellen.',type:'boolean'},
  {key:'points',group:'scene',title:'Punkte anzeigen',help:'Punktgeometrie ein- oder ausblenden.',type:'boolean'},
  {key:'labels',group:'scene',title:'Beschriftungen anzeigen',help:'Objekttexte und Modellbeschriftungen sichtbar machen.',type:'boolean'},
  {key:'grid',group:'scene',title:'Raster und Achsen',help:'Räumliche Orientierung ein- oder ausblenden.',type:'boolean'},
  {key:'ortho',group:'scene',title:'Orthografische Ansicht',help:'Parallele Projektion für die Betrachtung. Ändert keine Modellgeometrie.',type:'boolean'},
  {key:'log',group:'workspace',title:'Protokoll geöffnet',help:'Persönliche Sichtbarkeit. Die gemeinsame Protokollgestaltung C bleibt erhalten.',type:'boolean'},
  {key:'sidebar',group:'workspace',title:'Seitenleistenbreite',help:'Gespeicherter Größenwert für den dargestellten Bereich Entwickeln.',type:'number',min:140,max:480,unit:'px'},
  {key:'inspector',group:'workspace',title:'Inspektorbreite',help:'Unabhängig von der linken Seitenleiste. Feste Panelplätze bleiben erhalten.',type:'number',min:180,max:480,unit:'px'},
  {key:'logHeight',group:'workspace',title:'Protokollhöhe',help:'Gespeicherter Ausgangswert. Der verfügbare Platz im Arbeitsfenster begrenzt die tatsächliche Höhe.',type:'number',min:126,max:500,unit:'px'},
  {key:'units',group:'units',title:'Anzeigeeinheiten für Messkanäle',help:'Persönliche Auswahl je Kanal und SI-Dimension. Eigene Aufgabe mit eigenem Anwenden; Rohdaten und CSV bleiben unverändert.',type:'task',tags:'einheit meter centimeter kmh kanal messwerte'}
 ],
 channels:{position:{title:'Position x',si:'m',sample:1.25,presets:[['m',1],['cm',.01],['mm',.001],['km',1000]]},speed:{title:'Geschwindigkeit',si:'m/s',sample:12,presets:[['m/s',1],['km/h',1/3.6]]}}
});
