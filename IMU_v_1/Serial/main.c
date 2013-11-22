#include <fcntl.h>
#include <termios.h>
#include <termio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/serial.h>



struct termios data;
int COM1;

 static int rate_to_constant(int baudrate) {
 #define B(x) case x: return B##x
         switch(baudrate) {
                 B(50);     B(75);     B(110);    B(134);    B(150);
                 B(200);    B(300);    B(600);    B(1200);   B(1800);
                 B(2400);   B(4800);   B(9600);   B(19200);  B(38400);
                 B(57600);  B(115200); B(230400); B(460800); B(500000);
                 B(576000); B(921600); B(1000000);B(1152000);B(1500000);
         default: return 0;
         }
 #undef B
 }

 /* Open serial port in raw mode, with custom baudrate if necessary */
 int open_serialport(const char *device, int rate)
 {
         struct termios options;
         struct serial_struct serinfo;
         int fd;
         int speed = 0;
         fd = open(device,O_RDWR|O_NOCTTY);
         if (fd<0) printf("Error opening port\n");
         /* Open and configure serial port */
         if ((fd) == -1)
                 return -1;

         speed = rate_to_constant(rate);
         if (speed == 0) {
                 /* Custom divisor */
                 serinfo.reserved_char[0] = 0;
                 if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
                         return -1;
                 serinfo.flags &= ~ASYNC_SPD_MASK;
                 serinfo.flags |= ASYNC_SPD_CUST;
                 serinfo.custom_divisor = (serinfo.baud_base + (rate / 2)) / rate;
                 if (serinfo.custom_divisor < 1)
                         serinfo.custom_divisor = 1;
                 if (ioctl(fd, TIOCSSERIAL, &serinfo) < 0)
                         return -1;
                 if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
                         return -1;
                 if (serinfo.custom_divisor * rate != serinfo.baud_base) {
                         warnx("actual baudrate is %d / %d = %f",
                               serinfo.baud_base, serinfo.custom_divisor,
                               (float)serinfo.baud_base / serinfo.custom_divisor);
                 }
         }

         fcntl(fd, F_SETFL, 0);
         tcgetattr(fd, &options);
         cfsetispeed(&options, speed ?: B38400);
         cfsetospeed(&options, speed ?: B38400);
         cfmakeraw(&options);
         options.c_cflag |= (CLOCAL | CREAD | CS8);
         //options.c_cflag &= ~CRTSCTS;
         if (tcsetattr(fd, TCSANOW, &options) != 0)
                 return -1;

         return fd;
}

char read_USART(){
    char com;

    read(COM1,&com,1);
    return com;
}

void write_USART(char data) {
    write(COM1,&data,1);
}

int main(){
    COM1 = open_serialport("/dev/ttyUSB0",500000);
    printf("%d\n",COM1);
    unsigned char var;
    int16_t value
    ;
    int i;
    int a = 0;
    while(1) {
        var = read_USART();
        if (var == '\n'){
            printf("%d\n",value);
            value = 0;
            a = 0;}
        else {
            if (a == 0){
                value = (var << 8);
                a = 1;}
            else{
                value = value | var;
                }
}

    }
}

//for(i=0; i<200;i++)