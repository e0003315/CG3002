#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "I2Cdev.h"
#include "MPU6050.h"
//#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

#define STACK_SIZE   200
SemaphoreHandle_t processSemaphore = xSemaphoreCreateBinary();
SemaphoreHandle_t sendSemaphore = xSemaphoreCreateBinary();

MPU6050 accelgyro(0x68);
MPU6050 accelgyro2(0x69);
#define sen1 6
#define sen2 7
#define CURRENT_PIN A0
#define VOLTAGE_PIN A1
#define STACK_SIZE 200

//Constants
const int MPU1_addr=0x68;  // I2C address of the MPU-6050
const int MPU2_addr=0x69;
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
int a = 0;
int incomingByte = 0;
int flag =0;
char dataString[1000] = "";
char data[1000] = "";
char s[1000] = "";
char ack;

void readacc(int i){
  if(i==1){
    Wire.beginTransmission(MPU1_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU1_addr,14,true);  // request a total of 14 registers
  }
  else if(i==2){
    Wire.beginTransmission(MPU2_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU2_addr,14,true);  // request a total of 14 registers
  }
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

void sendReadings(void *p)
{
	for( ;; ) {
		//Serial.print("beforeprocessReadings ");
		if( xSemaphoreTake( sendSemaphore, 5000 ) == pdTRUE )
		{
			//code goes here
			  sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", AcX, AcY, AcZ,GyX,GyY,GyZ,AcX2,AcY2,AcZ2,GyX2,GyY2,GyZ2);
			  sprintf(s, "%d", strlen(data));
			  Serial.println(strlen(data));
			  Serial.println(data);

			Serial.println("3. sendReadings");
		}
	}
}
void processReadings(void *p)
{
	for( ;; ) {
		//Serial.print("beforeprocessReadings ");
		if( xSemaphoreTake( processSemaphore, 5000 ) == pdTRUE )
		{
			//code goes here

			Serial.println("2. processReadings");
			xSemaphoreGive(sendSemaphore);
		}
	}
}

void readSensors(void *p){
	int count =0;

	TickType_t xLastWakeTime;
	const TickType_t xPeriod = 15620; //1hertz currently (divide 80 for 80hz) (divide 100 for 100hertz)
	xLastWakeTime = 0;

	for( ;; ) {
		//code goes here

	    readacc(1);
	    readacc(2);
	    counter = counter+1;

	    if(counter == COUNTERSTOP){
	      counter = 0;
	      voltageSum += analogRead(VOLTAGE_PIN);
	      currentSum += analogRead(CURRENT_PIN);
	    }


		Serial.println(count);
		Serial.println("1. readSensor");

		xSemaphoreGive(processSemaphore);
		vTaskDelayUntil( &xLastWakeTime, xPeriod);

		count++;
	}

}

void setup()
{
	Serial.begin(115200);
}

void loop() {
	Serial.println("freeRTOS Pls work....");
	xTaskCreate(readSensors,           // Pointer to the task entry function
			"read",         // Task name
			STACK_SIZE,      // Stack size
			NULL,            // Pointer that will be used as parameter
			3,               // Task priority
			NULL);           // Used to pass back a handle by which the created task can be referenced.

	xTaskCreate(processReadings,           // Pointer to the task entry function
			"process",         // Task name
			STACK_SIZE,      // Stack size
			NULL,            // Pointer that will be used as parameter
			2,               // Task priority
			NULL);           // Used to pass back a handle by which the created task can be referenced.

	xTaskCreate(sendReadings,           // Pointer to the task entry function
			"send",         // Task name
			STACK_SIZE,      // Stack size
			NULL,            // Pointer that will be used as parameter
			1,               // Task priority
			NULL);           // Used to pass back a handle by which the created task can be referenced.

	vTaskStartScheduler();
}
