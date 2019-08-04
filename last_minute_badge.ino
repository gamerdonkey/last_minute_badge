#include <TM1637Display.h>

#define DISPLAY_CLK   14
#define DISPLAY_DIO   15
#define BUTTON        27

#define SCROLL_LIMIT 120
#define BLANK_LENGTH   6

#define BLANK       0x00

// 0-9, a-z, blank, dash, star
const uint8_t CHAR_SEGMENTS[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x3d, 0x76, 0x06, 0x1e, 0x76, 0x38, 0x55, 0x54, 0x3f, 0x73, 0x67, 0x50, 0x6d, 0x78, 0x3e, 0x1c, 0x2a, 0x76, 0x6e, 0x5b, 0x00, 0x40, 0x63 };
uint16_t limit = sizeof(CHAR_SEGMENTS);

uint8_t data[] = { 0x00, 0x00, 0x00, 0x00 };
uint8_t scroll_1_segments[SCROLL_LIMIT + 5];
uint16_t scroll_1_length = 0;
uint8_t scroll_2_segments[SCROLL_LIMIT + 5];
uint16_t scroll_2_length = 0;
uint8_t scroll_3_segments[SCROLL_LIMIT + 5];
uint16_t scroll_3_length = 0;

hw_timer_t* timer = NULL;
uint8_t mode = 0;
uint8_t buttonState = 0;
uint8_t previousButtonState = 0;
const uint8_t numModes = 3;

TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);

void IRAM_ATTR onTimer() {
  buttonState = digitalRead(BUTTON);
  if(buttonState == LOW && previousButtonState == HIGH) {
    if(mode < numModes) {
      mode++;
    }
    else {
      mode = 0;
    }
  }
  previousButtonState = buttonState;
}

void setup()
{
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000, true);
  timerAlarmEnable(timer);
  
  display.setBrightness(0x07);

  pinMode(BUTTON, INPUT);

  randomSeed(analogRead(0));

  encodeScrollString(scroll_1_length, scroll_1_segments, "the quick brown fox jumps over the lazy dog");
  encodeScrollString(scroll_2_length, scroll_2_segments, "crappy badge");
}

void loop()
{
  if(mode == 0) {
    display.clear();
    delay(500);
  }
  if(mode == 1) {
    for(int j = 0; j < 3; j++) {
      data[j] = data[j+1];
    }
    data[3] = CHAR_SEGMENTS[random(16)];
    display.setSegments(data, 4, 0);
    delay(250);
  }
  if(mode == 2) {
    for(int i = 0; i < scroll_1_length; i++) {
      for(int j = 0; j < 3; j++) {
        data[j] = data[j+1];
      }
      data[3] = scroll_1_segments[i];
      display.setSegments(data, 4, 0);
      delay(250);
    }
  }
  if(mode == 3) {
    for(int i = 0; i < scroll_2_length; i++) {
      for(int j = 0; j < 3; j++) {
        data[j] = data[j+1];
      }
      data[3] = scroll_2_segments[i];
      display.setSegments(data, 4, 0);
      delay(250);
    }
  }
}

void encodeScrollString(uint16_t& scrollLength, uint8_t* scrollSegments, String input) {
  scrollLength = constrain(input.length(), 0, SCROLL_LIMIT) + BLANK_LENGTH;

  for(int i = 0; i < scrollLength; i++) {
    if(i < BLANK_LENGTH) {
      scrollSegments[i] = BLANK;
    }
    else {
      scrollSegments[i] = encodeCharacter(input.charAt(i - BLANK_LENGTH));
    }
  }
}

uint8_t encodeCharacter(char input) {
  if(input == 32) {
    return CHAR_SEGMENTS[36]; // space
  }
  if(input == 42) {
    return CHAR_SEGMENTS[38]; // star/degrees
  }
  if(input == 45) {
    return CHAR_SEGMENTS[37]; // dash
  }
  if(input >= 65 and input <= 90) {
    return CHAR_SEGMENTS[input-55]; // uppercase A-Z
  }
  if(input >= 97 and input <= 122) {
    return CHAR_SEGMENTS[input-87]; // lowercase a-z
  }
  if(input >= 48 and input <= 57) {
    return CHAR_SEGMENTS[input-48]; // 0-9
  }
}
