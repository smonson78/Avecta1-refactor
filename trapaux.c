#include "globals.h"

int destroy(int type, int num)
{
  int x,y,i;
  char *c,*z;
  if(type == 1) {
    c = crumobj[num];
    x = *(c+6);
    y = *(c+7);
    }
  else {
    c = curmon[num];
    x = *(c+24);
    y = *(c+25);
    }
  z = zline[x][y];
  if(type == 1) {
    for(i=0;i<9;i++)
      *(c+i) = 0;
    *(z+1) = 0;
    if(*(z+5) > 0 || rumdata[crum][30])
      drawsq(x,y);
    }
  if(type == 2) {
    damage(num,*(c+2) + *(c+12));
    }
  return(1);
}

int empty(int num)
{
  crumobj[num][5] = 0;
  return(1);
}

int make(int id, int take, int trap, int con, int nmsg, int hide, int x, int y, int vis)
{
  int i;
  char *c;
  if((i=zline[x][y][1]) != 0)
    destroy(1,i);
  i = rumslot();
  if(i > 13) {
    destroy(1,13);
    i = 13;
    }
  c = crumobj[i];
  *c = id;
  *(c+1) = take;
  *(c+2) = trap;
  *(c+3) = con;
  *(c+4) = nmsg;
  *(c+5) = hide;
  *(c+6) = x;
  *(c+7) = y;
  *(c+8) = vis;
  zline[x][y][1] = i;
  if(*(c+8) == 1 && (rumdata[crum][30] == 1 || zline[x][y][5] == 1) ) 
    drawsq(x,y);
  return(i);
}

int settrap(int num, int trap)
{
crumobj[num][2] = trap;
triglist[0]++;
triglist[num] = trap;
}

int invtrap(char obj)
{
  int i,j,t=0;
  for(i=0;i<20;i++) {
    if(invtrig[i] == obj || obj == 69 || obj == 65 || obj == 63)
      t=1;
    }
  if(t == 0)
    return(0);
  for(j=14;j<25;j++) {
    if(triglist[j] == obj)
      return(1);
    }
  j = 14;
  while(triglist[j] != 0 && j < 25){
    j++;
    }
  if(j == 25)
    return(0);
  triglist[0]++;
  triglist[j] = obj;  /* REMEMBER! the object #s (40-80) are reserved traps!!! */
  return(1);
}

int untrap(int num)
{
  crumobj[num][2] = 0;
  triglist[num] = 0;
  triglist[0]--;
  return(1);
}

int conn(int robj, int room)
{
  crumobj[robj][3] = room;
  return(1);
}

int discon(int robj)
{
  crumobj[robj][3] = 0;
  return(1);
}

int blowup(int x, int y)
{
  char *z = zline[x][y],*c;
  int i;
  for(i=0;i<5;i++) {
    explode(x,y,0);
  }
  if(*(z+2) > 0 || (curmon[0][24] == x && curmon[0][25] ==y) ) {
    destroy(2,*(z+2));
  }
  if(*(z+1) > 0 && crumobj[*(z+1)][0] != 49) {
    c = crumobj[*(z+1)];
    if(*(c+3) != 0 || *(c+2) != 0)
      return(1);
    destroy(1,*(z+1));
  }
  return(1);
}

int getword(char *comp)
{
  int usedline,hold,row,col;
  prnt("-> [Type answer]: ");
  instring(row,col,junk,18,1);
  hold = usedline = 0;
  if(strcmp(junk,comp) != 0) 
    return(0);
  else 
    return(1);
}

int rumslot()
{
  int i;
  for(i=1;i<19;i++){
    if(crumobj[i][0] == 0)
      return(i);
    }
  destroy(1,18);
  rumdata[crum][152] = 0;
  return(18);
}

int makeslot(int numb)
{
  int i;
  if((numb = (numb - slotnum())) <= 0)
    return(1);
  for(i=13;i>13-numb;i--) {
    destroy(1,i);
  }
}

int slotnum()
{
  int i,j=0;
  for(i=1;i<19;i++) {
    if(crumobj[i][0] == 0)
      j++;
    }
  return(j);
} 

int thief(int pc)
{
  if(pc > 3)
    return(0);
  if(curmon[pc][37] > rnd(100))
    return(1);
  else
    return(0);
}

