
#include "globals.h"
#include "startaux.h"
#include "dorep.h"
#include "gemdefs.h"
#include "storsc.h"
#include "caux.h"
#include "cinput.h"
#include "start.h"

#include "f5.h"

int i9(int pc)
{
  uint8_t *c = curmon[pc];
  uint8_t *z;
  char *w = (pc == 0 ? pname : name[*(c+3)]);
  int i,j,ret,quit = 0;
  int16_t x, y;

if(pc > 0 && pc < 4 && curmon[0][15] != 9) {
  prnt("-> %s says 'I cannot abandon you!  I will stay with you!'", w, NULL, NULL, NULL, NULL, NULL);
  return(0);
}

if(pc > 0) {
  *(c+15) = 8;
  *(c+16) = curmon[0][16] + 1;
  return(1);
}

if(outside) {
  x = *(c+24);
  y = *(c+25);
  for(i= -1;i < 2 ;i++) {
     if(quit)
        break;
     for(j = -1; j < 2;j++) {
        if(quit)
          break;
        if(zline[x+i][y+j][0] == 2 && i*j == 0 && (i != 0 || j != 0)) { 
           if(i == -1 ) {
              *(c+24) = 5;
              *(c+30) -= 1;
              prnt("-> %s goes west...", w, NULL, NULL, NULL, NULL, NULL);
              quit = 1;
              }
           if(i == 1) {
              *(c+24) = 1; 
              *(c+30) += 1;
              prnt("-> %s goes east...", w, NULL, NULL, NULL, NULL, NULL);
              quit = 1;
              }
           if(j == -1) {
              *(c+25) = 5;
              *(c+30) -= 5;
              prnt("-> %s goes north...", w, NULL, NULL, NULL, NULL, NULL);
              quit = 1;
              }
           if(j == 1) {
              *(c+25) = 1;
              *(c+30) += 5;
              prnt("-> %s goes south...", w, NULL, NULL, NULL, NULL, NULL);
              quit = 1;
              }
           if(quit) {
             clrinp();
             *(c+15) = 8;
             *(c+16) = 1;
             return(1);
             }
           }
         }
      }
    }
  if(outside && !quit) {
    error(15);
    return(0);
  }
  domsg(5);
  header(w);
  winker = pc+1;
  sgetxy(&x,&y,0,0,0,&ret);
  winker = 0;
  undomsg();
  clrinp();
  if(!adjac(0,x,y)) {
    error(11);
    return(0);
  }
  z = zline[x][y];
  if(crumobj[*(z+1)][3] == 0) {
    error(15);
    return(0);
  }
  *(c+7) = *(c+8) = 0;      
  *(c+9) = *(z+1);
  *(c+10) = crumobj[*(z+1)][3];
  *(c+15) = 9;
  *(c+16) = 6;
  return(1);
}

int o9(int pc)
{
  uint8_t *c = curmon[pc];
  int x = *(c+24), y = *(c+25);
  uint8_t *z = zline[x][y];
  
  if(pc == 0 && crumobj[*(c+9)][5] == -2) {
      prnt("-> %s finds the exit is locked!", pname, NULL, NULL, NULL, NULL, NULL);
      *(c+30) = crum;
      return(1);
      }
  *(z+2) = 0;
  if(*(z+5) > 0 || rumdata[*(c+30)][30]) {
    Vsync();
    storsc(storbuf[pc],16*x,16*y,1,addr);
    }
  *(c+30) = *(c+10);
  if(pc != 0) {
    z = zline[*(c+26)][*(c+27)];
    *(c+30) = crumobj[*(z+1)][3];
    if(mode)
        dorep();
    return(-1);
    }
  if(pc == 0) {
    *(c+34) = 0;
    if(mode)
      prnt("-> %s flees!", &pname[0], NULL, NULL, NULL, NULL, NULL);
    *(c+18) = 8;
    }
  return -1;
}

