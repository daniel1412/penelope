#include "ADS1256.h"

ADS1256 ads;

void setup() 
{
	Serial.begin(9600);
	ads.init(5, 14, 25, 1700000);
	Serial.println(ads.speedSPI);
}

void loop() 
{
	ads.readInputToAdcValuesArray();
	for( int i = 0; i <= 7; i++ )
	{
    int valueRaw = ads.adcValues[i];
    float value = valueRaw*5.0/0x7fffff;
		Serial.print(value);   // Raw ADC integer value +/- 23 bits
		Serial.print( "      " );
	}
	Serial.println();
	delay( 500 );
}
