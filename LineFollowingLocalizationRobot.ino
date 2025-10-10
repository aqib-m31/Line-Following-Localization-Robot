
/*
 LineFollowingLocalizationRobot.ino
 A robot that uses two IR sensors to follow a black line grid and localize itself
 by stopping at grid intersections (line junctions).
 */

// IR Sensor Pins
// White = 0, Black = 1
const int IR_RIGHT = 5;
const int IR_LEFT = 4;

// Motor Driver Pins (PWM for motor speed control)
const int EN_A = 10;  // Right motor enable pin (Grey)
const int EN_B = 11;  // Left motor enable pin (Orange)

// Motor Control Pins
const int MOTOR_RIGHT_1 = 9;  // INPUT 1 - Purple
const int MOTOR_RIGHT_2 = 8;  // INPUT 2 - Blue
const int MOTOR_LEFT_1 = 6;   // INPUT 3 - Green
const int MOTOR_LEFT_2 = 7;   // INPUT 4 - Yellow

// IR sensor values
int leftIrValue = 0;
int rightIrValue = 0;

// Position tracking
int originX = 1;
int originY = 1;
int startX = 3;
int startY = 4;
int destX = 1;
int destY = 1;

// Direction enum
enum Direction {
  NORTH,
  WEST,
  SOUTH,
  EAST
};

// Assumptions:
// The robot will always face NORTH intially starting at bottom left corner of the gird. (south-west)
Direction currentDirection = NORTH;  // direction that the robot is facing
Direction toDirection;               // direction the robotr is supposed to face

int destinationReached = 0;
int startReached = 0;

void setup() {
  Serial.begin(9600);

  pinMode(EN_A, OUTPUT);
  pinMode(EN_B, OUTPUT);

  pinMode(MOTOR_LEFT_1, OUTPUT);
  pinMode(MOTOR_LEFT_2, OUTPUT);
  pinMode(MOTOR_RIGHT_1, OUTPUT);
  pinMode(MOTOR_RIGHT_2, OUTPUT);

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);
}

void loop() {
  if (startReached == 0) {
    moveForwardAndStopAtMark();  // Go to start pos
    gotoStart();
    startReached = 1;
  }
  if (destinationReached == 0) {
    originX = startX;
    originY = startY;
    startX = destX;
    startY = destY;
    gotoStart();
    destinationReached = 1;
  }
}

// Set motor speed using PWM
void setSpeed(unsigned int speed = 70) {
  analogWrite(EN_A, speed);
  analogWrite(EN_B, speed);
}

// Slight right adjustment for calibration
void calibrateRight(unsigned int time = 20) {
  digitalWrite(MOTOR_LEFT_1, HIGH);
  digitalWrite(MOTOR_LEFT_2, LOW);
  digitalWrite(MOTOR_RIGHT_1, HIGH);
  digitalWrite(MOTOR_RIGHT_2, LOW);
  delay(time);
}

// Slight left adjustment for calibration
void calibrateLeft(unsigned int time = 20) {
  digitalWrite(MOTOR_LEFT_1, LOW);
  digitalWrite(MOTOR_LEFT_2, HIGH);
  digitalWrite(MOTOR_RIGHT_1, LOW);
  digitalWrite(MOTOR_RIGHT_2, HIGH);
  delay(time);
}

// Move forward with optional speed and duration
void forward(unsigned int speed = 70, unsigned int time = 0) {
  setSpeed(speed);
  digitalWrite(MOTOR_LEFT_1, HIGH);
  digitalWrite(MOTOR_LEFT_2, LOW);
  digitalWrite(MOTOR_RIGHT_1, LOW);
  digitalWrite(MOTOR_RIGHT_2, HIGH);
  delay(time);
}

// Follow the line forward until intersection is reached
void calibrateForward(unsigned int speed = 70) {
  while (true) {
    leftIrValue = digitalRead(IR_LEFT);
    rightIrValue = digitalRead(IR_RIGHT);
    if (leftIrValue == 0 && rightIrValue == 0) {
      forward();
    } else if (leftIrValue == 1 && rightIrValue == 0) {
      calibrateLeft(15);
    } else if (rightIrValue == 1 && leftIrValue == 0) {
      calibrateRight(15);
    } else if (leftIrValue == 1 && rightIrValue == 1) {
      return;
    }
  }
}

// Move forward for a short distance before turning
void calibrateForwardforTurn(unsigned int speed = 70) {
  long int timeElapsed = millis();
  while (timeElapsed + 300 > millis()) {
    leftIrValue = digitalRead(IR_LEFT);
    rightIrValue = digitalRead(IR_RIGHT);
    if (leftIrValue == 1 && rightIrValue == 0) {
      calibrateLeft(15);
    } else if (rightIrValue == 1 && leftIrValue == 0) {
      calibrateRight(15);
    } else {
      forward();
    }
  }
}

// Stop all motors
void stop(unsigned int time = 1000) {
  digitalWrite(MOTOR_LEFT_1, LOW);
  digitalWrite(MOTOR_LEFT_2, LOW);
  digitalWrite(MOTOR_RIGHT_1, LOW);
  digitalWrite(MOTOR_RIGHT_2, LOW);
  delay(time);
}

// Move forward until junction is detected and stop
void moveForwardAndStopAtMark() {
  calibrateForward();
  while (true) {
    leftIrValue = digitalRead(IR_LEFT);
    rightIrValue = digitalRead(IR_RIGHT);
    if (leftIrValue == 1 && rightIrValue == 1) {
      forward();
    } else if (leftIrValue == 1 && rightIrValue == 0) {
      calibrateLeft(15);
    } else if (rightIrValue == 1 && leftIrValue == 0) {
      calibrateRight(15);
    } else if (leftIrValue == 0 && rightIrValue == 0) {
      stop(2000);
      break;
    }
  }
}

// Turn robot right until aligned with next direction
void turnRight() {
  calibrateForwardforTurn();
  int turnFlag = 0;
  do {
    leftIrValue = digitalRead(IR_LEFT);
    rightIrValue = digitalRead(IR_RIGHT);
    if (leftIrValue == 1 && rightIrValue == 1 && turnFlag == 1) {
      stop(1000);
      break;  // [D]
    } else if (leftIrValue == 0 && rightIrValue == 0) {
      turnFlag = 1;
      setSpeed(70);
      digitalWrite(MOTOR_LEFT_1, HIGH);  // HIGH
      digitalWrite(MOTOR_LEFT_2, LOW);
      digitalWrite(MOTOR_RIGHT_1, LOW);
      digitalWrite(MOTOR_RIGHT_2, LOW);
    } else {
      setSpeed(70);
      digitalWrite(MOTOR_LEFT_1, HIGH);  // HIGH
      digitalWrite(MOTOR_LEFT_2, LOW);
      digitalWrite(MOTOR_RIGHT_1, LOW);
      digitalWrite(MOTOR_RIGHT_2, LOW);
    }
  } while (true);
}

// Turn robot left until aligned with next direction
void turnLeft() {
  calibrateForwardforTurn();
  int turnFlag = 0;
  do {
    leftIrValue = digitalRead(IR_LEFT);
    rightIrValue = digitalRead(IR_RIGHT);
    if (leftIrValue == 1 && rightIrValue == 1 && turnFlag == 1) {
      stop(1000);
      break;  // [D]
    } else if (leftIrValue == 0 && rightIrValue == 0) {
      turnFlag = 1;
      setSpeed(70);
      digitalWrite(MOTOR_LEFT_1, LOW);
      digitalWrite(MOTOR_LEFT_2, LOW);
      digitalWrite(MOTOR_RIGHT_1, LOW);
      digitalWrite(MOTOR_RIGHT_2, HIGH);  // HIGH
    } else {
      setSpeed(70);
      digitalWrite(MOTOR_LEFT_1, LOW);
      digitalWrite(MOTOR_LEFT_2, LOW);
      digitalWrite(MOTOR_RIGHT_1, LOW);
      digitalWrite(MOTOR_RIGHT_2, HIGH);  // HIGH
    }
  } while (true);
}

// Align robot from current direction to target direction
void alignDirection(Direction &currentDirection, Direction toDirection) {
  int turnValue = ((currentDirection - toDirection + 4) % 4);
  if (turnValue == 3) {
    turnLeft();
  } else if (turnValue == 1) {
    turnRight();
  } else if (turnValue == 2) {
    turnRight();
    turnRight();
  }
  currentDirection = toDirection;
}

// Navigate robot to start position
void gotoStart() {
  while (true) {
    if (originX == startX && originY == startY) {  // already at starting position
      stop(1500);
      break;
    } else if ((startX - originX) >= 0 && (startY - originY) > 0) {  // first inequality checks if starting column is east of origin, second inequality checks it is not the same column
      alignDirection(currentDirection, EAST);
      // move to the same column as starting column
      while (originY != startY) {
        moveForwardAndStopAtMark();
        originY++;
      }
    } else if (startX - originX > 0) {
      alignDirection(currentDirection, NORTH);
      while (originX != startX) {
        moveForwardAndStopAtMark();
        originX++;
      }
    } else if ((startX - originX) <= 0 && (startY - originY) < 0) {  // first inequality checks if starting column is east of origin, second inequality checks it is not the same column
      alignDirection(currentDirection, WEST);
      // move to the same column as starting column
      while (originY != startY) {
        moveForwardAndStopAtMark();
        originY--;
      }
    } else if (startX - originX < 0) {
      alignDirection(currentDirection, SOUTH);
      while (originX != startX) {
        moveForwardAndStopAtMark();
        originX--;
      }
    }
  }
}