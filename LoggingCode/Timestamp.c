#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


int getTimeOfDay(char* buffer)
{
  int len;
  struct timeval tv;

  time_t curtime;

  gettimeofday(&tv, NULL);
  curtime=tv.tv_sec;

  len = strftime(buffer,30,"%d-%m-%Y_%T.",localtime(&curtime));
  len += sprintf(buffer + len, "%ld",tv.tv_usec);

  return len;
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
