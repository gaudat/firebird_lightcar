/*
  Firebird line car

  Mode of operation:
  L - - R
  X X X X <- Line sensors
  No line detected. Stop immediately

  X O O X
  Line is straight. Accelerate.

  O O X X
  Line is to the left. Decelerate and turn left

  X X O O
  Same as above, but turn right
*/
/*
// Pin definitions
#define P_ENCODER_L 2
#define P_ENCODER_R 3
#define P_SENSOR_RR 4
#define P_SENSOR_MR 5
#define P_SENSOR_ML 6
#define P_SENSOR_LL 7
#define P_PWM_IN1 8
#define P_PWM_IN2 9
#define P_PWM_IN3 10
#define P_PWM_IN4 11

// Motor parameters

// Maximum acceleration/deceleration of motor, pwm value per loop
const int max_acceleration = 255;

// Time delay for line loss detection

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Firebird line car");
  Serial.println("Init");
  pinMode(P_ENCODER_L, INPUT);
  pinMode(P_ENCODER_R, INPUT);
  pinMode(P_SENSOR_RR, INPUT);
  pinMode(P_SENSOR_MR, INPUT);
  pinMode(P_SENSOR_ML, INPUT);
  pinMode(P_SENSOR_LL, INPUT);
  pinMode(P_PWM_IN1, OUTPUT);
  digitalWrite(P_PWM_IN1,LOW);
  pinMode(P_PWM_IN2, OUTPUT);
  digitalWrite(P_PWM_IN2,LOW);
  pinMode(P_PWM_IN3, OUTPUT);
  digitalWrite(P_PWM_IN3,LOW);
  pinMode(P_PWM_IN4, OUTPUT);
  digitalWrite(P_PWM_IN4,LOW);
  Serial.println("Init done");
}

unsigned long lastLineLost = 0;
bool lineLost = false;

int targetLeftRate = 10000;
int targetRightRate = 10000;

void loop() {
  //Serial.println("Loop started at " + String(micros()));
  switch (digitalRead(P_SENSOR_LL) << 3 + digitalRead(P_SENSOR_ML) << 2 + digitalRead(P_SENSOR_MR) << 1 + digitalRead(P_SENSOR_RR)) {
    case B0110:
      {
        Serial.println("0110 detected. Accelerating.");
        // Everything is going well. turn straight and accelerate.
        int diff = targetLeftRate - targetRightRate;
        targetLeftRate -= diff / 10; // Acceleration relaxation time
        targetRightRate += diff / 10;
        targetLeftRate -= 100;
        targetRightRate -= 100;

      }
      break;
    case B0000:
      { Serial.println("Line lost.");
        // We lost the line. wait 0.1 s then stop everything
        if (!lineLost) {
          lineLost = true;
          lastLineLost = millis();
          moveMotorLeft(true, 10000);
          moveMotorRight(true, 10000);
        } else {
          if (millis() - lastLineLost > 100) {
            // One second of line lost
            Serial.println("Line lost timeout excedded. Stopping everything..");
            stopEverything();
          }
        }
      }
      break;
    // TURN LEFT STUFF

    case B1000:
      { Serial.println("Slowing down and turning left.");
        targetLeftRate += 150;
        targetRightRate += 100;
        // SLOW DOWN x3
    }  case B1100:
    case B0100:
      { Serial.println("Slowing down and turning left.");
        targetLeftRate += 150;
        targetRightRate += 100;
      }    // SLOW DOWN x2
    case B1110:
      { Serial.println("Slowing down and turning left.");
        targetLeftRate += 150;
        targetRightRate += 100;
        // SLOW DOWN x1
      }       break;
    // TURN RIGHT STUFF
    case B0001:
      { // SLOW DOWN x3
        Serial.println("Slowing down and turning right.");
        targetLeftRate += 100;
        targetRightRate += 150;
    }      case B0010:
    case B0011:
      { Serial.println("Slowing down and turning right.");
        targetLeftRate += 100;
        targetRightRate += 150;
        // SLOW DOWN x2
    }         case B0111:
      { Serial.println("Slowing down and turning right.");
        targetLeftRate += 100;
        targetRightRate += 150;
        // SLOW DOWN x1
      }            break;
    default:
      // INVALID COMBINATION, SLOW DOWN A BIT
      { Serial.println("Invalid input. Slowing down just to be safe.");
        targetLeftRate += 100;
        targetRightRate += 100;
      }
  }
  if (targetLeftRate > 10000) {
    targetLeftRate = 10000;
  }
  if (targetRightRate > 10000) {
    targetRightRate = 10000;
  }
  if (targetLeftRate < 2500) {
    targetLeftRate = 2500;
  }
  if (targetRightRate < 2500) {
    targetRightRate = 2500;
  }
  Serial.println("Target left rate = " + String(targetLeftRate) + ", Target right rate = " + String(targetRightRate));
  moveMotorLeft(true, targetLeftRate);
  moveMotorRight(true, targetRightRate);
  //Serial.println("Loop ended at " + String(micros()));
}


// Direction: true = forward, false = backward
boolean motorLeftDirection = true;
// IN1 = Right, go forward; IN2 = Right, go back; IN3 = Left, go forward, IN4 = Left, go back

void moveMotorLeft(bool direction, int targetEncoderRate) {
  // Move motor at the specified direction, aiming target encoder rate
  int pw = pulseIn(P_ENCODER_L, HIGH, 10000);
  if (direction && motorLeftDirection) {
    // motor is going forward
    if (targetEncoderRate > pw && pw != 0) {
      // motor is going too fast
      digitalWrite(P_PWM_IN3, LOW);
    } else {
      // motor is going too slow
      digitalWrite(P_PWM_IN3, HIGH);
    }
  } else if (!direction && !motorLeftDirection) {
    // motor is going backwards
    if (targetEncoderRate > pw && pw != 0) {
      // motor is going too fast
      digitalWrite(P_PWM_IN4, LOW);
    } else {
      // motor is going too slow
      digitalWrite(P_PWM_IN4, HIGH);
    }
  } else if (!direction && motorLeftDirection) {
    // we want to go back but the motor is going forward
    digitalWrite(P_PWM_IN4, HIGH);
    digitalWrite(P_PWM_IN3, LOW);
  } else if (direction && !motorLeftDirection) {
    // reverse of above case
    digitalWrite(P_PWM_IN3, HIGH);
    digitalWrite(P_PWM_IN4, LOW);
  }
}

bool motorRightDirection = true;

void moveMotorRight(bool direction, int targetEncoderRate) {
  // Move motor at the specified direction, aiming target encoder rate
  int pw = pulseIn(P_ENCODER_R, HIGH, 10000);
  if (direction && motorRightDirection) {
    // motor is going forward
    if (targetEncoderRate > pw && pw != 0) {
      // motor is going too fast
      digitalWrite(P_PWM_IN1, LOW);
    } else {
      // motor is going too slow
      digitalWrite(P_PWM_IN1, HIGH);
    }
  } else if (!direction && !motorRightDirection) {
    // motor is going backwards
    if (targetEncoderRate > pw && pw != 0) {
      // motor is going too fast
      digitalWrite(P_PWM_IN2, LOW);
    } else {
      // motor is going too slow
      digitalWrite(P_PWM_IN2, HIGH);
    }
  } else if (!direction && motorRightDirection) {
    // we want to go back but the motor is going forward
    digitalWrite(P_PWM_IN2, HIGH);
    digitalWrite(P_PWM_IN1, LOW);
  } else if (direction && !motorRightDirection) {
    // reverse of above case
    digitalWrite(P_PWM_IN1, HIGH);
    digitalWrite(P_PWM_IN2, LOW);
  }
}

void stopEverything() {
  Serial.println("Stopping everything...");
  digitalWrite(P_PWM_IN1, LOW);
  digitalWrite(P_PWM_IN2, LOW);
  digitalWrite(P_PWM_IN3, LOW);
  digitalWrite(P_PWM_IN4, LOW);
  Serial.println("Stopped");
}

*/
