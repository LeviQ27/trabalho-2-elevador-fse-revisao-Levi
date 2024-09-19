#ifndef CRC16_H  
#define CRC16_H  


// funcoes do crc.c
short CRC16(short crc, char data);
short calcula_CRC(unsigned char *commands, int size);

#endif