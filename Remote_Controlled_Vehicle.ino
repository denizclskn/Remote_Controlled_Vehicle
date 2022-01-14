#include <RedBot.h>
#define echoPin 6
#define trigPin 7
#define redLed 5

RedBotEncoder encoder = RedBotEncoder(A2, 2); 

int direc1 = 12;
int direc2 = 13;
int break1 = 9;
int break2 = 8;
int mspeed1 = 3;
int mspeed2 = 11;

int motorSpeed = 200;
int rspeed = 250;
int lspeed = 235;
int counter = 20; 
int lCount;
int rCount;
int turningSpeed = 150;

#include <IRremote.h>

int IR = 10;
IRrecv irrec(IR);
decode_results results;


#define CHD 0x40BF906F
#define CHU 0x40BF0AF5
#define VOLD 0x40BF58A7
#define VOLU 0x40BF8A75
#define OK 0x40BFF807
#define ZERO 0x40BF18E7
#define ICHI 0x40BF8877
#define NI 0x40BF48B7
#define SAN 0x40BFC837
#define YON 0x40BFD22D
#define GO 0x40BF28D7
#define ROKU 0x40BFA857
#define NANA 0x40BF6897
#define HACHI 0x40BF609F
#define KYUU 0x40BFE817
#define NORTH 0x40BF00FF
#define EAST 0x40BF40BF
#define WEST 0x40BFC03F
#define SOUTH 0x40BF807F


#include <Wire.h>
int HMC6352SlaveAddress = 0x42;
int HMC6352ReadAddress = 0x41;


void setup() {
  // put your setup code here, to run once:
  HMC6352SlaveAddress = HMC6352SlaveAddress >> 1;
  Wire.begin();
  irrec.enableIRIn();
  pinMode (redLed, OUTPUT);
  
  pinMode (trigPin, OUTPUT);
  pinMode (echoPin, INPUT);
  
  pinMode(direc1, OUTPUT); 
  pinMode(direc2, OUTPUT); 

  pinMode(break1, OUTPUT);
  pinMode(break2, OUTPUT); 
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(crashControl()){
    stopMotor();
    backward(150);
    delay(500);
  }
  if(irrec.decode(&results)){
    
    if(results.value == NI){
      forward(motorSpeed);
      
    }else if(results.value == YON){
      turnLeft(turningSpeed);
      
    }else if(results.value == ROKU){
      turnRight(turningSpeed);
      
    }else if (results.value == HACHI){
      backward(motorSpeed);
      
    }else if(results.value == GO){
      stopMotor();
      
    }else if(results.value == NORTH){
      while(location()>10 || location()< 350){
        turnLeft(turningSpeed);
      }
      forward(motorSpeed);
      
    }else if(results.value ==EAST){
      while(location()<80 || location > 100){
        turnRight(turningSpeed);
      }
      forward(motorSpeed);
    }else if(results.value == SOUTH){
      while(location()>190 || location()<170){
        turnRight(turningSpeed);
      }
      forward(motorSpeed);
      
    }else if(results.value == WEST){
      while(location()>280 || location()<260){
        turnLeft(turningSpeed);
      }
      forward(motorSpeed);      
    }
    
    
    irrec.resume();
  }
}

float location(){
  Wire.beginTransmission(HMC6352SlaveAddress);
  Wire.write(HMC6352ReadAddress);
  Wire.endTransmission();
  delay(6);
  Wire.requestFrom(HMC6352SlaveAddress, 2);
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  float headingSum = (MSB << 8) + LSB;
  float headingInt = headingSum / 10;
  return headingInt;
}
  

boolean crashControl(){
  long duration, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
  delay(50);

  if (distance <= 25){
    analogWrite(redLed, (25-distance)*15);
  }else if(distance <9){
    analogWrite(redLed, 255);
    return true;
  }else{
    analogWrite(redLed, 0);
    return false;
  }
}

void forward(int sspeed){
  if(crashControl()){
    stopMotor();
    backward(150);
    delay(500);
  }else{
    digitalWrite(direc1, HIGH);
    digitalWrite(break1, LOW);
    analogWrite(mspeed1, sspeed);

    digitalWrite(direc2, HIGH);
    digitalWrite(break2, LOW);
    analogWrite(mspeed2, sspeed);
    //straighten(sspeed);
  }
}

void turnLeft(int sspeed){
  if(crashControl()){
    stopMotor();
    backward(150);
    delay(500);
  }else{
    digitalWrite(direc1, HIGH);
    digitalWrite(break1, LOW);
    analogWrite(mspeed1, sspeed);

    digitalWrite(direc2, LOW);
    digitalWrite(break2, LOW);
    analogWrite(mspeed2, sspeed);
    //straighten(sspeed);
  }
}

void turnRight(int sspeed){
  if(crashControl()){
    stopMotor();
    backward(150);
    delay(500);
  }else{
    digitalWrite(direc1, LOW);
    digitalWrite(break1, LOW);
    analogWrite(mspeed1, sspeed);
  
    digitalWrite(direc2, HIGH);
    digitalWrite(break2, LOW);
    analogWrite(mspeed2, sspeed);
    //straighten(sspeed);
  }
}

void backward(int sspeed) {
  if(crashControl()){
    stopMotor();
    backward(150);
    delay(500);
  }else{
    digitalWrite(direc1, LOW);
    digitalWrite(break1, LOW);
    analogWrite(mspeed1, sspeed);

    digitalWrite(direc2, LOW);
    digitalWrite(break2, LOW);
    analogWrite(mspeed2, sspeed);
    //straighten(sspeed);
  }
}

void stopMotor(){
  analogWrite(mspeed1, 0);
  analogWrite(mspeed2, 0);

}

void straighten(int motor){
  lspeed = motor;
  rspeed = motor;
  for (int i=counter; i>=0; i--){
    lCount = encoder.getTicks(LEFT);    
    rCount = encoder.getTicks(RIGHT);   
    rCount = rCount * 65 / 100;  //R count çok büyük çıkıyordu.
    int error = lCount - rCount;
    int offset = 7;
    //lspeed arttıkça rCount artıyor.
    if(error<-20){
      lspeed -= error / offset;
      analogWrite(mspeed2, lspeed);
      encoder.clearEnc(BOTH);
     }else if (error>20){
      rspeed += error / offset;
      analogWrite(mspeed1, rspeed);
      encoder.clearEnc(BOTH);
    }
  }
}
