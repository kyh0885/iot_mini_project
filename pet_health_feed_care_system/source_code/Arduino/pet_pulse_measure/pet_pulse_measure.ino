/*

	Example of use of the FFT library
        Copyright (C) 2014 Enrique Condes

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
  In this example, the Arduino simulates the sampling of a sinusoidal 1000 Hz
  signal with an amplitude of 100, sampled at 5000 Hz. Samples are stored
  inside the vReal array. The samples are windowed according to Hamming
  function. The FFT is computed using the windowed samples. Then the magnitudes
  of each of the frequencies that compose the signal are calculated. Finally,
  the frequency with the highest peak is obtained, being that the main frequency
  present in the signal.
*/

#include <string.h>
#include "arduinoFFT.h"
#include <stdio.h>
#include <stdlib.h>

#define bt_rx 2
#define bt_tx 3

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
These values can be changed in order to evaluate the functions
*/
const uint16_t samples = 512; //This value MUST ALWAYS be a power of 2
const uint16_t signalFrequency = 1000;
const uint16_t samplingFrequency = 20;
const uint8_t amplitude = 100;
/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
double vReal[samples];
double vImag[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

char buf[20];
int j = 0;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);
  while(!Serial);
  Serial.println("start main");
  int i = 0;
  Serial.println("Ready");
}

void loop()
{

  for (uint16_t i = 0; i < samples; i++)
  {
    int pulse = analogRead(A0);
    vReal[i] = pulse;
    vImag[i] = 0.0; 
    Serial.println(pulse);
    delay(50);
  }
  Serial.println("measerment end");
  /* Print the results of the simulated sampling according to time */
  //Serial.println("Data:");
  //PrintVector(vReal, samples, SCL_TIME);
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);	/* Weigh data */
  //Serial.println("Weighed data:");
  //PrintVector(vReal, samples, SCL_TIME);
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
  //Serial.println("Computed Real values:");
  //PrintVector(vReal, samples, SCL_INDEX);
  //Serial.println("Computed Imaginary values:");
  //PrintVector(vImag, samples, SCL_INDEX);
  FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */

  double maxVal = 0;
  int maxIdx = 0;
  for( int i = 3; i < samples;i++ ){
    if( maxVal < vReal[i]){
      maxVal = vReal[i];
      maxIdx = i;
    } 
  }

  //btSerial.write("AT+");
  //Serial.println("Computed magnitudes:");
  PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);
  double x = FFT.MajorPeak(vReal, samples, samplingFrequency);
  int result = int(x*60);
  char temp[20]= {0,};
  sprintf(temp, "[udb]hel@%d\r\n", result);
  Serial1.write(temp);
}

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
	break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
	break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
	break;
    }
    Serial.print(abscissa, 6);
    if(scaleType==SCL_FREQUENCY)
      //Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}
