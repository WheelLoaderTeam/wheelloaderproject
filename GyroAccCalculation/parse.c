#include <stdio.h>
#include <stdlib.h>
#include <cfloat.h> //used for DBL_MAX
#define ss_checkfrequency 5;//ss StationaryState
#define ss_timethreshold 2;
#define ss_deltathreshold 3;

void parseFile(char *filename);

int main(int argc, char *argv[]) {
    parseFile(argv[1]);
}

double *x_acc, *y_acc, *z_acc, *x_head, *y_head, *z_head;

void parseFile(char *filename) {
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
        while (count < 12) {
            if (*ptr == ',')
                count++;
            ptr++;
        }
        sscanf(ptr,"%lf,%lf,%lf,%lf,%lf,%lf,",&x_acc[i],&y_acc[i],&z_acc[i],
               &x_head[i],&y_head[i],&z_head[i]);
    }
    
    //test
    /*for (i = 0; i<n-1; i++) {
        printf("%lf,%lf,%lf,%lf,%lf,%lf\n",x_acc[i],y_acc[i],z_acc[i],
               x_head[i],y_head[i],z_head[i]);
    }*/
}

int checkIfSS(){
	int i = 100;
	double x_max = DBL_MIN;
	double x_min = DBL_MAX;
	double y_max = DBL_MIN;
	double y_min = DBL_MAX;
	double z_max = DBL_MIN;
	double z_min = DBL_MAX;

	while(i--){
		if(&x_acc[i] > x_max){x_max = &x_acc[i];}
		if(&x_acc[i] < x_min){x_min = &x_acc[i];}
		if(&y_acc[i] > y_max){y_max = &y_acc[i];}
		if(&x_acc[i] < y_min){y_min = &y_acc[i];}
		if(&x_acc[i] > z_max){z_max = &z_acc[i];}
		if(&x_acc[i] < z_min){z_min = &z_acc[i];}
	}
	if( x_max - x_min < ss_deltathreshold && 
		y_max - y_min < ss_deltathreshold && 
		z_max - z_min < ss_deltathreshold){
		//calibrate() and determinedowndirection()
	}
	else{printf("System not at rest");}

}


