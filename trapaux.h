#ifndef __TRAPAUX_H
#define __TRAPAUX_H

int destroy(int type, int num);
int empty(int num);
int make(int id, int take, int trap, int con, int nmsg, int hide, int x, int y, int vis);
int settrap(int num,int trap);
int invtrap(char obj);
int untrap(int num);
int conn(int robj, int room);
int discon(int robj);
int blowup(int x, int y);


int getword(char *comp);
int rumslot();
int makeslot(int numb);
int slotnum();
int thief(int pc);


#endif