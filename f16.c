int i16(pc)
int pc;
{
int vbl(),off();
xbios_37();
xbios_38_off();
raton();
form_alert(1,"[1][ZZZ... not implemented][OK]");
rausmaus();
xbios_37();
xbios_38_vbl();
return(0);
}

int o16(pc)
int pc;
{
return(1);
}
