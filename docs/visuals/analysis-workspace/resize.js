/* Analysis adapters for the same splitter engine and appearance as Entwickeln. */
(() => {
  'use strict';
  const $=id=>document.getElementById(id),workspace=$('workspace'),data=$('data-panel'),chart=document.querySelector('.chart-wrap');
  const defaults={left:232,right:242,chart:null},sizes={...defaults},handles={},splitters={};
  workspace.classList.add('resizable-workspace');chart.id='analysis-chart';
  const visible=element=>getComputedStyle(element).display!=='none'&&!element.hidden;
  const clamp=(value,min,max)=>Math.max(min,Math.min(max,value));
  const widths=()=>{
    const left=visible($('run-panel'))?sizes.left:0,right=visible($('inspector'))?sizes.right:0;
    const space=Math.max(0,workspace.clientWidth-300),total=left+right;
    if(total<=space)return{left,right};
    const lmin=left?140:0,rmin=right?180:0,extra=Math.max(0,space-lmin-rmin),wanted=Math.max(1,total-lmin-rmin);
    return{left:lmin+extra*(left?left-lmin:0)/wanted,right:rmin+extra*(right?right-rmin:0)/wanted};
  };
  function limits(key){
    if(key==='chart')return[180,Math.max(180,Math.min(700,innerHeight-200))];
    const other=key==='left'?$('inspector'):$('run-panel'),otherWidth=visible(other)?other.getBoundingClientRect().width:0,min=key==='left'?140:180;
    return[min,Math.max(min,Math.min(480,workspace.clientWidth-otherWidth-300))];
  }
  function setStyle(element,key,value){if(element.style.getPropertyValue(key)!==value)element.style.setProperty(key,value);}
  function update(){
    const w=widths();setStyle(workspace,'--runs-track',w.left+'px');setStyle(workspace,'--details-track',w.right+'px');
    setStyle(chart,'--analysis-chart-height',sizes.chart===null?'auto':sizes.chart+'px');
    const height=sizes.chart===null?'':String(sizes.chart),changed=$('chart').dataset.previewHeight!==height;
    $('chart').dataset.previewHeight=height;
    const bounds=workspace.getBoundingClientRect();
    for(const [key,id] of [['left','run-panel'],['right','inspector']]){
      const h=handles[key],pane=$(id),r=pane.getBoundingClientRect(),hidden=innerWidth<=760||!visible(pane);if(h.hidden!==hidden)h.hidden=hidden;
      h.style.cssText=`left:${(key==='left'?r.right:r.left)-bounds.left+workspace.scrollLeft-4}px;top:${r.top-bounds.top+workspace.scrollTop}px;width:9px;height:${r.height}px`;
    }
    const dataBounds=data.getBoundingClientRect(),line=document.querySelector('.statistics-heading').getBoundingClientRect();
    const chartHidden=!visible(data)||!visible(chart);if(handles.chart.hidden!==chartHidden)handles.chart.hidden=chartHidden;
    handles.chart.style.cssText=`left:0;top:${line.top-dataBounds.top-4}px;width:100%;height:9px`;
    for(const splitter of Object.values(splitters))splitter.sync();
    if(changed)document.dispatchEvent(new Event('physim-preview-plot-resize'));
  }
  function create(key,parent,target,label){
    const h=document.createElement('div');h.id='resize-analysis-'+key;handles[key]=h;
    h.setAttribute('aria-label',label);h.setAttribute('aria-controls',target.id);parent.append(h);
    splitters[key]=PhysimResize.attach(h,{
      axis:()=>key==='chart'?'y':'x',limits:()=>limits(key),value:()=>key==='chart'?chart.getBoundingClientRect().height:target.getBoundingClientRect().width,
      sign:()=>key==='right'?-1:1,enabled:()=>key==='chart'||innerWidth>760,
      set:value=>{const [min,max]=limits(key);sizes[key]=clamp(value,min,max);update();},reset:()=>{sizes[key]=defaults[key];update();},
      snapshot:()=>({...sizes}),restore:snapshot=>{Object.assign(sizes,snapshot);update();}
    });
  }
  create('left',workspace,$('run-panel'),'Lauflistenbreite ändern');create('right',workspace,$('inspector'),'Detailsbreite ändern');create('chart',data,chart,'Messkurvenhöhe ändern');
  new MutationObserver(update).observe(workspace,{attributes:true,subtree:true,attributeFilter:['class','hidden']});
  const observer=new ResizeObserver(()=>{update();document.dispatchEvent(new Event('physim-preview-plot-resize'));});for(const pane of [workspace,chart,$('run-panel'),$('inspector')])observer.observe(pane);
  addEventListener('resize',update);update();
})();
