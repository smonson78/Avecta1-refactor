	
#include <gemdos.h>
#include <xbios.h>

#include "osbind.h"
#include "gemdefs.h"
#include "words.h"
#include "globals.h"

#include "startaux.h"
#include "title.h"
#include "fileio.h"
#include "switchru.h"
#include "tacmode.h"
#include "text.h"
#include "raton.h"
#include "rausmaus.h"

int stinit();

int main()
{
   int old, i, j;
   char *c = curmon[0];
   
   if (stinit() == 0 || ruminit() == 1) {
      goto end;
   }

   graf_mouse(0, (MFORM *)intin);

   j = Random();
   // Seed the random number generator maybe? No other references to srand in here
   // srand(j); 

   i = title();
   if(i == 1) 
   new = 0;
   else
   new = 1;
   specbuf[29] = new;
   crum = -1;
   while (i != 0) {
      j = 1;

      switch(i) {
      case 1:
         i = init(0);
         break;
      case 2:
         if ((j=fileio(2)) == -1) {
            raton();
            form_alert(1, errmsg[18]);
            rausmaus();
         }
         i = 0;
         break;
      default:
         i = 0;
         break;
      }

      while (i == 0 && j > 0) {
         i = switchrum();
         if (i > 0) {
            xbios_37();
            xbios_38_off();
            continue;
         }
         i = tacmode();
         xbios_37();
         xbios_38_off();
      }

      switch(i) {
         case -5:
            if ((j=fileio(2)) == -1) {
               raton();
               form_alert(1,errmsg[18]);
               rausmaus();
            }
            if (j == 0) {
               raton();
               form_alert(1,errmsg[33]);
               rausmaus();
            }
            i = 3;
            break;
         case -4: /* this is the store game option */
            if((j=fileio(1)) == -1) {
               form_alert(1,errmsg[18]);
            }
            i = 3;
            break;
         case -2:
            i = 0;
            break;
         case -1:
            i = console();
            break;
         case 1: 
            congratulate();
            i = 0;
            break;
         case 2: /* dungeon switch */
            old = dungeon;
            if (dungeon != 0) {
               *(c+54) = *(c+4);
               *(c+59) = 0;
            } else {
               police = 0;
               if (*(c+46) == 81) {
                  *(c+46) = 0;
               }
               *(c+52) = *(c+53) = 0;
               *(c+56) = *(c+24);
               *(c+57) = *(c+25);
               *(c+55) = crum;
            }
            if (loadnew() == 0) {
               raton();
               i = 0;
               form_alert(1, errmsg[32]);
               rausmaus();
               break;
            }
            if (old != 0) {
               *(c+30) = *(c+55);
               *(c+4) = 125;
               *(c+24) = *(c+56);
               *(c+25) = *(c+57);
               outside = 1;
            } else {
               *(c+4) = *(c+54);
               outside = 0;
               *(c+54) = 0;
            }
            i = 3;
            break;  
         default:
            i = title();
            break;
         }
   }
end:
   Setpalette(oldpal);
   raton();
   v_clsvwk(handle);
   appl_exit();
   return 0;
}


int prnt(char *s1, char *s2, char *s3, char *s4, char *s5, char *s6, char *s7) {
   int per = 0, i = 0, j, k = 0, l, len = 0, m, lines = 1;
   char buffer[30];
   char *c[6];

   vsf_interior(handle, 1);
   vsf_color(handle,0);
   v_bar(handle, lxy);
   vsf_interior(handle, 1);
   vsf_color(handle,1);
   c[0] = s2;
   c[1] = s3;
   c[2] = s4;
   c[3] = s5;
   c[4] = s6;
   c[5] = s7;
   l = strlen(s1);
   col = 2;
   row++;
   rausmaus();
   xbios_37();
   xbios_38_off();
   v_rvon(handle);
   while( l - i > 0) {
      j = 0;
      while (*(s1+i) != 32 && *(s1+i) != '%' && l-i > 0) {
         buffer[j++] = *(s1+i);
         i++;
      }

      if (*(s1+i) == 32 || l-i == 0 ) { 
         buffer[j] = '\0';
         len = strlen(buffer);
      }

      if (*(s1+i) == '%') {
         i++;
         len = strlen(c[k]);
         k++;
         if (*(s1+i+1) != 32) {
            per = 1;
         }
      }

      if (len + col + per > 41) {
         col = 2;
         lines++;
         row++;
         per = 0;
         xbios_37();
      }
      
      per = 0;

      if (row > 25) {
         vs_curaddress(handle,17,2);
         printf("                              ");
         vsf_interior(handle,1);
         vsf_color(handle,0);
         v_bar(handle,lxy);
         vsf_interior(handle,1);
         vsf_color(handle,1);
         for(m=0;m<4;m++) {
            pxy[1] = 136-2*m;
            pxy[3] = 199-2*m;
            pxy[5] = 134-2*m;
            pxy[7] = 197-2*m;
            bxy[1] = 198-2*m;
            bxy[3] = 199-2*m;
            vro_cpyfm(handle, 3, pxy, &psrc, &pdes);
            v_bar(handle, bxy);
            }
         row = 25;
         if ((usedline + lines > 7 
            && hold == 1 
            && l-i > 40) 
            || usedline + lines > 8) {
            textsix(1,204,8*24,6,"[More]");
            hold = 0;
            usedline = 0;
            xbios_38_vbl();
            Bconin(2);
            xbios_37();
            xbios_38_off();
            lines = 1;
            vs_curaddress(handle,25,2);
            printf("                              ");
         }
      }
      if(*(s1+i) == ' ' || l-i == 0) {
         textsix(1,6*col,8*(row-1),len,buffer);
      } else {
         textsix(1,6*col,8*(row-1),len,c[k-1]);
         col--;
      }
      col += len + 1;
      i++;
      }
   v_rvoff(handle);
   usedline += lines;
   xbios_38_vbl();
   raton();
   hold = 1;
   return(i);
}   

int stinit() {
   int i;
   int16_t rgb[3];

   appl_init();
   
   // Dummy values
   int16_t dum1, dum2, dum3, dum4;
   handle = graf_handle(&dum1, &dum2, &dum3, &dum4);

   for (i = 0; i < 10; work_in[i++] = 1)
      ;

   work_in[10] = 2;
   v_opnvwk(work_in, &handle, work_out);
   psrc.fd_w = pdes.fd_w = work_out[0] + 1;
   psrc.fd_h = pdes.fd_h = work_out[1] + 1;
   psrc.fd_wdwidth = pdes.fd_wdwidth = psrc.fd_w>>4;
   psrc.fd_stand = pdes.fd_stand = 1;
   psrc.fd_addr = pdes.fd_addr = (long)0;
   rausmaus();
   vs_curaddress(handle,1,1);

   // Erase to end of screen
   v_eeos(handle);
   // Save palette
   xbios_38_savpal();

   if(Getrez() != 0) {
      raton();
      form_alert(1,"[1][Reboot in Low Rez!][OK]");
      rausmaus();
      return(0);
   }
   Setpalette(newpal);
   xbios_37();
   addr = Logbase();
   vq_extnd(handle, 1, work_out);
   psrc.fd_nplanes = pdes.fd_nplanes = work_out[4];

   // Get default disk drive
   i = Dgetdrv();
   // Create drive letter
   path[0] = 'a' + i;

   num = 1;
   rgb[0] = rgb[1] = rgb[2] = 0;
   vs_color(handle, 0, rgb);
   xbios_37();
   return 1;  
}
