#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fft/tools/kiss_fftr.h"

#define FIXED_POINT 16
#define BUFFER_SIZE 512
#define FIR_SIZE 33

typedef struct{
	short coeffs[FIR_SIZE];
       	short maqs[FIR_SIZE*2];
			        short n;
}filter_type;

short audio_data[BUFFER_SIZE];
kiss_fft_scalar rin[BUFFER_SIZE+2];
kiss_fft_cpx   sout[BUFFER_SIZE+2];
kiss_fftr_cfg  kiss_fftr_state;

short calculate_FIR(filter_type *filter,short data)
{
	 int i = 0;
	 int output = 0;
	 filter->n ++;
	 if(filter->n == FIR_SIZE*2) filter->n = FIR_SIZE; 
	 filter->maqs[filter->n] = filter->maqs[filter->n-FIR_SIZE] = data;
	 for( i = 0; i < FIR_SIZE; i++){
	 	output = output + filter->maqs[filter->n-i] * filter->coeffs[i];
	 }
	 return output/32768;
}

filter_type filter0={.coeffs = {
-2654,-2179,-1717,-1270,-843,-438,-57,295,618,909,1165,1385,1567,1710,1813,1875,1896,1875,1813,1710,1567,1385,1165,909,618,295,-57,-438,-843,-1270,-1717,-2179,-2654
},.n=FIR_SIZE};

filter_type filter1={.coeffs = {
-2654,-2179,-1717,-1270,-843,-438,-57,295,618,909,1165,1385,1567,1710,1813,1875,1896,1875,1813,1710,1567,1385,1165,909,618,295,-57,-438,-843,-1270,-1717,-2179,-2654
},.n=FIR_SIZE};

filter_type filter2={.coeffs = {
-1368,-1310,-947,-414,46,183,-144,-886,-1789,-2468,-2541,-1775,-193,1899,3997,5544,6113,5544,3997,1899,-193,-1775,-2541,-2468,-1789,-886,-144,183,46,-414,-947,-1310,-1368
},.n=FIR_SIZE};

filter_type filter3={.coeffs = {
-486,139,-81,-1250,-1508, -136, 1096,693,-27,957,2525, 243,-3288,-6270,-3081,4132,7923,4132,-3081,-6270,-3288,1243,2525,957,-27,693,1096,-136,-1508,-1250,-81,139,-486
},.n=FIR_SIZE};

filter_type filter4={.coeffs = {
485,305,-627,-473,527,783,-599,-962,501,1422,-581,-1966,487,3477,-573,-10372,16867,-10372,-573,3477,487,-1966,-581,1422,501,-962,-599,783,527,-473,-627,305,485	
},.n=FIR_SIZE};

unsigned char color_array[13][10][3] = {
	{{255,50,50},{255,173,50},{214,255,50},{91,255,50},{50,255,132},{50,254,255},{50,132,255},{91,50,255},{214,50,255},{255,50,173}},
	{{255,25,25},{255,163,25},{209,255,25},{71,255,25},{25,255,117},{25,254,255},{25,117,255},{71,25,255},{209,25,255},{255,25,163}},
	{{255,0,0},{255,153,0},{203,255,0},{50,255,0},{0,255,102},{0,254,255},{0,101,255},{50,0,255},{203,0,255},{255,0,152}},
	{{229,0,0},{229,137,0},{183,229,0},{45,229,0},{0,229,91},{0,229,229},{0,91,229},{45,0,229},{183,0,229},{229,0,137}},
	{{204,0,0},{204,122,0},{163,204,0},{40,204,0},{0,204,81},{0,203,204},{0,81,204},{40,0,204},{163,0,204},{204,0,122}},
	{{178,0,0},{178,107,0},{142,178,0},{35,178,0},{0,178,71},{0,178,178},{0,71,178},{35,0,178},{142,0,178},{178,0,107}},
	{{153,0,0},{153,91,0},{122,153,0},{30,153,0},{0,153,61},{0,152,153},{0,61,153},{30,0,153},{122,0,153},{153,0,91}},
	{{127,0,0},{127,76,0},{101,127,0},{25,127,0},{0,127,51},{0,127,127},{0,50,127},{25,0,127},{101,0,127},{127,0,76}},
	{{102,0,0},{102,61,0},{81,102,0},{20,102,0},{0,102,40},{0,101,102},{0,40,102},{20,0,102},{81,0,102},{102,0,61},},
	{{76,0,0},{76,45,0},{61,76,0},{15,76,0},{0,76,30},{0,76,76},{0,30,76},{15,0,76},{61,0,76},{76,0,45}},
	{{51,0,0},{51,30,0},{40,51,0},{10,51,0},{0,51,20},{0,50,51},{0,20,51},{10,0,51},{40,0,51},{51,0,30}},
	{{25,0,0},{25,15,0},{20,25,0},{5,25,0},{0,25,10},{0,25,25},{0,10,25},{5,0,25},{20,0,25},{25,0,15}},
	{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,10},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
};

char address_table[5] = {9,4,2,7,3};
char color_order[10] = {0,9,2,6,4,1,5,7,3,8};
char NUM_LIGHTS = 10;
FILE *SERIAL;

void sendColor(char address,char r, char g,char b,char v)
{

//address = address + 1;
fprintf(SERIAL, "4,%d,%d,%d,%d,%d;\n",address,r,g,b,v);	
}



void main(void){

	
	int sum,average,level;
   int r,g,b;
	int peak[5];
   int max[5];
	int filt_out[5];
   int color[5];
	int i,j;
	int bytes_read;
   int temp_val;
	bytes_read = BUFFER_SIZE;
   setvbuf(stdout, NULL, _IONBF, 0);
	printf("Hello World!\n");
   SERIAL = fopen("/dev/ttyO1","w");
   setvbuf(SERIAL, NULL, _IONBF, 0);
   
   kiss_fftr_state = kiss_fftr_alloc(BUFFER_SIZE,0,0,0);
   
   sendColor(4,255,0,0,0);
   sendColor(9,0,255,0,0);
	color[0] = max[0] = 0;
	color[1] = max[1] = 0;
	color[2] = max[2] = 0;
	color[3] = max[3] = 0;
	color[4] = max[4] = 0;
	while(bytes_read==BUFFER_SIZE){
      // bytes_read = fread(&audio_data,sizeof(short),512,stdin);
      bytes_read = fread(&rin,sizeof(short),512,stdin);
		sum = 0;
		average = 0;
		peak[0] = 0;
		peak[1] = 0;
		peak[2] = 0;
		peak[3] = 0;
		peak[4] = 0;
		kiss_fftr( kiss_fftr_state, rin, sout );
		
      for(i =0; i< BUFFER_SIZE/2; i+=2){
         temp_val = sout[i].r +sout[i+1].r;
         printf("%8d ",temp_val);
      }
      printf("\n");
		
		
		// for(i = 0;i<bytes_read; i++){
		//        //sum +=audio_data[i];
		//        filt_out[0] = calculate_FIR(&filter0,audio_data[i]);
		//        filt_out[1] = calculate_FIR(&filter1,audio_data[i]);
		//        filt_out[2] = calculate_FIR(&filter2,audio_data[i]);
		//        filt_out[3] = calculate_FIR(&filter3,audio_data[i]);
		//        filt_out[4] = calculate_FIR(&filter4,audio_data[i]);
		//        for(j=0;j<5;j++){
		//        filt_out[j] = abs(filt_out[j]);
		//        peak[j] = (filt_out[j] > peak[j])? filt_out[j]: peak[j];
		//        }
		// 
		//     }
	   // for(j=0;j<5;j++){
	  //       if(peak[j]>max[j]){
	  //             max[j] = peak[j];
	  //       }
	  //          level = (12*peak[j])/max[j];
	  //          if(level >=10){
	  //             color[j] ++;
	  //             color[j] %=10;
	  //          }
	  //          printf("%8d:%8d ",level,max[j]);
	  //          r = color_array[12-level][color_order[color[j]]][0];
	  //          g = color_array[12-level][color_order[color[j]]][1];
	  //          b = color_array[12-level][color_order[color[j]]][2];
	  //       sendColor(address_table[j],r,g,b,0);
	  //       //sendColor(9,0,255,0,0);
	  //       }
	  //       printf ("\n");
		/*
		for(j=0;j<5;j++){
		intensity[j] = log10_10 * peak[j];
		}*/
		//printf("%8d:%8d:%8d:%8d:%8d\n", peak[0],peak[1],peak[2],peak[3],peak[4]);
		//printf("%d,%d,%d,%d,%d\n", peak[0],peak[1],peak[2],peak[3],peak[4]);
	}
	free(kiss_fftr_state);
}
