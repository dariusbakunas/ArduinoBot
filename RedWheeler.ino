#include <XBee.h>

//ardumoto shield
int pwm_a = 3;
int pwm_b = 11;
int dir_a = 12;
int dir_b = 13;

//xbee shield
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

void setup() {
  Serial.begin(9600);
  xbee.begin(Serial);
  
  // put your setup code here, to run once:
  pinMode(pwm_a, OUTPUT);
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);
  
  //set both motors to run at (100/255 = 39)% duty cycle (slow)
  //analogWrite(pwm_a, 100);
  //analogWrite(pwm_b, 100);
  
  analogWrite(pwm_a, 0);
  analogWrite(pwm_b, 0);
}

int command;

void loop() {
  xbee.readPacket();
  
  if (xbee.getResponse().isAvailable()){
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
      //got a zb rx packer
      xbee.getResponse().getZBRxResponse(rx);
      
      command = rx.getData(0);
      
      switch(command){
        case 1:
          forward();
          break;
        case 2:
          backward();
          break;
        case 3:
          halt();
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
}

void forward() //full speed forward
{
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, HIGH);
  analogWrite(pwm_a, 255);
  analogWrite(pwm_b, 255);
}

void forward_custom(int value){
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, HIGH);
  analogWrite(pwm_a, value);
  analogWrite(pwm_b, value);
}

void backward(){
  digitalWrite(dir_a, LOW);
  digitalWrite(dir_b, LOW);
  analogWrite(pwm_a, 255);
  analogWrite(pwm_b, 255);
}

void halt(){
  digitalWrite(dir_a, LOW);
  digitalWrite(dir_b, LOW);
  analogWrite(pwm_a, 0);
  analogWrite(pwm_b, 0);
}
