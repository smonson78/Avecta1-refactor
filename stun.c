int stun(pc)
int pc;
{
char *c = curmon[pc];
if( *(c+13) > rnd(100) ) 
  *(c+18) = 0;
}  
