// Fonte: Reza Ebrahimi - https://github.com/ebrezadev
// Version 1.0


#include "bmp280.h"
#include <wiringPiI2C.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>

int fd_bmp_1;
int fd_bmp_2;

/*writes an array (data[]) of arbitrary size (dataLength) to I2C address (deviceAddress), starting from an internal register address (startRegisterAddress)*/
void bmp280_write_array(uint8_t deviceAddress, uint8_t startRegisterAddress, uint8_t *data, uint8_t dataLength)
{
    int fd = (deviceAddress == 0x76) ? fd_bmp_1:fd_bmp_2;
    wiringPiI2CWrite(fd, startRegisterAddress);
    for (uint8_t i = 0; i < dataLength; i++)
    {
        wiringPiI2CWrite(fd, data[i]);
    }
}

/*reads an array (data[]) of arbitrary size (dataLength) from I2C address (deviceAddress), starting from an internal register address (startRegisterAddress)*/
void bmp280_read_array (uint8_t deviceAddress, uint8_t startRegisterAddress, uint8_t *data, uint8_t dataLength)
{
    int fd = (deviceAddress == 0x76) ? fd_bmp_1:fd_bmp_2;
    wiringPiI2CWrite(fd, startRegisterAddress);
    for (uint8_t i = 0; i < dataLength; i++)
    {
        data[i] = wiringPiI2CRead(fd);
    }
}

/*initiates the I2C peripheral and sets its speed*/
void bmp280_i2c_init()        
{

    fd_bmp_1 = wiringPiI2CSetup(0x76);
    fd_bmp_2 = wiringPiI2CSetup(0x77);

    if (fd_bmp_1 == -1)
    {
        printf("Erro ao inicializar a conexão I2C do elevador 1\n");
    }

    if (fd_bmp_2 == -1)
    {
        printf("Erro ao inicializar a conexão I2C do elevador 2\n");
    }
}

/*a delay function for milliseconds delay*/
void delay_function (uint32_t delayMS)
{
    usleep(delayMS * 1000);
}

/*implements a power function (used in altitude calculation)*/
float power_function (float x, float y)
{
  return pow(x, y);
}