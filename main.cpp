#include "I2C.h"
#include "ThisThread.h"
#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include <cstring>

#define tiempo_muestreo 1s
#define TMP102_ADDRESS 0x90

// Pines y puertos 
BufferedSerial serial(USBTX, USBRX);
I2C i2c(D14, D15);
Adafruit_SSD1306_I2c oled(i2c, D0);
AnalogIn ain(A0);

// Variables globales
float Vin = 0.0;
int ent = 0;
int dec = 0;
char men[40];

char comando[3] = {0x01, 0x60, 0xA0};
char data[2];

const char *mensaje_inicio = "Arranque del programa\n\r";

// Función para inicializar OLED
void inicializar_oled() {
    oled.begin();
    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.display();
    ThisThread::sleep_for(3000ms);
    oled.clearDisplay();
    oled.display();
}

// Función para leer el voltaje 
void leer_voltaje() {
    Vin = ain * 3.3;
    ent = int(Vin);
    dec = int((Vin - ent) * 10000);

    oled.clearDisplay();
    oled.display();
    sprintf(men, "El voltaje es:\n\r %01u.%04u volts\n\r", ent, dec);
    oled.setTextCursor(0, 2);
    oled.printf("%s", men);
    oled.display();
    serial.write(men, strlen(men));
}

// Función para leer el sensor 
void leer_sensor_tmp102() {
    comando[0] = 0; 
    i2c.write(TMP102_ADDRESS, comando, 1); 
    i2c.read(TMP102_ADDRESS, data, 2); 
    int16_t temp = (data[0] << 4) | (data[1] >> 4);
    float Temperatura = temp * 0.0625;
    ent = int(Temperatura);
    dec = int((Temperatura - ent) * 10000);

    oled.clearDisplay();
    oled.display();
    sprintf(men, "La Temperatura es:\n\r %01u.%04u Celsius\n\r", ent, dec);
    oled.setTextCursor(0, 2);
    oled.printf("%s", men);
    oled.display();
    serial.write(men, strlen(men));
}

// Función que ejecuta la lectura periódica en un hilo
void hilo_lectura() {
    while (true) {
        leer_voltaje();           
        leer_sensor_tmp102();    
        ThisThread::sleep_for(tiempo_muestreo);
    }
}

//tareas
Thread hilo (osPriorityNormal,4096, NULL, NULL);

int main() {
    // Inicializar OLED
    inicializar_oled();
    
    serial.write(mensaje_inicio, strlen(mensaje_inicio));

    hilo.start(hilo_lectura);

    while (true) {
        ThisThread::sleep_for(1000ms);
        
    }
}
