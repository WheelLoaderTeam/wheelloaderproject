#include "stdio.h"
#include "time.h"
#include "string.h"

//logging functions

int logcounter = 0;
char * logfiletime;
char logfilename[80];

char * currentDateTime()
{
  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  printf ("Current local time and date: %s", asctime(timeinfo));

  return asctime(timeinfo);
}

int  logRealTime(int timestamp, int packetID, char *event, int data1, int data2, int data3)
{
    printf("Log: %8d %4d %s  %d  %d  %d \n", timestamp, packetID, event, data1, data2, data3); //format data appropriately
}

int logToFile(int timestamp, int packetID, char *event, int data1, int data2, int data3)
{
    if(logcounter > 50000 || logcounter == 0)
    {
        *logfilename = '\0';
        logfiletime = currentDateTime();
        strcpy(logfilename, "LogFile");
        strcat(logfilename, logfiletime);
        strcat(logfilename, ".txt");
        logcounter = 1;
    }

    FILE *fp = fopen(logfilename, "a");                                                              //open the file
    fprintf(fp, "Log: %8d %4d %s  %d  %d  %d \n", timestamp, packetID, event, data1, data2, data3); //print passed data to file
    fclose(fp);

    logcounter ++;                                                                                  //close file
}

int main()
{
    int faketimestamp = 3191993;
    int fakepacketID = 1667;
    char *fakeevent = "MidNovemberTesting";
    int fakedata1 = 123;
    int fakedata2 = 3456;
    int fakedata3 = 78910;

    int testlength = 100000;
    char * currentime = currentDateTime();
    printf("%s\n", currentime);
    while(testlength--)
    {
        logToFile(faketimestamp, fakepacketID, fakeevent, fakedata1, fakedata2, fakedata3);
        logRealTime(faketimestamp, fakepacketID, fakeevent, fakedata1, fakedata2, fakedata3);
    }


}
