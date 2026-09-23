int explode(x,y,type)
int x,y,type;
{
int firebuff[],bitmap[][65],vbl(),off();
long int addr;
int i,j;
x *= 16;
y *= 16;
xbios_37();
xbios_38_off();
storsc(firebuff,x,y,0,addr);
for(i=1;i<10;i++) {
   xbios_37();
   blt(bitmap[132 + 2*type + (i%2)],x,y,addr);
   for(j=0;j<4000;j++);
   xbios_37();
   storsc(firebuff,x,y,1,addr);
   }
xbios_38_vbl();
return(1);
}
