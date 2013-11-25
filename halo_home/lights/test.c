#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kiss_fftr.h"
#include "_kiss_fft_guts.h"

#define BUFFER_SIZE 512
#define FIR_SIZE 23
#define NUM_LIGHTS 5

typedef struct{
	int coeffs[FIR_SIZE];
       	int maqs[FIR_SIZE*2];
			        short n;
}filter_type;

short audio_data[BUFFER_SIZE];
kiss_fft_scalar rin[BUFFER_SIZE];
kiss_fft_cpx   sout[BUFFER_SIZE];
kiss_fftr_cfg  kiss_fftr_state;


short calculate_FIR(filter_type *filter,int data)
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

int calculate_average(filter_type *filter,int data)
{
	 int i = 0;
	 int output = 0;
	 filter->n ++;
	 if(filter->n == FIR_SIZE*2) filter->n = FIR_SIZE; 
	 filter->maqs[filter->n] = filter->maqs[filter->n-FIR_SIZE] = data;
	 for( i = 0; i < FIR_SIZE; i++){
	 	output = output + filter->maqs[filter->n-i];
	 }
	 return output/FIR_SIZE;
}


int hamming_window512[512] = {1311,1311,1313,1316,1320,1325,1331,1339,1347,1357,1368,1380,1393,1407,1422,1439,1456,1475,1495,1515,1537,1561,1585,1610,1637,1664,1693,1722,1753,1785,1818,1852,1887,1923,1960,1998,2037,2077,2119,2161,2204,2248,2294,2340,2387,2435,2484,2535,2586,2638,2691,2745,2799,2855,2912,2969,3028,3087,3147,3208,3270,3333,3397,3461,3526,3592,3659,3727,3795,3864,3934,4005,4076,4148,4221,4295,4369,4444,4519,4595,4672,4750,4828,4907,4986,5066,5146,5227,5309,5391,5473,5556,5640,5724,5809,5894,5979,6065,6152,6238,6325,6413,6501,6589,6678,6767,6856,6945,7035,7125,7216,7306,7397,7488,7579,7671,7762,7854,7946,8038,8130,8223,8315,8407,8500,8593,8685,8778,8871,8963,9056,9148,9241,9334,9426,9518,9611,9703,9795,9887,9978,10070,10161,10252,10343,10434,10524,10615,10705,10794,10884,10973,11061,11150,11238,11326,11413,11500,11586,11673,11758,11843,11928,12013,12096,12180,12263,12345,12427,12508,12589,12669,12749,12828,12906,12984,13061,13137,13213,13289,13363,13437,13510,13583,13654,13725,13796,13865,13934,14002,14069,14135,14201,14266,14330,14393,14455,14517,14578,14637,14696,14754,14811,14867,14923,14977,15031,15083,15135,15185,15235,15284,15331,15378,15424,15469,15512,15555,15597,15638,15677,15716,15754,15790,15826,15860,15894,15926,15957,15987,16017,16045,16072,16097,16122,16146,16168,16190,16210,16229,16248,16265,16280,16295,16309,16321,16333,16343,16352,16360,16367,16372,16377,16380,16383,16384,16384,16383,16380,16377,16372,16367,16360,16352,16343,16333,16321,16309,16295,16280,16265,16248,16229,16210,16190,16168,16146,16122,16097,16072,16045,16017,15987,15957,15926,15894,15860,15826,15790,15754,15716,15677,15638,15597,15555,15512,15469,15424,15378,15331,15284,15235,15185,15135,15083,15031,14977,14923,14867,14811,14754,14696,14637,14578,14517,14455,14393,14330,14266,14201,14135,14069,14002,13934,13865,13796,13725,13654,13583,13510,13437,13363,13289,13213,13137,13061,12984,12906,12828,12749,12669,12589,12508,12427,12345,12263,12180,12096,12013,11928,11843,11758,11673,11586,11500,11413,11326,11238,11150,11061,10973,10884,10794,10705,10615,10524,10434,10343,10252,10161,10070,9978,9887,9795,9703,9611,9518,9426,9334,9241,9148,9056,8963,8871,8778,8685,8593,8500,8407,8315,8223,8130,8038,7946,7854,7762,7671,7579,7488,7397,7306,7216,7125,7035,6945,6856,6767,6678,6589,6501,6413,6325,6238,6152,6065,5979,5894,5809,5724,5640,5556,5473,5391,5309,5227,5146,5066,4986,4907,4828,4750,4672,4595,4519,4444,4369,4295,4221,4148,4076,4005,3934,3864,3795,3727,3659,3592,3526,3461,3397,3333,3270,3208,3147,3087,3028,2969,2912,2855,2799,2745,2691,2638,2586,2535,2484,2435,2387,2340,2294,2248,2204,2161,2119,2077,2037,1998,1960,1923,1887,1852,1818,1785,1753,1722,1693,1664,1637,1610,1585,1561,1537,1515,1495,1475,1456,1439,1422,1407,1393,1380,1368,1357,1347,1339,1331,1325,1320,1316,1313,1311,1311};

filter_type averages[5] = {{.n=FIR_SIZE},{.n=FIR_SIZE},{.n=FIR_SIZE},{.n=FIR_SIZE},{.n=FIR_SIZE}};

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

// unsigned char color_array[13][12][3] = {
//    {{255,102,102},{255,178,102},{254,255,102},{178,255,102},{102,255,102},{102,255,178},{102,254,255},{102,178,255},{102,102,255},{178,102,255},{255,102,254},{255,102,178}},
//    {{242,0,0},{242,121,0},{242,242,0},{121,242,0},{0,242,0},{0,242,121},{0,242,242},{0,121,242},{0,0,242},{121,0,242},{242,0,242},{242,0,121}},
//    {{164,0,0},{164,82,0},{164,164,0},{82,164,0},{0,164,0},{0,164,82},{0,164,164},{0,82,164},{0,0,164},{82,0,164},{164,0,164},{164,0,82}},
//    {{112,0,0},{112,56,0},{112,112,0},{56,112,0},{0,112,0},{0,112,56},{0,112,112},{0,56,112},{0,0,112},{56,0,112},{112,0,112},{112,0,56}},
//    {{76,0,0},{76,38,0},{76,76,0},{38,76,0},{0,76,0},{0,76,38},{0,76,76},{0,38,76},{0,0,76},{38,0,76},{76,0,76},{76,0,38}},
//    {{51,0,0},{51,25,0},{51,51,0},{25,51,0},{0,51,0},{0,51,25},{0,51,51},{0,25,51},{0,0,51},{25,0,51},{51,0,51},{51,0,25}},
//    {{35,0,0},{35,17,0},{35,35,0},{17,35,0},{0,35,0},{0,35,17},{0,35,35},{0,17,35},{0,0,35},{17,0,35},{35,0,35},{35,0,17}},
//    {{23,0,0},{23,11,0},{23,23,0},{11,23,0},{0,23,0},{0,23,11},{0,23,23},{0,11,23},{0,0,23},{11,0,23},{23,0,23},{23,0,11}},
//    {{16,0,0},{16,8,0},{16,16,0},{8,16,0},{0,16,0},{0,16,8},{0,16,16},{0,8,16},{0,0,16},{8,0,16},{16,0,16},{16,0,8}},
//    {{11,0,0},{11,5,0},{11,11,0},{5,11,0},{0,11,0},{0,11,5},{0,11,11},{0,5,11},{0,0,11},{5,0,11},{11,0,11},{11,0,5}},
//    {{7,0,0},{7,3,0},{7,7,0},{3,7,0},{0,7,0},{0,7,3},{0,7,7},{0,3,7},{0,0,7},{3,0,7},{7,0,7},{7,0,3}},
//    {{5,0,0},{5,2,0},{5,5,0},{2,5,0},{0,5,0},{0,5,2},{0,5,5},{0,2,5},{0,0,5},{2,0,5},{5,0,5},{5,0,2}},
//    {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
// };
unsigned char color_array[13][12][3] = {
   {{255,101,101},{255,178,101},{254,255,101},{178,255,101},{101,255,101},{101,255,178},{101,254,255},{101,178,255},{101,101,255},{178,101,255},{255,101,254},{255,101,178}},
   {{255,27,27},{255,141,27},{254,255,27},{141,255,27},{27,255,27},{27,255,141},{27,254,255},{27,141,255},{27,27,255},{141,27,255},{255,27,254},{255,27,141}},
   {{223,0,0},{223,111,0},{223,223,0},{111,223,0},{0,223,0},{0,223,111},{0,223,223},{0,111,223},{0,0,223},{111,0,223},{223,0,223},{223,0,111}},
   {{176,0,0},{176,88,0},{176,176,0},{88,176,0},{0,176,0},{0,176,88},{0,176,176},{0,88,176},{0,0,176},{88,0,176},{176,0,176},{176,0,88}},
   {{139,0,0},{139,69,0},{139,139,0},{69,139,0},{0,139,0},{0,139,69},{0,139,139},{0,69,139},{0,0,139},{69,0,139},{139,0,139},{139,0,69}},
   {{110,0,0},{110,55,0},{110,110,0},{55,110,0},{0,110,0},{0,110,55},{0,110,110},{0,55,110},{0,0,110},{55,0,110},{110,0,110},{110,0,55}},
   {{87,0,0},{87,43,0},{87,87,0},{43,87,0},{0,87,0},{0,87,43},{0,87,87},{0,43,87},{0,0,87},{43,0,87},{87,0,87},{87,0,43}},
   {{68,0,0},{68,34,0},{68,68,0},{34,68,0},{0,68,0},{0,68,34},{0,68,68},{0,34,68},{0,0,68},{34,0,68},{68,0,68},{68,0,34}},
   {{54,0,0},{54,27,0},{54,54,0},{27,54,0},{0,54,0},{0,54,27},{0,54,54},{0,27,54},{0,0,54},{27,0,54},{54,0,54},{54,0,27}},
   {{43,0,0},{43,21,0},{43,43,0},{21,43,0},{0,43,0},{0,43,21},{0,43,43},{0,21,43},{0,0,43},{21,0,43},{43,0,43},{43,0,21}},
   {{34,0,0},{34,17,0},{34,34,0},{17,34,0},{0,34,0},{0,34,17},{0,34,34},{0,17,34},{0,0,34},{17,0,34},{34,0,34},{34,0,17}},
   {{26,0,0},{26,13,0},{26,26,0},{13,26,0},{0,26,0},{0,26,13},{0,26,26},{0,13,26},{0,0,26},{13,0,26},{26,0,26},{26,0,13}},
	{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
};


char address_table[5] = {9,4,2,7,3};
char color_order[12] = {11,0,9,2,6,4,10,1,5,7,3,8};

FILE *SERIAL;


const int band_min[NUM_LIGHTS] = {0,4,10,57,113};
const int band_max[NUM_LIGHTS] = {3,9,56,112,256};
//const float threshold[NUM_LIGHTS] = {5,5,5.5,5.5,4.5};
const float threshold[NUM_LIGHTS] = {2.5,2.5,2.5,2.75,2.75,2.25};
void sendColor(char address,char r, char g,char b,char v)
{

//address = address + 1;
fprintf(SERIAL, "4,%d,%d,%d,%d,%d;\n",address,r,g,b,v);	
}



void main(void){

	
	int sum,average,level,level_last,count;
   char interval =0;
   int max_timeout ;
   float ratio;
   int r,g,b;
   int lights;
	int peak[5];
   int peak_last[5];
   int avg[5];
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
	color[0] = max[0] = avg[0] = 0;
	color[1] = max[1] =avg[1] = 0;
	color[2] = max[2] =avg[2] = 0;
	color[3] = max[3] =avg[3] = 0;
	color[4] = max[4] =avg[4] = 0;
	
   interval =0;
   max_timeout = 50;
	while(bytes_read==BUFFER_SIZE){
      // bytes_read = fread(&audio_data,sizeof(short),512,stdin);
      bytes_read = fread(&audio_data,sizeof(short),BUFFER_SIZE,stdin);
   
      for(i = 0; i< BUFFER_SIZE;i++){
         rin[i] = (audio_data[i]*hamming_window512[i])/16384;
      }
		
      
      
     
    
		
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
		if(interval == 0){
		   kiss_fftr( kiss_fftr_state, rin, sout );
		   for(lights = 0;lights<NUM_LIGHTS; lights++){
            sum = 0;
            count = 0;
            for(i = band_min[lights];i<=band_max[lights];i++){
               sum += sqrt(sout[i].r*sout[i].r + sout[i].i*sout[i].i);
               count ++;
            }
            peak[lights] = sum/ count ;
            avg[lights] = calculate_average(&(averages[lights]),peak[lights]);
         }
      
         for(j=0;j<NUM_LIGHTS;j++){
            if(peak[j]>max[j]){
                 max[j] = peak[j];
            }
                 ratio =(avg[j])? ((float)peak[j])/avg[j]:0;
                 level =(int)(threshold[j]*ratio);
                 if(level > 12) level = 12;
                 
                 ratio =(avg[j])? ((float)peak_last[j])/avg[j]:0;
                 level_last =(int)(threshold[j]*ratio);
                 if(level_last > 12) level_last = 12;
                 
                 
                 if(level <1 && level_last >1){
                    color[j] ++;
                    color[j] %=12;
                 }
                 peak_last[j] = peak[j];
                 
                 level = (max[j])? (12*peak[j])/max[j]:0;
                 printf("%d:%8d ",level,avg[j]);
                 r = color_array[12-level][color_order[color[j]]][0];
                 g = color_array[12-level][color_order[color[j]]][1];
                 b = color_array[12-level][color_order[color[j]]][2];
                 // if(level>1){
                  sendColor(address_table[j],r,g,b,0);
                 // }else{
                 //   sendColor(address_table[j],0,0,0,0);
                 // }
              //sendColor(9,0,255,0,0);
                 if(max_timeout == 0){
                    printf("Avg[%d]=%d ",j,avg[j]);
                    max[j]= (max[j]/4 > avg[j])? max[j]-(max[j]-avg[j])/2: max[j];
                    
                 }
         }
         printf ("\n");
         
         if(max_timeout == 0){
            max_timeout = 300;
         }
         max_timeout --;
      }
           
      interval ++;
      interval %=2;
      


		//printf("%8d:%8d:%8d:%8d:%8d\n", peak[0],peak[1],peak[2],peak[3],peak[4]);
		//printf("%d,%d,%d,%d,%d\n", peak[0],peak[1],peak[2],peak[3],peak[4]);
	}
	free(kiss_fftr_state);
}
