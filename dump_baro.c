#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
  short v; 

  while (read (0, &v, 2) == 2) {
    v |= (v&0x4000) << 1;
    printf ("%d\n", v); 
  }
  exit (0);
}
