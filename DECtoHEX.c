#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *DECtoHEX(long int decimalNumber)
{

  long int remainder,quotient;
  int i=0,j,temp;
  int k;
  char hexadecimalNumber[100];
  char *returnstring;

  returnstring = (char *)malloc(100);

  if ( decimalNumber == 0 ) {
    strcpy(returnstring,"0");
    return(returnstring);
  }

  quotient = decimalNumber;
  while(quotient!=0){
    temp = quotient % 16;
    //To convert integer into character
    if( temp < 10)
      temp =temp + 48;
    else
      temp = temp + 55;
    hexadecimalNumber[i++]= temp;
    quotient = quotient / 16;
  }

  hexadecimalNumber[i]='\0';

  k=0;
  for(j = i -1 ;j>= 0;j--)
    returnstring[k++] = hexadecimalNumber[j];
  returnstring[k] = '\0';

  return returnstring;
}
