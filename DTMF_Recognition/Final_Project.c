
#include <math.h>
#include "audio.h"
#include "platform.h"
#include "gpio.h"
#include <stdio.h>


volatile int16_t audio_chR =0;
volatile int16_t audio_chL =0;


#define N 4096
#define SAMPLING_FREQ 48000
#define A	PTD3
#define B	PTD1
#define C	PTB23
#define D	PTC3
#define E	PTC2
#define F	PTD2
#define G	PTD0

 short flag = 0;
 float temp;
 typedef struct 
 {
 float real; 
 float imag; 
 } COMPLEX; 

 COMPLEX samples[N]; 
 COMPLEX twiddle[N]; 
 
 //This represent the pressed keyPad
 char Button [4][3] = {{'1','2','3'}
                      ,{'4','5','6'}
                      ,{'7','8','9'}
                      ,{'*','0','#'}};
 											
	
float Magnitude[N];//This will hold the computed magnitude of the FFT values
float Magnitude1[N];//This will be used as an auxilliary storage for computation purpose
int index_frequencies[2];//This will store the detected frequencies' indexes												
double tone_frequencies[2];//This will be used to store the 											

int buf_ptr=0;//This will be used to iterate through the buffer
float32_t	buffer[N];//This will be used to stored the samples										
void I2S_HANDLER ( void ) {
 audio_IN = i2s_rx (); // 32 - bits ; 16 - bits channel left + 16 - bits channel right
 audio_chL = ( audio_IN & 0x0000FFFF );//retrieving the left channel data
 audio_chR = (( audio_IN >>16)& 0x0000FFFF );//retrieving the right channel data
	
 buffer[buf_ptr++] = (( audio_chR <<16 & 0xFFFF0000 )); //populating the buffer with the retrieved data
	 //This indicates that the buffer is full and ready to be processed
	 if(buf_ptr >= N)
	{
		buf_ptr = 0;
		flag =1;
	}
 audio_OUT = (( audio_chR <<16 & 0xFFFF0000 )) + ( audio_chL & 0x0000FFFF ); //reconstructing the original sound
 i2s_tx ( audio_OUT );//playing the sound
}

//This function will be used to display value on the seven segment display
 void seven_seg(char num)
 {
	 if (num == '0')
	 {
		gpio_set(A,1);
	 	gpio_set(B,1);
	 	gpio_set(C,1);
	 	gpio_set(D,1);
	 	gpio_set(E,1);
	 	gpio_set(F,1);
	 	gpio_set(G,0);
	 }
	 else if (num == '1')
	 {
		gpio_set(A,0);
	 	gpio_set(B,1);
	 	gpio_set(C,1);
	 	gpio_set(D,0);
	 	gpio_set(E,0);
	 	gpio_set(F,0);
	 	gpio_set(G,0);
	 }
	 
	  else if (num == '2')
	 {
		gpio_set(A,1);
	 	gpio_set(B,1);
	 	gpio_set(C,0);
	 	gpio_set(D,1);
	 	gpio_set(E,1);
	 	gpio_set(F,0);
	 	gpio_set(G,1);
	 }
	  else if (num == '3')
	 {
		gpio_set(A,1);
	 	gpio_set(B,1);
	 	gpio_set(C,1);
	 	gpio_set(D,1);
	 	gpio_set(E,0);
	 	gpio_set(F,0);
	 	gpio_set(G,1);
	 }
	  else if (num == '4')
	 {
		gpio_set(A,0);
	 	gpio_set(B,1);
	 	gpio_set(C,1);
	 	gpio_set(D,0);
	 	gpio_set(E,0);
	 	gpio_set(F,1);
	 	gpio_set(G,1);
	 }
	  else if (num == '5')
	 {
		gpio_set(A,1);
	 	gpio_set(B,0);
	 	gpio_set(C,1);
	 	gpio_set(D,1);
	 	gpio_set(E,0);
	 	gpio_set(F,1);
	 	gpio_set(G,1);
	 }
	  else if (num == '6')
	 {
	 	gpio_set(A,1);
	 	gpio_set(B,0);
	 	gpio_set(C,1);
	 	gpio_set(D,1);
	 	gpio_set(E,1);
	 	gpio_set(F,1);
		gpio_set(G,1); 
	 }
	  else if (num == '7')
	 {
		gpio_set(A,1);
	 	gpio_set(B,1);
	 	gpio_set(C,1);
	 	gpio_set(D,0);
	 	gpio_set(E,0);
	 	gpio_set(F,0);
	 	gpio_set(G,0);
	 }
	  else if (num == '8')
	 {
		gpio_set(A,1);
	 	gpio_set(B,1);
	 	gpio_set(C,1);
	 	gpio_set(D,1);
	 	gpio_set(E,1);
	 	gpio_set(F,1);
	 	gpio_set(G,1);
	 }
	  else if (num == '9')
	 {
		gpio_set(A,1);
	 	gpio_set(B,1);
	 	gpio_set(C,1);
	 	gpio_set(D,1);
	 	gpio_set(E,0);
	 	gpio_set(F,1);
	 	gpio_set(G,1);
	 }
	 else if (num == '*')
	 {
		gpio_set(A,1);
	 	gpio_set(B,1);
	 	gpio_set(C,1);
	 	gpio_set(D,0);
	 	gpio_set(E,1);
	 	gpio_set(F,1);
	 	gpio_set(G,1);
	 }
	 else if (num == '#')
	 {
		gpio_set(A,1);
	 	gpio_set(B,1);
	 	gpio_set(C,0);
	 	gpio_set(D,0);
	 	gpio_set(E,1);
	 	gpio_set(F,1);
	 	gpio_set(G,1);
	 }
 }
 
 //This will be used to compute the FFT of the signal obtained
 void fft(COMPLEX *Y, int M, COMPLEX *w) //input sample array, number of points 
{ 

		 COMPLEX temp1,temp2; //temporary storage variables 
		 int i,j,k; //loop counter variables 
		 int upper_leg, lower_leg; //index of upper/lower butterfly leg 
		 int leg_diff; //difference between upper/lower leg
		 int num_stages=0; //number of FFT stages, or iterations 
		 int index, step; //index and step between twiddle factor 
     i=1; //log(base 2) of # of points = # of stages 
		 
		 
     do 
     { 
      num_stages+=1; 
      i=i*2; 
     }while (i!=M);

     leg_diff=M/2; //starting difference between upper & lower legs 
     step=2; //step between values in twiddle.h 


 
		 for (i=0;i<num_stages;i++) //for M-point FFT 
 
		 { 
			 index=0; 
			 for (j=0;j<leg_diff;j++)
			 {
				 for (upper_leg=j;upper_leg<M;upper_leg+=(2*leg_diff)) 
				 { 
					 lower_leg=upper_leg+leg_diff; 
			     temp1.real=(Y[upper_leg]).real + (Y[lower_leg]).real; 
			     temp1.imag=(Y[upper_leg]).imag + (Y[lower_leg]).imag; 
			     temp2.real=(Y[upper_leg]).real - (Y[lower_leg]).real; 
			     temp2.imag=(Y[upper_leg]).imag - (Y[lower_leg]).imag; 
			     (Y[lower_leg]).real=temp2.real*(w[index]).real-temp2.imag*(w[index]).imag; 
			     (Y[lower_leg]).imag=temp2.real*(w[index]).imag+temp2.imag*(w[index]).real; 
			     (Y[upper_leg]).real=temp1.real; 
			     (Y[upper_leg]).imag=temp1.imag; 
				 } 
				 index+=step; 
			 } 
			 leg_diff=leg_diff/2; 
			 step*=2; 
			 }
		 j=0; 
			 for (i=1;i<(M-1);i++) //bit reversal for resequencing data*/ 
			 { 
				 k=M/2; 
				 while (k<=j) 
					 { 
						 j=j-k; 
						 k=k/2; 
					 } 
					 j=j+k;
					 if (i<j) 
						 { 
							 temp1.real=(Y[j]).real; 
			         temp1.imag=(Y[j]).imag; 
			         (Y[j]).real=(Y[i]).real; 
	             (Y[j]).imag=(Y[i]).imag; 
							 (Y[i]).real=temp1.real; 
							 (Y[i]).imag=temp1.imag; 
						 }
			 } 
			 return; 
			 
	 }

 int main ( void )
 {
	 //-------------------Variable Declaration----------------------------
	 int n; //For loop iterator
	 int r; //row index
	 int c; //coulmn index
	 double higher_limit_frequency;//EXPECTED + 3.5%EXPECTED
	 double lower_limit_frequency;//EXPECTED - 3.5%EXPECTED
	 double offset;//3.5%EXPECTED
	 int isFreqValid1 = 1;//Used to confirm presence of tone
	 int isFreqValid2	= 1;//Used to confirm presence of tone
	 int i = 0;//initializing the row and column frequencies for the DTMF
	 double row_values[4] = {697, 770, 852,941}; // DTMF row frequencies
	 double col_values[3] = {1209, 1336, 1477}; // DTMF col frequencies
	 float fmax = 0;//VALUE HOLDER
	 double Column_freq[3];//Differences will be computed into this array
	 double Row_freq[4];//Differences will be computed into this array
	 char pressed_key;//Hold the detected key
	 char previous_pressed_key;//Hold the previously found key
	 int min;//VaLUE HOLDER
	 
	 
		//Computing the twiddle factor
		for(n=0 ; n<N ; n++) 
	 { 
		 twiddle[n].real = cos(PI*n/N); 
		 twiddle[n].imag = -sin(PI*n/N); 
	 }
	 
		//Setting a few pins as output for the seven segment display
		gpio_set_mode(A,Output);
	 	gpio_set_mode(B,Output);
	 	gpio_set_mode(C,Output);
	 	gpio_set_mode(D,Output);
	 	gpio_set_mode(E,Output);
	 	gpio_set_mode(F,Output);
	 	gpio_set_mode(G,Output);
	 
	 //Set the seven segment display to 0
	  gpio_set(A,0);
	 	gpio_set(B,0);
	 	gpio_set(C,0);
	 	gpio_set(D,0);
	 	gpio_set(E,0);
	 	gpio_set(F,0);
	 	gpio_set(G,0);
		
		//Using the built in mic 
		//Using a sampling frequency of 48Kh
		audio_init ( hz48000 , dmic_in , intr , I2S_HANDLER );
	
	while (1)
	 {
			//When the buffer gets full, We enter this section of the programm
			while(flag == 1)
			{		
		 
			//Clear the sample array used to compute the fast fourier transform.	
		 for (n=0 ; n<N ; n++)
			{ 
				samples[n].real = 0.00;
				samples[n].imag = 0.00;
			}
			
			//Populate the sample array real part with the buffer array 
			//Polulate the sample array imaginary part with zeroes
		  for(n=0 ; n<N ; n++)
			{ 
				samples[n].real = buffer[n];
				samples[n].imag = 0.00; 
			}
		  fft(samples,N,twiddle); //call fast fourier transform function	
			
			// compute magnitude of signal
			for (n=0 ; n<4096 ; n++) 
			{ 
				Magnitude[n] = ((samples[n].real*samples[n].real) + (samples[n].imag*samples[n].imag));
			}
//-----------------------------------------------------------------------------------------------------				
			//Finding the highest frequency index in the bins
			fmax  = Magnitude[1];//start off assuming that the 1st element is the max
			index_frequencies[0] = 1;
			for (i = 1; i < 512; i++)//now compare it with the rest of the array, updataing the max all along
			{
				if (Magnitude[i] > fmax) 
				{
					fmax = Magnitude[i];
					index_frequencies[0] = i;
				}	
			}
			tone_frequencies[0] = (index_frequencies[0]*SAMPLING_FREQ)/N;		
			//Highest frequency found and located in tone_frequencies[0]
//--------------------------------------------------------------------------------------------------		
			//Compute high frequency error
			for(n=0;n<3;n++)
			{
				Column_freq[n] = tone_frequencies[0] - col_values[n];
				Column_freq[n] = sqrt(Column_freq[n]*Column_freq[n]);
			}
			//error is computed and located in Colum_freq
//--------------------------------------------------------------------------------------------------			
			//Compute minimum error index of column frequency 
			min =Column_freq[0];
			index_frequencies[0] = 0;
			for (n=0; n<3; n++)
			{
				if (min > Column_freq[n])
				{
					min = Column_freq[n];
					index_frequencies[0] = n;
				}
			}
			//minimum error index computed and located in index_frequencies[0]
			//c gets the index of the minimum error
			c = index_frequencies[0];

			//Decide whether to accept or reject the frequency obtained for more accurate detection
			offset = (col_values[c] * 3.5)/100;
			higher_limit_frequency = col_values[c] + offset;
			lower_limit_frequency = col_values[c] - offset;
			if(tone_frequencies[0] > lower_limit_frequency && tone_frequencies[0] < higher_limit_frequency)
			{
				isFreqValid1 = 1;
			}	
			else
			{
				isFreqValid1 = 0;
			}	
//-------------------------------------------------------------------
			//Finding the highest frequency index in the bins
			i = 0;
			for(n=0; n<512; n++)
			{
				if(n>59 && n<81)
        {
					Magnitude1[i++] = Magnitude[n];
				}
			}
			
			fmax = Magnitude1[0];
			for (n=0; n<i; n++)
			{
				if (Magnitude1[n] > fmax)
				{
					fmax = Magnitude1[n];
					index_frequencies[1] = n;
				}
			}
			index_frequencies[1] = index_frequencies[1] +59;
			tone_frequencies[1] = ((index_frequencies[1])*SAMPLING_FREQ)/N;	
			//Lowest frequency found and located in tone_frequencies[1]
//----------------------------------------------------------------------------------------------------			
			//Compute low frequency error
			for(n=0;n<4;n++)
			{
				Row_freq[n] = tone_frequencies[1] - row_values[n];
				Row_freq[n] = sqrt(Row_freq[n]*Row_freq[n]);
			}
			//error is computed and located in Row_freq
//----------------------------------------------------------------------------------------------------			
			//Compute minimum row frequency
			min = Row_freq[0];
			index_frequencies[1] = 0;
			for (n=0; n<4; n++)
			{
				if (min > Row_freq[n])
				{
					min = Row_freq[n];
					index_frequencies[1] = n;
				}
			}
			//minimum error is computed and located in index_frequencies[1];
			r = index_frequencies[1];
			
				//Decide whether to accept or reject the frequency obtained for more accury detection
			offset = (row_values[r] * 3.5)/100;
			higher_limit_frequency = row_values[r] + offset;
			lower_limit_frequency = row_values[r] - offset;
			if(tone_frequencies[1] > lower_limit_frequency && tone_frequencies[1] < higher_limit_frequency)
			{
				isFreqValid2 = 1;
			}	
			else
			{
				isFreqValid2 = 0;
			}
//----------------------------------------------------------------------------------------------------
			//This will check for valide frequencies and proceed on if a tone is detected
			//If no tone is detected, we go back to the interrupt
			if (isFreqValid1 && isFreqValid2)
			{
				pressed_key = Button [r][c];
				previous_pressed_key = pressed_key;
				seven_seg(pressed_key);//This will display the value
				flag = 0;
			}
			else
			{
				flag = 0;
			}
			
			}
			}
 }	
 
