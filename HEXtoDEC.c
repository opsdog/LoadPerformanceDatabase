
unsigned long hex2int(char *a, unsigned int len)
{
  int i;
  unsigned long val = 0;

  for(i=0;i<len;i++)
    if(a[i] <= 57)
      val += (a[i]-48)*(1<<(4*(len-1-i)));
    else
      val += (a[i]-55)*(1<<(4*(len-1-i)));
  return val;
}
