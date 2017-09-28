#include <Arduino_FreeRTOS.h>
#include <string.h>
#include <stdio.h>

char data[50] = {0};
int flag = 0;
int incomingByte = 0;
String dataString = "-23,123,-323,2654,-233,-232,125,5343,123,1232,4332";
String test = "";
char ack;

void setup() {
  // put your setup code here, to run once:
  Serial2.begin(115200);
  Serial.begin(115200);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(flag == 1){
   // a++;
    //Serial.println(dataString);
    //sprintf(dataString, "%02X", a); // convert a value to hexa
   // dataString.toCharArray(data, sizeof(dataString));
    Serial2.println(dataString);   // send the data
    int i=0;
    //Serial.print("hello");
    if(Serial2.available()){
      char received = Serial2.read();
      test += received;
      if(received == '\n'){
        Serial.print("Arduino received: ");
        Serial.println(test);
        test = "";
      }
    }
    delay(10);
  }
  else{
    if(Serial2.available()){
      char received = Serial2.read();
      if(received == '0'){
        Serial2.write('1');
        Serial.write("success");
      }
      if (received == '1'){
        flag = 1;
        Serial.write(flag);
      }
    }
  }
}
