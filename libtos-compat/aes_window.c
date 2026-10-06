#include "aes_window.h"
#include "aes.h"

int16_t wind_create(int16_t wi_crkind, int16_t wi_crwx,
  int16_t wi_crwy, int16_t wi_crww, int16_t wi_crwh)
{
  return 0;
}

int16_t wind_open(int16_t wi_ohandle, int16_t wi_owx,
  int16_t wi_owy, int16_t wi_oww, int16_t wi_owh)
{
  return 0;
}

int16_t wind_get(int16_t wi_ghandle, WF_Function wi_gfield,
    int16_t *wi_gw1, int16_t *wi_gw2, int16_t *wi_gw3, int16_t *wi_gw4)
{
  return 0;
}

int16_t wind_calc(WC_Type wi_ctype, int16_t wi_ckind,
    int16_t wi_cinx, int16_t wi_ciny,
    int16_t wi_cinw, int16_t wi_cinh,
    int16_t *coutx, int16_t *couty,
    int16_t *coutw, int16_t *couth)
{
  return 0;
}

int16_t wind_set(int16_t wi_shandle, WF_Function wi_sfield,
    int16_t wi_sw1, int16_t wi_sw2, int16_t wi_sw3, int16_t wi_sw4)
{
  return 0;
}

int16_t wind_update(WU_Type wi_ubegend)
{
  return 0;
}

int16_t wind_close(int16_t wi_clhandle)
{
  return 0;
}

int16_t wind_delete(int16_t wi_dhandle)
{
  return 0;
}



