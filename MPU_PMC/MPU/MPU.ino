#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Kalman.h"

#define RESTRICT_PITCH // Comment out to restrict roll to ±90deg instead - please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf
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

//IMU Data
MPU6050 accelgyro1(0x68);
MPU6050 accelgyro2(0x69);
const uint8_t MPU_addr1 = 0x68; // I2C address of the MPU-6050
const uint8_t MPU_addr2 = 0x69;
double AcX1, AcY1, AcZ1, Tmp, GyX1, GyY1, GyZ1;
double AcX2, AcY2, AcZ2, GyX2, GyY2, GyZ2;

Kalman kalmanX1, kalmanY1, kalmanZ1; // Create the Kalman instances
Kalman kalmanX2, kalmanY2, kalmanZ2;

double roll1, pitch1, yaw1; // Roll and pitch are calculated using the accelerometer while yaw is calculated using the magnetometer
double roll2, pitch2, yaw2; // Roll and pitch are calculated using the accelerometer while yaw is calculated using the magnetometer

double magX1, magY1, magZ1;
double magX2, magY2, magZ2;
double gyroXangle1, gyroYangle1, gyroZangle1; // Angle calculate using the gyro only
double kalAngleX1, kalAngleY1, kalAngleZ1; // Calculated angle using a Kalman filter
double gyroXangle2, gyroYangle2, gyroZangle2; // Angle calculate using the gyro only
double kalAngleX2, kalAngleY2, kalAngleZ2; // Calculated angle using a Kalman filter
double compAngleX1, compAngleY1, compAngleZ1; // Calculated angle using a complementary filter
double compAngleX2, compAngleY2, compAngleZ2; // Calculated angle using a complementary filter

uint8_t count = 0;
uint32_t timer;
int8_t i2cData[14]; // Buffer for I2C data
const uint16_t I2C_TIMEOUT = 1000;

#define MAG0MAX 603
#define MAG0MIN -578

#define MAG1MAX 542
#define MAG1MIN -701

#define MAG2MAX 547
#define MAG2MIN -556

float magOffset[3] = { (MAG0MAX + MAG0MIN) / 2, (MAG1MAX + MAG1MIN) / 2, (MAG2MAX + MAG2MIN) / 2 };
double magGain[3];

void setup() {
  delay(100);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  Serial.begin(115200);
  Wire.begin();
  TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency to 400kHz
  Wire.beginTransmission(MPU_addr1);

  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  //Configurating First IMU
  accelgyro1.initialize();
  accelgyro1.setXAccelOffset(-5407);
  accelgyro1.setYAccelOffset(-111);
  accelgyro1.setZAccelOffset(1246);
  accelgyro1.setXGyroOffset(99);
  accelgyro1.setYGyroOffset(-9);
  accelgyro1.setZGyroOffset(-97);

  Wire.beginTransmission(MPU_addr2);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  //Configuring Second IMU
  accelgyro2.initialize();
  accelgyro2.setXAccelOffset(-1459);
  accelgyro2.setYAccelOffset(3527);
  accelgyro2.setZAccelOffset(1094);
  accelgyro2.setXGyroOffset(42);
  accelgyro2.setYGyroOffset(16);
  accelgyro2.setZGyroOffset(30);

  delay(100);
  
  //This gives values in Integer
  //  accelgyro1.getMotion6(&AcX1, &AcY1, &AcZ1, &GyX1, &GyY1, &GyZ1);  
  //  accelgyro2.getMotion6(&AcX2, &AcY2, &AcZ2, &GyX2, &GyY2, &GyZ2);
  //This gives values in Double
  updateMPU6050(MPU_addr1); 
  updateMPU6050(MPU_addr2);
  updatePitchRoll();
  updateYaw();

  kalmanX1.setAngle(roll1); // First set roll starting angle
  //  gyroXangle1 = roll1;
  //  compAngleX1 = roll1;

  kalmanY1.setAngle(pitch1); // Then pitch
  //  gyroYangle1 = pitch1;
  //  compAngleY1 = pitch1;

  kalmanZ1.setAngle(yaw1); // And finally yaw
  //  gyroZangle1 = yaw1;
  //  compAngleZ1 = yaw1;

  kalmanX2.setAngle(roll2); // First set roll starting angle
  //  gyroXangle2 = roll2;
  //  compAngleX2 = roll2;

  kalmanY2.setAngle(pitch2); // Then pitch
  //  gyroYangle2 = pitch2;
  //  compAngleY2 = pitch2;

  kalmanZ2.setAngle(yaw2); // And finally yaw
  //  gyroZangle2 = yaw2;
  //  compAngleZ2 = yaw2;

  timer = micros(); // Initialize the timer

}
void loop() {

  //  accelgyro1.getMotion6(&AcX1, &AcY1, &AcZ1, &GyX1, &GyY1, &GyZ1);
  //  accelgyro2.getMotion6(&AcX2, &AcY2, &AcZ2, &GyX2, &GyY2, &GyZ2);
  updateMPU6050(MPU_addr1);
  updateMPU6050(MPU_addr2);

  /* Filters the Gyro Reading Only*/
  kalmanFilter();
  //OLD CODE

  // printPowerConsumption();
//  acceleration in g, gyro in angular velocity
//    sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
//    AcX1, AcY1, AcZ1,kalAngleX1,kalAngleY1,kalAngleZ1,AcX2,AcY2,AcZ2,kalAngleX2,kalAngleY2,kalAngleZ2);
//    Serial.println(data);

    count = count + 1;

    if(count == 100) {
    Serial.print("Sensor 1: ");
    printDouble(AcX1 / 16384.0, 3); Serial.print("g, "); printDouble(AcY1 / 16384.0, 3); Serial.print("g, "); printDouble(AcZ1 / 16384.0, 3); Serial.print("g, "); printDouble(kalAngleX1,2); Serial.print("Degree/s, ");
    printDouble(kalAngleY1,2); Serial.print("Degree/s, "); printDouble(kalAngleZ1,2); Serial.println("Degree/s, ");
    Serial.print("Sensor 2: ");
    printDouble(AcX2 / 16384.0, 3); Serial.print("g, "); printDouble(AcY2 / 16384.0, 3); Serial.print("g, ");
    printDouble(AcZ2 / 16384.0, 3); Serial.print("g, "); printDouble(kalAngleX2,2); Serial.print("Degree/s, "); printDouble(kalAngleY2,2); Serial.print("Degree/s, "); printDouble(kalAngleZ2,2);
    Serial.println("Degree/s, ");
    count = 0;
    }

//  Serial.print("K: ");
//  Serial.print(kalAngleX1); Serial.print("\t"); Serial.print(kalAngleY1); Serial.print("\t"); Serial.print(kalAngleZ1); Serial.print("\t");
//
//  Serial.print(kalAngleX2); Serial.print("\t"); Serial.print(kalAngleY2); Serial.print("\t"); Serial.print(kalAngleZ2); Serial.print("\t"); Serial.println(); Serial.println();

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

void updateMPU6050(uint8_t IMUAddress) {
  if (IMUAddress == MPU_addr1) {
    while (i2cRead(IMUAddress, 0x3B, i2cData, 14)); // Get accelerometer and gyroscope values
    AcX1 = ((i2cData[0] << 8) | i2cData[1]);
    AcY1 = -((i2cData[2] << 8) | i2cData[3]);
    AcZ1 = ((i2cData[4] << 8) | i2cData[5]);
    //  tempRaw = (i2cData[6] << 8) | i2cData[7];
    GyX1 = -(i2cData[8] << 8) | i2cData[9];
    GyY1 = (i2cData[10] << 8) | i2cData[11];
    GyZ1 = -(i2cData[12] << 8) | i2cData[13];
  }
  else if (IMUAddress == MPU_addr2) {
    while (i2cRead(IMUAddress, 0x3B, i2cData, 14)); // Get accelerometer and gyroscope values
    AcX2 = ((i2cData[0] << 8) | i2cData[1]);
    AcY2 = -((i2cData[2] << 8) | i2cData[3]);
    AcZ2 = ((i2cData[4] << 8) | i2cData[5]);
    //  tempRaw = (i2cData[6] << 8) | i2cData[7];
    GyX2 = -(i2cData[8] << 8) | i2cData[9];
    GyY2 = (i2cData[10] << 8) | i2cData[11];
    GyZ2 = -(i2cData[12] << 8) | i2cData[13];
  }
}

uint8_t i2cWrite(uint8_t IMUAddress, uint8_t registerAddress, uint8_t data, bool sendStop) {
  return i2cWrite(IMUAddress, registerAddress, &data, 1, sendStop); // Returns 0 on success
}

uint8_t i2cWrite(uint8_t IMUAddress, uint8_t registerAddress, uint8_t *data, uint8_t length, bool sendStop) {
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  Wire.write(data, length);
  uint8_t rcode = Wire.endTransmission(sendStop); // Returns 0 on success
  if (rcode) {
    Serial.print(F("i2cWrite failed: "));
    Serial.println(rcode);
  }
  return rcode; // See: http://arduino.cc/en/Reference/WireEndTransmission
}

uint8_t i2cRead(uint8_t IMUAddress, uint8_t registerAddress, uint8_t *data, uint8_t nbytes) {
  uint32_t timeOutTimer;
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  uint8_t rcode = Wire.endTransmission(false); // Don't release the bus
  if (rcode) {
    Serial.print(F("i2cRead failed: "));
    Serial.println(rcode);
    return rcode; // See: http://arduino.cc/en/Reference/WireEndTransmission
  }
  Wire.requestFrom(IMUAddress, nbytes, (uint8_t)true); // Send a repeated start and then release the bus after reading
  for (uint8_t i = 0; i < nbytes; i++) {
    if (Wire.available())
      data[i] = Wire.read();
    else {
      timeOutTimer = micros();
      while (((micros() - timeOutTimer) < I2C_TIMEOUT) && !Wire.available());
      if (Wire.available())
        data[i] = Wire.read();
      else {
        Serial.println(F("i2cRead timeout"));
        return 5; // This error value is not already taken by endTransmission
      }
    }
  }
  return 0; // Success
}

void updatePitchRoll() {
  // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // It is then converted from radians to degrees
#ifdef RESTRICT_PITCH // Eq. 25 and 26
  roll1 = atan2(AcY1, AcZ1) * RAD_TO_DEG;
  pitch1 = atan(-AcX1 / sqrt(AcY1 * AcY1 + AcZ1 * AcZ1)) * RAD_TO_DEG;
  roll2 = atan2(AcY2, AcZ2) * RAD_TO_DEG;
  pitch2 = atan(-AcX2 / sqrt(AcY2 * AcY2 + AcZ2 * AcZ2)) * RAD_TO_DEG;
#else // Eq. 28 and 29
  roll1 = atan(AcY1 / sqrt(AcX1 * AcX1 + AcZ1 * AcZ1)) * RAD_TO_DEG;
  pitch1 = atan2(-AcX1, AcZ1) * RAD_TO_DEG;
  roll2 = atan(AcY2 / sqrt(AcX2 * AcX2 + AcZ2 * AcZ2)) * RAD_TO_DEG;
  pitch2 = atan2(-AcX2, AcZ2) * RAD_TO_DEG;
#endif
}

void updateYaw() { // See: http://www.freescale.com/files/sensors/doc/app_note/AN4248.pdf
  magX1 *= -1; // Invert axis - this it done here, as it should be done after the calibration
  magZ1 *= -1;

  magX1 *= magGain[0];
  magY1 *= magGain[1];
  magZ1 *= magGain[2];

  magX1 -= magOffset[0];
  magY1 -= magOffset[1];
  magZ1 -= magOffset[2];

  magX2 *= -1; // Invert axis - this it done here, as it should be done after the calibration
  magZ2 *= -1;

  magX2 *= magGain[0];
  magY2 *= magGain[1];
  magZ2 *= magGain[2];

  magX2 -= magOffset[0];
  magY2 -= magOffset[1];
  magZ2 -= magOffset[2];

  double rollAngle1 = kalAngleX1 * DEG_TO_RAD;
  double pitchAngle1 = kalAngleY1 * DEG_TO_RAD;
  double rollAngle2 = kalAngleX2 * DEG_TO_RAD;
  double pitchAngle2 = kalAngleY2 * DEG_TO_RAD;

  double Bfy1 = magZ1 * sin(rollAngle1) - magY1 * cos(rollAngle1);
  double Bfx1 = magX1 * cos(pitchAngle1) + magY1 * sin(pitchAngle1) * sin(rollAngle1) + magZ1 * sin(pitchAngle1) * cos(rollAngle1);
  double Bfy2 = magZ2 * sin(rollAngle2) - magY2 * cos(rollAngle2);
  double Bfx2 = magX2 * cos(pitchAngle2) + magY2 * sin(pitchAngle2) * sin(rollAngle2) + magZ2 * sin(pitchAngle2) * cos(rollAngle2);

  yaw1 = atan2(-Bfy1, Bfx1) * RAD_TO_DEG;
  yaw2 = atan2(-Bfy2, Bfx2) * RAD_TO_DEG;

  yaw2 *= -1;
  yaw1 *= -1;
}

void kalmanFilter() {

  double dt = (double)(micros() - timer) / 1000000; // Calculate delta time, change based on the frequency we need to take for the readings
  timer = micros();


  /* Roll and pitch estimation */
  updatePitchRoll();
  double gyroXrate1 = GyX1 / 131.0; // Convert to deg/s
  double gyroYrate1 = GyY1 / 131.0; // Convert to deg/s
  double gyroXrate2 = GyX2 / 131.0; // Convert to deg/s
  double gyroYrate2 = GyY2 / 131.0; // Convert to deg/s

#ifdef RESTRICT_PITCH
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((roll1 < -90 && kalAngleX1 > 90) || (roll1 > 90 && kalAngleX1 < -90)) {
    kalmanX1.setAngle(roll1);
    kalAngleX1 = roll1;
  } else
    kalAngleX1 = kalmanX1.getAngle(roll1, gyroXrate1, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleX1) > 90)
    gyroYrate1 = -gyroYrate1; // Invert rate, so it fits the restricted accelerometer reading
  kalAngleY1 = kalmanY1.getAngle(pitch1, gyroYrate1, dt);

  if ((roll2 < -90 && kalAngleX2 > 90) || (roll2 > 90 && kalAngleX2 < -90)) {
    kalmanX2.setAngle(roll2);
    kalAngleX2 = roll2;
  } else
    kalAngleX2 = kalmanX2.getAngle(roll2, gyroXrate2, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleX2) > 90)
    gyroYrate2 = -gyroYrate2; // Invert rate, so it fits the restricted accelerometer reading
  kalAngleY2 = kalmanY2.getAngle(pitch2, gyroYrate2, dt);

#else
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((pitch1 < -90 && kalAngleY1 > 90) || (pitch1 > 90 && kalAngleY1 < -90)) {
    kalmanY1.setAngle(pitch1);
    kalAngleY1 = pitch1;
  } else
    kalAngleY1 = kalmanY1.getAngle(pitch1, gyroYrate1, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleY1) > 90)
    gyroXrate1 = -gyroXrate1; // Invert rate, so it fits the restricted accelerometer reading
  kalAngleX1 = kalmanX1.getAngle(roll1, gyroXrate1, dt); // Calculate the angle using a Kalman filter

  if ((pitch2 < -90 && kalAngleY2 > 90) || (pitch2 > 90 && kalAngleY2 < -90)) {
    kalAngleY2 = pitch2;
  } else
    kalAngleY2 = kalmanY2.getAngle(pitch2, gyroYrate2, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleY2) > 90)
    gyroXrate2 = -gyroXrate2; // Invert rate, so it fits the restricted accelerometer reading
  kalAngleX2 = kalmanX2.getAngle(roll2, gyroXrate2, dt); // Calculate the angle using a Kalman filter
#endif


  /* Yaw estimation */
  updateYaw();
  double gyroZrate1 = GyZ1 / 131.0; // Convert to deg/s
  double gyroZrate2 = GyZ2 / 131.0; // Convert to deg/s
  // This fixes the transition problem when the yaw angle jumps between -180 and 180 degrees
  if ((yaw1 < -90 && kalAngleZ1 > 90) || (yaw1 > 90 && kalAngleZ1 < -90)) {
    kalmanZ1.setAngle(yaw1);
    kalAngleZ1 = yaw1;
  } else
    kalAngleZ1 = kalmanZ1.getAngle(yaw1, gyroZrate1, dt); // Calculate the angle using a Kalman filter

  if ((yaw2 < -90 && kalAngleZ2 > 90) || (yaw2 > 90 && kalAngleZ2 < -90)) {
    kalmanZ2.setAngle(yaw2);
    kalAngleZ2 = yaw2;
  } else
    kalAngleZ2 = kalmanZ2.getAngle(yaw2, gyroZrate2, dt); // Calculate the angle using a Kalman filter
}

