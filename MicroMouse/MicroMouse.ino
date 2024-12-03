#define SensorL PA0
#define SensorR PA1
#define SensorF PA2
#define rightSideF PB1
#define rightSideB PB0
#define leftSideF PA7
#define leftSideB PA6
const int speed = 200;
unsigned long sensorTimer = millis() + 250;
void setup() {
  // put your setup code here, to run once:
  pinMode(SensorL, INPUT);
  pinMode(SensorR,INPUT);
  pinMode(SensorF,INPUT);
  pinMode(rightSideF,OUTPUT);
  pinMode(rightSideB,OUTPUT);
  pinMode(leftSideF,OUTPUT);
  pinMode(leftSideB,OUTPUT);
}

void loop() {
  if(millis() > sensorTimer){
    //Sample sensors may need to add some more condictions and stuff 
    sensorTimer += 250;
  }
  
}
//Use a timer to artfically delay the drivetrain code, don't use delay. 
void drive(char D){
  switch(D){
    case 'F':
      analogWrite(rightSideF,speed);
      analogWrite(leftSideF, speed);
      analogWrite(leftSideB,0);
      analogWrite(rightSideB,0);
    case 'B':
      analogWrite(rightSideB,speed);
      analogWrite(leftSideB, speed);
      analogWrite(leftSideF,0);
      analogWrite(rightSideF,0);
    case 'R':
      analogWrite(rightSideF, speed);
      analogWrite(leftSideB,speed/2);
      analogWrite(rightSideB,0);
      analogWrite(leftSideF,0);
    case 'L':
      analogWrite(leftSideF, speed);
      analogWrite(rightSideB,speed/2);
      analogWrite(rightSideF,0);
      analogWrite(leftSideB,0);
    default:
      analogWrite(rightSideF,0);
      analogWrite(leftSideF, 0);
      analogWrite(leftSideB,0);
      analogWrite(rightSideB,0);
  }
}
