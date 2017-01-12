#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  int i;
  unsigned int len;
  unsigned long val = 0;
  char *a;

  len = strlen(argv[1]);
  a = (char *)malloc(len);
  strcpy(a,argv[1]);

  for(i=0;i<len;i++)
    if(a[i] <= 57)
      val += (a[i]-48)*(1<<(4*(len-1-i)));
    else
      val += (a[i]-55)*(1<<(4*(len-1-i)));
  printf("%ld\n",val);
}
