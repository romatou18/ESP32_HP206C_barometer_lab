#include <HP20x_dev.h>
#include "Arduino.h"
#include "Wire.h" 
#include <KalmanFilter.h>
unsigned char ret = 0;

/* Instance */
grove::KalmanFilter t_filter;    //temperature filter
grove::KalmanFilter p_filter;    //pressure filter
grove::KalmanFilter a_filter;    //altitude filter


#define I2C_CTRL 1
#if I2C_CTRL == 0
#define WireInstance Wire
#define SDA 21
#define SCL 22
#define FREQ 400000L

#else 
#define WireInstance Wire1
#define SDA 27
#define SCL 26
#define FREQ 400000L

#endif
HP20x_dev HP20x(I2C_CTRL, SDA, SCL, FREQ);
#define ENABLE_I2C_DEBUG_BUFFER 1

void scanner()
{
	 byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    WireInstance.beginTransmission(address);
    error = WireInstance.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(2000);  
}

void setup()
{  
  Serial.begin(9600);        // start serial for output
  
  Serial.println("****HP20x_dev demo by seeed studio****\n");
  Serial.println("Calculation formula: H = [8.5(101325-P)]/100 \n");
  WireInstance.begin(SDA, SCL, FREQ);
  scanner();
  /* Power up,delay 150ms,until voltage is stable */
  delay(150);
  /* Reset HP20x_dev */
  HP20x.begin();
  delay(1000);
  
  /* Determine HP20x_dev is available or not */
  bool found = false;
  while(!found)
  {
	ret = HP20x.isAvailable();
	if(OK_HP20X_DEV == ret)
	{
		Serial.println("HP20x_dev is available.\n");   
		found = true; 
	}
	else
	{
		Serial.println("HP20x_dev isn't available.\n");
		delay(500);
		ret = HP20x.isAvailable();
	}
  }
  
}
 

void loop()
{
    char display[40];
    if(OK_HP20X_DEV == ret)
    { 
	  Serial.println("------------------\n");
	  long Temper = HP20x.ReadTemperature();
	  Serial.println("Temper:");
	  float t = Temper/100.0;
	  Serial.print(t);	  
	  Serial.println("C.\n");
	  Serial.println("Filter:");
	  Serial.print(t_filter.Filter(t));
	  Serial.println("C.\n");
 
      long Pressure = HP20x.ReadPressure();
	  Serial.println("Pressure:");
	  float p = Pressure/100.0;
	  Serial.print(p);
	  Serial.println("hPa.\n");
	  Serial.println("Filter:");
	  Serial.print(p_filter.Filter(p));
	  Serial.println("hPa\n");
	  
	  long Altitude = HP20x.ReadAltitude();
	  Serial.println("Altitude:");
	  float a = Altitude/100.0;
	  Serial.print(a);
	  Serial.println("m.\n");
	  Serial.println("Filter:");
	  Serial.print(a_filter.Filter(a));
	  Serial.println("m.\n");
	  Serial.println("------------------\n");
      delay(1000);
    }
}