#include <wiringPiI2C.h>
#include <bmp280.h>
#include <pthread.h>
#include <uart.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>

// // Endereços I2C padrão dos sensores BMP280
#define BMP280_ADDR1 0x76
#define BMP280_ADDR2 0x77

uint8_t deviceAddress;
sensors_t sensor_data_e1;
sensors_t sensor_data_e2;


float enviaOledTemperatura_e1()
{    
    bmp280_init();
    delay(50);
    i2c_address_t verificaSensor = bmp280_check_connected_address();
    delay(50);
    if (verificaSensor != BOTH_DETECTED)
        if(verificaSensor == I2C_ADDRESS_1)
        {
            printf("Sensor BMP280 do elevador 1 Não está funcionando!");
        }
        else if (verificaSensor == I2C_ADDRESS_2)
        {
            printf("Sensor BMP280 do elevador 2 Não está funcionando!");
        }
        else
        {
            printf("Nenhum dos sensores estão funcionando!");
        }
    
    sensor_data_e1.temperature = bmp280_get_temperature(BMP280_ADDR1);
    delay(50);
    return (float) sensor_data_e1.temperature;
}

float enviaOledTemperatura_e2()
{
    bmp280_init();
    delay(50);
    i2c_address_t verificaSensor = bmp280_check_connected_address();
    delay(50);
    if (verificaSensor != BOTH_DETECTED)
        if (verificaSensor == I2C_ADDRESS_1)
        {
            printf("Sensor BMP280 do elevador 1 Não está funcionando!");
        }
        else if (verificaSensor == I2C_ADDRESS_2)
        {
            printf("Sensor BMP280 do elevador 2 Não está funcionando!");
        }
        else
        {
            printf("Nenhum dos sensores estão funcionando!");
        }

    sensor_data_e2.temperature = bmp280_get_temperature(BMP280_ADDR1);
    delay(50);

    return (float)sensor_data_e2.temperature;
}