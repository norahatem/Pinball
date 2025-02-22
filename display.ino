#include "./defines.h"

//char userInput[DISPLAY_SIZE + 1] = {0};

//int temp[DISPLAY_SIZE];
int score = 0;
int highest_score = 0;
int lives = 3;
int score_multiplier = 1;
//String game_mode = "";
String name = "";
boolean bombs_disabled = 0;

String userInput;

gameMode currentMode = STANDBY;
//GameMode previousMode;

void setup()
{
  // put your setup code here, to run once:
  setup_ht16k33(addr_1);
  //Serial.begin(9600);              // Initialize serial communication at 9600 baud
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(updateDisplay);
  Serial.begin(9600);
}

boolean invalidate= true;
void loop()
{
  updateDisplay();
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    String userInput = Serial.readStringUntil('\n');
    Serial.print("You typed: ");
    Serial.println(userInput);
    //show(addr_1, userInput);
    currentMode = userInput.toInt();
    invalidate = true;
    // updateDisplay();
  }
}

void updateDisplay(){
  if(!invalidate) return;
  switch(currentMode){
      case STANDBY:
        show(addr_2, "READY");
        break;
      case START_GAME:
      show(addr_2, "STARTED");
      show(addr_1, "000000 3");
        break;
      case GAME_OVER:
      show(addr_2, "GAMEOVER");
        if(score> highest_score){
          currentMode = NAME_ENTRY;
        }
        break;
      case FREEZE:
        show(addr_2, "FREEZE");
        break;
      case FRENZY:
        show(addr_2, "FRENZY");
        score_multiplier = 2;
        break;
      case NAME_ENTRY:
        show(addr_2, "HIGH");
        break;
    }
    invalidate=false;
}

void setup_ht16k33(uint8_t addr)
{
  Wire.begin();
  // start oscillation
  Wire.beginTransmission(addr);
  Wire.write(0x21);
  Wire.endTransmission();
  //...................
  // set brightness level --max
  Wire.beginTransmission(addr);
  Wire.write(0xEF);
  Wire.endTransmission();
  //......................
  // display ON, blinking OFF
  Wire.beginTransmission(addr);
  Wire.write(0x81);
  Wire.endTransmission();
}


void show(uint8_t addr, String data){
  for(uint8_t i = 0; i < DISPLAY_SIZE; i++){
    //temp[i] = data[i];
    if(data[i] >= A_UPPER && data[i] <= Z_UPPER){
      transmit_data(addr_1, digit_place[DISPLAY_SIZE -1 -i], letters_upper[data[i]-A_UPPER] & 0xFF, letters_upper[data[i]-A_UPPER] >> 8);
    } 
    else if(data[i] >= a_lower && data[i] <= z_lower){
      transmit_data(addr_1, digit_place[DISPLAY_SIZE -1 -i], letters_lower[data[i]-a_lower] & 0xFF, letters_lower[data[i]-a_lower] >> 8);
    }
    else if(data[i] >= zero && data[i] <= nine){
      transmit_data(addr_1, digit_place[DISPLAY_SIZE -1 -i], numbers[data[i]- zero] & 0xFF, numbers[data[i]- zero] >> 8);
    }
    else if(data[i] == 32 || data[i] == 0){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], SPACE & 0xFF, SPACE >> 8);
    } 
    else if(data[i] == 43){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], PLUS & 0xFF, PLUS >> 8);
    }
    else if(data[i] == 45){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], MINUS & 0xFF, MINUS >> 8);
    }
    else if(data[i] == 42){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], MULTIPLY & 0xFF, MULTIPLY >> 8);
    }
  }
}


void transmit_data(uint8_t addr, uint8_t reg, uint8_t byte1, uint8_t byte2){
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(byte1);
  Wire.write(byte2);
  Wire.endTransmission();
}