#include "globals.h"

#include "f0.h"
#include "f1.h"
#include "f2.h"
#include "f3.h"
#include "f4.h"
#include "f5.h"
#include "f6.h"
#include "f7.h"
#include "f8.h"
#include "f9.h"
#include "f10.h"
#include "f11.h"
#include "f12.h"
#include "f13.h"
#include "f14.h"
#include "f15.h"
#include "f16.h"

// Screen VDI handle
int16_t handle;

int16_t newpal[16] = {
  // Using negative numbers for these hex values is a bit of a wank
  -1912, -120, -1905, -600, -536, -1832, -55, -820, -8, -1793, -311, -1912, -113, -19, -627, 0xfff
};
int16_t oldpal[16];

int16_t pxy[8] = {12, 136, 319-75, 199, 12, 134, 319-75, 197};
int16_t bxy[4] = {12, 197, 319-75, 199};
int16_t lxy[4] = {0, 127, 319-75, 128};
int16_t vxy[4] = {255, 0, 255, 199};

MFDB psrc, pdes;
char *savname[] = {"OUTSIDE.DAT", "DEMON.DAT", "TROND.DAT", "CAVE.DAT", "DDEMON.DAT", "DTROND.DAT", "DCAVE.DAT"};
int fromout = 0, new = 0, lev = 0;
int monster = 0, hold = 0;

// "Police" is a flag that is 0 when the police are not after you, but if set to 1 then you are 
// wanted by the police and will be pursued until your death.
int police = 0;

int usedline = 0, prev = 0, prevy = 0;
int row, col = 2;
int grflist[13];
int winroom;

int (*inverb[])(int pc)  = {i0,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14};
int (*outverb[])(int pc) = {o0,o1,o2,o3,o4,o5,o6,o7,o8,o9,o10,o11,o12,o13,o14};

int winker = 0, winktime = 0, combat = 0;
char *posture[] = {"All Out","Standard","Defensive","Fall Back"};
char *wordmod[] = {"None","Low","Medium","Severe","Dead","Panicked"};
                      /* chance,damage,weight */

uint8_t putbuf[320], specbuf[40];
char monbuf[320],junk[20];
char weapon[][3] = { {0,0,0},                                 /* no weapon */
                     {60,12,10},                              /* broad sword */
                     {60,8,5},                              /* scimitar  */
                     {60,6,5},                                  /* mace */
                     {40,3,1},                                  /* dagger */
                     {-1,12,8}};                                  /* bow */
char *wepname[] = {"","Broadsword","Scimitar","Mace","Dagger","Bow"};
char wepmatx[4][4] = { {0,-40,-40,-40},
                       {40,0,20,-20},
                       {40,-20,0,20},
                       {40,20,-20,0} };
char *statword[] = {"Points","Spell","Good Hand",
                    "Other Hand"," OPTIONS:","Good Hand",
                    "Other Hand","Inventory","  Quit"};
long int saddr;

int16_t fillpic[41][65];

int mode = 0, dismax;
int pursuit[3] = {0, 0, 0};
uint8_t trigval[80][6];
char invtrig[20];
char path[64] = "a:*.SAV";
char *filename = "________.___";
char *spell[] = {
  "Cure","Search","Freeze","Unvenom","Vorpal",
  "Speed","Armor","Disarm","Bolt","Thief",
  "Fear","Fireball","Blink","Reveal","Heal",
  "Prism","Block","Charm","Wallfire","Destroy",
  "Summon","Death","Life","Teleport",
  "CANCEL"
};

// Spell magic points costs I think
char spelunit[] = {0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5};
char *name[] = {
  "goblin","skeleton","spider","firewolf","ghost","thrang", "ghoul","snake","vampire",
  "gork","Melkthrop","merchant", "bat","Eirik","guard","pedestrian","Mage Leveth",
  "orc","crusher","flamer","rat","bartender","rogue"
};


// If it's curmon[0]:
//   44 - number of arrows owned but also "bane"?
//   49 - gold amount

// If it's curmon[5..11]:
//   31 - intell routine
//   41 - base chance 1 attack
//   42 - base chance 2 attack
//   43 - base chance 3 attack
//   44 - bane
//   45 - poison mode
//   50 - physical strength

// It's almost impossible to move curmon[] from an array of chars to a structure.
// The author has done lots of very strange things such as have pointers that might
// be to different arrays at different times, and the offsets still have to work.
// Goodbye strongly typed language.
// I will leave this one until last.

uint8_t curmon[12][60] = {
  // curmon 0 - the player. Only 15 default bytes populated out of 60.
  {1, 4, 50, 0, 81, 5, 60, 8, 0, 0, 0, 4, 3, 90, 10},
};

uint8_t invnpc[4][20];

// Monster types
// see struct permon_t in header
permon_t permon[] = {
                  {0},
  /* spider */    {1,0,20,2,83,6,100,0,0,0,0,4,2,95,0,13,1,30,0,0,1,0,0},
  /* skelly */    {1,0,10,1,85,6,100,0,0,0,0,5,0,101,0,8,0,50,0,0,1,0,0},
  /* snyke  */    {1,0,50,7,87,6,60,4,100,0,0,5,2,101,0,13,2,40,10,0,1,0,0},
  /* dogs   */    {1,0,8,3,89,6, 100,0,0,0,0,3,0,90,0,8,0,30,0,0,1,0,0},
  /* goblins */   {1,0,14,0,91,10,20,6,80,4,100,4,2,90,0,10,0,30,50,60,1,0,0},
  /* bats   */    {1,0,10,12,93,8,20,4,100,0,0,2,0,97,0,5,0,30,60,0,1,0,0},
  /* thrang */    {1,0,70,5,95,9,12,0,0,0,0,6,0,101,42,8,0,0,0,0,1,0,0},
  /* Babaran */   {1,0,50,13,97,0,0,0,0,0,0,3,3,101,0,20,0,0,2,0},
  /* Alph? */     {0},
  /* Millie */    {0},
  /* spooks */    {1,0,20,4,103,6,40,0,40,4,100,2,0,100,0,0,0,40,0,40,1,52,1},
  /* ghouls */    {1,0,20,6,105,10,40,0,0,6,100,5,2,100,0,12,1,40,0,60,1,0,0},
  /* vampire */   {1,0,60,8,107,20,60,20,80,10,100,3,0,90,0,20,0,40,60,20,1,56,1},
  /* gork */      {1,0,80,9,109,40,50,30,100,0,0,3,4,100,0,20,1,30,70,0,1,47,1},
  /* merchant */  {1,0,10,11,108,0,0,0,0,0,0,6,0,0,0,1,0,0,0,0,5,0,0},
  /* Melkthrop */ {1,0,50,10,111,22,16,12,0,0,0,4,100,110,100,5,0,0,0,0,1,0,1},
  /* cop */       {1,0,20,14,106,10,40,6,80,4,100,4,1,85,0,8,0,30,50,30,1,0,0},
  /* pedest */    {1,0,10,15,110,0,0,0,0,0,0,5,0,10,0,2,0,0,0,0,6,0,0},
  /* Lev */       {1,0,100,16,112,12,16,22,0,0,0,6,100,110,126,2,0,0,0,0,7,0,1},
  /* orc */       {1,0,18,17,102,24,30,16,70,12,100,4,2,96,0,12,0,30,60,10,1,0,0},
  /* crusher */   {1,0,60,18,113,20,100,0,0,0,0,3,4,101,0,20,0,50,0,0,1,0,1},
  /* flamer */    {1,0,20,19,114,12,0,0,0,0,0,3,0,101,50,0,0,0,0,0,1,0,1},
  /* rat */       {1,0,12,20,119,10,20,0,0,6,100,2,0,101,0,4,0,50,0,80,1,0,0},
  /* barten */    {1,0,20,21,115,8,100,0,0,0,0,4,1,95,0,10,0,40,0,0,7,0,0},
  /* rogue */     {1,0,35,22,120,20,30,16,70,12,100,3,4,96,0,12,0,60,60,60,7,0,0}
};

// Storekeepers
int8_t selllist[][4] = {
  // 0 = item that they sell
  // 1 = minimum price they will accept
  // 2 = 
  // 3 = 0 if friendly, 2 if they like you, -1 if you stole from them
  {81, 3, 5, 0},
  {42, 10, 20, 0},
  {69, 25, 35, 0},
  {45, 20, 30, 0}
};

char weight[41] = {0,-1,4,7,10,17,-1,18,8,-1,-1,20,-1,-1,16,8,8,-1,7,8,
                   -1,-1,16,20,-1,-1,-1,8,-1,14,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                   -1,12};
char *mod[] = {"0","L","M","H","X","P","C"};
int16_t crudbuf[65];
char *stuff[] = {"","                    "};
char *errmsg[] = {"",
                   "[1][That path is blocked!|     Try again!][OK]",
                   "[1][That's the same square!|      Try again!][OK]",
                   "[1][ That square can't be | seen. Try again!][OK]",
                   "[1][ There is nothing to  | examine. Try again!][OK]",
                   "[1][  The object must be  | adjacent. Try again!][OK]",
                   "[1][ There is nothing to  |  take.  Try again!][OK]",
                   "[1][ The inventory is too | full.  Drop something.][OK]",
                   "[1][ There is no character|  there. Try again!][OK]", 
                   "[1][  The target must be  | adjacent. Try again!][OK]",
                   "[1][ The inventory is empty.|  Try something else!][OK]",
                   "[1][ The location must be | adjacent. Try again!][OK]",
                   "[1][ The object | is closed!][OK]",
                   "[1][  Insufficent spell  |  units!  Try again!][OK]",
                   "[1][ There is nothing to | disarm! Try again!][OK]",
                   "[1][ There is no exit! |   Try again!][OK]",
                   "[2][ Will the object  |    be taken? ][Yes| No]",
                   "[2][ Do you wish to  ][Abort|Quit|File]",
                   "[1][ Read/Write failure. ][OK]",
                   "[2][ Do you wish to      ][Stop|Save|Load]",
 /* #20 errmsg */  "[1][  No weapon in hand! ][OK]",
                   "[1][ A bow must be FIRED!][OK]",
                   "[1][  A bow requires  | two free hands!][OK]",
                   "[1][ There is no   | object there!][OK]",
                   "[1][ This object is   | immoveable!][OK]",
                   "[1][ There is nothing | to rook/snoop!][OK]",
                   "[1][   There are no   | torches left!][OK]",
                   "[1][ There is nothing |   in hand!][OK]",
/* 28 */           "[1][ Insufficient   | magic ability!][OK]",
                   "[1][  Hands are full! ][OK]",
                   "[1][ No bow or arrows ][OK]",
"[2][  Which hand's object? ][Good |Other]",
"[1][ Insufficient Disk Space! |    Fatal Error! | Program Shutdown!][OK]",
"[1][ The attempted restore does |  NOT match current dungeon! ][OK]"};

/* target - domsg index - speltime */
char spelinfo[24][3] = {
  {3, 3, 6 },  /* cure light wounds */
  {0, 0, 6 },  /* search */
  {1, 3, 6 },  /* freeze */
  {3, 3, 6 },  /* unvenom */
  {3, 3, 6 },  /* vorpal */
  {3, 3, 6 },  /* speed */
  {3, 3, 6 },  /* armor */
  {2, 2, 6 },  /* disarm */
  {1, 3, 6 },  /* bolt */
  {2, 2, 6 },  /* unlock */
  {1, 3, 6 },  /* fear */
  {1, 3, 8 },  /* fireball */
  {3, 3, 8 },  /* blink */
  {0, 0, 10 }, /* reveal */
  {3, 2, 15 }, /* heal */
  {1, 3, 10 }, /* prism */
  {2, 2, 6 },  /* block */
  {1, 3, 10 }, /* charm */
  {0, 3, 10 }, /* firewall */
  {2, 2, 8 },  /* destroy */
  {0, 1, 20 }, /* summon */
  {1, 3, 8 },  /* death */
  {2, 3, 30 }, /* life */
  {0, 0, 10}   /* teleport */
};

char *com[] = {"the ",
               "  place",
               "  object",
               "  target",
               "   spot",
               "   exit",
               "   item",
               "   item"};
char fname[] = "grafx.dat";

// Player name.
char pname[20];

/* load the collection of trees into memory */
char *verblist[] = {"Cast ","Examine","Drag/Eat","Search","Take/Drop","Wait",
                    "X-it","Rook/Snoop","Fire Arrow","Burn/Snuff","Give",
                    "Voice","Quit/File"};

uint8_t zline[16][8][7];                    

room_data_t new_rumdata[80];
uint8_t (*new_rumdata_as_array)[80][sizeof(room_data_t)] = (uint8_t (*)[80][sizeof(room_data_t)])&new_rumdata;
uint8_t rumdata[80][157];

room_zero_data_t room_zero;

// Per room (157 bytes):
// - 0   = oldx and oldy (4 bits each) - x in lower, y in upper
// - 1   = another set of xy coords

// - 16  = background tile 1 pattern
// - 17  = background tile 1 style
// - 18  = background tile 1 colour
// - 19  = background tile 2 pattern
// - 20  = background tile 2 style
// - 21  = background tile 2 colour

// - 30  = room has background graphics if non-zero
// - 31  = Object data for object 1 in room (9 bytes)
// - 40  = Object data for object 2 in room (9 bytes)
// - 49  = Object data for object 3 in room (9 bytes)
// - 58  = Object data for object 4 in room (9 bytes)
// - 67  = Object data for object 5 in room (9 bytes)
// - 76  = Object data for object 6 in room (9 bytes)
// - 85  = Object data for object 7 in room (9 bytes)
// - 94  = Object data for object 8 in room (9 bytes)
// - 103 = Object data for object 9 in room (9 bytes)
// - 112 = Object data for object 10 in room (9 bytes)
// - 121 = Object data for object 11 in room (9 bytes)
// - 130 = Object data for object 12 in room (9 bytes)
// - 139  = Object data for object 13 in room (9 bytes)

uint8_t triglist[25];

// This was declared as [18][9] but is accessed as [19][9] in trapaux.c
// crumobj[][] is used to hold data about objects in the current room
// hence "current room objects" -> crumobj
uint8_t crumobj[19][9];
// The first dimension is the objects in the room.
// 0 - unknown
// 1..13  - 13 room objects like furniture copied from rumdata[]
// 14..18 - Blanked out to zeroes on room entry

// Each 9-byte object structure is copied from rumdata:
// 0      - object ID
//          ...object ID is an index into obj[] for names, for example
//          ...and into bitmap data too
// 3 - (for doors) the room number that it leads to
// 6 - object X coord
// 7 - object Y coord
// 8 - if the object is visible

char *att[][4] = {
  {"","","",""},
  /* spidey*/        {"","bite/all","",""},
  /* skelly*/        {"","blade/all","",""},
  /* snake */        {"","crush/all","bite/std","bite/def"},
  /* dogs */         {"","bite/all","",""},
  /* goblin*/        {"","mace/all","mace/std","mace/def"},
  /* bat*/           {"","bite/all","claw/std",""},
  /* thrang*/        {"","","",""},
  /* Bab */          {"","","",""},
  /* */              {"","","",""},
  /* */              {"","","",""},
  /* spook */        {"","drain/all","","drain/def"},
  /* ghoul */        {"","bite/all","","claw/def"},
  /* vampire */      {"","bite/all","strike/std","glare/def"},
  /* gork */         {"","spray/all","slash/std",""},
  /* merchant */     {"","","",""},
  /* melkthrop */    {"","","",""},
  /* cop */          {"","club/all","club/std","club/def"},
  /* ped */          {"","","",""},
  /* Lev */          {"","","",""},
  /* orc */          {"","slash/all","slash/std","slash/def"},
  /* crush */        {"","crush/all","",""},
  /* flamer */       {"","","",""},
  /* rat */          {"","bite/all","","bite/def"},
  /* bar */          {"","club/all","",""},
  /* rogue */        {"","blade/all","blade/std","blade/def"}
};

char *monmsg[] = {
"-> The goblin, a look of inhuman brutality gripping its face, \
storms toward %s swinging a mace and chanting 'Melkthrop! Melkthrop!'.",
"-> The skeleton lurches crazily across the floor, brandishing its \
rusty scimitar at %s, an unholy light emanating from its empty eyesockets.",
"-> The spider exudes a rank odor, a mixture of decomposing flesh and \
venom.  Its seven red eyes gleam malevolently as it scuttles across the \
floor toward %s.",
"-> The firewolf, its fangs bared in a vicious snarl, bounds across the floor \
toward %s.",
"-> The ghost wails an ethereal cry of insatiable hunger for life as it floats \
toward %s.",
"-> The thrang, a dessicated corpse clad in rotting regal garments, glares at \
%s with glowing red eyes set in sunken sockets. Once a great mage, it has \
passed into a state of undead through arcane and evil magics.",
"-> The ghoul slavers, a charnel house stench reeking from its mottled flesh, \
as it staggers after %s.",
"-> The snake writhes in a reptilian frenzy as it slithers after %s.",
"-> The vampire, its eyes glowing in a once-patrician face, advances toward \
%s, exuding a paralyzing charm.",
"-> The enormous gork, a constant stream of poisonous saliva coursing from its \
mouth and its talons flicking, advances after %s. Gorks are renowned for their\
 resistance to magic of all forms.",
"-> Melkthrop, a major but somewhat small-minded demon from the lower planes \
of Hell, prepares to hurl a spell at %s.",
"-> The merchant, a look of avarice apparent despite an unctuous smile, \
peers hopefully at %s's goldpouch.",
"-> The screeching bat, slavering and gnashing its sharp ratlike teeth, \
flutters after %s!",
"-> Eirik Bloodaxe, heir to the western provinces of Nordheim, stands stolidly \
surveying the room. His incredible strength is already a legend throughout \
Avecta, as is his loyalty to friends and his hatred toward foes.",
"-> The taciturn guard, armed with a large truncheon, comes directly at \
%s!",
"-> The pedestrian is busy about some urgent affair.",
"",
"-> The orc, a definite bruiser, ambles toward %s, confidently brandishing \
his tulwar!",
"-> The enormous crusher, gigantic and simple-minded, has only one thought: \
to heave his bulk on top of %s, inflicting vast damage!",
"-> The flamer, formerly sole tenant of the cavern, is a sentient fire. \
It hisses and crackles at %s!",
"-> The rat, its sharp teeth snapping, rears on its backlegs, hissing and \
spitting at %s!",
"-> The bartender has a rosy glow from sampling too much of his wares.  Closer \
examination reveals a fair amount of strength beneath an amiable surface.",
"-> The rogue, noticing %s's interest, issues a gesture of contempt."};

// The number of the current room and the index into rumdata[]
int crum;

// Sprite data for 200 sprites, each 16x16 (128 bytes) with 2 wasted bytes
int16_t bitmap[200][65];

// The number of objects in the current room - never used.
//int objnum;

int storbuf[12][130], num,time=0;
int firebuff[130];
int16_t contrl[12], intin[128], intout[128], ptsin[128], ptsout[128];
int16_t work_in[11], work_out[57];
char *obj[] = {
  "","door","chair","bed","desk","stove","altar","bookshelf",
  "table","fireplace","statue","garbage heap","trapdoor",
  "tapestry","throne","urn","chest","hole",
  "barrel","skeleton",
  "stairs which go down","stairs which go up",
  "laboratory table","bathtub",
  "lamp post","ectoplasmic converter","fountain","bench",
  "iron gate","table with food","bush",
  /*31*/
  "orc machine","X-shaped rune","sign","announcement",
  "Red Lion","fire","wrecked wagon","","","dead Eirik",
  "broadsword","scimitar","mace","dagger","bow",
  "bone key","button","old key","rune","black key",
  "tongs","rod","bottle","poster","potion",
  "garlic bud","device","bag","note","note",
  "old bone","large book","vile key","magic tome","mask",
  "black ball","note","note","power vial","bag",
  "","","","","",
  "","","","","seekshard","torch","","","",""
};

int outside, dungeon;

char eats[] = {
  0,0,0,0,0,
  0,1,1,1,0,
  1,0,0,1,0,
  1,1,0,0,0,
  0,0,0,0,0,
  0,0,0,0,1,
  1,0,0,0,0,
  0,0,0,0,0,
  0
};


// Screen logical address
uint16_t *addr;