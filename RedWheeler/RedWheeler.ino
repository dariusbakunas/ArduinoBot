#include <XBee.h>

#define CMD_VEL 1

//ardumoto shield
int pwm_a = 3;
int pwm_b = 11;
int dir_a = 12;
int dir_b = 13;

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
  
  pinMode(pwm_a, OUTPUT);
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);
  
  analogWrite(pwm_a, 0);
  analogWrite(pwm_b, 0);
}

void set_motor_speed(speed_a, speed_b){
  analogWrite(pwm_a, speed_a);
  analogWrite(pwm_b, speed_b);
}

void apply_motor(int linear, int angular
  stopped = false;

  if (linear == 1){
    forward();
  } else if (linear == 2){
    backward();
  } else if (linear == 0){
    if (angular == 1){
      right();
    } else if (angular == 2){
      left();
    } else {
      halt();
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
  
  //stop if there was no command for more than 250ms
  if (!stopped && cmd_duration >= 500)
    halt();
}

void reset_time(){
  cmd_start_time = millis();
}

void forward()
{     
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, HIGH);
  set_motor_speed(255, 255);
}

void backward(){
  digitalWrite(dir_a, LOW);
  digitalWrite(dir_b, LOW);
  set_motor_speed(255, 255);
}

void left(){
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, LOW);
  set_motor_speed(255, 255);
}

void right(){
  digitalWrite(dir_b, HIGH);
  digitalWrite(dir_a, LOW);
  set_motor_speed(255, 255);
}

void halt(){
  digitalWrite(dir_a, LOW);
  digitalWrite(dir_b, LOW);
  set_motor_speed(0, 0);
}
