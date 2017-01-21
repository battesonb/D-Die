// Pins
#define LATCH 6
#define DATA 5
#define CLK 4

#define TOGGLE 2
#define SELECT 3

// Constants
#define VIEW_DELAY 2000

typedef enum State {
  idle,
  sides,
  rolls
} State;

typedef enum ButtonState {
  low,
  high
} ButtonState;

const byte faces[6] = { 4, 6, 8, 10, 12, 20 };
const byte numbers[10] = { 0xfc, 0x90, 0x7a, 0xda, 0x96, 0xce, 0xee, 0x98, 0xfe, 0x9e };

volatile int currentViewDelay = 0;
volatile int rollCount = 1;
volatile int faceIndex = 0;
volatile int currentRollValue = 0;
volatile State state = idle;

volatile ButtonState toggleBtnState = high;
volatile ButtonState selectBtnState = high;

void setup() {
  delay(2000);
  Serial.begin(9600);
  Serial.println("begin");
  pinMode(LATCH, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(CLK, OUTPUT);

  pinMode(TOGGLE, INPUT);
  pinMode(SELECT, INPUT);

  attachInterrupt(0, toggleInterrupt, CHANGE);
  attachInterrupt(1, toggleSelect, CHANGE);
  reset();
  show(numbers[0]);
  show(numbers[0]);
}

// TODO replace with interrupts!
void loop() {
  //meat();
  delay(10);
}

void meat() {
  reset();
  if(state == idle) {
    if(currentRollValue >= 0) {
      show(currentRollValue);
    }
   } else if(state == sides) {
    show(faces[faceIndex]);
   } else {
    show(rollCount);
   }
}

// Toggles the states
void toggleInterrupt() {
  toggleBtnState = digitalRead(TOGGLE);
  delay(100);
  ButtonState currentBtnState = digitalRead(TOGGLE);
  if(toggleBtnState == currentBtnState && toggleBtnState == LOW) {
    Serial.println("Toggle interrupt");
    show(numbers[1]);
    show(numbers[1]);
    if(state == idle) {
      state = sides;
    } else if(state == sides) {
      state = rolls;
    } else {
      state = idle;
    }
  }
  toggleBtnState = currentBtnState;
}

void toggleSelect() {
  selectBtnState = digitalRead(SELECT);
  delay(100);
  ButtonState currentBtnState = digitalRead(SELECT);
  if(selectBtnState == currentBtnState && selectBtnState == LOW) {
    Serial.println("Select interrupt");
    show(numbers[2]);
    show(numbers[2]);
    if(state == idle) {
      roll();
    } else if(state == sides) {
      faceIndex += 1;
      if(faceIndex > 9) {
        faceIndex = 0;
      }
    } else {
      rollCount += 1;
      if(rollCount > 9) {
        rollCount = 1;
      }
    }
  }
  selectBtnState = currentBtnState;
}

void roll() {
  
}

void reset() {
  show(0x00);
  show(0x00);
}

void show(int value) {
  digitalWrite(LATCH, LOW);
  for(int i = 7; i >= 0; i--) {
    shiftOut((value & (1 << i)) == 0);
  }
  digitalWrite(LATCH, HIGH);
}

void shiftOut(bool high) {
  digitalWrite(CLK, LOW);
  digitalWrite(DATA, high ? HIGH : LOW);
  digitalWrite(CLK, HIGH);
  digitalWrite(DATA, HIGH);
}

