#include "Communication.h"
#define DIR 9
#define STEP 8
#define ST_ENABLE 7
#define LIMITSW 6
#define BACKLIGHT 5

#define led 13
#define STEPUP    LOW
#define STEPDOWN  HIGH

#define STEPENABLE LOW
#define STEPDISABLE HIGH

unsigned char stepSpeed = 0;
unsigned char stepSpeedCount = 0;
unsigned int stepNum = 0;
unsigned char stepDir = 0;

int elevatorPos = 0;
int setElevatorPos = 0;
int deltaElevatorPos = 0;
unsigned char initStepSpeed = 3;

/*====================================
         Communication variables
    ======================================*/
struct {
  unsigned char run: 1;
  unsigned char display: 1;
  unsigned char resetElevator: 1;
  unsigned char startPrint: 1;
} flag;

char command[100];
unsigned char command_index = 0;
boolean stringComplete = false;  // whether the string is complete
/*=====================================
          Timer variables
    =====================================*/
unsigned long currentMicros;
unsigned long previousMicros_100Hz = 0;        // will store last time it was called for 100Hz timer
const long interval_100Hz = 10000;           // interval at which to do 100Hz task(micros)

unsigned long previousMicros_50Hz = 0;        // will store last time it was called for 50Hz timer
const long interval_50Hz = 20000;           // interval at which to do 50Hz task (micros)

unsigned long previousMicros_500Hz = 0;       // will store last time it was called for 50Hz timer
const long interval_500Hz = 2000;           // interval at which to do 500Hz task (micros)


unsigned long previousMicros_25Hz = 0;       // will store last time it was called for 25Hz timer
const long interval_25Hz = 40000;           // interval at which to do 500Hz task (micros)
/*=====================================
          Printing variables
          Timer: 25hz
          Lead of screw of elevator: 8mm
          Stepper motor: 2 phases, 200 steps/rev
    =====================================*/
#define PRINTING_TASK_INIT 0
#define PRINTING_TASK_CURING 1
#define PRINTING_TASK_SEPARATOR_UP 2
#define PRINTING_TASK_SEPARATOR_DOWN 3
#define PRINTING_TASK_WAITFORSETTLE 4
#define PRINTING_TASK_IDLE 10
#define TASK_BEGIN 0
#define TASK_RUN 1
#define TASK_END 2

unsigned char layerHeight = 2;// 2 full steps = 8mm/200x2=0.08mm=80um
unsigned char curringTime = 100;//150; problem of backlight not uniform // 125 periods of 25Hz timer = 125/25=5s
unsigned char curringTimeBase = 175; //250: 10s
unsigned char numOfLayerBase = 3;
unsigned char layerCountBase = 0;

unsigned char liquidSettlingTime = 50; // 25 periods = 1s
unsigned char seperatorHeight = 100; //200 full steps = 8mm
unsigned char numOfLayer = 70;
unsigned char layerCount = 0;

unsigned char initLayerPos = 2; //tuning manually

unsigned int printing_timer = 0;



struct {
  unsigned char taskStt;
  unsigned char taskID;
  unsigned char basePrint: 1;
} print_status;

void reset_print_para() {
  layerCountBase = 0;
  printing_timer = 0;
  layerCount = 0;
  print_status.taskStt = TASK_BEGIN;
  print_status.taskID = PRINTING_TASK_IDLE;
  print_status.basePrint = 0;
}
void reset_system() {
  flag.resetElevator = 0;
  digitalWrite(DIR, STEPDOWN);
}
void SetElevatorPos(int setPos, unsigned char sp) {
  if (setPos >= 0) {
    setElevatorPos = setPos;
    deltaElevatorPos = setPos - elevatorPos;
    if (deltaElevatorPos >= 0) {
      stepDir = STEPUP;
      stepNum = deltaElevatorPos;
    } else {
      stepDir = STEPDOWN;
      stepNum = -deltaElevatorPos;
    }
    digitalWrite(DIR, stepDir);
  }
  stepSpeed = sp;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(ST_ENABLE, OUTPUT);
  digitalWrite(ST_ENABLE, STEPDISABLE);//high: disenable | low: enable
  Serial.begin(115200);
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  pinMode(LIMITSW, INPUT);
  pinMode(led, OUTPUT);
  pinMode(BACKLIGHT, OUTPUT);

  digitalWrite(DIR, HIGH);
  digitalWrite(ST_ENABLE, STEPENABLE);//high: disenable | low: enable
  Serial.println("Done");

  reset_system();
}


unsigned char step_stage = 0;
void loop() {
  serialEvent();
  currentMicros = micros();
  //your code begin from here
  if (currentMicros - previousMicros_500Hz >= interval_500Hz)
  {
    previousMicros_500Hz = currentMicros;
    //your code begin from here
    //     digitalWrite(STEP, step_stage);
    //     digitalWrite(led,step_stage);

    if (flag.resetElevator == 0)
    {
      if (digitalRead(LIMITSW) == LOW)
      {
        if (stepSpeedCount != 0) {
          stepSpeedCount--;
        } else {
          step_stage ^= 1;
          digitalWrite(STEP, step_stage);
          digitalWrite(led, step_stage);
          stepSpeedCount = initStepSpeed;
        }
      }
      else
      {
        flag.resetElevator = 1;
        elevatorPos = 0;
        step_stage = LOW;
        digitalWrite(STEP, step_stage);
        digitalWrite(led, step_stage);
      }
    }
    else {
      if (stepSpeedCount != 0) {
        stepSpeedCount--;
      } else
      {
        if (stepNum != 0) {
          if (step_stage == LOW) {
            step_stage = HIGH;
          } else {
            step_stage = LOW;
            stepNum--;
            if (stepDir == STEPUP)
              elevatorPos++;
            else
              elevatorPos--;

            Serial.println(elevatorPos);
          }
          digitalWrite(STEP, step_stage);
          digitalWrite(led, step_stage);
        }
        stepSpeedCount = stepSpeed;
      }
    }
  }

  if (currentMicros - previousMicros_100Hz >= interval_100Hz)
  {
    previousMicros_100Hz = currentMicros;
    //your code begin from here
    //if(digitalRead(LIMITSW)==HIGH)
    //Serial.println("limit on");
    //else
    //Serial.println("limit off");


  }

  if (currentMicros - previousMicros_50Hz >= interval_50Hz)
  {
    previousMicros_50Hz = currentMicros;
    //your code begin from here

  }

  if (currentMicros - previousMicros_25Hz >= interval_25Hz)
  {
    previousMicros_25Hz = currentMicros;
    //your code begin from here
    if (flag.startPrint) {
      switch (print_status.taskID) {
        case PRINTING_TASK_INIT:
          switch (print_status.taskStt) {
            case TASK_BEGIN:
              Serial.println("PRINTING_TASK_INIT begin ");
              print_status.taskStt = TASK_RUN;
              print_status.basePrint = 1;
              SetElevatorPos(initLayerPos, 3);
              break;
            case TASK_RUN:
              if (setElevatorPos == elevatorPos)
                print_status.taskStt = TASK_END;
              break;
            case TASK_END:
              print_status.taskStt = TASK_BEGIN;
              print_status.taskID = PRINTING_TASK_WAITFORSETTLE;
              Serial.println("PRINTING_TASK_INIT end ");
              break;
            default:
              break;
          }
          break;
        case PRINTING_TASK_CURING:
          switch (print_status.taskStt) {
            case TASK_BEGIN:
              Serial.println("PRINTING_TASK_CURING begin ");
              print_status.taskStt = TASK_RUN;
              // turn on the light
              digitalWrite(BACKLIGHT, HIGH);
              break;
            case TASK_RUN:
              printing_timer++;
              if (print_status.basePrint) {
                if (printing_timer >= curringTimeBase)
                {
                  print_status.taskStt = TASK_END;
                  printing_timer = 0;
                }
              }
              else {
                if (printing_timer >= curringTime)
                {
                  print_status.taskStt = TASK_END;
                  printing_timer = 0;
                }
              }
              break;
            case TASK_END:
              print_status.taskStt = TASK_BEGIN;
              print_status.taskID = PRINTING_TASK_SEPARATOR_UP;
              Serial.println("PRINTING_TASK_CURING end ");
              // turn off the light
              digitalWrite(BACKLIGHT, LOW);
              break;
            default:
              break;
          }
          break;
        case PRINTING_TASK_SEPARATOR_UP:
          switch (print_status.taskStt) {
            case TASK_BEGIN:
              Serial.println("PRINTING_TASK_SEPARATOR_UP begin ");
              print_status.taskStt = TASK_RUN;
              SetElevatorPos(seperatorHeight + layerCount * layerHeight + initLayerPos + layerCountBase * layerHeight, 2);
              if (print_status.basePrint)
                layerCountBase++;
              else
                layerCount++;

              break;
            case TASK_RUN:
              if (setElevatorPos == elevatorPos)
                print_status.taskStt = TASK_END;
              break;
            case TASK_END:
              print_status.taskStt = TASK_BEGIN;
              print_status.taskID = PRINTING_TASK_SEPARATOR_DOWN;
              Serial.println("PRINTING_TASK_SEPARATOR_UP end ");
              if (print_status.basePrint) {
                if (layerCountBase == numOfLayerBase)
                {
                  print_status.basePrint = 0;
                  Serial.println("PRINTING Base finish ");
                } else
                  print_status.taskID = PRINTING_TASK_SEPARATOR_DOWN;
              }
              else {
                if (layerCount == numOfLayer)
                {
                  print_status.taskID = PRINTING_TASK_IDLE;
                  Serial.println("PRINTING finish ");
                }
                else
                  print_status.taskID = PRINTING_TASK_SEPARATOR_DOWN;
              }
              break;
            default:
              break;
          }
          break;
        case PRINTING_TASK_SEPARATOR_DOWN:
          switch (print_status.taskStt) {
            case TASK_BEGIN:
              Serial.println("PRINTING_TASK_SEPARATOR_DOWN begin ");
              print_status.taskStt = TASK_RUN;
              SetElevatorPos(layerCount * layerHeight + layerCountBase * layerHeight + initLayerPos, 2);
              break;
            case TASK_RUN:
              if (setElevatorPos == elevatorPos)
                print_status.taskStt = TASK_END;
              break;
            case TASK_END:
              print_status.taskStt = TASK_BEGIN;
              print_status.taskID = PRINTING_TASK_WAITFORSETTLE;
              Serial.println("PRINTING_TASK_SEPARATOR_DOWN end ");
              break;
            default:
              break;
          }
          break;
        case PRINTING_TASK_WAITFORSETTLE:
          switch (print_status.taskStt) {
            case TASK_BEGIN:
              Serial.println("PRINTING_TASK_WAITFORSETTLE begin ");
              print_status.taskStt = TASK_RUN;
              // turn on the light
              break;
            case TASK_RUN:
              printing_timer++;
              if (printing_timer >= liquidSettlingTime)
              {
                print_status.taskStt = TASK_END;
                printing_timer = 0;
              }
              break;
            case TASK_END:
              print_status.taskStt = TASK_BEGIN;
              print_status.taskID = PRINTING_TASK_CURING;
              Serial.println("PRINTING_TASK_WAITFORSETTLE end ");
              // turn off the light
              break;
            default:
              break;
          }
          break;
        default:

          break;
      }

      //        #define PRINTING_TASK_INIT 0
      //#define PRINTING_TASK_CURING 1
      //#define PRINTING_TASK_SEPARATOR_UP 2
      //#define PRINTING_TASK_SEPARATOR_DOWN 3
      //#define PRINTING_TASK_WAITFORSETTLE 4

      if (layerCount < numOfLayer) {
        /*======= init printing============
           - run to init Pos
        */
        /*====== first layers printer=====
           - curing
           - seperator move up
           - seperator move down + new layer height
           - wait for liquid settle down
        */
      }
    }
  }
}


/*============================================
         comunication with computer throught serial port
         run every loop
         do not use delay in the loop if don't want to lost data
    ==============================================*/


void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    command[command_index] = inChar;
    command_index++;

    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }

  if (stringComplete) {
    //    Serial.print(command);
    switch (command[0])
    {
      case RESET_SYSTEM:
        reset_system();
        reset_print_para();
        Serial.println("system reset: ");
        break;
        case CM_MOTOR_ON:
         Serial.println("motor on: ");
          digitalWrite(ST_ENABLE, STEPENABLE);//high: disenable | low: enable
        break;
        case CM_MOTOR_OFF:
         Serial.println("motor off: ");
          digitalWrite(ST_ENABLE, STEPDISABLE);//high: disenable | low: enable
        
        break;
      case CM_PRINTER_RUN_ON:
        flag.startPrint = 1;
        print_status.taskID = PRINTING_TASK_INIT;
        Serial.print("start printing: ");
        break;
      case CM_PRINTER_RUN_OFF:
        reset_print_para();
        flag.startPrint = 0;
        Serial.print("stop printing: ");
        SetElevatorPos(1000, 1);
        break;
      case CM_BACKLIGHT_ON:
        digitalWrite(BACKLIGHT, HIGH);
        Serial.println("backlight on: ");
        break;
      case CM_BACKLIGHT_OFF:
        digitalWrite(BACKLIGHT, LOW);
        Serial.println("backlight offl: ");
        break;
      case SET_SPEED_:
        stepSpeed = atoi(command + 1);
        Serial.print("stepSpeed: ");
        Serial.println(stepSpeed);
        stepSpeedCount = stepSpeed;
        break;
      case SET_STEP_UP:
        stepNum = atoi(command + 1);
        stepDir = STEPUP;
        digitalWrite(DIR, stepDir);
        Serial.print("stepNum up: ");
        Serial.println(stepNum);
        break;
      case SET_STEP_DOWN:
        stepNum = atoi(command + 1);
        stepDir = STEPDOWN;
        digitalWrite(DIR, stepDir);
        Serial.print("stepNum down: ");
        Serial.println(stepNum);
        break;
      case SET_POS:
        SetElevatorPos(atoi(command + 1), 0);
        //        setElevatorPos = atoi(command + 1);
        //        if (setElevatorPos >= 0) {
        //          deltaElevatorPos = setElevatorPos - elevatorPos;
        //          if (deltaElevatorPos >= 0) {
        //            stepDir = STEPUP;
        //            stepNum = deltaElevatorPos;
        //          } else {
        //            stepDir = STEPDOWN;
        //            stepNum = -deltaElevatorPos;
        //          }
        //          digitalWrite(DIR, stepDir);
        Serial.print("set step pos: ");
        Serial.println(setElevatorPos);
        //        }
        break;
      default:
        break;
    }
    memset(command, 0, sizeof(command));
    command_index = 0;
    // clear the string:
    stringComplete = false;
  }
}
