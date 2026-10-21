#include <stdint.h>
#include <stdio.h>
#include "aes.h"
#include "tos.h"
#include "libc.h"

#include "sdl_graph.h"
#include "st_globals.h"
#include "gfx.h"

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

int16_t screen_phandle;
int16_t screen_vhandle;

int16_t screen_rez;
int16_t colour_screen;
int16_t x_max;
int16_t y_max;

// VDI does a stupid palette translation
int get_vdi_colour(int index) {
   switch (index) {
      case 1:
         return 15;
      case 4:
      case 12:
         return index;
      case 2:
      case 3:
      case 8:
      case 9:
      case 10:
      case 11:
         return index - 1;
      case 5:
      case 13:
         return index + 1;
      case 6:
         return index - 3;
      case 7:
      case 15:
         return index - 2;
      case 14:
         return 4;
   }
   return 0;
}

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
   GLOBAL_LOCK();
   // Assume double-size pixels
   *x = globals.x_mouse / 2;
   *y = globals.y_mouse / 2;
   *pstatus = globals.mouse_buttons;
   GLOBAL_UNLOCK();
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

   // Example format [2][ Do you wish to  ][Abort|Quit|File]
   // First number, icon: 0 - none, 1 - !, 2 - ?, 3 - stop sign
   int icon = 2;
   char message[1024];
   const char *p = fo_astring;

   // Find icon number
   while (*p == ' ') {
      p++;
   }
   // Skip [
   p++;
   icon = *p++ - '0';
   // printf("Icon number is %d\n", icon);
   // Skip ] character
   p++;
   // Skip [ character
   p++;
   int c = 0;
   while (c < 1023 && *p != ']') {
      if (*p == '|') {
         message[c] = '\n';
      } else {
         message[c] = *p;
      }
      c++;
      p++;
   }
   // Terminating NUL
   message[c] = '\0';
   // printf("Message is %s\n", message);
   
   // Skip ] character
   p++;
   // Skip [ character
   p++;

   // Get buttons

   // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Alert", fo_astring, NULL);

   // Let's assume there are 5
   c = 0;
   int name_pos = 0;
   char button_names[5][80];
   SDL_MessageBoxButtonData buttons[5];

   while (c < 5 && *p != ']' && *p != '\0') {
      if (*p == '|') {
         button_names[c][name_pos] = '\0';
         // printf("Button %d name is %s\n", c, button_names[c]);

         buttons[c].flags = 0;
         buttons[c].buttonID = c + 1;
         buttons[c].text = button_names[c];
         c++;
         name_pos = 0;
      } else {
         button_names[c][name_pos++] = *p;
      }

      p++;
   }
   // Terminate last button name
   button_names[c][name_pos] = '\0';
   buttons[c].flags = 0;
   buttons[c].buttonID = c + 1;
   buttons[c].text = button_names[c];
   // printf("Button %d name is %s\n", c, button_names[c]);


   const SDL_MessageBoxData messageboxdata = {
      icon == 2 ? SDL_MESSAGEBOX_INFORMATION : SDL_MESSAGEBOX_WARNING,               // Flags (INFORMATION, WARNING, ERROR)
      NULL,                                     // Parent window (NULL means standalone)
      "Action Required",                          // Dialog Title (UTF-8 string)
      message,     // Dialog Message text
      c + 1,                   // Total number of buttons
      buttons,                                  // Pointer to button array
      NULL                              // Pointer to color scheme
   };

   int buttonid;
   if (!SDL_ShowMessageBox(&messageboxdata, &buttonid)) {
      SDL_Log("Error displaying message box: %s", SDL_GetError());
      SDL_Quit();
      return 1;
   }

   // buttonid has the button selected or -1 for closed without a choice

   // Return 1, 2, 3 etc for button selected

   return buttonid;
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
   // printf("vr_recfl(style %d)\n", globals.video.interior_fill_pattern);
   GLOBAL_LOCK();
   switch (globals.video.interior_fill_pattern) {
      case 0: 
         // Hollow
         printf("(vr_recfl unhandled style 0)\n");
         break;
      case 1: 
         // Solid
         filled_rect(pxyarray[0], pxyarray[1], 
            pxyarray[2] - pxyarray[0] + 1, pxyarray[3] - pxyarray[1] + 1, 
            get_vdi_colour(globals.video.current_2d_colour));
         break;
      case 2: 
         // Pattern
         printf("(vr_recfl unhandled style 2)\n");
         break;
      case 3: 
         // Hatch
         printf("(vr_recfl unhandled style 3)\n");
         break;
      case 4: 
         // User-defined style
         pattern_rect(pxyarray[0], pxyarray[1], 
            pxyarray[2] - pxyarray[0] + 1, pxyarray[3] - pxyarray[1] + 1, 
            (uint16_t *)globals.video.udpat);
         break;
   }
  
   GLOBAL_UNLOCK();
   // printf("vr_recfl() done\n");
}

// Mostly the same as vr_recfl
void v_bar(int16_t handle, int16_t *pxyarray)
{
   // Draw 2D filled bar
   vr_recfl(handle, pxyarray);
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
   // Erase to end of line
   GLOBAL_LOCK();
   // printf("v_eeol()\n");
   // To the end of the line (may not be full screen width)
   filled_rect(globals.video.x_text * 8, globals.video.y_text * 8,
      320 - (globals.video.x_text * 8), 8,
      globals.video.current_bgcolour);
   // printf("v_eeol() done\n");

   GLOBAL_UNLOCK();
}

void v_eeos(int16_t handle) {
   // Erase to end of screen
   GLOBAL_LOCK();

   // printf("v_eeos()\n");
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
   // printf("v_eeos() done\n");

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
   GLOBAL_LOCK();   
   globals.video.palette[color_index] = *rgb_in;
   GLOBAL_UNLOCK();
}

// Set fill colour
int16_t vsf_color(int16_t handle, int16_t color_index)
{
   GLOBAL_LOCK();
   globals.video.current_2d_colour = color_index;
   GLOBAL_UNLOCK();
   return 0;
}

// Set text colour
int16_t vst_color(int16_t handle, int16_t color_index)
{
   GLOBAL_LOCK();
   // This is only supposed to apply to VDI calls
   //globals.video.current_colour = color_index;
   GLOBAL_UNLOCK();
   return 0;
}

int16_t vsf_interior(int16_t handle, int16_t style)
{
   GLOBAL_LOCK();
   globals.video.interior_fill_pattern = style;
   GLOBAL_UNLOCK();
   return 0;
}

void vsf_udpat(int16_t handle, int16_t *pfill_pat, int16_t planes)
{
   GLOBAL_LOCK();
   for (int i = 0; i < planes * 16; i++) {
      globals.video.udpat[i] = pfill_pat[i];
   }
   globals.video.udpat_planes = planes;
   GLOBAL_UNLOCK();
}

// Set clipping rectangle
void vs_clip(int16_t handle, int16_t clip_flag, int16_t *pxyarray)
{
   printf("vs_clip(%d,%d - %d,%d)\n", pxyarray[0], pxyarray[1], pxyarray[2], pxyarray[3]);
}

// Copy onscreen rectangle 
void vro_cpyfm(int16_t handle, int16_t vr_mode, int16_t *pxyarray, MFDB *psrcMFDB, MFDB *pdesMFDB)
{
   // printf("vro_cpyfm()\n");
   GLOBAL_LOCK();
   copy_rect(pxyarray[0], pxyarray[1], 
      pxyarray[2] - pxyarray[0] + 1, 
      pxyarray[3] - pxyarray[1] + 1,
      pxyarray[4], pxyarray[5]
   );
   GLOBAL_UNLOCK();
   // printf("vro_cpyfm() done\n");
}

// Get fill attributes
void vqf_attributes(int16_t handle, int16_t *attrib)
{
}

int16_t vsf_style(int16_t handle, int16_t style_index)
{
   // printf("vsf_style(%d)\n", style_index);   
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
   // This is rather complicated because we can only show the file selector from the main thread.
   // Therefore we will send an event queue message to show the dialog box

   printf("fsel_input(%s, %s)\n", fs_iinpath, fs_iinsel);
   GLOBAL_LOCK();
   globals.game_waits_for_file_selector = 1;
   GLOBAL_UNLOCK();

   SDL_Event event;
   SDL_zero(event);
   event.type = custom_event_save_file;
   file_selector_t data;
   event.user.data1 = &data;

   SDL_PushEvent(&event);
   
   // Now wait for the file selector to be done
   do {
     SDL_Delay(1);
   } while (globals.game_waits_for_file_selector);

   *fs_iexbutton = globals.fs_result.selected[0] == '\0' ? 0 : 1;
   char *p = globals.fs_result.selected;
   printf("Received back: %s\n", globals.fs_result.selected);
   char *filename = globals.fs_result.selected;
   // Search for last directory separator
   while (*p) {
      if (*p == '/') {
         // printf("Separator seen: %s\n", p);
         filename = p + 1;
         // printf("Filename now: %s\n", filename);
      }
      p++;
   }
   // printf("Filename finally: %s\n", filename);   
   strncpy(fs_iinsel, filename, 13);
   // Force null terminator
   fs_iinsel[12] = '\0';
   // printf("...now fs_iinsel is: %s\n", fs_iinsel);

   // printf("fsel_input() finished\n");

   return 0;
}
