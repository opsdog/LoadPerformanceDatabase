/*

  program to create the gnuplot colors for an arbitrary number of plots

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif


main(int argc, char *argv[])
{

  int NumPlots = 5;
  int i;
  float RedStart,
    GreenStart,
    BlueStart,
    RedEnd,
    GreenEnd,
    BlueEnd;
  float ColorRedInc,
    ColorGreenInc,
    ColorBlueInc;
  float RedValue,
    GreenValue,
    BlueValue;

#ifdef DEBUG
  printf("Number of Args:  %d\n",argc);
#endif

  if ( argc != 2 ) {
    printf("usage:  MDay-ColorMap NumPlots\n");
    return;
  }

  sscanf(argv[1],"%d",&NumPlots);

  RedStart=0.0; GreenStart=0.0; BlueStart=255.0;
  RedEnd=255.0; GreenEnd=0.0; BlueEnd=0.0;

  ColorRedInc = (RedEnd - RedStart)/(float)(NumPlots - 1);
  ColorGreenInc = (GreenEnd - GreenStart)/(float)(NumPlots - 1);
  ColorBlueInc = (BlueEnd - BlueStart)/(float)(NumPlots - 1);

#ifdef PRINTCOLORMAPS
  printf("\n\n  Colormaps:\n");
  printf("    %f %f %f --> %f  %f %f\n",RedStart, GreenStart,
	 BlueStart, RedEnd, GreenEnd, BlueEnd);
  printf("    Inc1's: %f, %f, %f\n", 
	 ColorRedInc, ColorGreenInc, ColorBlueInc);
#endif

  for (i=0,RedValue=RedStart,GreenValue=GreenStart,BlueValue=BlueStart;
       i<NumPlots;
       i++,
	 RedValue+=ColorRedInc, GreenValue+=ColorGreenInc,
	 BlueValue+=ColorBlueInc) {

    fprintf(stderr,"    Assigned color (%f, %f, %f) - %d\n",
	   RedValue, GreenValue, BlueValue, (i + 1));

    printf("set style line %d lc rgb '#%02x%02x%02x' lw 2 pt 1\n",
	   (i + 1), (int)(RedValue + 0.4), 
	   (int)(GreenValue + 0.4), 
	   (int)(BlueValue + 0.4));

  }  /*  assign all colors  */

}  /*  end of main  */
