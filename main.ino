#include <Arduino.h>

// Configurações dos Pinos
const int pinoX = 34; 
const int pinoY = 35;

// Configurações do Joystick (Resistência)
const int CENTRO = 512;
const int MARGEM = 150; // Deadzone para não mover sozinho

void setup() {
    // Mesma velocidade do seu SerialController.cs
    Serial.begin(115200);

    #ifdef ESP32
        analogReadResolution(10); // 0 a 1023
        analogSetAttenuation(ADC_11db); // Leitura completa de 0 a 3.3V
    #endif

    pinMode(pinoX, INPUT);
    pinMode(pinoY, INPUT);
}

void loop() {
    int leituraX = analogRead(pinoX);
    int leituraY = analogRead(pinoY);

    char estadoX = 'P';
    char estadoY = 'P';

    // Lógica Eixo X (D, E, P)
    if (leituraX > (CENTRO + MARGEM)) {
        estadoX = 'D';
    } else if (leituraX < (CENTRO - MARGEM)) {
        estadoX = 'E';
    }

    // Lógica Eixo Y (C, B, P)
    if (leituraY > (CENTRO + MARGEM)) {
        estadoY = 'C';
    } else if (leituraY < (CENTRO - MARGEM)) {
        estadoY = 'B';
    }

    // Monta o pacote de 2 bytes exatos para o stream.Read(buffer, 0, 2)
    byte pacote[2] = {(byte)estadoX, (byte)estadoY};

    // Envia os bytes crus
    Serial.write(pacote, 2);
    
    // Garante que os dados saiam do buffer do chip para o Windows imediatamente
    Serial.flush();

    // Delay de 50ms para sincronizar com o processamento da Unity
    delay(50);
}
