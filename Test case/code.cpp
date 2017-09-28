 // MPU-6050 Short Example Sketch
// By Arduino User JohnChi
// August 17, 2014
// Public Domain
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

MPU6050 accelgyro(0x68);
#define sen1 6
#define sen2 7
#define CURRENT_PIN A0
#define VOLTAGE_PIN A1

//Constants
const int MPU_addr=0x68;  // I2C address of the MPU-6050
const float  RS = 0.1;
const float RL = 10;
const int VOLTAGE_REF = 5;
const int NUM_SAMPLES = 10;
const float potRatio = 2;

// Global Variables
float current = 0;
float voltage = 0;
float power = 0;
float cumPower = 0;
int sample_count = 0;
int toggle = 1;

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
void setup(){
  Serial.begin(115200);

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
  accelgyro.setXAccelOffset(-5445);
  accelgyro.setYAccelOffset(-100);
  accelgyro.setZAccelOffset(1235);
  accelgyro.setXGyroOffset(127);
  accelgyro.setYGyroOffset(-7);
  accelgyro.setZGyroOffset(-81);

  digitalWrite(sen1, LOW);
  digitalWrite(sen2, HIGH);
  accelgyro.initialize();
  accelgyro.setXAccelOffset(-1476);
  accelgyro.setYAccelOffset(3537);
  accelgyro.setZAccelOffset(1108);
  accelgyro.setXGyroOffset(44);
  accelgyro.setYGyroOffset(18);
  accelgyro.setZGyroOffset(28);
}

void readacc(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

}

void printacc(int i){
//  Serial.print("AcX"); Serial.print(i); Serial.print(" = "); Serial.print(AcX);
//  Serial.print(" | AcY"); Serial.print(i); Serial.print(" = "); Serial.print(AcY);
//  Serial.print(" | AcZ"); Serial.print(i); Serial.print(" = "); Serial.print(AcZ);
//  Serial.print(" | GyX"); Serial.print(i); Serial.print(" = "); Serial.print(GyX);
//  Serial.print(" | GyY"); Serial.print(i); Serial.print(" = "); Serial.print(GyY);
//  Serial.print(" | GyZ"); Serial.print(i); Serial.print(" = "); Serial.println(GyZ);
//  if(i==2)
//    Serial.println();

    Serial.print(AcX); Serial.print(',');
    Serial.print(AcY); Serial.print(',');
    Serial.print(AcZ); Serial.print(',');
    Serial.print(GyX); Serial.print(',');
    Serial.print(GyY); Serial.print(',');
    Serial.print(GyZ); Serial.print(',');
 
}

void printPowerConsumption() {
  while(sample_count < NUM_SAMPLES) {
    voltage += analogRead(VOLTAGE_PIN); //sum
    current += analogRead(CURRENT_PIN); //sum
    sample_count++;
    delay(100);
  }
    voltage = (voltage / (float)NUM_SAMPLES * VOLTAGE_REF) / 1023.0;
  voltage = voltage * potRatio;
  current = (current / (float)NUM_SAMPLES * VOLTAGE_REF) / 1023.0;
  current = current / (RS*RL);
  // Follow the equation given by the INA169 datasheet to
  // determine the current flowing through RS. Assume RL = 10k
  // Is = (Vout x 1k) / (RS x RL)
  power = current * voltage;
  cumPower += current;
  Serial.print("Voltage: ");
  Serial.print(voltage, 3); //3dp
  Serial.print (" V  ");
  Serial.print("Current: ");
  Serial.print(current, 3); //output: 3dp
  Serial.print(" A  ");
  Serial.print("Power: ");
  Serial.print(power, 3); //3dp
  Serial.print(" W  ");
  Serial.print("CumPower: ");
  Serial.print(cumPower*1000/3600, 0); //int /sec
  Serial.println(" mAh");
  sample_count = 0;
  voltage = 0;
  current = 0;
}


void loop(){
  digitalWrite(sen1, LOW);
  digitalWrite(sen2, HIGH);
  readacc();
  printacc(1);
  
  digitalWrite(sen1, HIGH);
  digitalWrite(sen2, LOW);
  readacc();
  printacc(2);
  Serial.println(toggle);
//  printPowerConsumption();
  delay(9); 
}