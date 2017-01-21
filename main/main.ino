// Pins
#define LATCH 6
#define DATA 5
#define CLK 4

#define TOGGLE 2
#define SELECT 3

// Constants
#define VIEW_DELAY 2000
#define DEBOUNCE_TIME 200

typedef enum State {
  idle,
  sides,
  rolls,
  rolling,
  total
} State;

typedef enum ButtonState {
  low,
  high
} ButtonState;

const byte faces[6] = { 4, 6, 8, 10, 12, 20 };
const byte maxRolls[6] = { 24, 16, 12, 9, 8, 4 };
const byte numbers[10] = { 0xfc, 0x90, 0x7a, 0xda, 0x96, 0xce, 0xee, 0x98, 0xfe, 0x9e };

volatile int currentViewDelay = 0;
volatile int rollCount = 1;
volatile int faceIndex = 5;
volatile int currentRollValue = 0;
volatile State state = idle;

// Animation
// segments 0x0, 0x8, 0x10, 0x80, 0x40, 0x20, 0x4, 0x2

const byte terminals[16] = { 0x0, 0x8,
                           0x8, 0x0,
                           0x10, 0x0,
                           0x80, 0x0,
                           0x40, 0x0,
                           0x0, 0x40,
                           0x0, 0x20,
                           0x0, 0x4};

void setup() {
  //delay(2000);
  Serial.begin(9600);
  pinMode(LATCH, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(CLK, OUTPUT);

  pinMode(TOGGLE, INPUT);
  pinMode(SELECT, INPUT);

  attachInterrupt(0, toggleInterrupt, CHANGE);
  attachInterrupt(1, toggleSelect, CHANGE);
  reset();

  
}

// TODO replace with interrupts!
void loop() {
  if(state == rolling) {
    for(int i = 0; i < 16; i+= 2) {
      pushDigit(terminals[i]);
      pushDigit(terminals[i + 1]);
      outRegister();
      delay(60);
    }
    roll();
    state = total;
    updateDisplay();
  }
}

void updateDisplay() {
  reset();
  if(state == idle) {
    reset();
  } else if(state == sides) {
    show(faces[faceIndex]);
  } else if(state == rolls) {
    if(rollCount > 0) {
      show(rollCount);
    } else {
      reset();
    }
  } else {
    show(currentRollValue);
  }
}

// Toggles the states
void toggleInterrupt() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if(interrupt_time - last_interrupt_time > DEBOUNCE_TIME) {
    if(digitalRead(TOGGLE) == LOW) {
      Serial.println("Toggle interrupt pressed");
      if(state == idle) {
        state = sides;
      } else if(state == sides) {
        state = rolls;
      } else if(state == rolls) {
        state = idle;
      } else if(state == total) {
        state = idle;
      }
      updateDisplay();
    }
  }
  last_interrupt_time = interrupt_time;
}

void toggleSelect() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if(interrupt_time - last_interrupt_time > DEBOUNCE_TIME) {
    if(digitalRead(SELECT) == LOW) {
      Serial.println("Select interrupt pressed");
      if(state == idle) {
        state = rolling;
      } else if(state == sides) {
        faceIndex += 1;
        if(faceIndex > 5) {
          faceIndex = 0;
        }
        rollCount = 1;
      } else if(state == rolls) {
        rollCount += 1;
        if(rollCount > maxRolls[faceIndex]) {
          rollCount = 1;
        }
      } else if(state == total) {
        state = rolling;
      }
      updateDisplay();
    }
  }
  last_interrupt_time = interrupt_time;
}

void roll() {
  currentRollValue = 0;
  for(int i = 0; i < rollCount; i++) {
    currentRollValue += random(0, faces[faceIndex]) + 1;
  }
}

void reset() {
  pushDigit(0x00);
  pushDigit(0x00);
  outRegister();
}

void show(int value) {
  int remainder = max(0, min(value, 99));
  if(remainder < 10) {
    pushDigit(numbers[remainder]);
    pushDigit(0x00);
  } else {
    pushDigit(numbers[remainder % 10]);
    pushDigit(numbers[remainder / 10]);
  }
  outRegister();
}

void pushDigit(int value) {
  for(int i = 7; i >= 0; i--) {
    shiftOut((value & (1 << i)) == 0);
  }
}

void outRegister() {
  digitalWrite(LATCH, LOW);
  digitalWrite(LATCH, HIGH);
}

void shiftOut(bool high) {
  digitalWrite(CLK, LOW);
  digitalWrite(DATA, high ? HIGH : LOW);
  digitalWrite(CLK, HIGH);
  digitalWrite(DATA, HIGH);
}

