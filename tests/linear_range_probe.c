#include "physim/numerics.h"
#include <stdio.h>
int main(void) {
 size_t n;double tol,a[1024],b[32],x[32];
 while(scanf("%zu %la",&n,&tol)==2) {
  if(!n || n>32)return 2;
  for(size_t i=0;i<n*n;i++)if(scanf("%la",a+i)!=1)return 2;
  for(size_t i=0;i<n;i++){if(scanf("%la",b+i)!=1)return 2;x[i]=7+(double)i;}
  ps_result r=ps_linear_solve(a,b,n,tol,x);printf("%d",r);
  for(size_t i=0;i<n;i++)printf(" %a",x[i]);
  putchar('\n');
 }
 return ferror(stdin)?2:0;
}
