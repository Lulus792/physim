'use strict';
// Bounded browser-study lexer; native integration must use the actual editor's symbol/scope data.
window.PhysimCodeContext=(()=>{
 const types=new Set(window.PHYSIM_COMPLETION_DATA.symbols.filter(x=>x.kind==='type').map(x=>x.name));
 const keywords=new Set('auto break case char const continue default do double else enum extern float for goto if inline int long register restrict return short signed sizeof static struct switch typedef union unsigned void volatile while bool true false NULL _Atomic _Bool _Static_assert'.split(' '));
 const languageKeywords=new Set('break case continue default else enum false for func if import in let mutating nil return self static struct switch true var while Int64 Float64 Bool String Void Vec2 Vec3 Vec4 Quat Unit Quantity Channel Dataset Series Plot Distribution SensorConfig Sensor Measurement Table Body Contacts ContactSolver ContactResult DistanceJoint JointResult Submersion ContactConstraint JointConstraint ConstraintResult Sweep Aabb CollisionPair Mat3 Mat4 Bezier3 Optional'.split(' '));
 const pattern=/\/\/[^\n]*|\/\*[\s\S]*?(?:\*\/|$)|"(?:\\.|[^"\\])*(?:"|$)|'(?:\\.|[^'\\])*(?:'|$)|#[^\n]*|\b\d+(?:\.\d+)?(?:[eE][+-]?\d+)?\b|\b[A-Za-z_]\w*\b/g;
 const escape=x=>String(x).replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
 function tokens(text,language=false){const result=[];let at=0;for(const m of text.matchAll(pattern)){if(m.index>at)result.push({start:at,end:m.index,text:text.slice(at,m.index),kind:''});const v=m[0],kind=v.startsWith('//')||v.startsWith('/*')?'comment':v[0]==='"'||v[0]==="'"?'string':v[0]==='#'?'preprocessor':/^\d/.test(v)?'number':(language?languageKeywords:keywords).has(v)?'keyword':v.startsWith('ps_')||types.has(v)?'type':'';result.push({start:m.index,end:m.index+v.length,text:v,kind});at=m.index+v.length;}if(at<text.length)result.push({start:at,end:text.length,text:text.slice(at),kind:''});return result;}
 function html(text,cursor=null,language=false){let marked=false;const marker='<span id="caret" class="caret-marker">&#8203;</span>';let result='';for(const t of tokens(text,language)){let v=escape(t.text);if(cursor!==null&&!marked&&cursor>=t.start&&cursor<=t.end){v=escape(text.slice(t.start,cursor))+marker+escape(text.slice(cursor,t.end));marked=true;}result+=t.kind?'<span class="syntax-'+t.kind+'">'+v+'</span>':v;}if(cursor!==null&&!marked)result+=marker;return result;}
 function mask(text){return tokens(text).map(t=>['comment','string','preprocessor'].includes(t.kind)?t.text.replace(/[^\n]/g,' '):t.text).join('');}
 function scopes(text){const stack=[-1];for(let i=0;i<text.length;i++){if(text[i]==='{')stack.push(i);else if(text[i]==='}')stack.pop();}return stack;}
 function call(text,cursor){const before=mask(text.slice(0,cursor)),stack=[];for(let i=0;i<before.length;i++){if(before[i]==='(')stack.push(i);else if(before[i]===')')stack.pop();}if(!stack.length)return null;const at=stack[stack.length-1],name=before.slice(0,at).match(/([A-Za-z_]\w*)\s*$/)?.[1];if(!name||keywords.has(name))return null;return {name,at};}
 function locals(text,cursor){
  const clean=mask(text),before=clean.slice(0,cursor),active=scopes(before),found=new Map(),params=[];
  const declaration=/\b((?:const\s+)?(?:ps_[A-Za-z_]\w*|double|float|int|bool|unsigned|Vec[234]|Float64|Int64)\s*\*?)\s+([A-Za-z_]\w*)/g;
  const functions=/\b(?:void|double|float|int|bool|ps_[A-Za-z_]\w*)\s+([A-Za-z_]\w*)\s*\(([^)]*)\)\s*\{/g;
  for(const fn of before.matchAll(functions)){const scope=fn.index+fn[0].length-1;if(!active.includes(scope))continue;const begin=fn.index+fn[0].indexOf('(')+1;for(const d of fn[2].matchAll(declaration))params.push({name:d[2],type:d[1].trim(),at:begin+d.index,scope});}
  for(const d of before.matchAll(declaration)){const end=d.index+d[0].length;if(/^\s*\(/.test(before.slice(end)))continue;const semi=before.indexOf(';',end);if(semi<0||/[{}]/.test(before.slice(end,semi)))continue;const scope=scopes(before.slice(0,d.index)).at(-1);if(!active.includes(scope))continue;const inHeader=before.slice(end,semi).includes('){')||/\)\s*\{/.test(before.slice(end,semi));if(inHeader)continue;found.set(d[2],{name:d[2],type:d[1].trim(),at:d.index,scope});}
  for(const p of params)if(!found.has(p.name))found.set(p.name,p);
  return [...found.values()].map(v=>({name:v.name,module:'Lokal',signature:v.type+' '+v.name,kind:'variable',language:false,instance:false,summary:'Im sichtbaren Gültigkeitsbereich vor dem Cursor deklariert.',parameters:[],reference:'Aktuelle Datei · Zeile '+(text.slice(0,v.at).split('\n').length),documented:false,local:true}));
 }
 return {html,mask,call,locals};
})();
