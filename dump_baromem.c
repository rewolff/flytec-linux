
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

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

char *short_opts = "s:e:d:";

struct option options[] = {
  {"start", required_argument, NULL, 's'},
  {"end",   required_argument, NULL, 'e'}, 
  {"device", required_argument, NULL, 'd'},
  {NULL, 0, NULL, 0}};


int main (int argc, char **argv)
{
  int start = 0xb0;
  int end = 0x100; 
  char *dev = "/dev/ttyUSB1";
  char buf[1024];
  int fd, val, i;
  unsigned char *eeval;
  struct termios tv;
  int c, idx, n;

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

  eeval = malloc (end-start); 
  fd = open (dev, O_RDWR); 

  tcgetattr (fd, &tv);
  tv.c_iflag &= ~(IGNPAR | INLCR | IXON | IXOFF | ICRNL);
  tv.c_iflag |= IGNBRK | IGNPAR;

  tv.c_cflag |= CRTSCTS;

  tv.c_oflag &= ~(OPOST);
  tv.c_oflag &= ~(OPOST | ONLCR);

  tv.c_lflag &= ~(ISIG | ICANON | IEXTEN);

  cfsetispeed (&tv, 9600);
  cfsetospeed (&tv, 9600);

  tcsetattr (fd, TCSANOW, &tv);

  comm_6010 (fd, "A00xxBD", 2, buf);

  //  fprintf (stderr, "got hello: %s\n", buf); 

  write (fd, "ADDxx86\r\n", 9);
  while (1) {
     alarm (3); 
     n = read (fd, buf, 100);
     if (n < 0) break;
     write (1, buf, n);
  }

  exit (0);
}  
  
