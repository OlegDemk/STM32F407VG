/*
 * main_sensor_file.c
 *
 *  Created on: May 27, 2021
 *      Author: odemki
 */

#include "main.h"

#include "screens/oled/oled.h"
#include "screens/oled/gfx.h"

#include "sensors/mpu6050.h"
#include <sensors/hmc5883l.h>

extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

//MPU6050_t MPU6050;

void detect_bme280(void );
void detect_mpu6050(void );
void detect_apds9960(void );
void denect_hmc5883l(void );
void detect_ms5611(void );
void detect_oled_screen(void );
void detect_ds3231(void );

/*зробити структуру , в якій записувати в bool поля статус дівайсів
 *
 */

struct {
	// Sensors
	bool BME280_ready_status;		// Temperature, Humidity and pleasure
	bool MPU6050_ready_status; 		// IMU acceleration and gyroscope
	bool HMC5883L_ready_status; 	// Magnetometer
	bool MS5611_ready_status;		// Reassure
	bool APDS9960_ready_status; 	// Motion sensor

	// Clock
	bool OLED_ready_status;			// OLED screen
	bool DS3231_ready_status; 		// RTC Clock module

	// variable for data from i2c device
	int  BME280_temperature;
	int  BME280_humidity;
	int  BME280_preasure;

	int MPU6050_acceleration_x;
	int MPU6050_acceleration_y;
	int MPU6050_acceleration_z;
	int MPU6050_gyro_x;
	int MPU6050_gyro_y;
	int MPU6050_gyro_z;
	float MPU6050_temperature;



}i2c_device;


//----------------------------------------------------------------------------------------
void detect_all_sensors(void)
{
	detect_bme280();
	detect_mpu6050();
	detect_apds9960();
	denect_hmc5883l();
	detect_ms5611();
	detect_oled_screen();
	detect_ds3231();
}

//----------------------------------------------------------------------------------------
void detect_bme280(void)
{
	uint16_t STATUS=0;
	uint16_t addres_device = 0x76;  		 	// BME280
	uint16_t id_addr = 0xD0;
	uint8_t id = 96;							// in hex form
	uint8_t buff=0;        						// Return 0x96 -> Dec 60

	STATUS = HAL_I2C_Mem_Read(&hi2c3, addres_device<<1, id_addr, 1, &buff, 1, 1000);
	//HAL_OK == 0
	if((buff == id) && (STATUS == 0))
	{
		i2c_device.BME280_ready_status = true;
	}
	else
	{
		i2c_device.BME280_ready_status = false;
	}
}
//----------------------------------------------------------------------------------------
void detect_mpu6050(void)
{
	uint16_t STATUS = 0;
	uint16_t addres_device = 0xD0;
	uint16_t id_addr = 0x75;
	uint8_t id = 104;							// in hex form
	uint8_t buff = 0;

	STATUS = HAL_I2C_Mem_Read(&hi2c2, addres_device, id_addr, 1, &buff, 1, 1000);

	if((buff == id) && (STATUS == 0))
	{
		i2c_device.MPU6050_ready_status = true;
	}
	else
	{
		i2c_device.MPU6050_ready_status = false;
	}

	// turn on direct i2c communication with hmc5883l
	if(i2c_device.MPU6050_ready_status == true)
	{
		uint8_t data = 0x00;
		STATUS = HAL_I2C_Mem_Write(&hi2c2, addres_device, 0x6A, 1, &data, 1, 1000);		// Disable i2c master mode
		data = 0x02;
		STATUS = HAL_I2C_Mem_Write(&hi2c2, addres_device, 0x37, 1, &data, 1, 1000);		// Enable i2c master bypass mode

		// Check waited registers
		buff = 0;
		STATUS = HAL_I2C_Mem_Read(&hi2c2, addres_device, 0x6A, 1, &buff, 1, 1000);
		buff = 0;
		STATUS = HAL_I2C_Mem_Read(&hi2c2, addres_device, 0x37, 1, &buff, 1, 1000);

		if(STATUS == 0)
		{
			denect_hmc5883l();     // Don't work  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
	}
}
//----------------------------------------------------------------------------------------
void denect_hmc5883l(void)
{
	uint16_t STATUS = 0;
	uint16_t addres_device = 0x1E;       //0x1E                		 ///// 0x3C
	uint16_t addr = 0x0A;				// HMC5883L_REG_IDENT_A      ///// 0xA0
	//uint8_t id = 0xa8;							// in hex form
	uint8_t buff = 0;

	//STATUS = HAL_I2C_Mem_Write(&hi2c2, addres_device, 0x37, 1, &data, 1, 1000);

	STATUS = HAL_I2C_Mem_Read(&hi2c2, addres_device<<1 , addr, 1, &buff, 1, 1000);
	if(STATUS == 0)
	{
		i2c_device.HMC5883L_ready_status = true;
	}
	else
	{
		i2c_device.HMC5883L_ready_status = false;
	}

//	HMC5883L_setRange(HMC5883L_RANGE_1_3GA);
//	HMC5883L_setMeasurementMode(HMC5883L_CONTINOUS);
//	HMC5883L_setDataRate(HMC5883L_DATARATE_15HZ);
//	HMC5883L_setSamples(HMC5883L_SAMPLES_1); HMC5883L_setOffset(0, 0);
//
//	Vector mag = HMC5883L_readRaw();

}
//----------------------------------------------------------------------------------------
void detect_ms5611(void)
{
	uint16_t STATUS = 0;
	uint16_t addres_device = 0x77;
	uint16_t id_addr = 0x00 ;
	uint8_t buff = 0;

	STATUS = HAL_I2C_Mem_Read(&hi2c2, addres_device<<1, id_addr, 1, &buff, 1, 1000);

	if(STATUS == 0)
	{
		i2c_device.MS5611_ready_status = true;
	}
	else
	{
		i2c_device.MS5611_ready_status = false;
	}
}
//----------------------------------------------------------------------------------------
void detect_apds9960(void)
{
	uint16_t STATUS = 0;
	uint16_t addres_device = 0x39;
	uint16_t id_addr = 0x92;
	uint8_t id = 0xa8;							// in hex form
	uint8_t buff = 0;

	STATUS = HAL_I2C_Mem_Read(&hi2c2, addres_device<<1, id_addr, 1, &buff, 1, 1000);

	if((buff == id) && (STATUS == 0))
	{
		i2c_device.APDS9960_ready_status = true;
	}
	else
	{
		i2c_device.APDS9960_ready_status = false;
	}
}
//----------------------------------------------------------------------------------------
void detect_oled_screen(void)
{
	uint16_t STATUS = 0;
	uint8_t buffer[2];
	buffer[0] = 0x00;
	buffer[1] = 0xAE;

	STATUS = HAL_I2C_Master_Transmit(&hi2c3, 0x78,buffer, 2, 1000);

	if(STATUS == 0)
	{
		i2c_device.OLED_ready_status = true;
	}
	else
	{
		i2c_device.OLED_ready_status = false;
	}
}
//----------------------------------------------------------------------------------------
void detect_ds3231(void)
{
	uint16_t STATUS = 0;
	uint16_t addres_device = 0x68;
	uint16_t id_addr = 0x00;						// Read seconds
	uint8_t buff = 0;

	STATUS = HAL_I2C_Mem_Read(&hi2c3, addres_device<<1, id_addr, 1, &buff, 1, 1000);

	if(STATUS == 0)
	{
		i2c_device.DS3231_ready_status = true;
	}
	else
	{
		i2c_device.DS3231_ready_status = false;
	}
}
//----------------------------------------------------------------------------------------


















	    //



