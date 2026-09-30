#include <stdint.h>

// Unknown function

// This source code is NOT in the game binary and will not work:
/*
          .globl _toggle
.text   
_toggle: link R14,#-4
         clr.l R0
         clr.l R1
         clr.l R2
         move.w 8(R14),R0
         move.w 10(R14),R1
         move.l 12(R14),R8
         mulu #2560,R1
         mulu #8,R0
         add.l R0,R1   
         add.l R1,R8
         clr.l R5
         move.w #15,R5
it:      move.l R8,R9
top:     move.w (R9),R4
         not R4
         move.w R4,(R9)
         move.w 2(R9),R4
         not R4
         move.w R4,2(R9)
         move.w 4(R9),R4
         not R4
         move.w R4,4(R9)
         move.w 6(R9),R4
         not R4
         move.w R4,6(R9)
         addq #8,R9 
         dbf R3,top
         add #160,R8
         dbf R5,it
         unlk R14
         rts
*/

// --- 0  sp
// --- 4  reserved
// --- 8  x
// --- 10 y
// --- 12 unknown
// --- 14 addr - unused?
void toggle(int x, int y, int unknown1, int16_t *addr) {
// In the real source, I find this similar function which MUST be the true source code because 
// the assembly source given makes no sense.

    // 2e40:	4e56 fffc      	linkw %fp,#-4
    // 2e44:	4280           	clrl %d0
    // 2e46:	4281           	clrl %d1
    // 2e48:	4282           	clrl %d2
    // 2e4a:	302e 0008      	movew %fp@(8),%d0
    uint32_t d0 = x;    
    // 2e4e:	322e 000a      	movew %fp@(10),%d1
    uint32_t d1 = y;
    // 2e52:	342e 000c      	movew %fp@(12),%d2
    uint16_t d2 = unknown1;
    // 2e56:	206e 000e      	moveal %fp@(14),%a0
    int16_t *a0 = addr;

    // 2e5a:	c2fc 0500      	muluw #1280,%d1
    d1 *= 1280;

    // 2e5e:	c0fc 0004      	muluw #4,%d0
    d0 *= 4;

    // 2e62:	d280           	addl %d0,%d1
    d1 += d0;

    // d1 = (x * 4) + (y * 1280);

    // 2e64:	d1c1           	addal %d1,%a0
    a0 += d1 / 2; // halved because of 16 bit pointer width

    // 2e66:	4285           	clrl %d5
    // 2e68:	3a3c 0007      	movew #7,%d5
    // loop 8 times
    int32_t d5 = 7;

    do {
      // 2e6c:	2248           	moveal %a0,%a1
      int16_t *a1 = a0;

      // 2e6e:	3602           	movew %d2,%d3
      int16_t d3 = d2; //the unknown value, 0 or 7. This is how many times to loop (1 or 8):

      do {
        // 2e70:	3811           	movew %a1@,%d4
        // 2e72:	4644           	notw %d4
        // 2e74:	3284           	movew %d4,%a1@
        a1[0] = ~a1[0];

        // 2e76:	3829 0002      	movew %a1@(2),%d4
        // 2e7a:	4644           	notw %d4
        // 2e7c:	3344 0002      	movew %d4,%a1@(2)
        a1[1] = ~a1[1];

        // 2e80:	5849           	addqw #4,%a1
        a1 += 2;

        // 2e82:	51cb ffec      	dbf %d3,0x2e70
        d3--;
      } while (d3 >= 0);

      // 2e86:	d0fc 00a0      	addaw #160,%a0
      a0 += 80;
      // ...go to the next scanline

      // 2e8a:	51cd ffe0      	dbf %d5,0x2e6c
      d5--;
    } while (d5 >= 0);
    
    // 2e8e:	4e5e           	unlk %fp
    // 2e90:	4e75           	rts

}

