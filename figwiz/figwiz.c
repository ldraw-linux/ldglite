#include <stdio.h>
#include <stdlib.h>


char cmd[1024];
char Oblique[] = "-a0.707104,0,0.707104,0.353553,0.866025,-0.353553,-0.612372,0.5,0.612372";

//*****************************************************************
main(int argc, char ** argv)
{
  sprintf(cmd, "ldglite -v200,300 -l3 -q -s2 -fm0x80 -p %s %s",
	  Oblique, "Minifig1.ldr");
  system ( cmd );      /* exec the new cmd */

  exit ( 1 );
}

