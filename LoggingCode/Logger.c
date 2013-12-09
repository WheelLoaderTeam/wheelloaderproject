#include "stdio.h"
#include "time.h"
#include "string.h"

//logging functions


int LogRealTime(int time, int packetID, char *event, int data1, int data2, int data3)
{
    printf("Log: %8d %4d %s %d %d %d \n", time, packetID, event, data1, data2, data3); //format data appropriately
}

int LogToFile(int time, int packetID, char *event, int data1, int data2, int data3)
{
    FILE *fp = fopen("LoggingTest.txt", "a"); //open the file
    fprintf(fp, "Log: %8d %4d %s %d %d %d \n", time, packetID, event, data1, data2, data3); //print passed data to file
    fclose(fp); //close file
}

int main()
{
    int faketime = 3191993;
    int fakepacketID = 1667;
    char *fakeevent = "MidNovemberTesting";
    int fakedata1 = 123;
    int fakedata2 = 3456;
    int fakedata3 = 78910;

    int testlength = 100;
    while(testlength--)
    {
     LogToFile(faketime, fakepacketID, fakeevent, fakedata1, fakedata2, fakedata3);
     LogRealTime(faketime, fakepacketID, fakeevent, fakedata1, fakedata2, fakedata3);
    }

}
