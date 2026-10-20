/* Shared splitter interaction, based on the selected Entwickeln workbench. */
(() => {
  'use strict';
  let drag=null;
  function finish(cancel=false){
    if(!drag)return;const current=drag;drag=null;
    if(cancel)current.options.restore(current.snapshot);
    if(current.handle.hasPointerCapture(current.pointer))current.handle.releasePointerCapture(current.pointer);
    document.body.classList.remove('resizing');document.body.style.cursor='';
  }
  function attach(handle,options){
    handle.classList.add('resize-handle');handle.tabIndex=0;handle.setAttribute('role','separator');
    handle.title='Ziehen oder Pfeiltasten · Umschalt: 40 · Pos1/Ende: Grenzen · Enter/Doppelklick: Standard · Escape: Abbrechen';
    function sync(){
      const axis=options.axis(),[min,max]=options.limits(),value=options.value();
      handle.setAttribute('aria-orientation',axis==='x'?'vertical':'horizontal');
      for(const [key,n] of [['min',min],['max',max],['now',value]])handle.setAttribute('aria-value'+key,String(Math.round(n)));
      handle.setAttribute('aria-valuetext',Math.round(value)+(options.unit||' Pixel'));
      if(drag?.handle===handle&&(handle.hidden||!handle.getClientRects().length))finish(true);
    }
    handle.addEventListener('pointerdown',event=>{
      if(event.button!==0||options.enabled?.()===false)return;
      finish(true);event.preventDefault();handle.focus();const axis=options.axis();
      drag={handle,options,pointer:event.pointerId,axis,start:axis==='x'?event.clientX:event.clientY,value:options.value(),snapshot:options.snapshot()};
      handle.setPointerCapture(event.pointerId);document.body.classList.add('resizing');document.body.style.cursor=axis==='x'?'col-resize':'row-resize';
    });
    handle.addEventListener('pointermove',event=>{
      if(drag?.handle!==handle)return;
      const delta=(drag.axis==='x'?event.clientX:event.clientY)-drag.start;
      options.set(drag.value+delta*(options.sign?.()??1));sync();
    });
    handle.addEventListener('pointerup',()=>{if(drag?.handle===handle)finish();});
    for(const name of ['pointercancel','lostpointercapture'])handle.addEventListener(name,()=>{if(drag?.handle===handle)finish(true);});
    handle.addEventListener('dblclick',()=>{finish();options.reset();sync();});
    handle.addEventListener('keydown',event=>{
      if(options.enabled?.()===false)return;
      const axis=options.axis(),negative=axis==='x'?'ArrowLeft':'ArrowUp',positive=axis==='x'?'ArrowRight':'ArrowDown';
      if(event.key===negative||event.key===positive){event.preventDefault();options.set(options.value()+(event.key===positive?1:-1)*(options.sign?.()??1)*(event.shiftKey?40:10));}
      else if(event.key==='Home'||event.key==='End'){event.preventDefault();options.set(options.limits()[event.key==='Home'?0:1]);}
      else if(event.key==='Enter'){event.preventDefault();options.reset();}
      sync();
    });
    return{sync};
  }
  document.addEventListener('keydown',event=>{if(drag&&event.key==='Escape'){event.preventDefault();event.stopImmediatePropagation();finish(true);}},true);
  addEventListener('resize',()=>finish(true));addEventListener('blur',()=>finish(true));
  window.PhysimResize=Object.freeze({attach});
})();
