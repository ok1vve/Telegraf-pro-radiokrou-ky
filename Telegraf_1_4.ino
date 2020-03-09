//Dětský telegraf pro potřeby radiokroužků

#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();
#include "pitches.h"

//NASTAVENÍ !!!

int   ton           = 1000;                         //výška tónu bzučáku
int   txtimeoutset  = 1000;                         //prodleva přepnutí TX>RX

//procesní proměnné
int   delkatecky       ;                            //délka tečky (čas v ms)
int   tolerance        ;                           //tolerance délky tečky v ms
byte  zaklicovano   = LOW;                          //hodnota polohy klíče
int   delkastisku   = 0;                            //naměřená délka stisku klíče
byte  predchozistav = LOW;                          //předchozí pooha klíče
int   txtimeout          ;  
int   delkabzuku;
int   bzuk;
int   bzukt;
int   val;                                          //promenna pripravena k uchovani hodnot
int   y=1;
unsigned long casovacstart = 0;                     // kdy naposledy stisknuto [ms]
unsigned long txtimeoutstart = 0;                   // kdy naposledy stisknuto [ms]
unsigned long bzukstart;     

#define BZUCAK   9
#define KLIC     12
#define RX       0
#define TX       10
#define POT      A0

void setup() {

  //Serial.begin(9600);
  pinMode(BZUCAK, OUTPUT);
  pinMode(KLIC, INPUT_PULLUP);
  pinMode(RX, INPUT);
  pinMode(TX, OUTPUT);
  pinMode(POT, INPUT);
    mySwitch.enableReceive(RX);                      // Přijímač na přerušení  0 => to je pin #2
    mySwitch.enableTransmit(TX);                            
    mySwitch.setProtocol(1);                         // Možnost změnit protokol(základní je 1, možnost je 2)    
    //mySwitch.setPulseLength(350);                  // Možnost změnit délku pulsu (350)    
    mySwitch.setRepeatTransmit(4);                   // Možnost změnit počet opakování  (4)
    
    tone(BZUCAK, NOTE_C4, 200); delay(200); tone(BZUCAK, NOTE_D4, 200);delay(200);tone(BZUCAK, NOTE_E4, 200);delay(200);tone(BZUCAK, NOTE_F4, 200);delay(200);tone(BZUCAK, NOTE_G4, 200);delay(200);tone(BZUCAK, NOTE_F4, 200);delay(200);tone(BZUCAK, NOTE_E4, 200);delay(200);tone(BZUCAK, NOTE_D4, 200);delay(200);tone(BZUCAK, NOTE_C4, 300);delay(200);tone(BZUCAK, NOTE_E4, 200);delay(200);tone(BZUCAK, NOTE_G4, 200);delay(200);tone(BZUCAK, NOTE_C5, 400);delay(400);
}
void loop() {
  zaklicovano = digitalRead (KLIC);                  //hodnota zaklíčováno se změní přistisku klíče (tlačítka);
  val = analogRead(POT);                             //nastavení délky tečky v rozsahu 50-200 ms
  
           if (zaklicovano == HIGH) {txtimeoutstart = (millis());}                   //start časovače pro TX>RX timeout      
               txtimeout=millis()-txtimeoutstart ;              //výpočet TXtimeoutu    
               if(txtimeout < txtimeoutset){
               tx();}
           else rx(); 
           
      delkatecky = map(val, 0, 1023, 50, 200);
      if((delkatecky < y-2)||(delkatecky>y+2)){
      tone(BZUCAK, NOTE_C5, delkatecky);delay(delkatecky);tone(BZUCAK, NOTE_C5, delkatecky);delay(delkatecky);tone(BZUCAK, NOTE_C5, delkatecky);delay(delkatecky);tone(BZUCAK, NOTE_C5, delkatecky);delay(delkatecky);tone(BZUCAK, NOTE_C5, delkatecky);delay(delkatecky);
      y=delkatecky;
      tolerance = map(delkatecky,50,200,80,0);
      }
}
void rx(){
               
                if (mySwitch.available()){        
                  
                  if((mySwitch.getReceivedValue())==5393){     //sekvece příjem tečka  
                               tone(BZUCAK,ton);
                               bzukstart = millis();  
                               delkabzuku = delkatecky ;     
                               //Serial.println("RXtecka ");
                  }
                  if((mySwitch.getReceivedValue())==5396){     //sekvence příjem čárka
                               tone(BZUCAK,ton);
                               bzukstart = millis();       
                               delkabzuku = (3*delkatecky);   
                               //Serial.println("RXcarka ");   
                  }
                 
                  mySwitch.resetAvailable();
                }  
                  bzuk = millis()- bzukstart;                     //konec bzuku
                  if((bzuk > delkabzuku)&&(delkabzuku!=0)){               
                      noTone(BZUCAK);
                      delkabzuku=0 ;
                      //Serial.print(delkabzuku);
                  }               
          }
void tx(){    
                if ((zaklicovano == HIGH) && (zaklicovano != predchozistav)){
                  casovacstart = millis();
                  predchozistav = digitalRead(KLIC);             // uloží stav klíče pro další cyklus
                  tone(BZUCAK,ton);                              //příposlech
                  }
            
                if ((zaklicovano == LOW) && (zaklicovano != predchozistav)) {
                    predchozistav = digitalRead(KLIC);             // uloží stav klíče pro další cyklus
                    delkastisku = (millis() - casovacstart);       //výpočet délky stisku klíče                    
                    //Serial.print(delkastisku); 
                    noTone(BZUCAK);                                    
                  if (delkastisku < (delkatecky + tolerance)) {  //stisknuta tečka, odešli tečku
                       mySwitch.send(5393, 24);                     
                       //Serial.println(" Tecka");
                  }
                  if (delkastisku > (delkatecky + tolerance)) {  //stisknuta čárka, odešli čárku
                      mySwitch.send(5396, 24);                   
                      //Serial.println(" Carka");      
                  }                 
                 }               
          }


           
