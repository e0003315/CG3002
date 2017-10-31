#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
#include <String.h>

MPU6050 accelgyro1(0x68);
MPU6050 accelgyro2(0x69);
#define CURRENT_PIN A0
#define VOLTAGE_PIN A14
#define STACK_SIZE 200

//semaphores
SemaphoreHandle_t processSemaphore = xSemaphoreCreateBinary();
SemaphoreHandle_t sendSemaphore = xSemaphoreCreateBinary();

//Constants
const int MPU1_addr=0x68;  // I2C address of the MPU-6050
const int MPU2_addr=0x69;
const float  RS = 0.1;
const float RL = 10;
const int VOLTAGE_REF = 5;
const int NUM_SAMPLES = 10;
const float POTRATIO = 2;
const int COUNTERSTOP = 6;

// Global Variables
float current = 0;
float voltage = 0;
float avgVoltage = 5;
float currentSum = 0;
float voltageSum = 0;
float power = 0;
float cumPower = 0;
int16_t AcX1,AcY1,AcZ1,GyX1,GyY1,GyZ1;
int16_t AcX2,AcY2,AcZ2,GyX2,GyY2,GyZ2;
int counter = 0;
int power_counter =0;
int flag =0;
char data[1000] = "";
char s[10] = "";
char c[10] = "";
char checksum;

void readacc(){
  accelgyro1.getMotion6(&AcX1, &AcY1, &AcZ1, &GyX1, &GyY1, &GyZ1);
  accelgyro2.getMotion6(&AcX2, &AcY2, &AcZ2, &GyX2, &GyY2, &GyZ2);
}

void readData(void *p){
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 1;

  

  // Initialize the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();
  for( ;; ) {
    readacc();


    if(counter == COUNTERSTOP){
      counter = 0;
      voltageSum += analogRead(VOLTAGE_PIN);
      currentSum += analogRead(CURRENT_PIN); 
    }
    counter = counter+1;
    xSemaphoreGive(processSemaphore);
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
  }
}

void processPowerWrapper(void *p){
  for (;;) {
     if (xSemaphoreTake (processSemaphore, 1) == pdTRUE) {
        if (power_counter>=62){
            processPower();
            power_counter =  0;
            counter = 0;
        }
        power_counter += 1;
        xSemaphoreGive(sendSemaphore);
     }
  }
}

void processPower(){
    voltage = (voltageSum / (float)NUM_SAMPLES * VOLTAGE_REF) / 1023.0;
    voltage = voltage * POTRATIO;
    avgVoltage = (avgVoltage + voltage)/2; 
    current = (currentSum / (float)NUM_SAMPLES * VOLTAGE_REF) / 1023.0;
    current = current / (RS*RL);
    power = current * voltage;
    cumPower += power * 1000 / avgVoltage;
    currentSum = 0;
    voltageSum = 0;
}

void handshake(){
  Serial.println("In handshake!");
  while (flag !=1) {
  if(Serial2.available()){
      char received = Serial2.read();
      if(received == '0'){
        Serial2.write('1');
      }
      if (received == '1'){
        flag = 1;
      }
   }
  }
  Serial.println("Out handshake");
}

void serialize (){
  char temp[10];
  int i=0;
  checksum = '0';
  sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d|",AcX1,AcY1,AcZ1,GyX1,GyY1,GyZ1,AcX2,AcY2,AcZ2,GyX2,GyY2,GyZ2);
  dtostrf(current,1,3,&temp[0]);
  strcat(data,temp);
  strcat(data,"|");
  dtostrf(voltage,1,3,&temp[0]);
  strcat(data,temp);
  strcat(data,"|");
  dtostrf(power,1,3,&temp[0]);
  strcat(data,temp);
  strcat(data,"|");
  dtostrf(cumPower/3600,1,3,&temp[0]);
  strcat(data,temp);
  strcat(data,"|");
   
  sprintf(s, "%d%c", strlen(data), 's');
  char *p = data;
  while(*p != '\0'){
    checksum ^= *p;
    p++;
  }
  sprintf(c, "%d%c", checksum, 'c');
  strcat (data, "d");
  strcat(data, s);
  strcat(data, c);
}

void sendData(void *p){
  int size;
  
  
  for( ;; ) {
    if (xSemaphoreTake(sendSemaphore, 1) == pdTRUE ) {
        serialize();
        Serial.println(data);
        Serial2.write(data, strlen(data)); 
        if(Serial2.available()){
          char received = Serial2.read();
          if(received == '2'){
            serialize();
            Serial2.write(data, strlen(data)); 
          }
        }
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

    // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif
  
  accelgyro1.initialize();
  accelgyro1.dmpInitialize();
  accelgyro1.setDMPEnabled(true);
  accelgyro1.setXAccelOffset(-5407);
  accelgyro1.setYAccelOffset(-111);
  accelgyro1.setZAccelOffset(1246);
  accelgyro1.setXGyroOffset(99);
  accelgyro1.setYGyroOffset(-9);
  accelgyro1.setZGyroOffset(-97);
  accelgyro1.setFullScaleAccelRange(MPU6050_ACCEL_FS_4);

  accelgyro2.initialize();
  accelgyro2.dmpInitialize(); 
  accelgyro2.setDMPEnabled(true);
  accelgyro2.setFullScaleAccelRange(MPU6050_ACCEL_FS_4);
  accelgyro2.setXAccelOffset(-1459);
  accelgyro2.setYAccelOffset(3527);
  accelgyro2.setZAccelOffset(1094);
  accelgyro2.setXGyroOffset(42);
  accelgyro2.setYGyroOffset(16);
  accelgyro2.setZGyroOffset(30);

  handshake();
  if (flag ==1){
  xTaskCreate(readData,"readData", STACK_SIZE, NULL, 3,NULL);
  xTaskCreate(processPowerWrapper, "processPowerWrapper", STACK_SIZE, NULL, 2,NULL);
  xTaskCreate(sendData, "sendData", STACK_SIZE, NULL, 1,NULL);
}
}

void loop() {
}


