#include <XBee.h>

#define CMD_VEL 1

#define CMD_FWD 1
#define CMD_BCK 2
#define CMD_LFT 3
#define CMD_RHT 4


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

void apply_motor(int linear, int angular){
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

void forward() //full speed forward
{     
  last_cmd = CMD_FWD;
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, HIGH);
  analogWrite(pwm_a, 255);
  analogWrite(pwm_b, 255);
  stopped = false;
}

void backward(){
  last_cmd = CMD_BCK;
  digitalWrite(dir_a, LOW);
  digitalWrite(dir_b, LOW);
  analogWrite(pwm_a, 255);
  analogWrite(pwm_b, 255);
  stopped = false;
}

void left(){
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, LOW);
  analogWrite(pwm_a, 255);
  analogWrite(pwm_b, 255);
  stopped = false;
}

void right(){
  digitalWrite(dir_b, HIGH);
  digitalWrite(dir_a, LOW);
  analogWrite(pwm_b, 255);
  analogWrite(pwm_a, 255);
  stopped = false;
}

void halt(){
  digitalWrite(dir_a, LOW);
  digitalWrite(dir_b, LOW);
  analogWrite(pwm_a, 0);
  analogWrite(pwm_b, 0);
  stopped = true;
}
