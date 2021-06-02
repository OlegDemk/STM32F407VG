/*
 * ms5611.c
 *
 *  Created on: May 30, 2021
 *      Author: odemki
 */

#include "sensors/ms5611.h"

extern I2C_HandleTypeDef hi2c2;

//#include "i2c.h"
//#include "systick.h"

#define NUM_CALIBRATION_DATA 6

uint16_t fc[NUM_CALIBRATION_DATA]; // 6 factory calibration data
uint32_t raw_pressure, raw_temperature;
enum MS5611_OSR selected_osr = MS5611_OSR_4096;

/**
 * select a hal I2C to talk to MS5611
 * @param i2cx The i2c port to talk to MS5611
 */
void ms5611_set_i2c(I2C_HandleTypeDef* i2cx){
	ms5611_i2cx = i2cx;
}

/**
 * read from MS5611 with default I2C address
 * @param register_address register/command to request data
 * @param length length of bytes to request from MS5611
 * @param output output data
 * @return HAL_STATUS, 0(HAL_OK) = success
 */
uint8_t ms5611_read_i2c(uint8_t register_address,uint8_t length,uint8_t* output)
{
	//return I2C_read(ms5611_i2cx,MS5611_I2C_ADDR,register_address,length,output);
	uint8_t STATUS = 0;
	STATUS = HAL_I2C_Mem_Read(&hi2c2, MS5611_I2C_ADDR<<1, register_address, 1, output, length, 1000);
	return STATUS;
}

/**
 * Write to MS5611 with default I2C address
 * @param register_address register/command to send
 * @param length length of bytes to write to MS5611
 * @param output buffer to hold data to be sent
 * @return HAL_STATUS, 0(HAL_OK) = success
 */
uint8_t ms5611_write_i2c(uint8_t register_address,uint8_t length,uint8_t* input)
{
	//STATUS = I2C_write(ms5611_i2cx,MS5611_I2C_ADDR,register_address,length,input);
	uint8_t STATUS = 0;
	STATUS = HAL_I2C_Mem_Read(&hi2c2, MS5611_I2C_ADDR<<1, register_address, 1, input, length, 1000);
	return STATUS;

}

/**
 * set ADC resolution, from MS5611_OSR_256 to MS5611_OSR_4096
 */
void ms5611_osr_select(enum MS5611_OSR osr){
	selected_osr = osr;
}

/**
 * Initialize MS5611: read and store factory calibration data.
 */
void ms5611_init(){
	//read 6 factory calibration data
	for (int i = 0; i < NUM_CALIBRATION_DATA; i++){
		uint8_t reg_addr = MS5611_CMD_READ_PROM + (i << 1);//interval 2
		uint8_t buffer[2] = {0};
		ms5611_read_i2c(reg_addr,2,buffer);

		fc[i] = (uint16_t)(buffer[0] << 8 | buffer[1]);
	}
}

/**
 * Read raw pressure from MS5611.
 */
void ms5611_update_pressure(){
	uint8_t buffer[3] = {0x00,0x00,0x00};
	int state;
	state = ms5611_write_i2c(MS5611_CMD_CONVERT_D1 | (selected_osr << 1),0,buffer);

	HAL_Delay(12);//time delay necessary for ADC to convert, must be >= 9.02ms

	state = ms5611_read_i2c(MS5611_CMD_ADC_READ,3,buffer);
	raw_pressure = ((uint32_t)buffer[0] << 16) | ((uint32_t)buffer[1] << 8) | ((uint32_t)buffer[2]);

}

/**
 * Read raw temperature from MS5611.
 */
void ms5611_update_temperature(){
	uint8_t buffer[3] = {0x00,0x00,0x00};
	int state;
	state = ms5611_write_i2c(MS5611_CMD_CONVERT_D2 | (selected_osr << 1),0,buffer);

	HAL_Delay(12);//time delay necessary for ADC to convert, must be >= 9.02ms

	state = ms5611_read_i2c(MS5611_CMD_ADC_READ,3,buffer);
	raw_temperature = ((uint32_t)buffer[0] << 16) | ((uint32_t)buffer[1] << 8) | ((uint32_t)buffer[2]);
}

/**
 *	Read raw temperature and pressure from MS5611
 */
void ms5611_update(){
	ms5611_update_temperature();
	ms5611_update_pressure();
}

/**
 * Get calibrated temperature, unit: Celsius degrees
 * @return calibrated temperature
 */
double ms5611_get_temperature(){
	uint32_t dT = raw_temperature - ((uint32_t)fc[4] * 256);
	double TEMP = 2000.0 + dT * (fc[5] / (8388608.0));//unit 0.01 C

	double T2=0;
	if (TEMP < 2000){
		//temperature < 20 Celsius
		T2 = dT * (dT / (2147483648.0));
	}

	TEMP = TEMP - T2;
	return TEMP / 100;
}

/**
 * Get calibrated pressure, unit: mBar
 * @return calibrated pressure
 */
double ms5611_get_pressure(){

	uint32_t dT = raw_temperature - ((uint32_t)fc[4] * 256);
	double TEMP = 2000.0 + dT * (fc[5] / (8388608.0));//unit 0.01 C

	double OFF = fc[1] * (65536) + fc[3] * dT / (128);
	double SENS = fc[0] * (32768) + fc[2] * dT / (256);

	double P = (raw_pressure * SENS / (2097152.0) - OFF) / (32768);//unit 0.01mbar

	double T2=0, OFF2=0, SENS2=0;
	if (TEMP < 2000){
		//temperature < 20 Celsius
		T2 = dT * dT / (2147483648);
		OFF2 = 5 * (TEMP-2000) * (TEMP-2000) / 2;
		SENS2 = 5 * (TEMP-2000) * (TEMP-2000) / 4;

		if (TEMP < -1500){
			//temperature < -15 Celsius
			OFF2 = OFF2 + 7 * (TEMP + 1500) * (TEMP + 1500);
			SENS2 = SENS2 + 11/2 * (TEMP + 1500) * (TEMP + 1500);
		}
	}

	TEMP = TEMP - T2;
	OFF = OFF - OFF2;
	SENS = SENS - SENS2;

	P = (raw_pressure * SENS / (2097152.0) - OFF) / (32768);//unit mbar
	return P / 100;//unit mbar
}
