#ifndef __GEMDEFS_H
#define __GEMDEFS_H

#include <stdint.h>

#include <gemdos.h>
#include <xbios.h>
#include <aes.h>

typedef struct font_hdr
{
   int16_t     font_id;        /* Font number                        0 */
   int16_t     point;          /* Size in points                     2 */
   int8_t      name[32];       /* Name of the font                   4 */
   uint16_t    first_ade;      /* First character in font            36 */
   uint16_t    last_ade;       /* Last character in font             38 */
   uint16_t    top;            /* Distance: Top line    <-> Baseline 40 */
   uint16_t    ascent;         /* Distance: Ascent line <-> Baseline 42 */
   uint16_t    half;           /* Distance: Half line   <-> Baseline 44 */
   uint16_t    descent;        /* Distance: Descent line<-> Baseline 46 */
   uint16_t    bottom;         /* Distance: Bottom line <-> Baseline 48 */
   uint16_t    max_char_width; /* Largest character width            50 */
   uint16_t    max_cell_width; /* Largest character cell width       52 */
   uint16_t    left_offset;    /* Left offset for italic (skewed)    54 */
   uint16_t    right_offset;   /* Right offset for italic (skewed)   56 */
   uint16_t    thicken;        /* Thickening factor for bold         58 */
   uint16_t    ul_size;        /* Width of underline                 60 */
   uint16_t    lighten;        /* Mask for light (0x5555)            62 */
   uint16_t    skew;           /* Mask for italic (0x5555)           64 */
   uint16_t    flags;          /* Various flags:
                                  Set for system font
                                   Bit 1: Set if horizontal offset
                                          table is in use
                                   Bit 2: Set if Motorola format
                                   Bit 3: Set if non-proportional    66 */
   uint8_t     *hor_table;     /* Pointer to horizontal offset table 68 */
   uint16_t    *off_table;     /* Pointer to character offset table  72 */
   uint16_t    *dat_table;     /* Pointer to font image              76 */
   uint16_t    form_width;     /* Width of the font image            80 */
   uint16_t    form_height;    /* Height of the font image           82 */
   struct font_hdr *next_font;     /* Pointer to next font header    84 */
} FONT_HDR;

typedef struct
{
  int16_t  v_planes,               /*   0: # Bit planes (1, 2 or 4)     */
           v_lin_wr,               /*   2: # bytes/scanline             */
           *contrl,
           *intin,
           *ptsin,                 /*  12: Coordinates input            */
           *intout,
           *ptsout,                /*  20: Coordinates output           */
           fg_bp_1,                /*  24: Plane 0                      */
           fg_bp_2,                /*  26: Plane 1                      */
           fg_bp_3,                /*  28: Plane 2                      */
           fg_bp_4,                /*  30: Plane 3                      */
           lstlin;                 /*  32: Draw last pixel of a line    */
                                   /*      (1) or don't draw it (0)     */
  uint16_t ln_mask;                /*  34: Line pattern                 */
  int16_t  wrt_mode,               /*  36: Writing modes                */
           x1, y1, x2, y2;         /*  38: Coordinate                   */
  void     *patptr;                /*  46: Fill pattern                 */
  uint16_t patmsk;                 /*  50: Fill pattern "mask"          */
  int16_t  multifill,              /*  52: Fill pattern for planes      */
           clip,                   /*  54: Flag for clipping            */
           xmn_clip, ymn_clip,
           xmx_clip, ymx_clip,     /*  60: Clipping rectangle           */
                                   /*      Rest for text_blt:           */
           xacc_dda,               /*  64: Set to 0x8000 before text    */
                                   /*      output                       */
           dda_inc,                /*  66: Scaling increment            */
           t_sclsts,               /*  68: Scaling direction            */
           mono_status,            /*  70: Proportional font            */
           sourcex, sourcey,       /*  72: Coordinates in font          */
           destx, desty,           /*  76: Screen coordinates           */
           delx, dely;             /*  80: Width and height of character*/
  uint16_t *fbase;                 /*  84: Pointer to font data         */
  int16_t  fwidth,                 /*  88: Width of font form           */
           style;                  /*  90: Font style effect            */
  uint16_t litemask,               /*  92: Mask for light               */
           skewmask;               /*  94: Mask for italic              */
  int16_t  weight,                 /*  96: Width for bold               */
           r_off,                  /*  98: Italic offset right          */
           l_off,                  /* 100: Italic offset left           */
           scale,                  /* 102: Scaling flag yes/no          */
           chup,                   /* 104: Character rotation angle *10 */
           text_fg;                /* 106: Text foreground colour       */
  void     *scrtchp;               /* 108: Pointer to 2 contiguous      */
                                   /*      scratch buffers              */
  int16_t  scrpt2,                 /* 112: Index in buffer              */
           text_bg,                /* 114: Unused                       */
           copy_tran,              /* 116: --                           */
           (*fill_abort)( void );  /* 118: Tests seedfill               */
} LINEA;

// Not in my library yet, but needed:
void vbl_animation_off();
void vbl_animation_on();
void save_palette();

void linea_init(LINEA **parameter_block, FONT_HDR ***sysfont_pointers);
void linea_textblock_transfer();
void linea_showmouse();
void linea_hidemouse();


#define VBL_LIST ((volatile int16_t (**)())0x4ce)

#endif