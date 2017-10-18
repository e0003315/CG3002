#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

#define CURRENT_PIN A0
#define VOLTAGE_PIN A1
char data[1000] = "";
const float  RS = 0.1;
const float RL = 10;
const int VOLTAGE_REF = 5;
const int NUM_SAMPLES = 10;
const float POTRATIO = 2;
float current = 0;
float voltage = 0;
float avgVoltage = 5;
float power = 0;
float cumPower = 0;
int sample_count = 0;
char buffer [1000];

//IMU Data
MPU6050 accelgyro1(0x68);
MPU6050 accelgyro2(0x69);
const uint8_t MPU_addr1 = 0x68; // I2C address of the MPU-6050
const uint8_t MPU_addr2 = 0x69;

uint16_t packetSize1;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount1;     // count of all bytes currently in FIFO
uint8_t fifoBuffer1[64]; // FIFO storage buffer
uint16_t packetSize2;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount2;     // count of all bytes currently in FIFO
uint8_t fifoBuffer2[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q1;           // [w, x, y, z]         quaternion container
VectorInt16 aa1;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal1;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld1;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity1;    // [x, y, z]            gravity vector
float euler1[3];         // [psi, theta, phi]    Euler angle container
float ypr1[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
Quaternion q2;           // [w, x, y, z]         quaternion container
VectorInt16 aa2;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal2;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld2;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity2;    // [x, y, z]            gravity vector
float euler2[3];         // [psi, theta, phi]    Euler angle container
float ypr2[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif
  Serial.begin(115200);

  //Configurating First IMU
  
  accelgyro1.initialize();
//  accelgyro1.setXAccelOffset(-5407);
//  accelgyro1.setYAccelOffset(-111);
//  accelgyro1.setZAccelOffset(1246);
//  accelgyro1.setXGyroOffset(99);
//  accelgyro1.setYGyroOffset(-9);
//  accelgyro1.setZGyroOffset(-97);

  accelgyro1.setXAccelOffset(-1887);
  accelgyro1.setYAccelOffset(119);
  accelgyro1.setZAccelOffset(1149);
  accelgyro1.setXGyroOffset(79);
  accelgyro1.setYGyroOffset(1);
  accelgyro1.setZGyroOffset(-21);
  accelgyro1.dmpInitialize();
  accelgyro1.setDMPEnabled(true);
  packetSize1 = accelgyro1.dmpGetFIFOPacketSize();
  
//  Wire.beginTransmission(MPU_addr2);
//  Wire.write(0x6B);  // PWR_MGMT_1 register
//  Wire.write(0);     // set to zero (wakes up the MPU-6050)
//  Wire.endTransmission(true);

  //Configuring Second IMU
//  accelgyro2.initialize();
//  accelgyro2.setXAccelOffset(-1459);
//  accelgyro2.setYAccelOffset(3527);
//  accelgyro2.setZAccelOffset(1094);
//  accelgyro2.setXGyroOffset(42);
//  accelgyro2.setYGyroOffset(16);
//  accelgyro2.setZGyroOffset(30);
//  accelgyro2.dmpInitialize(); 
//  accelgyro2.setDMPEnabled(true);
//  packetSize2 = accelgyro2.dmpGetFIFOPacketSize();


}
void loop() {

  //  accelgyro1.getMotion6(&AcX1, &AcY1, &AcZ1, &GyX1, &GyY1, &GyZ1);
  //  accelgyro2.getMotion6(&AcX2, &AcY2, &AcZ2, &GyX2, &GyY2, &GyZ2);
  fifoCount1 = accelgyro1.getFIFOCount();
  fifoCount2 = accelgyro2.getFIFOCount();
  while (fifoCount1 < packetSize1) fifoCount1 = accelgyro1.getFIFOCount();
  accelgyro1.getFIFOBytes(fifoBuffer1, packetSize1);
  fifoCount1 -= packetSize1;
  
//  while (fifoCount2 < packetSize2) fifoCount2 = accelgyro2.getFIFOCount();
//  accelgyro2.getFIFOBytes(fifoBuffer2, packetSize2);
//  fifoCount2 -= packetSize2;

  //MPU6050_DMP6 Data Here!
  accelgyro1.dmpGetQuaternion(&q1, fifoBuffer1);
  accelgyro1.dmpGetAccel(&aa1, fifoBuffer1);
  accelgyro1.dmpGetGravity(&gravity1, &q1);
  accelgyro1.dmpGetLinearAccel(&aaReal1, &aa1, &gravity1);
  Serial.print("areal\t");
  Serial.print(aaReal1.x);
  Serial.print("\t");
  Serial.print(aaReal1.y);
  Serial.print("\t");
  Serial.println(aaReal1.z);
  accelgyro1.dmpGetQuaternion(&q1, fifoBuffer1);
  accelgyro1.dmpGetGravity(&gravity1, &q1);
  accelgyro1.dmpGetYawPitchRoll(ypr1, &q1, &gravity1);
  Serial.print("ypr1\t");
  Serial.print(ypr1[0] * 180/M_PI);
  Serial.print("\t");
  Serial.print(ypr1[1] * 180/M_PI);
  Serial.print("\t");
  Serial.println(ypr1[2] * 180/M_PI);  

  
     
//  acceleration in g, gyro in angular velocity
//    sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
//    AcX1, AcY1, AcZ1,kalAngleX1,kalAngleY1,kalAngleZ1,AcX2,AcY2,AcZ2,kalAngleX2,kalAngleY2,kalAngleZ2);
//    Serial.println(data);

//    count = count + 1;

//    if(count == 100) {
//    printPowerConsumption();
//    Serial.print(AcX1/16384.0); Serial.print("\t"); Serial.print(AcY1/16384.0); Serial.print("\t"); Serial.print(AcZ1/16384.0); Serial.println("\t");
//    convertToString(AcX1 / 16384.0, AcY1/ 16384.0, AcZ1/ 16384.0, kalAngleX1, kalAngleY1, kalAngleZ1, AcX2/ 16384.0, AcY2/ 16384.0, AcZ2/ 16384.0, kalAngleX2, kalAngleY2, kalAngleZ2, current, voltage, buffer);
//    Serial.println (buffer);
//    memset(buffer, 0, 50);
//    Serial.print("Sensor 1: ");
//    printDouble(AcX1 / 16384.0, 3); Serial.print("g, "); printDouble(AcY1 / 16384.0, 3); Serial.print("g, "); printDouble(AcZ1 / 16384.0, 3); Serial.print("g, "); printDouble(kalAngleX1,2); Serial.print("Degree/s, ");
//    printDouble(kalAngleY1,2); Serial.print("Degree/s, "); printDouble(kalAngleZ1,2); Serial.println("Degree/s, ");
//    Serial.print("Sensor 2: ");
//    printDouble(AcX2 / 16384.0, 3); Serial.print("g, "); printDouble(AcY2 / 16384.0, 3); Serial.print("g, ");
//    printDouble(AcZ2 / 16384.0, 3); Serial.print("g, "); printDouble(kalAngleX2,2); Serial.print("Degree/s, "); printDouble(kalAngleY2,2); Serial.print("Degree/s, "); printDouble(kalAngleZ2,2);
//    Serial.println("Degree/s, ");
//    Serial.println("");
//    count = 0;
//    }

}

void printPowerConsumption() {
  while (sample_count < NUM_SAMPLES) {
    voltage += analogRead(VOLTAGE_PIN); //sum
    current += analogRead(CURRENT_PIN); //sum
    sample_count++;
    delay(100);
  }
  voltage = (voltage / (float)NUM_SAMPLES * VOLTAGE_REF) / 1023.0;
  voltage = voltage * POTRATIO;
  avgVoltage = (avgVoltage + voltage) / 2;
  current = (current / (float)NUM_SAMPLES * VOLTAGE_REF) / 1023.0;
  current = current / (RS * RL);
  // Follow the equation given by the INA169 datasheet to
  // determine the current flowing through RS. Assume RL = 10k
  // Is = (Vout x 1k) / (RS x RL)
  power = current * voltage;
  cumPower += power * 1000 / avgVoltage;
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
  Serial.print(cumPower / 3600, 0); //int /sec
  Serial.println(" mAh");
  sample_count = 0;
  voltage = 0;
  current = 0;
}

void printDouble( double val, byte precision) {
  // prints val with number of decimal places determine by precision
  // precision is a number from 0 to 6 indicating the desired decimial places
  // example: printDouble( 3.1415, 2); // prints 3.14 (two decimal places)

  Serial.print (int(val));  //prints the int part
  if ( precision > 0) {
    Serial.print("."); // print the decimal point
    unsigned long frac;
    unsigned long mult = 1;
    byte padding = precision - 1;
    while (precision--)
      mult *= 10;

    if (val >= 0)
      frac = (val - int(val)) * mult;
    else
      frac = (int(val) - val ) * mult;
    unsigned long frac1 = frac;
    while ( frac1 /= 10 )
      padding--;
    while (  padding--)
      Serial.print("0");
    Serial.print(frac, DEC) ;
  }
}


void convertToString(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j, double k, double l, double m, double n, char* buffer){
  char temp [10];
  dtostrf(a,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(b,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(c,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(d,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(e,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(f,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(g,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(h,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(i,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(j,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(k,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(l,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(m,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
  dtostrf(n,1,3,&temp[0]);
  strcat(buffer, temp);
  strcat(buffer, ",");
}
