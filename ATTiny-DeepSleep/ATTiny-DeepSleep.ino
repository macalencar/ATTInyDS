#include "DHT.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#define SLEEP_MINUTES 1 //15
#define WD_SECONDS 5 //60
#define DEVICE_ID "0x100"
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

byte saveADCSRA;                  
volatile byte counterWD = 0;      //WDCounter
volatile byte counterHOUR= 0;     //minutes counter 


void sleepNow(){
  set_sleep_mode ( SLEEP_MODE_PWR_DOWN ); 
  saveADCSRA = ADCSRA;                    
  ADCSRA = 0;                             
  power_all_disable ();
  noInterrupts ();                       
  resetWatchDog ();                       
  sleep_enable ();                        
  interrupts ();                      
  sleep_cpu ();                         
  sleep_disable ();                    
  power_all_enable ();                   
  // ADCSRA = saveADCSRA;
}

void resetWatchDog(){
  MCUSR = 0;
  WDTCR = bit ( WDCE ) | bit ( WDE ) | bit ( WDIF ); 
  WDTCR = bit ( WDIE ) | bit ( WDP2 )| bit ( WDP1 );                                         
  wdt_reset (); 
}

ISR(WDT_vect){
  wdt_disable (); 
  counterWD ++;                    
}

void setup() {
  Serial.begin(9600);
  resetWatchDog();
  dht.begin();
}

void loop() {
  if ( counterWD == (WD_SECONDS-1) ){                 
    if(counterHOUR == (SLEEP_MINUTES-1)){    
        String data=DEVICE_ID;
        data+=",";
        data+=String((uint8_t)dht.readTemperature()); 
        data+=",";
        data+=String((uint8_t)dht.readHumidity()); 
        data+=",";
        data+=String((uint8_t)millis());
        data+="\r";
        for(int i=0;i<data.length();i++){ Serial.write(data[i]); }
        Serial.flush();
        counterHOUR = 0;
    }else{counterHOUR++;}
    counterWD = 0;
  } 
  sleepNow ();  
}
