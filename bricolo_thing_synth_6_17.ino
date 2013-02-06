
//variables setup

//boolean debug= true;  //set to true to change baud rate to 9600 & activate debugger 
boolean debug= false;

#define pot_1 0
#define numStrikeVals 64

int chan = 2;

byte incomingByte;
byte note;
byte pNote;
byte velocity;
byte currentNote;


float scaleCalc; 

float potValue;

long lastNoteOff;

int analogIn;
int pAnalogIn = analogIn;


int statusLed = 13;   // select the pin for the LED

int action=2; //0 =note off ; 1=note on ; 2= nada

#define numNotes 64 //how many outputs
//int firstPin = 22; //what's the first pin
int lowNote = 24;

int solenoid_1 = 11;
long strikeMark=0;
float strikeTime =2500.0;
float strikeTime_upper =2000.0;

float pStrikeTime=strikeTime;
float strikeTimeLow = 125;
float strikeTimeHigh = 3000;

float strikeVals[10] = {800.0,1400.0,2000.0,2200.0,2500.0,2800.0,3000.0,3500.0,4000.0,4500.0};
//float strikeVals[numStrikeVals];

boolean strikeState =false;
long restMark=0;
long restTime;

float baseTime = 15000;
//float baseTime = 20000;

boolean pulseState=false;

float restVals[numNotes];

//long restVals[] ={8000,7382.65, 6799.95, 6249.95, 5730.82, 5240.83, 4778.33, 4341.79, 3929.75, 3540.84, 3173.75, 2827.26, 2500,13,14,15,16,17,18,19,20,21,22,23,24};
//int restVals[] ={1,3,5,7,1,3,5,7,1,3,5,7,1,3,5,71,3,5,7,1,3,5,7};


//setup: declaring iputs and outputs and begin serial

void setup() {
if(debug== true){
  Serial.begin(9600);  
}
else(Serial.begin(31250));


pinMode(statusLed,OUTPUT);   // declare the LED's pin as output
pinMode(solenoid_1,OUTPUT);
scaleCalc=196.0/185.0;
//Serial.println(scaleCalc, 5);

//populate rest time values
for(int i=0; i<numNotes; i++){
float x = pow(scaleCalc,i);

  restVals[i] =baseTime/x-strikeTime;


  // Serial.println(i);
  //Serial.println(restVals[i], 5);

}
/*
for(int i=0; i<numStrikeVals+1; i++){
strikeVals[i]=(i+1)*100;
}
*/



}

  


//loop: wait for serial data, and interpret the message
void loop () {
  
  
timeoutCheck();
 //checkPot();
if(debug== true){
debugger();
}

  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // wait for as status-byte, channel 1, note on or off
    if (incomingByte== 143+chan){ // note on message starting starting
      action=1;
    }else if (incomingByte== 127+chan){ // note off message starting
      action=0;
    }else if (incomingByte== 207+chan){ // aftertouch message starting
       //not implemented yet
    }else if (incomingByte== 159+chan){ // polypressure message starting
       //not implemented yet
    }else if ( (action==0)&&(note==0) ){ // if we received a "note off", we wait for which note (databyte)
      note=incomingByte;
    
      playNote(note, 0);
      note=0;
      velocity=0;
      action=2;
    }else if ( (action==1)&&(note==0) ){ // if we received a "note on", we wait for the note (databyte)
      pNote=note;
      note=incomingByte;
     // currentNote=note;
      
    }else if ( (action==1)&&(note!=0) ){ // ...and then the velocity
      velocity=incomingByte;
      playNote(note, velocity);
      note=0;
      velocity=0;
      action=0;
    }else{
      //nada
    }
  }
  
pulse();


}

void blink(){
  digitalWrite(statusLed, HIGH);
  delay(100);
  digitalWrite(statusLed, LOW);
  delay(100);
}


void playNote(byte note, byte velocity){
  int value=LOW;
  if (velocity >10){
      value=HIGH;
  }else{
   value=LOW;
 
  } 

 //since we don't want to "play" all notes we wait for a note in range

 if(note>=lowNote && note<lowNote+numNotes){
   
  
   
   if(value==HIGH){
         
   pulseState=true;
  // strikeTime=3*1000;
   //restTime=map(restVals[note-35],12,0 , 8000, 200);
   restTime=restVals[note-lowNote];
 }
   if(value==LOW){
    pulseState=false;
    
   } 
 }
  /*
   
   if(note>=35 && note<60){
   byte myPin=note-13; // to get a pinnumber between 2 and 9
   digitalWrite(myPin, value);
 
   
 }
*/
}

void pulse(){
//strikeTime=strike;
//restTime=rest;

if(pulseState==true){  
 /* 
  digitalWrite(solenoid_1, HIGH);
  delayMicroseconds(strikeTime);
   digitalWrite(solenoid_1, LOW);
   delayMicroseconds(restTime); 
  */

 
if(micros()-restMark>=restTime && strikeState==false){
strikeState=true;
digitalWrite(solenoid_1, LOW);
digitalWrite(solenoid_1, HIGH);
strikeMark=micros();

}
///*
if(micros()-strikeMark>=strikeTime && strikeState==true){
strikeState=false;
digitalWrite(solenoid_1, LOW);
restMark=micros();
  lastNoteOff = millis();

}
//*/
}

}
void checkPot(){
analogIn = analogRead(pot_1);

if( analogIn != pAnalogIn){ 
//strikeTime= strikeVals[map(analogIn, 0, 1023, 0, numStrikeVals-1)];


  strikeTime= strikeVals[map(analogIn, 0, 1023, 0, 9)];
//strikeTime= map(analogIn, 0, 1023, strikeTimeLow, strikeTimeHigh);

pAnalogIn=analogIn;
}


if(strikeTime !=pStrikeTime){
for(int i=0; i<numNotes; i++){
float x = pow(scaleCalc,i);
  restVals[i] =baseTime/x-strikeTime;
  // Serial.println(i);
  //Serial.println(restVals[i], 5);
}

pStrikeTime=strikeTime;

/*
if(strikeTime>3500){
baseTime =30000;
lowNote = 12;
}
if(strikeTime<=3500){
baseTime =15000;
lowNote = 24;
}

if(strikeTime<=1000){
baseTime =7500;
lowNote = 36;
}
*/
}
 
}


void timeoutCheck(){
//  if(millis()-lastNoteOff>1000){
if(millis()-lastNoteOff>50){
 digitalWrite(solenoid_1, LOW);
 }
}

void debugger(){
Serial.println(analogRead(pot_1));
Serial.println(strikeTime);
delay(1000);
}
