#include "./defines.h"

//char userInput[DISPLAY_SIZE + 1] = {0};

//int temp[DISPLAY_SIZE];

uint16_t score = 0;
uint8_t previous_score = score;
uint16_t highest_score = 0;
uint8_t lives = 3;
uint8_t score_multiplier = 1;
//String name = "";
char name[DISPLAY_SIZE+1] = "A";
char currentLetter = 'A';

boolean invalidate= true;

String userInput; //simulate data received from other microcontroller

command receivedCommand = No_command; // start with nothing

gameMode currentMode = STANDBY;

void setup()
{
  // put your setup code here, to run once:
  setup_ht16k33(addr_1);
  setup_ht16k33(addr_2);
  reset_displays();
  // disabling the timer for now as it kind of causes issues with serial - i guess
  // Timer1.initialize(1000000);
  // Timer1.attachInterrupt(updateDisplay);
  Serial.begin(9600);
  updateDisplay();
}

const unsigned long interval = 500; // 1 second
unsigned long previousMillis = 0;  // Stores the last time the function was called
unsigned long currentMillis = millis();

void loop()
{
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Update time
    manage(); //call manage function
  }
  // put your main code here, to run repeatedly:
  //check for user input -- simulating the commands from the other arduino 
  if(Serial.available()){
    String userInput = Serial.readStringUntil('\n');
    Serial.print("You typed: ");
    Serial.println(userInput);
    receivedCommand = userInput.toInt();
    //invalidate = true;
    processCommand();
    //updateDisplay();
    Serial.println(currentMode);
  }
}

uint8_t seconds = 0;
boolean normalMode = true;

void manage(){
  seconds += 1;
  if(seconds>=10){
    seconds = 0;
    normalMode = true;
    score_multiplier = 1;
    invalidate = true;
  }
  updateDisplay();
}

void reset_displays(){
  show(addr_1, "");
  show(addr_2, "");
}

uint8_t name_position = 0;
void processCommand(){
  switch(receivedCommand){
    case No_command:
      break;
    case START:
      //reset_displays();
      lives = 3;
      score = 0;
      //name = "A";
      memset(name, 0, 9);
      name[0] = 'A'; name[1] = 'A';
      currentMode = START_GAME;
      break;
    case LIFE_LOST:
    if(lives <= 0) break;
      lives -= 1;
      if(lives == 0){
        currentMode = GAME_OVER;
        break;
      }
      //show(addr_2, "-1 LIVES");
      break;
    case FRENZY_com:
      normalMode = false;
      currentMode = FRENZY;
      score_multiplier = 2;
      //start a timer interrupt to come and disable the frenzy mode after 5 seconds
      break;
    case FREEZE_com:
      normalMode = false;
      currentMode = FREEZE;
      //start a timer interrupt to come and disable the freeze mode after 5 seconds
      break;
    case LOW_FRUIT:
      score += (10*score_multiplier);
      currentMode = START_GAME;
      Serial.print("SCORE: ");
      Serial.println(score);
      break;
    case HIGH_FRUIT:
      score += (20*score_multiplier);
      currentMode = START_GAME;
      Serial.print("SCORE: ");
      Serial.println(score);
      break;
    case BOMB:
      if(score >= (10*score_multiplier) && currentMode!= FREEZE){
        score -= (10*score_multiplier);
      }
      currentMode = START_GAME;
      Serial.print("SCORE: ");
      Serial.println(score);
      break;
      /////////////////////////////////////////////////////////////////////////////////////
  case NEXT_LETTER:
    currentLetter += 1;
    Serial.print("next letter: ");
    Serial.println(currentLetter);
    if(currentLetter > 'Z'){
      currentLetter = 'A'; //this is to loop around, there is probably a better way to do it but yea :|
    }
    name[strlen(name) - 1] = currentLetter;
    break;
  case PREVIOUS_LETTER:
    currentLetter -= 1;
    if(currentLetter <'A'){
      currentLetter = 'Z'; //to loop around the other way,
    }
    name[strlen(name) - 1] = currentLetter;
    break;
  case LETTER_CHOSEN:
    // //should write code to go to the next letter --done!
    // if(name[0] = 'A'){
    //   name[0] = currentLetter;
    //   name[1] = 'A';
    // }else{
    //   name[strlen(name) - 1] = currentLetter;
    // }
    currentLetter = 'A'; //reset curent char to A so that the user can loop through agan
    name[strlen(name)] = currentLetter;
    //strncat(name, &currentLetter, 1);// +'A' is to go to the next charachter position!
    break;
  case NAME_ENTERED:
    //probably what i could do is display high name and then wait to receive a reset signal and then go back to standby mode!!!
    //the displaying is already done! -- however should remove the A that i add after all names for next character
    //name = name.remove last character
    if(strlen(name) > 0){
      name[strlen(name) - 1] = '\0'; //remove last letter added when the letter is chosen
    }
    break;
  case RESET:
    currentMode = STANDBY;
    break;
  }
  invalidate=true;
}


void updateDisplay(){
  char buffer[9];
  if(normalMode && ((currentMode == FREEZE) || currentMode == FRENZY)){
  currentMode = START_GAME;
  }
  if(!invalidate) return;
  switch(currentMode){
    case STANDBY:
      show(addr_1, "READY TO");
      show(addr_2, "PLAY");
      break;
    case START_GAME:
      show(addr_2, "PLAYING");
      sprintf(buffer, "%06d %d", score, lives);
      show(addr_1, String(buffer));
      break;
    case GAME_OVER:
      //need to stop accepting any input unless it's start
      //how to do it
      sprintf(buffer, "%06d %d", score, lives);
      show(addr_1, String(buffer));
      show(addr_2, "GAMEOVER");
      if(score> highest_score){
        currentMode = NAME_ENTRY;
        highest_score = score;
        name[0] = 'A';
        name[1] = '\0';
      }
      break;
    case FREEZE:
      show(addr_2, "FREEZE");
      break;
    case FRENZY:
      show(addr_2, "FRENZY");
      //score_multiplier = 2;
      break;
    case NAME_ENTRY:
      show(addr_1, "HIGH");
      show(addr_2, String(name));
      // Serial.print("name: ");
      // Serial.println(name);
      break;
    // case NORMAL:
    //   show(addr_2, "PLAYING");
    //   break;
    case SCORE_CHANGE:
      char score_diff[8];
      sprintf(score_diff, "%d", score - previous_score);
      previous_score = score;
      show(addr_2, String(score_diff));
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
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], letters_upper[data[i]-A_UPPER] & 0xFF, letters_upper[data[i]-A_UPPER] >> 8);
    } 
    else if(data[i] >= a_lower && data[i] <= z_lower){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], letters_lower[data[i]-a_lower] & 0xFF, letters_lower[data[i]-a_lower] >> 8);
    }
    else if(data[i] >= zero && data[i] <= nine){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], numbers[data[i]- zero] & 0xFF, numbers[data[i]- zero] >> 8);
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