

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

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



void get_mem_from_flytec (char *dev, int start, int end, unsigned char *buf)
{
  char cmd[1024];
  FILE *pp;

  sprintf (cmd, "./dump_6010 -d %s -s %x -e %x", dev, start, end);
  pp = popen (cmd, "r");
  fread (buf, start-end, 1, pp);
  pclose (pp);
}


int main (int argc, char **argv)
{
  unsigned char buf[64*1024];
  char *dev = "/dev/ttyUSB1";
  int nflights;
  struct maxvals *mv;
  int i, fd;
  char flight_basename[128];
  char fname[128];
  int baro_size;

  if (getenv ("VARIO_DEV")) 
    dev = getenv ("VARIO_DEV");

  get_mem_from_flytec (dev, 0xf0, 0xf2, buf);

  nflights = buf[1];

  mv = (struct maxvals *) buf;
  
  for (i=0;i<nflights;i++) {
    printf ("downloading flight %d: ", i);fflush (stdout); 
    get_mem_from_flytec (dev, 0x100 + i*0x15, 0x115 + i*0x15, buf);
    
    sprintf (flight_basename, "flight_%02x-%02x-%02x_%02x:%02x", 
	     mv->date[0], mv->date[1], mv->date[2], 
	     mv->time[1], mv->time[0]);

    sprintf (fname, "%s.minmaxbin", flight_basename);
    printf ("%s ", fname);fflush (stdout);
    
    fd = open (fname, O_RDONLY);
    if (fd >= 0) {
      read (fd, buf+sizeof(struct maxvals), sizeof (struct maxvals));
      if (memcmp (buf, buf+sizeof(struct maxvals), sizeof (struct maxvals)) == 0) {
	printf ("Flight already downloaded skipping barogram.\n");
	continue;
      } else {
	printf ("different flight already exists.\n");
	continue;
      }
    } else {
      printf (" created ");fflush (stdout);
      fd = open (fname, O_WRONLY | O_TRUNC | O_CREAT, 0666);
      write (fd, buf, sizeof (struct maxvals));
      close (fd);
    }

    baro_size =  mv->baro_end+1 - mv->baro_start;
    if (baro_size < 0) {
      baro_size += 0x10000 - 0x500; 
      get_mem_from_flytec (dev, mv->baro_start, 0x10000, buf);
      get_mem_from_flytec (dev, 0x500, mv->baro_end+1, buf + 0x10000 - mv->baro_start);
    } else 
      get_mem_from_flytec (dev, mv->baro_start, mv->baro_end+1, buf);

    sprintf (fname, "%s.barobin", flight_basename);
    printf ("creating %s", fname);fflush (stdout);
    fd = open (fname, O_WRONLY | O_TRUNC | O_CREAT, 0666);
    write (fd, buf, baro_size);
    close (fd);
    printf (" .\n");

  }
  exit (0);
}
