#include <ADCTouch.h>

#define LED1 6
#define LED2 5
#define LED3 4
#define LED_D 7
#define LED_I 8
#define TOUCH_PIN A5
#define THRES 350
#define SAMPLE_TIME 48
#define SAMPLES 5
#define TOUT 500
#define TRIG_SAMPLES 10
#define THRES_TOUCH 100

int LED[3]= {LED1,LED2,LED3};
int SENS[3] = {2,1,0};
int MEAS[3] = {0,0,0};
int ACCU[3] = {0,0,0};
int RETR[3] = {0,0,0};
int estado = 4;
int ref0, ref1;     //reference values to remove touch pin offset
int disabled = 0;
unsigned long t_ant = 0;
unsigned long t_act = 0;
String serial_in = "";


void setup()
{
  Serial.begin(115200);   
  ref0 = ADCTouch.read(TOUCH_PIN, 500);    //create reference values to remove offset            
  for (int i=0;i<3;i++){
    pinMode(LED[i],OUTPUT);
    digitalWrite(LED[i],LOW);  
  }
  pinMode(LED_I,OUTPUT);
  pinMode(LED_D,OUTPUT);
  digitalWrite(LED_I,LOW);
  digitalWrite(LED_D,LOW); 
  //Serial.println("Device Ready!");
  
}
 
void loop()
{
  int value0 = ADCTouch.read(TOUCH_PIN,10);
  value0 -= ref0;
  if ((abs(value0)>THRES_TOUCH)||(disabled==1)){
    if (estado != -1) Serial.println("B");
    disabled = 1;
    digitalWrite(LED_I,HIGH);
    digitalWrite(LED_D,HIGH);
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED3,HIGH);
    estado = -1;
  }
  else{
    for (int i=0;i<3;i++){
      MEAS[i] = analogRead(SENS[i]);
      //Serial.println(MEAS[i]);
      if (MEAS[i]>THRES) {
        if ((ACCU[i]==SAMPLES-1)||(RETR[i]==TRIG_SAMPLES)) {
          Serial.print("B");
          Serial.println(i+1);
          digitalWrite(LED[i],HIGH);
          estado = 4;
          RETR[i]=0;
        }
        ACCU[i] ++;
        if (ACCU[i]>SAMPLES){
          ACCU[i]=SAMPLES;
          RETR[i]++;
          estado = 4;
        }
      } 
      else{
        ACCU[i] --;
        RETR[i]=0;
        if (ACCU[i]==1) {
          //Serial.print(i);
          //Serial.println(" OFF");
          digitalWrite(LED[i],LOW);
        }
        if (ACCU[i]<0) ACCU[i]=0;
      }                    
    }
  }

  
  t_act = millis();
  switch (estado) {
  case 1:
    if ((t_act-t_ant)>TOUT) estado = 4;
    else{
       digitalWrite(LED_I,HIGH);   
    }
    break;
  case 2:
    if ((t_act-t_ant)>TOUT) estado = 4;
    else{
       if (MEAS[0]>THRES) {
        estado = 1;
        t_ant = t_act;
        Serial.println("GTL");
       }
       
    }
    break;
  case 3:
    if ((t_act-t_ant)>TOUT) estado = 4;
    else{
       if (MEAS[1]>THRES) {
        estado = 2;
        t_ant = t_act;
       }
    }
    break;
  case 4:
    digitalWrite(LED_I,LOW);
    digitalWrite(LED_D,LOW);
    if (MEAS[0]>THRES) estado = 5;
    if (MEAS[2]>THRES) estado = 3;
    t_ant = t_act;
    break;
  case 5:
    if ((t_act-t_ant)>TOUT) estado = 4;
    else{
       if (MEAS[1]>THRES) {
        estado = 6;
        t_ant = t_act;
       }   
    }
    break;
  case 6:
    if ((t_act-t_ant)>TOUT) estado = 4;
    else{
       if (MEAS[2]>THRES) {
        estado = 7;
        t_ant = t_act;
        Serial.println("GTR");
       }
    }
    break;
  case 7:
    if ((t_act-t_ant)>TOUT) estado = 4;
    else{
       digitalWrite(LED_D,HIGH);
       
    }
    break;
  default:
    // statements
    break;
  }
  EnableCheck();
  delay(SAMPLE_TIME);
}

void EnableCheck(){
  if (Serial.available() > 0) {
    serial_in = Serial.readString();   
    if (serial_in == "D") {
      disabled=0;
      digitalWrite(LED_I,LOW);
      digitalWrite(LED_D,LOW);
      digitalWrite(LED1,LOW);
      digitalWrite(LED2,LOW);
      digitalWrite(LED3,LOW);
      estado = 4;
      Serial.println("D");
    }
  }
}
