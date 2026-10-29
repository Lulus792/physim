const assert=require('node:assert/strict'),{compare}=require('../docs/visuals/file-safety-options/unified-diff.js');
const fs=require('node:fs'),path=require('node:path');
const samples=[['a\nb\nc','a\nx\nc'],['','new'],['old',''],['same','same'],['a\n','a'],['θ\nβ\nβ','θ\nα\nβ'],['<script>\n&','&\n<script>'],['','']];
function verify(a,b){const result=compare(a,b),split=s=>s===''?[]:s.split('\n');assert.deepEqual(result.rows.filter(r=>r.kind!=='add').map(r=>r.text),split(a));assert.deepEqual(result.rows.filter(r=>r.kind!=='delete').map(r=>r.text),split(b));assert.deepEqual(result.rows.filter(r=>r.oldNumber!==null).map(r=>r.oldNumber),split(a).map((_,i)=>i+1));assert.deepEqual(result.rows.filter(r=>r.newNumber!==null).map(r=>r.newNumber),split(b).map((_,i)=>i+1));return result;}
samples.forEach(([a,b])=>verify(a,b));
assert.deepEqual(compare('a\nb\nc','a\nx\nc').rows,[{kind:'context',text:'a',oldNumber:1,newNumber:1},{kind:'delete',text:'b',oldNumber:2,newNumber:null},{kind:'add',text:'x',oldNumber:null,newNumber:2},{kind:'context',text:'c',oldNumber:3,newNumber:3}]);
const values=['','x','x\ny','y\nx','x\nx','x\n','\n','θ\n<>&'];let pairs=0;
for(const a of values)for(const b of values){verify(a,b);pairs++;}
assert.equal(verify(Array(400).fill('before').join('\n'),Array(400).fill('after').join('\n')).precise,false);
const out=path.resolve(__dirname,'../build/file-safety-options-review');fs.mkdirSync(out,{recursive:true});const report={passed:true,samples:samples.length,pairs,boundedFallback:true,checks:['Explicit unified row/number expectation','Both original texts reconstructed independently from rows','Empty/insert/delete/unchanged/Unicode/duplicate/trailing-newline cases','Bounded large-comparison fallback preserves both texts']};fs.writeFileSync(path.join(out,'diff-unit.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));
