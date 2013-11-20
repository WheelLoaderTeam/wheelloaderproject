#include <stdio.h>
#include <stdlib.h>
#include <float.h> 				//DBL_MAX
#include <math.h> 				//arcsin
#define ss_checkfrequency 5		//ss = StationaryState
#define ss_deltathreshold 0.50
#define ss_numrecords	  200   //At 100 hz = 2 seconds of data
int parseFile(char *filename);

int main(int argc, char *argv[]) {
	int buffer_position;
    buffer_position = parseFile(argv[1]);
	checkIfSS(buffer_position);
}

double *x_acc, *y_acc, *z_acc, *x_head, *y_head, *z_head;

int parseFile(char *filename) {
    char buf[1024];
    char *ptr;
    int count, i, n = 0;
    FILE *f = fopen(filename,"r");
    
    //find out the number of lines
    while (fgets(buf,1024,f)) {
        n++;
    }
    rewind(f);
    
    //allocate memory
    x_acc = calloc(n-1,sizeof(double));
    y_acc = calloc(n-1,sizeof(double));
    z_acc = calloc(n-1,sizeof(double));
    x_head = calloc(n-1,sizeof(double));
    y_head = calloc(n-1,sizeof(double));
    z_head = calloc(n-1,sizeof(double));
    
    //ignore first line
    fgets(buf,1024,f);
    
    //read the data
    for (i = 0; i<n-1; i++) {
        fgets(buf,1024,f);
        ptr = buf;
        count = 0;
        while (count < 11) {
            if (*ptr == ',')
                count++;
            ptr++;
        }
        sscanf(ptr,"%lf,%lf,%lf,%lf,%lf,%lf,",&x_acc[i],&y_acc[i],&z_acc[i],
               &x_head[i],&y_head[i],&z_head[i]);
    }
    
    /*//test
    for (i = 0; i<n-1; i++) {
        printf("%lf,%lf,%lf,%lf,%lf,%lf\n",x_acc[i],y_acc[i],z_acc[i],
               x_head[i],y_head[i],z_head[i]);
    }*/
	return n-2;
}

int checkIfSS(int buf_pos){
	int i = ss_numrecords;
	double x_min = DBL_MAX, x_max = -1 * DBL_MAX;
	double y_min = DBL_MAX, y_max = -1 * DBL_MAX;
	double z_min = DBL_MAX, z_max = -1 * DBL_MAX;

	//printf("Buf Pos = %d\n", buffer_position);
	do{
		if(buf_pos == 0){
			//wrap around to correct position
		}
		if(x_acc[buf_pos] > x_max){x_max = x_acc[buf_pos];}
		if(x_acc[buf_pos] < x_min){x_min = x_acc[buf_pos];}
		if(y_acc[buf_pos] > y_max){y_max = y_acc[buf_pos];}
		if(y_acc[buf_pos] < y_min){y_min = y_acc[buf_pos];}
		if(z_acc[buf_pos] > z_max){z_max = z_acc[buf_pos];}
		if(z_acc[buf_pos] < z_min){z_min = z_acc[buf_pos];}
		buf_pos--;
	}while(i--)
	//test
	printf("x_delta = %lf, y_delta = %lf, z_delta = %lf \n", x_max - x_min, y_max - y_min, z_max - z_min);

	if( ((x_max - x_min) < ss_deltathreshold) && 
		((y_max - y_min) < ss_deltathreshold) && 
		((z_max - z_min) < ss_deltathreshold)){
		printf("System is at rest\n");
		return 1;
	}
	else{
		printf("System not at rest\n");
		return 0;
	}
}

//This function calculates pitch(y-rotation) and roll(x-rotation) based on the static acceleration.
//These values are then sent to the simulator, and also used as the "zero" point for the gyro data
//which prevents drift error.

void accPitchRoll(int buf_pos){
	int i = ss_numrecords;
	double x_avg = 0, y_avg = 0;
	double pitch_rad = 0, roll_rad = 0;

	do{
		if(buf_pos == 0){
			//wrap around to correct position
		}
		x_avg+= x_acc[buf_pos];
		y_avg+= y_acc[buf_pos];
		buf_pos--;
	}while(i--)

	x_avg = (x_avg/ss_numrecords);
	y_avg = (y_avg/ss_numrecords);
	pitch_rad = asin (y_avg);
	roll_rad  = asin (x_avg);
}





