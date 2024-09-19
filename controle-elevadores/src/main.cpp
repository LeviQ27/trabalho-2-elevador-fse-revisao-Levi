
extern "C" {
#include <wiringPi.h>
#include "elevadorControle.h"
#include "eventos.h"
#include "uart.h"
#include "fila.h"
#include "i2c.h"
#include "calibraSensores.h"
}
#include <bcm2835.h>
#include <cstdint>
#include <cstdio>
#include "SSD1306_OLED.hpp"

#define myOLEDwidth 128
#define myOLEDheight 64
#define FULLSCREEN (myOLEDwidth * (myOLEDheight / 8))
#define HALFSCREEN ((myOLEDwidth / 2) * (myOLEDheight / 8))
SSD1306 myOLED(myOLEDwidth, myOLEDheight); // instantiate a OLED object

// =============== Function prototype ================
bool SetupTest(void);
void TestLoop(void);
void imprimeDadosElevador_e1(void);
void imprimeDadosElevador_e2(void);
void EndTest(void);

int posi(int posicao)
{
    if (posicao >= 0 && posicao <= 2171) return 1;
    if (posicao >= 6451 && posicao <= 6471) return 2;
    if (posicao >= 12311 && posicao <= 12331) return 3;
    if (posicao >= 21691 && posicao <= 21711) return 4;
    return 0;
}

// ======================= Main ===================
void *oled(void *arg)
{
    if (!SetupTest())
        return NULL;

    while (termina != 1)
    {
        //TestLoop();
        imprimeDadosElevador_e1();
        imprimeDadosElevador_e2();
    }
    
    EndTest();
    return NULL;

}

int main(){
    
    //calibraSensorAndar(1);
    //calibraSensorAndar(2);
    
    
    if (wiringPiSetup () == -1){
        return 1 ;
    }
    if (uartIniciar() == -1) {
        return -1; 
    }

    fila_1 = criaFila();
    fila_2 = criaFila();
   

    filaUnica(fila_1, 0);
    filaUnica(fila_2, 0);
    pthread_t threadBotoes, threadEncoder, threadOLED, threadPWM;

    pinosConfigura();

    pthread_create(&threadBotoes, NULL, lerBotoesThread, NULL);
    pthread_create(&threadEncoder, NULL, lerEncoderThread, NULL);
    pthread_create(&threadOLED, NULL, oled, NULL);
    pthread_create(&threadPWM, NULL, escreverPWMThread, NULL);

    delay(300);

    motorControle();

    return 0;
}

bool SetupTest()
{
    const uint16_t I2C_Speed = BCM2835_I2C_CLOCK_DIVIDER_626; //  bcm2835I2CClockDivider enum , see readme.
    const uint8_t I2C_Address = 0x27;
    bool I2C_debug = false;
    printf("OLED Test Begin\r\n");

    // Check if Bcm28235 lib installed and print version.
    if (!bcm2835_init())
    {
        printf("Error 1201: init bcm2835 library , Is it installed ?\r\n");
        return false;
    }

    // Turn on I2C bus (optionally it may already be on)
    if (!myOLED.OLED_I2C_ON())
    {
        printf("Error 1202: bcm2835_i2c_begin :Cannot start I2C, Running as root?\n");
        bcm2835_close(); // Close the library
        return false;
    }

    printf("SSD1306 library Version Number :: %u\r\n", myOLED.getLibVerNum());
    printf("bcm2835 library Version Number :: %u\r\n", bcm2835_version());
    bcm2835_delay(500);
    myOLED.OLEDbegin(I2C_Speed, I2C_Address, I2C_debug); // initialize the OLED
    myOLED.OLEDFillScreen(0xF0, 0);                      // splash screen bars, optional just for effect
    bcm2835_delay(1000);
    return true;
}

void EndTest()
{
    myOLED.OLEDPowerDown(); // Switch off display
    myOLED.OLED_I2C_OFF();  // Switch off I2C , optional may effect other programs & devices
    bcm2835_close();        // Close the library
    printf("OLED Test End\r\n");
}

void imprimeDadosElevador_e1(void)
{
    float temperatura = enviaOledTemperatura_e1();
    int posicao = posi(posicaoElevador1);
    uint8_t screenBuffer[HALFSCREEN];
    if (!myOLED.OLEDSetBufferPtr(myOLEDwidth, myOLEDheight, screenBuffer, sizeof(screenBuffer)))
        return;
    myOLED.OLEDclearBuffer();
    myOLED.setTextColor(WHITE);
    myOLED.setCursor(0, 1);
    myOLED.print("Andar:\n");
    switch (posicao)
    {
    case 1:
        myOLED.print("Têrreo\n");
        break;
    case 2:
        myOLED.print("Primeiro Andar\n");
        break;
    case 3:
        myOLED.print("Segundo Andar\n");
        break;
    case 4:
        myOLED.print("Terceiro Andar\n");
        break;

    default:
        break;
    }
    myOLED.print("Estado:\n");
    myOLED.print(estado_e1);
    myOLED.print("\nTemperatura:\n");
    myOLED.print(temperatura, 2);
    myOLED.OLEDupdate();
    delay(500);
    if (temperatura > 0.0f) temperaturaEnviar_e1(temperatura);
    delay(100);
}

void imprimeDadosElevador_e2(void)
{
    float temperatura = enviaOledTemperatura_e2();
    int posicao = posi(posicaoElevador2);
    uint8_t screenBuffer[HALFSCREEN];
    if (!myOLED.OLEDSetBufferPtr(myOLEDwidth, myOLEDheight, screenBuffer, sizeof(screenBuffer)))
        return;
    myOLED.OLEDclearBuffer();
    myOLED.setTextColor(WHITE);
    myOLED.setCursor(0, 1);
    myOLED.print("Andar:\n");
    switch (posicao)
    {
    case 1:
        myOLED.print("Têrreo\n");
        break;
    case 2:
        myOLED.print("Primeiro Andar\n");
        break;
    case 3:
        myOLED.print("Segundo Andar\n");
        break;
    case 4:
        myOLED.print("Terceiro Andar\n");
        break;

    default:
        break;
    }
    myOLED.print("Estado:\n");
    myOLED.print(estado_e1);
    myOLED.print("\nTemperatura:\n");
    myOLED.print(temperatura, 2);
    myOLED.OLEDupdate();
    delay(500);
    if (temperatura > 0.0f)
        temperaturaEnviar_e2(temperatura);
    delay(100);
}

void TestLoop()
{

    // Define a buffer to cover whole screen
    uint8_t screenBuffer[FULLSCREEN];
    if (!myOLED.OLEDSetBufferPtr(myOLEDwidth, myOLEDheight, screenBuffer, sizeof(screenBuffer)))
        return;
    myOLED.OLEDclearBuffer();
    myOLED.setTextColor(WHITE);
    myOLED.setCursor(10, 10);
    myOLED.print("Hello World.");
    myOLED.OLEDupdate();
    delay(5000);
}

// ============== EOF =========