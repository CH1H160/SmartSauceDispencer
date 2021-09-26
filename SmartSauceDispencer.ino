#include "M5Atom.h"
#include "Arduino_JSON.h"
#include <ESP32Servo.h>

const char input[] = "{\"function\":\"Object Recognition\",\"args\":[\"SmartDispenser\"]}";

Servo servo1;
const int32_t S_PIN = 25;
const int32_t MIN_US = 500;
const int32_t MAX_US = 2400;

const String SOYSAUCE = "\"EggFried\"";  // 醤油判定文字列
const String SAUCE = "\"HamCutlet\"";    // ソース判定文字列
const float THRESHOLD = 0.6;

// サーボを動かす
void moveServo(String _mode){
  
  if(_mode == SOYSAUCE){
    servo1.write(95);
    M5.dis.drawpix(0, 0x0000ff);
  }
  else if(_mode == SAUCE){
    servo1.write(45);
    M5.dis.drawpix(0, 0xf00000);
  }
  
  delay(2000);
  
  servo1.write(70);
  M5.dis.drawpix(0, 0x00f000);
}

void setup() {

  M5.begin(true, false, true);
  M5.dis.drawpix(0, 0x00f000);
  
  Serial2.begin(115200, SERIAL_8N1, 32, 26);
  
  servo1.setPeriodHertz(50);
  servo1.attach(S_PIN, MIN_US, MAX_US);
  servo1.write(70);
  
  delay(2000);                                        // UnitV2の起動を待つ
  Serial2.println(JSON.stringify(JSON.parse(input))); // function切替
}

void loop(){
  M5.update();

  if(Serial2.available()) {
    String receiveString = Serial2.readStringUntil('\n');
    if(receiveString[0] == '{'){

      // []を除去
      receiveString.replace("[", "" );
      receiveString.replace("]", "" );
      
      JSONVar myval = JSON.parse(receiveString);

      // 醤油/ソース判定
      if(JSON.stringify(myval["running"]) == "\"Object Recognition\""){
        if(JSON.stringify(myval["obj"]["prob"]).toFloat() > THRESHOLD){
          moveServo(JSON.stringify(myval["obj"]["type"]));
        }
      }
    }
  }
}
