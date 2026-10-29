// Small, bounded line comparison for the isolated design fixtures. Not a merge engine.
(function(root){
 'use strict';
 const lines=text=>text===''?[]:String(text).split('\n');
 function compare(before,after){
  const a=lines(before),b=lines(after),ops=[];
  const precise=(a.length+1)*(b.length+1)<=100000;
  if(precise){
   const matrix=Array.from({length:a.length+1},()=>new Uint32Array(b.length+1));
   for(let i=a.length-1;i>=0;i--)for(let j=b.length-1;j>=0;j--)matrix[i][j]=a[i]===b[j]?1+matrix[i+1][j+1]:Math.max(matrix[i+1][j],matrix[i][j+1]);
   let i=0,j=0;
   while(i<a.length||j<b.length){
    if(i<a.length&&j<b.length&&a[i]===b[j]){ops.push({kind:'context',text:a[i]});i++;j++;}
    else if(i<a.length&&(j===b.length||matrix[i+1][j]>=matrix[i][j+1]))ops.push({kind:'delete',text:a[i++]});
    else ops.push({kind:'add',text:b[j++]});
   }
  }else{a.forEach(text=>ops.push({kind:'delete',text}));b.forEach(text=>ops.push({kind:'add',text}));}
  let oldNumber=0,newNumber=0;
  const rows=ops.map(op=>({...op,oldNumber:op.kind==='add'?null:++oldNumber,newNumber:op.kind==='delete'?null:++newNumber}));
  return {rows,removed:rows.filter(r=>r.kind==='delete').length,added:rows.filter(r=>r.kind==='add').length,oldLines:a.length,newLines:b.length,precise};
 }
 const api=Object.freeze({compare});root.PhysimUnifiedDiff=api;
 if(typeof module==='object'&&module.exports)module.exports=api;
})(globalThis);
