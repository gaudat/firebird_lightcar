// Encoders
#define ENCL 2
#define ENCR 3

// Motors
#define MOTRF 4
#define MOTRR 5
#define MOTLF 6
#define MOTLR 7

// IRs
#define IR1 A0
#define IR2 A1
#define IR3 A2
#define IR4 A3
#define IR5 A4

// LED
#define LED 13


void setup() {
  Serial.begin(115200);
  Serial.println("Init pin");
  pinMode(ENCL,INPUT);
  pinMode(ENCR,INPUT);
  pinMode(MOTRF,OUTPUT);
  digitalWrite(MOTRF,LOW);
  pinMode(MOTRR,OUTPUT);
  digitalWrite(MOTRR,LOW);
  pinMode(MOTLF,OUTPUT);
  digitalWrite(MOTLF,LOW);
  pinMode(MOTLR,OUTPUT);
  digitalWrite(MOTLR,LOW);
  pinMode(IR1,INPUT);
  pinMode(IR2,INPUT);
  pinMode(IR3,INPUT);
  pinMode(IR4,INPUT);
  pinMode(IR5,INPUT);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  
  Serial.println("White detect");
  for (int i=0;i<10;i++) {
  digitalWrite(LED,HIGH);
  ir1White += analogRead(IR1);  
  ir2White += analogRead(IR2);
  ir3White += analogRead(IR3);
  ir4White += analogRead(IR4);
  ir5White += analogRead(IR5);
  digitalWrite(LED,LOW);
  Serial.println(String(ir1White) + " " + String(ir2White) + " " + String(ir3White)
  + " " + String(ir4White) + " " + String(ir5White));
  delay(100);
  }
  Serial.println("Setup done");
}

// Calibration levels
int ir1White = 0;
int ir1Black = 0;
int ir2White = 0;
int ir2Black = 0;
int ir3White = 0;
int ir3Black = 0;
int ir4White = 0;
int ir4Black = 0;
int ir5White = 0;
int ir5Black = 0;

// Timers
unsigned long softTimer = 0;
unsigned long lastLineLost = 0;
bool lineLost = false;

// Target speed
int targetLeftRate = 10000;
int targetRightRate = 10000;

int isBlack(int pin) {
return analogRead(pin) < threshold(pin) - 10
}

int threshold(int pin) {
switch (pin) {
case IR1:
return ir1White;
case IR2:
return ir2White;
case IR3:
return ir3White;
case IR4:
return ir4White;
case IR5:
return ir5White;
}
}

void loop() {
//Serial.println("Loop started at " + String(micros()));
Serial.println(isBlack(IR5) << 4 + isBlack(IR4) << 3 + isBlack(IR3) << 2 + isBlack(IR2) << 1 + isBlack(IR1),BIN);
  /*switch (isBlack(IR5) << 4 + isBlack(IR4) << 3 + isBlack(IR3) << 2 + isBlack(IR2) << 1 + isBlack(IR1)) {
    case B00100:
    case B01110:
      {
        Serial.println("We are straight. Accelerating.");
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
  */
  Serial.println("Target left rate = " + String(targetLeftRate) + ", Target right rate = " + String(targetRightRate));
  moveMotorLeft(true, targetLeftRate);
  moveMotorRight(true, targetRightRate);
  Serial.println("Loop ended at " + String(micros()));
  delay(500);
}


void moveMotorRight(int targetEncoderRate) {
  // Move motor at the specified direction, aiming target encoder rate
  int pw = pulseIn(ENCR, HIGH, 10000);
  if (targetEncoderRate > pw && pw != 0) {
      // motor is going too fast
      digitalWrite(MOTRF, LOW);
      digitalWrite(MOTRR,HIGH);
    } else {
      // motor is going too slow
      digitalWrite(MOTRR,LOW);
      digitalWrite(MOTRF, HIGH);
    }
}

void moveMotorLeft(int targetEncoderRate) {
  // Move motor at the specified direction, aiming target encoder rate
  int pw = pulseIn(ENCL, HIGH, 10000);
  if (targetEncoderRate > pw && pw != 0) {
      // motor is going too fast
      digitalWrite(MOTLF, LOW);
      digitalWrite(MOTLR,HIGH);
    } else {
      // motor is going too slow
      digitalWrite(MOTRR,LOW);
      digitalWrite(MOTRF, HIGH);
    }
}
