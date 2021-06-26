/*
 * main_sensor_file.h
 *
 *  Created on: May 27, 2021
 *      Author: odemki
 */

#ifndef INC_SENSORS_MAIN_SENSOR_FILE_H_
#define INC_SENSORS_MAIN_SENSOR_FILE_H_

void detect_all_sensors_and_init(void);
void measure_sensors(void);
void am2302_measure(void);


struct {
	// Sensors
	bool BME280_ready_status;		// Temperature, Humidity and pleasure
	bool MPU6050_ready_status; 		// IMU acceleration and gyroscope
	bool HMC5883L_ready_status; 	// Magnetometer
	bool MS5611_ready_status;		// Reassure
	bool APDS9960_ready_status; 	// Motion sensor

	bool AM2302_ready_status;

	// Clock
	bool OLED_ready_status;			// OLED screen
	bool DS3231_ready_status; 		// RTC Clock module

	// variable for data from i2c device
	float  BME280_temperature;
	float  BME280_humidity;
	float  BME280_preasure;

	// Data from MPU6050
	double MPU6050_acceleration_Ax;
	double MPU6050_acceleration_Ay;
	double MPU6050_acceleration_Az;
	double MPU6050_gyro_Gx;
	double MPU6050_gyro_Gy;
	double MPU6050_gyro_Gz;
	float MPU6050_temperature;

	// Data from MS5611
	double MS5611_temperature;
	double MS5611_pressure;

	// Data from AM2302
	float AM2302_temperature;
	float AM2302_humidity;

	// Data from APDS9960
	bool APDS9960_Gesture_UP;
	bool APDS9960_Gesture_DOWN;
	bool APDS9960_Gesture_RIGHT;
	bool APDS9960_Gesture_LEFT;
	// ..... some another data fron APDS9960




}i2c_device;


#endif /* INC_SENSORS_MAIN_SENSOR_FILE_H_ */
