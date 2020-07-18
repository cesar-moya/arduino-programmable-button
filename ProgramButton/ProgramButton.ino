#include <EEPROM.h>

#define BUTTON_UP 3       //Green
#define BUTTON_UP_PRG 4   //Blue
#define LED_SYS 8     //Yellow

int EEPROM_ADDRESS = 0;
int btnUpPressed = 0;
bool goingUp = false;

struct StoredProgram {
  bool isSet = false;
  float timeUp = 0;
  float timeDown = 0;
};
StoredProgram savedProgram;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_UP, INPUT); 
  pinMode(BUTTON_UP_PRG, INPUT);
  pinMode(LED_SYS, OUTPUT);

  //long timeUp = 5000;
  //long timeDown = 5000;
  //saveToEEPROM(timeUp, timeDown);
  readFromEEPROM();
}

void loop() {
  handleButtonProgramUp();

  btnUpPressed = digitalRead(BUTTON_UP);
  if(btnUpPressed){
    playProgramInLedSeconds();
    //goUp();
  }else if(goingUp){
    stopMoving();
  }else{
    //If nothing is pressed, idle and turn off LED
    stopMoving();
    Serial.println("Waiting...");
  }  
}

void handleButtonProgramUp(){
  int count = 0;
  while(digitalRead(BUTTON_UP_PRG)){
    digitalWrite(LED_SYS, HIGH);
    Serial.println("BUTTON UP | PROGRAM | Pressed");
    delay(10);
    count++;
    if(count >= 100){
      Serial.println("BUTTON UP | PROGRAM | Entered program mode");
      while(true){
          Serial.println("BUTTON UP | PROGRAM | Waiting for instruction...");
          digitalWrite(LED_SYS, HIGH);
          delay(50);
          digitalWrite(LED_SYS, LOW);
          delay(50);

          long startTime = millis();
          bool exitProgramMode = false;
          while(digitalRead(BUTTON_UP)){
            exitProgramMode = true;
            digitalWrite(LED_SYS, HIGH);
            Serial.println("BUTTON UP | PROGRAM | Recording UP...");
            //goUp();
          }
          if(exitProgramMode){
            long endTime = millis();
            int elapsed = int(endTime - startTime);
            
            Serial.println("BUTTON UP | PROGRAM | Saving to EEPROM...");
            saveToEEPROM(elapsed, elapsed);
            Serial.println("BUTTON UP | PROGRAM | EEPROM save finished");

            
            Serial.print("BUTTON UP | PROGRAM | Elapsed: "); Serial.println(elapsed);
            Serial.println("BUTTON UP | PROGRAM | Exiting Program mode");
            break; //end program mode
          }
      }
      count = 0;
    }
  }  
}

void saveToEEPROM(long timeUp, long timeDown){
  thinkingBlink();
  Serial.println("Saving to EEPROM");
  savedProgram.timeUp = (float)timeUp; 
  savedProgram.timeDown = (float)timeDown; 
  savedProgram.isSet = true;
  EEPROM.put(EEPROM_ADDRESS, savedProgram);
  successBlink();
}

void readFromEEPROM(){
  thinkingBlink();
  Serial.println("Reading from EEPROM");
  EEPROM.get(EEPROM_ADDRESS, savedProgram);
  Serial.println("TimeUp:");   
  long timeUpInt = (long)savedProgram.timeUp; //static_cast<long>
  Serial.println(timeUpInt);
  Serial.println("TimeDown:"); 
  long timeDownInt = (long)savedProgram.timeDown;
  Serial.println(timeDownInt);
  Serial.println("IsSet?");
  Serial.println(savedProgram.isSet);
  successBlink();
}

void successBlink(){
  digitalWrite(LED_SYS, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  for(int i = 0; i < 4; i++){
    digitalWrite(LED_SYS, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_SYS, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
}

void thinkingBlink(){
  digitalWrite(LED_SYS, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  for(int i = 0; i < 40; i++){
    digitalWrite(LED_SYS, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(30);
    digitalWrite(LED_SYS, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    delay(30);
  }
}

void builtInBlink(int secs){
  digitalWrite(LED_BUILTIN, LOW);
  for(int i = 0; i < secs; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW); 
    delay(500);
  }
}

void playProgramInLedSeconds(){
  if(savedProgram.isSet){
    for(long i = 0; i < savedProgram.timeUp / 1000; i++){
      digitalWrite(LED_SYS, LOW);
      delay(500);
      digitalWrite(LED_SYS, HIGH);
      delay(500);
    }
    thinkingBlink();
    for(long i = 0; i < savedProgram.timeDown / 1000; i++){
      digitalWrite(LED_SYS, LOW);
      delay(500);
      digitalWrite(LED_SYS, HIGH);
      delay(500);
    }
  }
}

//Send PWM signal to L298N enX pin (sets motor speed)
void goUp(){
  Serial.println("UP");
  goingUp = true;
  digitalWrite(LED_BUILTIN, HIGH);
  //digitalWrite(LED_SYS, HIGH);
}

void stopMoving(){
  goingUp = false;
  //digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LED_SYS, LOW);
}
