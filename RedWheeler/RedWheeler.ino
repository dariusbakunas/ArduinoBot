#include <XBee.h>

//commands
#define CMD_VEL 1

//ardumoto shield
const byte PWM_A = 3;
const byte PWM_B = 11;
const byte DIR_A = 12;
const byte DIR_B = 13;

// Clockwise and counter-clockwise definitions.
// Depending on how you wired your motors, you may need to swap.
#define CW  1
#define CCW 0

// Motor definitions to make life easier:
#define MOTOR_A 0
#define MOTOR_B 1

boolean stopped = false;

//xbee shield
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();
int command;
unsigned long cmd_start_time;
unsigned long last_cmd;

void setup() {
  Serial.begin(9600);
  xbee.begin(Serial);
  
  setupMotorShield();
}

void apply_motor(int linear, int angular){
  stopped = false;

  if (linear == 1){
    goForward();
  } else if (linear == 2){
    goBackward();
  } else if (linear == 0){
    if (angular == 1){
      goRight();
    } else if (angular == 2){
      goLeft();
    } else {
      stopMotor(MOTOR_A);
      stopMotor(MOTOR_B);
      stopped = true;
    }
  }
}

void loop() {
  int linear, angular; 
  int cmd_duration = 0; 
  xbee.readPacket();
  
  if (xbee.getResponse().isAvailable()){
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
      //got a zb rx packer
      xbee.getResponse().getZBRxResponse(rx);
      
      command = rx.getData(0);
      
      switch(command){
        case CMD_VEL:
          //cmd_vel
          reset_time();  //new command
          linear = rx.getData(1);
          angular = rx.getData(2);
          apply_motor(linear, angular);
          break;
        default:
          break;
      }
      
    } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE){
      // the local XBee sends this response on certain events,
      // like association/dissociation
      xbee.getResponse().getModemStatusResponse(msr);
      
      if (msr.getStatus() == ASSOCIATED){
      } else if (msr.getStatus() == DISASSOCIATED){
      }
    }
  }
  
  //check time since last command
  cmd_duration = millis() - cmd_start_time;
  
  //stop if there was no command for more than 800ms
  if (!stopped && cmd_duration >= 800){
    stopMotor(MOTOR_A);
    stopMotor(MOTOR_B);
  }
}

void reset_time(){
  cmd_start_time = millis();
}

// driveArdumoto drives 'motor' in 'dir' direction at 'spd' speed
void driveMotor(byte motor, byte dir, byte spd){
  if (motor == MOTOR_A){
    digitalWrite(DIR_A, dir);
    analogWrite(PWM_A, spd);
  }else if (motor == MOTOR_B){
    digitalWrite(DIR_B, dir);
    analogWrite(PWM_B, spd);
  }
}

void setupMotorShield(){
  //all pins should be setup as outputs
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  
  //initialize all pins as low:
  digitalWrite(PWM_A, LOW);
  digitalWrite(PWM_B, LOW);
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
}

void goForward()
{ 
  driveMotor(MOTOR_A, CW, 255);
  driveMotor(MOTOR_B, CW, 255);  
}

void goBackward(){
  driveMotor(MOTOR_A, CCW, 255);
  driveMotor(MOTOR_B, CCW, 255);
}

void goLeft(){
  driveMotor(MOTOR_A, CW, 255);
  driveMotor(MOTOR_B, CCW, 255);
}

void goRight(){
  driveMotor(MOTOR_A, CCW, 255);
  driveMotor(MOTOR_B, CW, 255);
}

void stopMotor(byte motor)
{
  driveMotor(motor, 0, 0);
}
