#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <crc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <pthread.h>
#include <errno.h>
#include <elevadorControle.h>

int static uart_filestream = -1;

int crcVerifica(unsigned char* rx_buffer, int tamanho_rx)
{
    short crc = (rx_buffer[tamanho_rx - 2] & 0xFF) | ((rx_buffer[tamanho_rx - 1] & 0xFF) << 8);
    short comparaCrc = calcula_CRC(rx_buffer, tamanho_rx - 2);

    if (crc == comparaCrc) return 1;
    else return 0;
}

int uartIniciar()
{
    uart_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_filestream == -1)
    {
        printf("Erro:\nNao foi possivel iniciar a UART.\n");
        return -1;
    }

    struct termios options;
    tcgetattr(uart_filestream, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    tcflush(uart_filestream, TCIFLUSH);
    tcsetattr(uart_filestream, TCSANOW, &options);

    return 0;
}

void uartFechar()
{
    if (uart_filestream!= -1)
    {
        close(uart_filestream);
        uart_filestream = -1;
    }
}

void captaBotao_e1(unsigned char *dados)
{
    pthread_mutex_lock(&uartMutex);
    unsigned char tx_buffer[10];
    tx_buffer[0] = 0x01;
    tx_buffer[1] = 0x03;
    tx_buffer[2] = 0x00;
    tx_buffer[3] = 0xB;
    tx_buffer[4] = 6;
    tx_buffer[5] = 6;
    tx_buffer[6] = 0;
    tx_buffer[7] = 3;

    short crc = calcula_CRC(tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[8] = (unsigned char)(crc & 0xFF);
    tx_buffer[9] = (unsigned char)((crc >> 8) & 0xFF);

    if (uart_filestream != -1)
    {
        int contador = write(uart_filestream, tx_buffer, sizeof(tx_buffer));
        if (contador <= 0)
        {
            perror("Erro:\nFalha na transmissao UART\n");
            return;
        }
    }
    else
    {
        perror("Erro:\nNao foi inicializada a porta UART\n");
        return;
    }

    tcflush(uart_filestream, TCIFLUSH);
    delay(50);

    if (uart_filestream != -1)
    {
        unsigned char rx_buffer[256];
        int tamanho_rx = read(uart_filestream, rx_buffer, 255);

        if (tamanho_rx < 0) perror("Erro:\nFalha na leitura UART.\n");
        else if (crcVerifica(rx_buffer, tamanho_rx)) memcpy(dados, &rx_buffer[2], 11);
    }
    else perror("Erro:\nNao foi inicializada a porta UART\n");

    pthread_mutex_unlock(&uartMutex);
}

void captaBotao_e2(unsigned char *dados)
{
    pthread_mutex_lock(&uartMutex);
    unsigned char tx_buffer[10];
    tx_buffer[0] = 0x01;
    tx_buffer[1] = 0x03;
    tx_buffer[2] = 0xA0;
    tx_buffer[3] = 0xB;
    tx_buffer[4] = 6;
    tx_buffer[5] = 6;
    tx_buffer[6] = 0;
    tx_buffer[7] = 3;

    short crc = calcula_CRC(tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[8] = (unsigned char)(crc & 0xFF);
    tx_buffer[9] = (unsigned char)((crc >> 8) & 0xFF);

    if (uart_filestream != -1)
    {
        int contador = write(uart_filestream, tx_buffer, sizeof(tx_buffer));
        if (contador <= 0)
        {
            perror("Erro:\nFalha na transmissao UART\n");
            return;
        }
    }
    else
    {
        perror("Erro:\nNao foi inicializada a porta UART\n");
        return;
    }

    tcflush(uart_filestream, TCIFLUSH);
    delay(50);

    if (uart_filestream != -1)
    {
        unsigned char rx_buffer[256];
        int tamanho_rx = read(uart_filestream, rx_buffer, 255);

        if (tamanho_rx < 0)
            perror("Erro:\nFalha na leitura UART.\n");
        else if (crcVerifica(rx_buffer, tamanho_rx))
            memcpy(dados, &rx_buffer[2], 11);
    }
    else
        perror("Erro:\nNao foi inicializada a porta UART\n");

    pthread_mutex_unlock(&uartMutex);
}

int captaEncoder_e1()
{
    pthread_mutex_lock(&uartMutex);
    unsigned int vlrInt = 30000;
    unsigned char tx_buffer[10] = {0x01, 0x23, 0xC1, 0x00, 6, 6, 0, 3};

    short crc = calcula_CRC(&tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[8] = (unsigned char)(crc & 0xFF);
    tx_buffer[9] = (unsigned char)((crc >> 8) & 0xFF);

    if (uart_filestream != -1)
    {
        int contador = write(uart_filestream, &tx_buffer, sizeof(tx_buffer));
        if (contador <= 0) printf("Error buffer tx UART.\n");
    }
    tcflush(uart_filestream, TCIFLUSH);
    delay(50);

    if (uart_filestream != -1)
    {
        unsigned char rx_buffer[256];
        int tamanho_rx = read(uart_filestream, (void *)rx_buffer, 255);

        if (tamanho_rx < 0) printf("Erro:\nFalha na leitura di Encoder.\n");
        else if (tamanho_rx > 0)
            if (crcVerifica(rx_buffer, tamanho_rx))
            {
                unsigned char dados[4];
                memcpy(dados, &rx_buffer[3], 7);
                char stringHex[9];
                sprintf(stringHex, "%02X%02X%02X%02X", dados[3], dados[2], dados[1], dados[0]);
                vlrInt = strtoul(stringHex, NULL, 16);
            }
    }
    pthread_mutex_unlock(&uartMutex);
    return vlrInt;

}

int captaEncoder_e2()
{
    pthread_mutex_lock(&uartMutex);
    unsigned int vlrInt = 30000;
    unsigned char tx_buffer[10] = {0x01, 0x23, 0xC1, 0x01, 6, 6, 0, 3};

    short crc = calcula_CRC(&tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[8] = (unsigned char)(crc & 0xFF);
    tx_buffer[9] = (unsigned char)((crc >> 8) & 0xFF);

    if (uart_filestream != -1)
    {
        int contador = write(uart_filestream, &tx_buffer, sizeof(tx_buffer));
        if (contador <= 0)
            printf("Error buffer tx UART.\n");
    }
    tcflush(uart_filestream, TCIFLUSH);
    delay(50);

    if (uart_filestream != -1)
    {
        unsigned char rx_buffer[256];
        int tamanho_rx = read(uart_filestream, (void *)rx_buffer, 255);

        if (tamanho_rx < 0)
            printf("Erro:\nFalha na leitura di Encoder.\n");
        else if (tamanho_rx > 0)
            if (crcVerifica(rx_buffer, tamanho_rx))
            {
                unsigned char dados[4];
                memcpy(dados, &rx_buffer[3], 7);
                char stringHex[9];
                sprintf(stringHex, "%02X%02X%02X%02X", dados[3], dados[2], dados[1], dados[0]);
                vlrInt = strtoul(stringHex, NULL, 16);
            }
    }
    pthread_mutex_unlock(&uartMutex);
    return vlrInt;
}

int btnApagar_e1(int base)
{
    pthread_mutex_lock(&uartMutex);
    unsigned char addr[11] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
    unsigned char tx_buffer[11] = {0x01, 0x06, addr[base], 0x01, 0x00, 0x06, 0x06, 0x00, 0x03};

    short crc = calcula_CRC(&tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[9] = (unsigned char)(crc & 0xFF);
    tx_buffer[10] = (unsigned char)((crc >> 8) & 0xFF);

    printf("\nApagando botão %d \n", base);

    if (uart_filestream != -1)
    {
        int count = write(uart_filestream, &tx_buffer, sizeof(tx_buffer));
        if (count <= 0)
        {
            printf("UART TX error\n");
        }
    }

    tcflush(uart_filestream, TCIFLUSH);
    delay(20);

    //----- CHECK FOR ANY RX BYTES -----
    if (uart_filestream != -1)
    {
        unsigned char rx_buffer[256];
        int rx_length = read(uart_filestream, (void *)rx_buffer, 255);

        if (rx_length < 0)
        {
            printf("Erro na leitura.\n");
        }
        else if (crcVerifica(rx_buffer, rx_length))
        {
        }
    }
    pthread_mutex_unlock(&uartMutex);
    return 0;
}

int btnApagar_e2(int base)
{
    pthread_mutex_lock(&uartMutex);
    unsigned char addr[11] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};
    unsigned char tx_buffer[11] = {0x01, 0x06, addr[base], 0x01, 0x00, 0x06, 0x06, 0x00, 0x03};

    short crc = calcula_CRC(&tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[9] = (unsigned char)(crc & 0xFF);
    tx_buffer[10] = (unsigned char)((crc >> 8) & 0xFF);

    printf("\nApagando botão %d \n", base);

    if (uart_filestream != -1)
    {
        int count = write(uart_filestream, &tx_buffer, sizeof(tx_buffer));
        if (count <= 0)
        {
            printf("UART TX error\n");
        }
    }

    tcflush(uart_filestream, TCIFLUSH);
    delay(20);

    //----- CHECK FOR ANY RX BYTES -----
    if (uart_filestream != -1)
    {
        unsigned char rx_buffer[256];
        int rx_length = read(uart_filestream, (void *)rx_buffer, 255);

        if (rx_length < 0)
        {
            printf("Erro na leitura.\n");
        }
        else if (crcVerifica(rx_buffer, rx_length))
        {
        }
    }
    pthread_mutex_unlock(&uartMutex);
    return 0;
}

int pwmEnviar_e1(int pwm)
{
    pthread_mutex_lock(&uartMutex);

    unsigned char tx_buffer[14] = {0x01, 0x16, 0xC2, 0x00, 0, 0, 0, 0, 0x06, 0x06, 0x00, 0x03};
    memcpy(&tx_buffer[3], &pwm, sizeof(pwm));

    short crc = calcula_CRC(&tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[12] = (unsigned char)(crc & 0xFF);
    tx_buffer[13] = (unsigned char)((crc >> 8) & 0xFF);

    if (uart_filestream != -1)
    {
        int count = write(uart_filestream, &tx_buffer, sizeof(tx_buffer));
        if (count <= 0)
        {
            printf("UART TX error\n");
        }
    }

    tcflush(uart_filestream, TCIFLUSH);
    delay(20);

    if (uart_filestream != -1)
    {
        unsigned char rx_buffer[256];
        int rx_length = read(uart_filestream, rx_buffer, 255);

        if (rx_length < 0)
        {
            perror("Erro na leitura UART");
        }
        else if (crcVerifica(rx_buffer, rx_length))
        {
        }
    }
    else
    {
        perror("Erro - A porta UART não foi inicializada");
    }
    pthread_mutex_unlock(&uartMutex);
    return 0;
}

int pwmEnviar_e2(int pwm)
{
    pthread_mutex_lock(&uartMutex);

    unsigned char tx_buffer[14] = {0x01, 0x16, 0xC2, 0x01, 0, 0, 0, 0, 0x06, 0x06, 0x00, 0x03};
    memcpy(&tx_buffer[3], &pwm, sizeof(pwm));

    short crc = calcula_CRC(&tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[12] = (unsigned char)(crc & 0xFF);
    tx_buffer[13] = (unsigned char)((crc >> 8) & 0xFF);

    if (uart_filestream != -1)
    {
        int count = write(uart_filestream, &tx_buffer, sizeof(tx_buffer));
        if (count <= 0)
        {
            printf("UART TX error\n");
        }
    }

    tcflush(uart_filestream, TCIFLUSH);
    delay(20);

    if (uart_filestream != -1)
    {
        unsigned char rx_buffer[256];
        int rx_length = read(uart_filestream, rx_buffer, 255);

        if (rx_length < 0)
        {
            perror("Erro na leitura UART");
        }
        else if (crcVerifica(rx_buffer, rx_length))
        {
        }
    }
    else
    {
        perror("Erro - A porta UART não foi inicializada");
    }
    pthread_mutex_unlock(&uartMutex);
    return 0;
}

void temperaturaEnviar_e1(float temperatura)
{
    pthread_mutex_lock(&uartMutex);

    unsigned char tx_buffer[14] = {0x01, 0x16, 0xD1, 0x00, 0, 0, 0, 0, 0x06, 0x06, 0x00, 0x03};
    memcpy(&tx_buffer[3], &temperatura, sizeof(temperatura));

    short crc = calcula_CRC(&tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[12] = (unsigned char)(crc & 0xFF);
    tx_buffer[13] = (unsigned char)((crc >> 8) & 0xFF);

    if (uart_filestream != -1)
    {
        int count = write(uart_filestream, &tx_buffer, sizeof(tx_buffer));
        if (count <= 0)
        {
            printf("UART TX error\n");
        }
    }

    tcflush(uart_filestream, TCIFLUSH);
    delay(20);

    if (uart_filestream != -1)
    {

        unsigned char rx_buffer[256];
        int rx_length = read(uart_filestream, rx_buffer, 255);

        if (rx_length < 0)
        {
            perror("Erro na leitura UART");
        }
        else if (crcVerifica(rx_buffer, rx_length))
        {
        }
    }
    else
    {
        perror("Erro - A porta UART não foi inicializada");
    }

    pthread_mutex_unlock(&uartMutex);
}

void temperaturaEnviar_e2(float temperatura)
{
    pthread_mutex_lock(&uartMutex);

    unsigned char tx_buffer[14] = {0x01, 0x16, 0xD1, 0x01, 0, 0, 0, 0, 0x06, 0x06, 0x00, 0x03};
    memcpy(&tx_buffer[3], &temperatura, sizeof(temperatura));

    short crc = calcula_CRC(&tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[12] = (unsigned char)(crc & 0xFF);
    tx_buffer[13] = (unsigned char)((crc >> 8) & 0xFF);

    if (uart_filestream != -1)
    {
        int count = write(uart_filestream, &tx_buffer, sizeof(tx_buffer));
        if (count <= 0)
        {
            printf("UART TX error\n");
        }
    }

    tcflush(uart_filestream, TCIFLUSH);
    delay(20);

    if (uart_filestream != -1)
    {

        unsigned char rx_buffer[256];
        int rx_length = read(uart_filestream, rx_buffer, 255);

        if (rx_length < 0)
        {
            perror("Erro na leitura UART");
        }
        else if (crcVerifica(rx_buffer, rx_length))
        {
        }
    }
    else
    {
        perror("Erro - A porta UART não foi inicializada");
    }

    pthread_mutex_unlock(&uartMutex);
}