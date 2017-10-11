#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

#define CURRENT_PIN A0
#define VOLTAGE_PIN A1
MPU6050 accelgyro1(0x68);
MPU6050 accelgyro2(0x69);

char data[1000] = "";
const int MPU_addr1=0x68;  // I2C address of the MPU-6050
const int MPU_addr2=0x69;
int16_t AcX1,AcY1,AcZ1,Tmp,GyX1,GyY1,GyZ1;
int16_t AcX2,AcY2,AcZ2,GyX2,GyY2,GyZ2;
int toggle = 10;
const float  RS = 0.1;
const float RL = 10;
const int VOLTAGE_REF = 5;
const int NUM_SAMPLES = 10;
const float POTRATIO = 2;
float current = 0;
float voltage = 0;
float power = 0;
float cumPower = 0;
int sample_count = 0;


void setup(){
  pinMode(7,OUTPUT);
  digitalWrite(7,HIGH);
  
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(MPU_addr1);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
 
  accelgyro1.initialize();
  accelgyro1.setXAccelOffset(-5407);
  accelgyro1.setYAccelOffset(-111);
  accelgyro1.setZAccelOffset(1246);
  accelgyro1.setXGyroOffset(99);
  accelgyro1.setYGyroOffset(-9);
  accelgyro1.setZGyroOffset(-97);
  
  Wire.begin();
  Wire.beginTransmission(MPU_addr2);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  
  accelgyro2.initialize();
  accelgyro2.setXAccelOffset(-1377);
  accelgyro2.setYAccelOffset(3532);
  accelgyro2.setZAccelOffset(1092);
  accelgyro2.setXGyroOffset(54);
  accelgyro2.setYGyroOffset(9);
  accelgyro2.setZGyroOffset(16);

}
void loop(){
  Wire.beginTransmission(MPU_addr1);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr1,14,true);  // request a total of 14 registers
  accelgyro1.getMotion6(&AcX1,&AcY1,&AcZ1,&GyX1,&GyY1,&GyZ1);

  
  Wire.beginTransmission(MPU_addr2);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr2,14,true);  // request a total of 14 registers
  accelgyro2.getMotion6(&AcX2,&AcY2,&AcZ2,&GyX2,&GyY2,&GyZ2);

  printPowerConsumption();
  //acceleration in g, gyro in angular velocity
//  sprintf(data, "%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%d", 
//  AcX1/16384.0, AcY1/16384.0, AcZ1/16384.0,GyX1/131.0,GyY1/131.0,GyZ1/131.0,AcX2/16384.0,AcY2/16384.0,AcZ2/16384.0,GyX2/131.0,GyY2/131.0,GyZ2/131.0,toggle);
//  Serial.println(data);
  Serial.print("Sensor 1: ");
  printDouble(AcX1/16384.0,3);Serial.print("g, ");printDouble(AcY1/16384.0,3);Serial.print("g, ");printDouble(AcZ1/16384.0,3);Serial.print("g, ");printDouble(GyX1/131.0,3);Serial.print("Degree/s, ");
  printDouble(GyY1/131.0,3);Serial.print("Degree/s, ");printDouble(GyZ1/131.0,3);Serial.println("Degree/s, ");
  Serial.print("Sensor 2: ");
  printDouble(AcX2/16384.0,3);Serial.print("g, ");printDouble(AcY2/16384.0,3);Serial.print("g, ");
  printDouble(AcZ2/16384.0,3);Serial.print("g, ");printDouble(GyX2/131.0,3);Serial.print("Degree/s, ");printDouble(GyY2/131.0,3);Serial.print("Degree/s, ");printDouble(GyZ2/131.0,3);
  Serial.println("Degree/s, ");
//
// Serial.println();
}

void printPowerConsumption() {
  while(sample_count < NUM_SAMPLES) {
    voltage += analogRead(VOLTAGE_PIN); //sum
    current += analogRead(CURRENT_PIN); //sum
    sample_count++;
    delay(100);
  }
    voltage = (voltage / (float)NUM_SAMPLES * VOLTAGE_REF) / 1023.0;
  voltage = voltage * POTRATIO;
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

void printDouble( double val, byte precision){
 // prints val with number of decimal places determine by precision
 // precision is a number from 0 to 6 indicating the desired decimial places
 // example: printDouble( 3.1415, 2); // prints 3.14 (two decimal places)

 Serial.print (int(val));  //prints the int part
 if( precision > 0) {
   Serial.print("."); // print the decimal point
   unsigned long frac;
   unsigned long mult = 1;
   byte padding = precision -1;
   while(precision--)
      mult *=10;
      
   if(val >= 0)
     frac = (val - int(val)) * mult;
   else
     frac = (int(val)- val ) * mult;
   unsigned long frac1 = frac;
   while( frac1 /= 10 )
     padding--;
   while(  padding--)
     Serial.print("0");
   Serial.print(frac,DEC) ;
 }
}

