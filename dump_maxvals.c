

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

struct maxvals {
  short maxA1;
  short maxA2;
  short maxvario;
  short minvario;
  unsigned char flighttime[3];
  unsigned char date[3];
  unsigned char time[2];
  unsigned char baroheader;
  unsigned short baro_start;
  unsigned short baro_end; 
} __attribute__((__packed__));



int main (int argc, char **argv)
{
  struct maxvals mv;
  int i; 

  if (sizeof (mv) != 21) {
    printf ("Maxvals struct is wrong. \n");
    exit (1); 
  }
  i=1;
  while (read (0, &mv, sizeof (mv)) == 21) {
    printf ("--------- flight %d --------------\n", i++);
    printf ("MaxA1: %d\n", mv.maxA1); 
    printf ("MaxA2: %d\n", mv.maxA2); 
    printf ("maxvario: %f\n", .00390625*mv.maxvario); 
    printf ("minvario: %f\n", .00390625*mv.minvario);
    printf ("flighttime: %02x:%02x:%02x\n", 
	    mv.flighttime[2], mv.flighttime[1], mv.flighttime[0]); 
    printf ("date: %02x-%02x-%02x\n", mv.date[0], mv.date[1], mv.date[2]); 
    printf ("time: %02x:%02x\n", mv.time[1], mv.time[0]); 
    printf ("baro_hdr: %02x\n", mv.baroheader); 
    printf ("baro: %04x - %04x\n", mv.baro_start, mv.baro_end);
    printf ("\n");
	    
  }
  exit (0);
}
  
