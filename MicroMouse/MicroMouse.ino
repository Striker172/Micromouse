#define SensorL PA0
#define SensorR PA1
#define SensorF PA2
#define button PB12
int buttonCount = 0;
unsigned long sensorTimer = millis() + 250;
unsigned long buttonTimer = millis() + 500;
int prevButtonState = LOW;
void setup() {
  // put your setup code here, to run once:
  pinMode(SensorL, INPUT);
  pinMode(SensorR,INPUT);
  pinMode(SensorF,INPUT);
  pinMode(button, INPUT_PULLDOWN);
}

void loop() {
  if(millis() > sensorTimer){
    //Sample sensors may need to add some more condictions and stuff 
    sensorTimer += 250;
  }
  if(millis() > buttonTimer){
    //This puts it into a different state, we can play around with it.
    //Like map mode, then processing mode and such.
    int buttonState = digitalRead(button);
        if(buttonState == HIGH && prevButtonState == LOW){
            if(buttonCount >= 3){
                buttonCount = 0;
            }
            prevButtonState = HIGH;
            buttonCount++;
        }
        else if(buttonState == LOW){
           prevButtonState = LOW;
        }
        buttonTimer += 500;
    }
  }
  
}
