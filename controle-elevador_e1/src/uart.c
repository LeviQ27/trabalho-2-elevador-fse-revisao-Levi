#include <unistd.h>  //Used for UART
#include <fcntl.h>   //Used for UART
#include <termios.h> //Used for UART
#include <crc16.h>
#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <elevadorControle.h>
#include <pthread.h>
#include <errno.h>

// Descritor de arquivo global para a porta serial
int static uart0_filestream = -1;

int verificaCrc(unsigned char* rx_buffer, int rx_length){
    short crc = (rx_buffer[rx_length - 2] & 0xFF) | ((rx_buffer[rx_length - 1] & 0xFF) << 8);
    short crcCmp = calcula_CRC(rx_buffer, rx_length - 2);
    
    if(crc == crcCmp){
        return 1;
    } else {
        return 0;
    }
}

int uartIniciar() {
    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY); // Open in non-blocking read/write mode
    if (uart0_filestream == -1) {
        printf("Erro - Não foi possível iniciar a UART.\n");
        return -1;
    }

    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD; //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    return 0;
}

void uartFechar() {
    if (uart0_filestream != -1) {
        close(uart0_filestream);
        uart0_filestream = -1;
    }
}

void e1_captaBotoes(unsigned char *dados) {
    pthread_mutex_lock(&e1_uartMutex);
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

    if (uart0_filestream != -1) {
        int count = write(uart0_filestream, tx_buffer, sizeof(tx_buffer));
        if (count <= 0) {
            perror("(Botão e1): Erro durante a transmissão UART\n");
            return;
        }
    } else {
        perror("(Botão e1): Erro - A porta UART não foi inicializada\n");
        return;
    }

    tcflush(uart0_filestream, TCIFLUSH);
    delay(50);
    
    if (uart0_filestream != -1) {
        
        unsigned char rx_buffer[256];
        int rx_length = read(uart0_filestream, rx_buffer, 255);

        if (rx_length < 0) {
            perror("(Botão e1): Erro na leitura UART\n");
        } else if (verificaCrc(rx_buffer, rx_length)){
            memcpy(dados, &rx_buffer[2], 11);
        }
    } else {
        perror("(Botão e1): Erro - A porta UART não foi inicializada\n");
    }

    pthread_mutex_unlock(&e1_uartMutex);
}

int e1_captaEncoder() {
    pthread_mutex_lock(&e1_uartMutex);
    unsigned int valorInteiro = 30000;
    unsigned char tx_buffer[10] = {0x01, 0x23, 0xC1, 0x00, 6, 6, 0, 3};

    short crc = calcula_CRC(&tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[8] = (unsigned char)(crc & 0xFF);
    tx_buffer[9] = (unsigned char)((crc >> 8) & 0xFF);

    if (uart0_filestream != -1) {
        int count = write(uart0_filestream, &tx_buffer, sizeof(tx_buffer));
        if (count <= 0) {
            printf("(Encoder e1): UART TX error\n");
        }
    }
    tcflush(uart0_filestream, TCIFLUSH);
    delay(50);

    if (uart0_filestream != -1) {
        unsigned char rx_buffer[256];
        int rx_length = read(uart0_filestream, (void *)rx_buffer, 255);

        if (rx_length < 0) {
            printf("(Encoder e1): Erro na leitura encoder.\n");
        } else if (rx_length > 0) {
            if (verificaCrc(rx_buffer, rx_length)) {
                unsigned char dados[4];
                memcpy(dados, &rx_buffer[3], 7);
                char hexString[10];
                sprintf(hexString, "%02X%02X%02X%02X", dados[3], dados[2], dados[1], dados[0]);
                valorInteiro = strtoul(hexString, NULL, 16);
           }
        }
    }
    pthread_mutex_unlock(&e1_uartMutex);
    return valorInteiro;
    
}

void e1_botaoApaga(int base){
    pthread_mutex_lock(&e1_uartMutex);

    unsigned char addr[11] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
    unsigned char tx_buffer[11] = {0x01, 0x06, addr[base], 0x01, 0x00, 0x06, 0x06, 0x00, 0x03};

    short crc = calcula_CRC(&tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[9] = (unsigned char)(crc & 0xFF); 
    tx_buffer[10] = (unsigned char)((crc >> 8) & 0xFF);

    printf("\nApagando botão %d \n", base);

    if (uart0_filestream != -1){
        int count = write(uart0_filestream, &tx_buffer, sizeof(tx_buffer));
        if (count <= 0)
        {
            printf("(Apaga Botao e1): UART TX error\n");
        }
    }

    tcflush(uart0_filestream, TCIFLUSH);
    delay(20);

    //----- CHECK FOR ANY RX BYTES -----
    if (uart0_filestream != -1){
        unsigned char rx_buffer[256];
        int rx_length = read(uart0_filestream, (void *)rx_buffer, 255);

        if (rx_length < 0){
            printf("(Apaga Botao e1): Erro na leitura.\n");
        }else if (verificaCrc(rx_buffer, rx_length)){
        }
    }
    pthread_mutex_unlock(&e1_uartMutex);
}

void e1_pwmEnvia(int pwm){
    pthread_mutex_lock(&e1_uartMutex);

    unsigned char tx_buffer[14] = {0x01, 0x16, 0xC2, 0x00, 0, 0, 0, 0, 0x06, 0x06, 0x00, 0x03};
    memcpy(&tx_buffer[4], &pwm, sizeof(pwm));

    short crc = calcula_CRC(&tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[12] = (unsigned char)(crc & 0xFF); 
    tx_buffer[13] = (unsigned char)((crc >> 8) & 0xFF);

    if (uart0_filestream != -1){
        int count = write(uart0_filestream, &tx_buffer, sizeof(tx_buffer));
        if (count <= 0){
            printf("(PWM e1): UART TX error\n");
        }
    }

    tcflush(uart0_filestream, TCIFLUSH);
    delay(20);

    if (uart0_filestream != -1) {
        unsigned char rx_buffer[256];
        int rx_length = read(uart0_filestream, rx_buffer, 255);

        if (rx_length < 0) {
            perror("(PWM e1): Erro na leitura UART\n");
        } else if (verificaCrc(rx_buffer, rx_length)){
        }
    } else {
        perror("(PWM e1): Erro - A porta UART não foi inicializada\n");
    }
    pthread_mutex_unlock(&e1_uartMutex);
}

void e1_temperaturaEnvia(float temperatura){
    pthread_mutex_lock(&e1_uartMutex);

    unsigned char tx_buffer[14] = {0x01, 0x16, 0xD1, 0x00, 0, 0, 0, 0, 0x06, 0x06, 0x00, 0x03};
    memcpy(&tx_buffer[4], &temperatura, sizeof(temperatura));

    short crc = calcula_CRC(&tx_buffer, sizeof(tx_buffer) - 2);

    tx_buffer[12] = (unsigned char)(crc & 0xFF); 
    tx_buffer[13] = (unsigned char)((crc >> 8) & 0xFF);

    if (uart0_filestream != -1){
        int count = write(uart0_filestream, &tx_buffer, sizeof(tx_buffer));
        if (count <= 0)
        {
            printf("(Temp e1): UART TX error\n");
        }
    }

    tcflush(uart0_filestream, TCIFLUSH);
    delay(20);

    if (uart0_filestream != -1) {
        
        unsigned char rx_buffer[256];
        int rx_length = read(uart0_filestream, rx_buffer, 255);

        if (rx_length < 0) {
            perror("(Temp e1): Erro na leitura UART\n");
        } else if (verificaCrc(rx_buffer, rx_length)){
        }
    } else {
        perror("(Temp e1): Erro - A porta UART não foi inicializada\n");
    }

    pthread_mutex_unlock(&e1_uartMutex);
}