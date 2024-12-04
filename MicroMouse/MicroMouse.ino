#define SensorL PA0
#define SensorR PA1
#define SensorF PA2
#define rightSideF PB1
#define rightSideB PB0
#define leftSideF PA7
#define leftSideB PA6
const int speed = 200;
unsigned long sensorTimer = millis() + 250;
enum Direction {NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3};
Direction currDirect;
int x, y;
void setup() {
  // put your setup code here, to run once:
  pinMode(SensorL, INPUT);
  pinMode(SensorR,INPUT);
  pinMode(SensorF,INPUT);
  pinMode(rightSideF,OUTPUT);
  pinMode(rightSideB,OUTPUT);
  pinMode(leftSideF,OUTPUT);
  pinMode(leftSideB,OUTPUT);
  x = 0, y =0;
}

void loop() {
  if(millis() > sensorTimer){
    //Sample sensors may need to add some more condictions and stuff 
    sensorTimer += 250;
  }
  
}
void center(){
  //Basically read the sensor val until they all read a value greater than the center or something like that
  //It may not work if its too close idk tho, needs more testing 
}
//Use a timer to artfically delay the drivetrain code, don't use delay. 
void drive(char D){
  switch(D){
    case 'F':
      analogWrite(rightSideF,speed);
      analogWrite(leftSideF, speed);
      analogWrite(leftSideB,0);
      analogWrite(rightSideB,0);
      switch(currDirect) {
            case NORTH: y += 1; break;
            case EAST:  x += 1; break;
            case SOUTH: y -= 1; break;
            case WEST:  x -= 1; break;
        }
    case 'B':
      analogWrite(rightSideB,speed);
      analogWrite(leftSideB, speed);
      analogWrite(leftSideF,0);
      analogWrite(rightSideF,0);
      switch(currDirect) {
            case NORTH: y -= 1; break;
            case EAST:  x -= 1; break;
            case SOUTH: y += 1; break;
            case WEST:  x += 1; break;
        }
    case 'R':
      analogWrite(rightSideF, speed);
      analogWrite(leftSideB,speed/2);
      analogWrite(rightSideB,0);
      analogWrite(leftSideF,0);
      changeDirect('R');
    case 'L':
      analogWrite(leftSideF, speed);
      analogWrite(rightSideB,speed/2);
      analogWrite(rightSideF,0);
      analogWrite(leftSideB,0);
      changeDirect('L');
    default:
      analogWrite(rightSideF,0);
      analogWrite(leftSideF, 0);
      analogWrite(leftSideB,0);
      analogWrite(rightSideB,0);
  }
}
void changeDirec(char turn) {
    if(turn == 'R') {
        currDirect = Direction((currDirect + 1) % 4);
    } else if(turn == 'L') {
        currDirect = Direction((currDirect + 3) % 4);
    }
}
