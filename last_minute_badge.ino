#include <TM1637Display.h>

#define DISPLAY_CLK           14
#define DISPLAY_DIO           15
#define BUTTON                27
#define BATT_VOLT            A13

#define NUM_MODES              6

#define SCROLL_LIMIT         120
#define SCROLL_PADDING         6
#define BLANK_SEGMENT       0x00

#define COUNTDOWN_SECS      3*60
#define DELAY_MILLIS         250

// 0-9, a-z, blank, dash, star
const uint8_t CHAR_SEGMENTS[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x3d, 0x76, 0x06, 0x1e, 0x76, 0x38, 0x55, 0x54, 0x3f, 0x73, 0x67, 0x50, 0x6d, 0x78, 0x3e, 0x1c, 0x2a, 0x76, 0x6e, 0x5b, 0x00, 0x40, 0x63 };

// scrolling globals
uint8_t data[] = { 0x00, 0x00, 0x00, 0x00 };
uint8_t scroll_1_segments[SCROLL_LIMIT + 5];
uint16_t scroll_1_length = 0;
uint8_t scroll_2_segments[SCROLL_LIMIT + 5];
uint16_t scroll_2_length = 0;
uint8_t scroll_3_segments[SCROLL_LIMIT + 5];
uint16_t scroll_3_length = 0;
uint16_t scrollPosition = 0;

// countdown globals
long timeRemaining = 0;
long countdownStart = 0;
long countdownMinutes = 0;
long countdownSeconds = 0;
uint8_t bangScrollSegments[SCROLL_LIMIT + 5];
uint16_t bangScrollLength = 0;
uint8_t dudScrollSegments[SCROLL_LIMIT + 5];
uint16_t dudScrollLength = 0;

// mode globals and button setup
uint8_t mode = 0;
uint8_t lastMode = 0;
uint8_t buttonState = 0;
uint8_t previousButtonState = 0;

hw_timer_t* timer = NULL;

void IRAM_ATTR onTimer() {
  buttonState = digitalRead(BUTTON);
  if(buttonState == LOW && previousButtonState == HIGH) {
    if(mode < NUM_MODES - 1) {
      mode++;
    }
    else {
      mode = 0;
    }
  }
  previousButtonState = buttonState;
}

// display setup
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);

void setup()
{
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000, true);
  timerAlarmEnable(timer);
  
  display.setBrightness(0x07);

  pinMode(BUTTON, INPUT);
  pinMode(BATT_VOLT, INPUT);

  randomSeed(analogRead(0));

  encodeScrollString(scroll_1_length, scroll_1_segments, "boop");
  encodeScrollString(scroll_2_length, scroll_2_segments, "crappy badge");
  encodeScrollString(scroll_3_length, scroll_3_segments, "hacker");

  encodeScrollString(bangScrollLength, bangScrollSegments, "bang      ");
  encodeScrollString(dudScrollLength, dudScrollSegments, "dud      ");
}

void loop()
{
  if(mode != lastMode) {
    scrollPosition = 0;
    for(int i = 0; i < 4; i++) {
      data[i] = BLANK_SEGMENT;
    }
  }

  // scroll modes
  if(mode < 4) {
    switch(mode) {
      case 0:
        for(int j = 0; j < 3; j++) {
          data[j] = data[j+1];
        }
        data[3] = CHAR_SEGMENTS[random(16)];
        break;
      case 1:
        scrollText(data, scroll_1_segments, scroll_1_length, scrollPosition);
        break;
      case 2:
        scrollText(data, scroll_2_segments, scroll_2_length, scrollPosition);
        break;
      case 3:
        scrollText(data, scroll_3_segments, scroll_3_length, scrollPosition);
        break;
    }
    display.setSegments(data, 4, 0);
  }

  // countdown mode
  if(mode == 4) {
    if(timeRemaining <= 0) {
      timeRemaining = COUNTDOWN_SECS;
      countdownStart = millis();
    }

    timeRemaining = (COUNTDOWN_SECS - (millis() - countdownStart) / 1000);
    countdownMinutes = timeRemaining / 60 * 100;
    countdownSeconds = timeRemaining - (60 * countdownMinutes / 100);
    display.showNumberDecEx(countdownMinutes + countdownSeconds, 0b01000000, true);

    if(timeRemaining == 0) {
      delay(1000);
      if(random(2)) {
        for(uint16_t i = 0; i < bangScrollLength;) {
          scrollText(data, bangScrollSegments, bangScrollLength, i);
          display.setSegments(data, 4, 0);
          delay(DELAY_MILLIS);
        }
      }
      else {
        for(uint16_t i = 0; i < dudScrollLength;) {
          scrollText(data, dudScrollSegments, dudScrollLength, i);
          display.setSegments(data, 4, 0);
          delay(DELAY_MILLIS);
        }
      }
    }
  }

  // battery voltage monitor
  if(mode == 5) {
    display.showNumberDec(map(analogRead(BATT_VOLT), 0, 2305, 0, 420));
  }

  lastMode = mode;
  delay(DELAY_MILLIS);
}

void scrollText(uint8_t* output, const uint8_t* scrollText, const uint16_t& scrollLength, uint16_t& scrollPosition) {
  if(scrollPosition >= scrollLength) {
    scrollPosition = 0;
  }

  for(int j = 0; j < 3; j++) {
    output[j] = output[j+1];
  }
  output[3] = scrollText[scrollPosition++];
}

void encodeScrollString(uint16_t& scrollLength, uint8_t* scrollSegments, String input) {
  scrollLength = constrain(input.length(), 0, SCROLL_LIMIT) + SCROLL_PADDING;

  for(int i = 0; i < scrollLength; i++) {
    if(i < SCROLL_PADDING) {
      scrollSegments[i] = BLANK_SEGMENT;
    }
    else {
      scrollSegments[i] = encodeCharacter(input.charAt(i - SCROLL_PADDING));
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
