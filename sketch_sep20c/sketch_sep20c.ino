#include <Arduino_FreeRTOS.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

MPU6050 accelgyro(0x68);
#define sen1 6
#define sen2 7
#define CURRENT_PIN A0
#define VOLTAGE_PIN A1

char data[50];
int a = 0;
int incomingByte = 0;
int flag =0;
String dataString = "";
char ack;

//Constants
const int MPU_addr=0x68;  // I2C address of the MPU-6050
const float  RS = 0.1;
const float RL = 10;
const int VOLTAGE_REF = 5;
const int NUM_SAMPLES = 10;
const float POTRATIO = 2;
const int COUNTERSTOP = 8;

// Global Variables
float current = 0;
float voltage = 0;
float currentSum = 0;
float voltageSum = 0;
float power = 0;
float cumPower = 0;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int16_t AcX2,AcY2,AcZ2,Tmp2,GyX2,GyY2,GyZ2;
int counter = 0;
int power_counter =0;

void readacc(int i){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  if(i==1){
    AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
    AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  }
  else if(i==2){
    AcX2=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
    AcY2=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ2=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp2=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX2=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY2=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ2=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  }
}

void readData(){
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 12.5;

  // Initialize the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();
  for( ;; ) {
    
    digitalWrite(sen1, LOW);
    digitalWrite(sen2, HIGH);
    readacc(1);
  
    digitalWrite(sen1, HIGH);
    digitalWrite(sen2, LOW);
    readacc(2);
    counter = counter+1;

    if(counter == COUNTERSTOP){
      counter = 0;
      voltageSum += analogRead(VOLTAGE_PIN);
      currentSum += analogRead(CURRENT_PIN); 
    }
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
  }
}

void processPowerWrapper(){
  power_counter += 1;
  if (power_counter==10)
    processPower();
}

void processPower(){
    voltage = (voltageSum / (float)NUM_SAMPLES * VOLTAGE_REF) / 1023.0;
    voltage = voltage * POTRATIO;
    current = (currentSum / (float)NUM_SAMPLES * VOLTAGE_REF) / 1023.0;
    current = current / (RS*RL);
    power = current * voltage;
    cumPower += current;
    currentSum = 0;
    voltageSum = 0;
}

void handshake(){
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

void serialize (){
  char checksum = 0;
  int i=0;
  dataString += AcX + ",";// + AcY + "," + AcZ + "," + Tmp + "," + GyX + "," + GyY + "," + GyZ;
  dataString.toCharArray(data, sizeof(dataString));
  for (i=1; i<= sizeof(dataString); i++){
    checksum ^= data[i];
  }
  data[50] = checksum;
}

void sendData(){
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 1000;

  // Initialize the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();
  for( ;; ) {
    handshake();
    if (flag ==1){
      serialize();
//      for(int i=0; i<47; i++)
        Serial2.write(data, sizeof(dataString));
      //send data to Rpi3
    }
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  pinMode(sen1, OUTPUT);
  pinMode(sen2, OUTPUT);

  digitalWrite(sen1, HIGH);
  digitalWrite(sen2, LOW);
  accelgyro.initialize();
  accelgyro.setXAccelOffset(-5407);
  accelgyro.setYAccelOffset(-111);
  accelgyro.setZAccelOffset(1246);
  accelgyro.setXGyroOffset(99);
  accelgyro.setYGyroOffset(-9);
  accelgyro.setZGyroOffset(-97);

  digitalWrite(sen1, LOW);
  digitalWrite(sen2, HIGH);
  accelgyro.initialize();
  accelgyro.setXAccelOffset(-1417);
  accelgyro.setYAccelOffset(3542);
  accelgyro.setZAccelOffset(1089);
  accelgyro.setXGyroOffset(61);
  accelgyro.setYGyroOffset(-8);
  accelgyro.setZGyroOffset(26);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  int count = 0;
  int i=0;
  for (i=0; i<10; i++){
    readData();
    processPowerWrapper();
    sendData();
  }
}
