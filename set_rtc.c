
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <sys/time.h>
#include <time.h>

#include <getopt.h>

void comm_6010 (int fd, char *send, int timeout, char *ret)
{
  int i;
  char ch; 

  write (fd, send, strlen (send)); 
  write (fd, "\r\n", 2);
  i = 0;
  while (read (fd, &ch, 1) > 0) {
    if (ch == '\n') {
      ret[i-1] = 0;
      break;
    }
    ret[i++] = ch;
  }
}


void send_6010 (int fd, char *send, int timeout)
{
  write (fd, send, strlen (send)); 
  write (fd, "\r\n", 2);
  usleep (timeout);
}


char *short_opts = "d:";

struct option options[] = {
  {"device", required_argument, NULL, 'd'},
  {NULL, 0, NULL, 0}
};


int main (int argc, char **argv)
{
  int start = 0xb0;
  int end = 0x100; 
  char *dev = "/dev/ttyUSB1";
  char buf[1024];
  int fd;
  struct termios tv;
  int c, idx;
  struct tm *tm;
  struct timeval tvv;

  if (getenv ("VARIO_DEV")) 
    dev = getenv ("VARIO_DEV");

  for (;;) {
    c = getopt_long (argc, argv, short_opts, options, &idx);
    if (c < 0) break;

    switch (c) {
    case 's':sscanf (optarg, "%x", &start); break;
    case 'e':sscanf (optarg, "%x", &end); break;
    case 'd':dev = strdup (optarg); break;
    default:
      fprintf (stderr, "unknown option %c(%d)\n", c, c);
      exit (1);
    }
  }

  fd = open (dev, O_RDWR);

  tcgetattr (fd, &tv);
  tv.c_iflag &= ~(IGNPAR | INLCR | IXON | IXOFF | ICRNL);
  tv.c_iflag |= IGNBRK | IGNPAR;

  tv.c_cflag |= CRTSCTS;

  //  tv.c_oflag &= ~(OPOST);
  tv.c_oflag &= ~(OPOST | ONLCR);

  tv.c_lflag &= ~(ISIG | ICANON | IEXTEN);

  cfsetispeed (&tv, 9600);
  cfsetospeed (&tv, 9600);

  tcsetattr (fd, TCSANOW, &tv);

  comm_6010 (fd, "A00xxBD", 2, buf);

  //  fprintf (stderr, "got hello: %s\n", buf); 


  gettimeofday (&tvv, NULL);
  tm = localtime (&tvv.tv_sec); 
  
  printf ("year = %d, month = %d, day = %d, hour = %d, min = %d, sec = %d\n", 
	  tm->tm_year, 
	  tm->tm_mon, 
	  tm->tm_mday, 
	  tm->tm_hour, 
	  tm->tm_min, 
	  tm->tm_sec);

  sprintf (buf, "A%02dxx26", 19 + tm->tm_year / 100);
  send_6010 (fd, buf, 40000);

  sprintf (buf, "A%02dxx25", tm->tm_year % 100);
  send_6010 (fd, buf, 40000);

  sprintf (buf, "A%02dxx24", tm->tm_mon + 1);
  send_6010 (fd, buf, 40000);

  sprintf (buf, "A%02dxx23", tm->tm_mday);
  send_6010 (fd, buf, 40000);

  sprintf (buf, "A%02dxx22", tm->tm_hour);
  send_6010 (fd, buf, 40000);

  sprintf (buf, "A%02dxx21", tm->tm_min);
  send_6010 (fd, buf, 40000);

  sprintf (buf, "A%02dxx20", tm->tm_sec);
  send_6010 (fd, buf, 40000);

  exit (0);
}  
  
