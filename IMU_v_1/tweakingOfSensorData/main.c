#include <stdlib.h>
#include <stdint.h>
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

typedef struct{
    float accX;
    float accY;
    float accZ;
    float rotX;
    float rotY;
    float rotZ;
}sensor_data;

struct termios data;
int COM1;
// static acceleration in meters per second^2 and rotation in radians


 static int rate_to_constant(int baudrate) {
 #define B(x) case x: return B##x
         switch(baudrate) {
                 B(50); B(75); B(110); B(134); B(150);
                 B(200); B(300); B(600); B(1200); B(1800);
                 B(2400); B(4800); B(9600); B(19200); B(38400);
                 B(57600); B(115200); B(230400); B(460800); B(500000);
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
//sensor_data receiveSensorData(){
    sensor_data sensorData;
    COM1 = open_serialport("/dev/ttyUSB0",500000); //Open USB port
   // printf("%d\n",COM1); //Just a test to see if port is opened
    unsigned char var;
    uint16_t acc_x;
    uint16_t acc_y;
    uint16_t acc_z;

    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t Max_size = 0x03FF;
    float Max_voltage = 3.3f;
    uint16_t Zero_data_x = 0x01FD;
    uint16_t Zero_data_y = 0x01FF;
    uint16_t Zero_data_z = 0x01FD;
    float sensitivity = 0.192f;
    float gravity = 9.82f;
    float pi = 3.14159f;
    int gyro_scale = 80;
    int pi_scale = 180;
    int error_flag = 0;
    int pkg_cntr = 0;                       //Counter to keep track of packets
    int correct_pkg = 0;                    //Flag to see if in right place or not, 0 if startup haven't been observed, 1 if all is OK (?)
    while(1) {
        var = 0;
        var = read_USART();
        pkg_cntr++;
        if (var == 0x00){                   //Check if zero Byte is sent
            if (5 <= pkg_cntr && pkg_cntr <= 22){           //If this is not a part of the "beginning zeroes"
                correct_pkg = 1;            //Set flag that indicates "correct" package
            }
            else if (pkg_cntr <5){          //If still in beginning
                correct_pkg = 0;            //Not "correct" package
            }
        }
        else {                              //If values are received
            if (pkg_cntr <5){               //BUT, beginning has not been observed
                pkg_cntr = 0;               //Restart counter
                correct_pkg = 0;            //Not "correct" package
            }
            if (5<=pkg_cntr && pkg_cntr<=22){            //If values after beginning zeroes
               correct_pkg = 1;             //set flag that indicates "correct" package
            }
        }
        if (correct_pkg == 1){              //If startup has been observed, fill in data
            if (pkg_cntr == 5){             //GyroX, Byte 4 (Check data, and sensor data)
                if (((var >> 2 ) == 0x01) | ((var >>2) == 0x05) ){ //Check ST bits to see if data is OK
                    gyro_x = (var<<14);
                }
                else{
                    printf("An error has occured 1 \n");
                    printf("%d\n", var);
                    error_flag = 1;
                    //return 0;               //End here if corrupt data
                }
            }
            if (pkg_cntr == 6){             //GyroX, Byte 3 (Sensor data)
                gyro_x = gyro_x | (var<<6);
            }
            if (pkg_cntr == 7){             //GyroX, Byte 2 (Sensor data)
                gyro_x = gyro_x | (var>>2);
            }
            if (pkg_cntr == 8){             //GyroX, Byte 1 (Error data)
                if ((var>>1) != 0x00){      //If any of the error bits are set
                    if ((var>>1) == 0x02){  //This "error" is set if value exceeds 512 which is OK, don't know why it exists..
                        printf("A non-error has occured\n");
                        printf("%d\n",var);
                    }
                    else {
                        printf("An error has occured 2\n");
                        printf("%d\n",var);
                        //return 0;
                    }
                }
            }
            if (pkg_cntr == 9){             //GyroY, Byte 4 (Check data, and sensor data)
                if (((var >> 2 ) == 0x01) | ((var >>2) == 0x05) ){ //Check ST bits to see if data is OK
                        gyro_y = (var<<14);
                }
                else{
                    printf("An error has occured 3\n");
                    printf("%d\n", var);
                    error_flag = 1;
                }
            }
            if (pkg_cntr == 10){            //GyroY, Byte 3 (Sensor data)
                gyro_y = gyro_y | (var<<6);
            }
            if (pkg_cntr == 11){            //GyroY, Byte 2 (Sensor data)
                gyro_y = gyro_y | (var>>2);
            }
            if (pkg_cntr == 12){            //GyroY, Byte 1 (Error data)
                if ((var>>1) != 0x00){      //If any of the error bits are set
                    if ((var>>1) == 0x02){  //This "error" is set if value exceeds 512 which is OK, don't know why it exists..
                        printf("A non-error has occured\n");
                        printf("%d\n",var);
                    }
                    else {                      //All else is a real error
                        printf("An error has occured 4\n");
                        printf("%d\n",var);
                        //return 0;
                    }
                }
            }
            if (pkg_cntr == 13){            //GyroZ, Byte 4 (Check data, and sensor data)
                if (((var >> 2 ) == 0x01) | ((var >>2) == 0x05) ) { //Check ST bits to see if data is OK
                    gyro_z = (var<<14);
                }
                else{
                    printf("An error has occured 5\n");
                    printf("%d\n",var);
                    error_flag = 1;
                }
            }
            if (pkg_cntr == 14){            //GyroZ, Byte 3 (Sensor data)
                gyro_z = gyro_z | (var<<6);
            }
            if (pkg_cntr == 15){            //GyroZ, Byte 2 (Sensor data)
                gyro_z = gyro_z | (var>>2);
            }
            if (pkg_cntr == 16){            //GyroZ, Byte 1 (Error data)
                if ((var>>1) != 0x00){      //If any of the error bits are set
                    if ((var>>1) == 0x02){  //This "error" is set if value exceeds 512 which is OK, don't know why it exists..
                    printf("A non-error has occured\n");
                    printf("%d\n",var);
                    }
                    else {                      //All else is a real error
                        printf("An error has occured 6\n");
                        printf("%d\n",var);
                        //return 0;
                    }
                }
            }
            if (pkg_cntr == 17){            //AccX, high value
                acc_x = (var << 8);
            }
            if (pkg_cntr == 18){            //AccX, low value
                acc_x = acc_x | var;
            }
            if (pkg_cntr == 19){            //AccY, high value
                acc_y = (var << 8);
            }
            if (pkg_cntr == 20){            //AccY, low value
                acc_y = acc_y | var;
            }
            if (pkg_cntr == 21){            //AccZ, high value
                acc_z = (var << 8);
            }
            if (pkg_cntr == 22){            //AccZ, low value
                acc_z = acc_z | var;
                pkg_cntr = 0;               //Reset counter when end of sending is reached
                // Put everythin in a struct and call processdata ?
                //ATTENTION! Some of the received data must be sign changed: rotY, should be inverted (turns out the acc chip has weird axis-defs)
                //printf("%d\n",acc_z);
                if (error_flag == 0){
                    sensorData.accX = (acc_x-Zero_data_x)*(Max_voltage/sensitivity)*gravity/Max_size;
                    sensorData.accY = (acc_y-Zero_data_y)*(Max_voltage/sensitivity)*gravity/Max_size;
                    sensorData.accZ = (acc_z-Zero_data_z)*(Max_voltage/sensitivity)*gravity/Max_size;
                    sensorData.rotX = (gyro_x/gyro_scale)*(pi/pi_scale);
                    sensorData.rotY = -(gyro_y/gyro_scale)*(pi/pi_scale);
                    sensorData.rotZ = (gyro_z/gyro_scale)*(pi/pi_scale);
                    printf("%f\n", sensorData.accY);
                    }
                else
                    printf("Bad data\n");
                exit(0);
                //return sensorData;
            }
        }
    }
}


//int i;
//for(i=0; i<200;i++)
//            printf("%d\n",value); //Print out value
