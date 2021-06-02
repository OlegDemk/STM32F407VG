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

#include "sensors/bme280_defs.h"
#include "sensors/bme280.h"

#include "sensors/ms5611.h"


extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

//MPU6050_t MPU6050;
// Functions for detect i2c devices
void detect_bme280(void );
void detect_mpu6050(void );
void detect_apds9960(void );
void denect_hmc5883l(void );
void detect_ms5611(void );
void detect_oled_screen(void );
void detect_ds3231(void );


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// BME280 part
struct bme280_dev dev;
struct bme280_data comp_data;
int8_t rslt;

int8_t init_bme280(void);
void bme280_measure(void);

//----------------------------------------------------------------------------------------
int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  if(HAL_I2C_Master_Transmit(&hi2c3, (id << 1), &reg_addr, 1, 10) != HAL_OK) return -1;
  if(HAL_I2C_Master_Receive(&hi2c3, (id << 1) | 0x01, data, len, 10) != HAL_OK) return -1;

  return 0;
}
//----------------------------------------------------------------------------------------
void user_delay_ms(uint32_t period)
{
  HAL_Delay(period);
}
//----------------------------------------------------------------------------------------
int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  int8_t *buf;
  buf = malloc(len +1);
  buf[0] = reg_addr;
  memcpy(buf +1, data, len);

  if(HAL_I2C_Master_Transmit(&hi2c3, (id << 1), (uint8_t*)buf, len + 1, HAL_MAX_DELAY) != HAL_OK) return -1;

  free(buf);
  return 0;
}
// End BME280 part
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// MPU6050 part
int8_t init_mpu6050(void);
void mpu6050_measure(void);

MPU6050_t MPU6050;
// End MPU6050 part
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// MPU6050 part
int8_t init_ms5611(void);
void ms5611_measure(void);
// End MPU6050 part
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------



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


}i2c_device;


//----------------------------------------------------------------------------------------
void detect_all_sensors_and_init(void)
{
	detect_bme280();
	if(i2c_device.BME280_ready_status == true)
	{
		init_bme280();
	}

	detect_mpu6050();
	if(i2c_device.MPU6050_ready_status == true)
	{
		init_mpu6050();
	}

	denect_hmc5883l();								// Don't work

	detect_ms5611();
	if(i2c_device.MS5611_ready_status == true)
	{
		init_ms5611();
	}


	detect_apds9960();
	detect_oled_screen();
	detect_ds3231();

}
//---------------------------------------------------------------------------------------
// Measure one time
void measure_sensors(void)
{
	bme280_measure();
	mpu6050_measure();
	ms5611_measure();


//	apds9960();
}

//----------------------------------------------------------------------------------------
int8_t init_ms5611(void)
{
	ms5611_set_i2c(&hi2c2);
	ms5611_init();

}
//----------------------------------------------------------------------------------------
void ms5611_measure(void)
{
	ms5611_update();

	double temp = ms5611_get_temperature();
	double pressure = ms5611_get_pressure();
}
//----------------------------------------------------------------------------------------
int8_t init_mpu6050(void)
{
	if(MPU6050_Init(&hi2c2) == 1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
//----------------------------------------------------------------------------------------
void mpu6050_measure(void)
{
	MPU6050_Read_All(&hi2c2, &MPU6050);

	i2c_device.MPU6050_acceleration_Ax = MPU6050.Ax;
	i2c_device.MPU6050_acceleration_Ay = MPU6050.Ay;
	i2c_device.MPU6050_acceleration_Az = MPU6050.Az;

	i2c_device.MPU6050_gyro_Gx = MPU6050.Gx;
	i2c_device.MPU6050_gyro_Gy = MPU6050.Gy;
	i2c_device.MPU6050_gyro_Gz = MPU6050.Gz;

	i2c_device.MPU6050_temperature = MPU6050.Temperature;
}
//----------------------------------------------------------------------------------------
int8_t init_bme280(void)
{
	dev.dev_id = BME280_I2C_ADDR_PRIM;
	dev.intf = BME280_I2C_INTF;
	dev.read = user_i2c_read;
	dev.write = user_i2c_write;
	dev.delay_ms = user_delay_ms;

	rslt = bme280_init(&dev);

	dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	dev.settings.osr_p = BME280_OVERSAMPLING_16X;
	dev.settings.osr_t = BME280_OVERSAMPLING_2X;
	dev.settings.filter = BME280_FILTER_COEFF_16;
	rslt = bme280_set_sensor_settings(BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL, &dev);

	//	  rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
	rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, &dev);

	dev.delay_ms(40);
}
//----------------------------------------------------------------------------------------
void bme280_measure(void)
{
	rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);

	if(rslt == BME280_OK)
	{
		// Save data in main structure
		i2c_device.BME280_temperature = comp_data.temperature;
		i2c_device.BME280_humidity = comp_data.humidity;
		i2c_device.BME280_preasure = comp_data.pressure;
		//temperature = comp_data.temperature / 100.0;      /* °C  */
		//humidity = comp_data.humidity / 1024.0;           /* %   */
		//pressure = comp_data.pressure / 10000.0;          /* hPa */
	}
}
//----------------------------------------------------------------------------------------




























// Function for detect i2c devices ////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Temperature, humidity and pressure sensor
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
// 1. IMU Module. Measure Acceleration X, Y, Z and Gyroscope X, Y, Z.
// 2. Turn on ability work with  hmc5883l ( Magnetometer  sensor ).
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




