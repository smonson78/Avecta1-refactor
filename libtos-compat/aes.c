#include <stdint.h>
#include <stdio.h>
#include "aes.h"
#include "tos.h"
#include "libc.h"

#include "st_globals.h"
#include "gfx.h"

// static AESPB c;
// static VDIPB v;

// this is the AES pointer block
int16_t global[15];
int16_t control[7];
int16_t int_in[16];
int16_t int_out[10];
void *addr_in[8];
void *addr_out[2];

// This is the VDI pointer block
int16_t vdi_control[12];

// Make it easier to access iptr and iptr2 which are 32-bit pointers stored in 2 array members
#define iptr(x) do{ vdi_control[7] = ((uint32_t)(x)) >> 16; vdi_control[8] = ((uint32_t)(x)); }while(0)
#define iptr2(x) do{ vdi_control[9] = ((uint32_t)(x)) >> 16; vdi_control[10] = ((uint32_t)(x)); }while(0)

// int16_t vdi_intin[1024];
// int16_t vdi_intout[512];
// int16_t vdi_ptsin[1024];
// int16_t vdi_ptsout[256];

int16_t screen_phandle;
int16_t screen_vhandle;

int16_t screen_rez;
int16_t colour_screen;
int16_t x_max;
int16_t y_max;

void set_screen_attr()
{
}

#if 1
// none of this is explained properly in tos.hyp

void aes()
{
}


void vdi()
{
}


int16_t appl_init()
{
   return 0;
}

int16_t crys_if(int16_t opcode)
{
   return 0;
}

void vq_extnd (int16_t handle, int16_t owflag, int16_t *work_out)
{
}

void vq_mouse(int16_t handle, int16_t *pstatus, int16_t *x, int16_t *y)
{
}

void v_opnvwk(int16_t *work_in, int16_t *handle, int16_t *work_out)
{
}

void v_clswk(int16_t handle)
{
}

void v_clsvwk(int16_t handle)
{
}

void v_contourfill(int16_t handle, int16_t x, int16_t y, int16_t index)
{
}

// This could be wrong. Hard to find documentation on it
void v_justified(int16_t handle, int16_t x, int16_t y, int8_t *string, int16_t length, 
   int16_t word_space, int16_t char_space)
{
}

// Polyline
void v_pline(int16_t handle, int16_t count, int16_t *pxyarray)
{
}

int16_t graf_mouse(int16_t gr_monumber, MFORM *gr_mofaddr)
{
   return 0;
}

int16_t graf_handle(int16_t *gr_hwchar, int16_t *gr_hhchar,
	int16_t *gr_hwbox, int16_t *gr_hhbox)
{
   return 0;
}

int16_t appl_exit()
{
   return 0;
}

int16_t appl_write(int16_t ap_wid, int16_t ap_wlength, void *ap_wpbuff) 
{
   return 0;
}

int16_t form_alert(int16_t fo_adefbttn, const char *fo_astring)
{
   printf("Form alert: %s\n", fo_astring);
   return 0;
}

int16_t form_dial(int16_t fo_diflag, int16_t fo_dilittlx, int16_t fo_dilittly, int16_t fo_dilittlw,
   int16_t fo_dilittlh, int16_t fo_dibigx, int16_t fo_dibigy, int16_t fo_dibigw, int16_t fo_dibigh)
{
   return 0;
}

int16_t evnt_mesag(int16_t *msg)
{
   return 0;
}

int16_t evnt_multi (int16_t ev_mflags,  int16_t ev_mbclicks,
  int16_t ev_mbmask,  int16_t ev_mbstate,
  int16_t ev_mm1flags, int16_t ev_mm1x,
  int16_t ev_mm1y, int16_t ev_mm1width,
  int16_t ev_mm1height, int16_t ev_mm2flags,
  int16_t ev_mm2x, int16_t ev_mm2y,
  int16_t ev_mm2width, int16_t ev_mm2height,
  int16_t *ev_mmgpbuff, int16_t ev_mtlocount,
  int16_t ev_mthicount, int16_t *ev_mmox,
  int16_t *ev_mmoy, int16_t *ev_mmbutton,
  int16_t *ev_mmokstate, int16_t *ev_mkreturn,
  int16_t *ev_mbreturn)
{
   return 0;
}

void vr_recfl(int16_t handle, int16_t *pxyarray)
{
}

void v_bar(int16_t handle, int16_t *pxyarray)
{
}

void v_ellipse(int16_t handle, int16_t x, int16_t y, int16_t xradius,
    int16_t yradius)
{
}

void v_gtext(int16_t handle, int16_t x, int16_t y, const char *string)
{
}

void v_rvon(int16_t handle) {
   GLOBAL_LOCK();
   globals.video.reverse_video = 1;
   GLOBAL_UNLOCK();
}

void v_rvoff(int16_t handle) {
   GLOBAL_LOCK();
   globals.video.reverse_video = 0;
   GLOBAL_UNLOCK();   
}

void v_eeol(int16_t handle) {
}

void v_eeos(int16_t handle) {
   // Erase to end of screen
   GLOBAL_LOCK();

   // To the end of the line (may not be full screen width)
   filled_rect(globals.video.x_text * 8, globals.video.y_text * 8,
      320 - (globals.video.x_text * 8), 8,
      globals.video.current_bgcolour);

   // From there to the end of the screen
   if (globals.video.y_text < 24) {
      int start_line = (globals.video.y_text * 8) + 8;
       filled_rect(0, start_line,
          320, 200 - start_line,
          globals.video.current_bgcolour);
   }

   GLOBAL_UNLOCK();
}

// Move the cursor to the current row and column
void vs_curaddress (int16_t handle, int16_t row, int16_t column) {
   //printf("vs_curaddress\n");
   GLOBAL_LOCK();
   globals.video.x_text = column - 1;
   globals.video.y_text = row - 1;
   GLOBAL_UNLOCK();
}

int16_t vswr_mode(int16_t handle, int16_t mode)
{
   return 0;
}

// Set palette colour
void vs_color(int16_t handle, int16_t color_index, int16_t *rgb_in)
{
   //printf("vs_col() %d to 0x%03x\n", color_index, *rgb_in);
   GLOBAL_LOCK();   
   globals.video.palette[color_index] = *rgb_in;
   GLOBAL_UNLOCK();
}

// Set fill colour
int16_t vsf_color(int16_t handle, int16_t color_index)
{
   return 0;
}

int16_t vst_color(int16_t handle, int16_t color_index)
{
   return 0;
}

int16_t vsf_interior(int16_t handle, int16_t style)
{
   return 0;
}

void vsf_udpat(int16_t handle, int16_t *pfill_pat, int16_t planes)
{
}

// Set clipping rectangle
void vs_clip(int16_t handle, int16_t clip_flag, int16_t *pxyarray)
{
}

void vro_cpyfm(int16_t handle, int16_t vr_mode, int16_t *pxyarray, MFDB *psrcMFDB, MFDB *pdesMFDB)
{
}

// Get fill attributes
void vqf_attributes(int16_t handle, int16_t *attrib)
{
}

int16_t vsf_style(int16_t handle, int16_t style_index)
{
    return 0;
}

int16_t vsf_perimeter(int16_t handle, int16_t per_vis)
{
   return 0;
}

int16_t rsrc_load(const char *re_lpfname)
{
   return 0;
}

int16_t rsrc_gaddr(int16_t re_gtype, int16_t re_gindex, OBJECT **gaddr)
{
   return 0;
}

int16_t rsrc_free()
{
   return 0;
}

void v_circle(int16_t handle, int16_t x, int16_t y, int16_t radius)
{
}

int16_t menu_bar(OBJECT *me_btree, Menu_Operation me_bshow)
{
   return 0;
}

int16_t menu_tnormal(OBJECT *me_ntree, int16_t me_ntitle, int16_t me_nnormal)
{
   return 0;
}

// A convenience function to fill in an OBJECT structure in one line.
// FIXME: move this to aes_object.c
void new_object(OBJECT *o, uint16_t type, void *spec, uint16_t x, uint16_t y,
		uint16_t width, uint16_t height) {
}
#endif

int16_t fsel_input(char *fs_iinpath, char *fs_iinsel, int16_t *fs_iexbutton) {
   return 0;
}
