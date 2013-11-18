#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


char * getTimeOfDay(char* buffer)
{
  struct timeval tv;

  time_t curtime;

  gettimeofday(&tv, NULL);
  curtime=tv.tv_sec;
  int len;
  len = strftime(buffer,30,"%d-%m-%Y_%T.",localtime(&curtime));
  sprintf(buffer + len, "%ld",tv.tv_usec);

  return 0;
}

int main()
{
    int i = 1000;
    while(i--)
    {
    char buffmain[255];
    getTimeOfDay(buffmain);
    printf("%s\n",buffmain);
    }
}
