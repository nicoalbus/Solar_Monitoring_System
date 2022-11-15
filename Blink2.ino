
#define BLYNK_TEMPLATE_ID           "TMPLcKfUubKA"
#define BLYNK_DEVICE_NAME           "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "7UdjrCzKCYAimYeG2uvAQX7jEvgChEWa"
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>

#define VOLTAGE_SCALE     2


char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Albusuaifai";     // SSID:  Totalplay-2FA6     Albusuaifai
char pass[] = "********";        //      

BlynkTimer timer;

const int oneWireBus = 4;     
OneWire oneWire(oneWireBus); 
DallasTemperature sensors(&oneWire);

unsigned long last_time =0;
unsigned long current_time =0;
float power1 = 0 ; // Power in Watt
float energy =0 ; // Emergy in Watt-Hour

// This function sends Arduino's uptime every second to Virtual Pin 2.
void sendSensor()
{
  // temperature
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0)- 1.5;
  Serial.print(temperatureC);
  Serial.println(" C");
  Blynk.virtualWrite(V0, temperatureC);

  // light intensity
  float lux = ( return_lux(32));
  Serial.print(lux); Serial.println(" lux");
  Blynk.virtualWrite(V1,lux);
  
  // Voltage
  float v = abs( return_voltage_value(34)) ;    // GPIO 34 for voltage
  Serial.print(v);
  Serial.println(" Volt");
  Blynk.virtualWrite(V2,v);

  // Current
  float current = v / 3.989;
  Serial.print(current);
  Serial.print("  mA");
  Blynk.virtualWrite(V3,current);

  // Calculate power and energy
  power1 = current * v ; // calculate power in mWatt
  last_time = current_time;
  current_time = millis();    
  energy = energy +  power1 *(( current_time -last_time) /3600000.0) ; // calculate power in Watt-Hour // 1 Hour = 60mins x 60 Secs x 1000 Milli Secs
  Serial.print(power1,5);
  Serial.println("  mW");
  Serial.print(energy);
  Serial.println("  mWh");
  Blynk.virtualWrite(V4, power1);
  Blynk.virtualWrite(V6,energy);
  Serial.println();
  
  float saving = 6.5 * ( energy /1000 ); // 6.5 is cost per kWh // used just for example
  // Send status to widget
  WidgetLED Status(V5);
  if (v > 0.5) {
    Status.on();} else {
      Status.off();
  }
  // Send warning notification
  if ((lux > 10) && (v <0.5)){
    Blynk.logEvent("warning");
  }
}

double return_voltage_value(int pin_no)
{
  double tmp = 0;
  double ADCVoltage = 0;
  double inputVoltage = 0;
  double avg = 0;
  for (int i = 0; i < 100; i++)
  {
    tmp = tmp + analogRead(pin_no);
  }
  avg = tmp / 100;
  ADCVoltage = ((avg * 3.3) / (4095))+ 0.07; // 0.184 is offset adjust by heat and try
  inputVoltage = ADCVoltage * VOLTAGE_SCALE;
  delay(200);
  return inputVoltage;
}

double return_lux(int pin){
  double avg1 = 0;
  double tmp1 = 0;
  for (int i = 0; i < 100; i++)
  {
    tmp1 = tmp1 + analogRead(pin);
  }
  avg1 = tmp1 / 100;
  float Vout = avg1*(3.3/4095);
  float RLDR = (10000 * (3.3 - Vout))/Vout; 
  float lux1 = 500/(RLDR/1000); // Conversion resitance to lumen
  return lux1;
}

void setup()
{
  
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);

  sensors.begin();
  
  timer.setInterval(1000L, sendSensor);
}

void loop()
{
 
  Blynk.run();
  timer.run();
  
}
