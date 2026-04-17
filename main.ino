// ==========================================
// CONTROLE ABSOLUTO POR RESISTÊNCIA (JOYSTICK)
// ==========================================

class Eixo {
  private:
    byte pino;
    char cmdPos, cmdNeg, cmdParado;
    
    // Configurações da Deadzone (Baseado em 1023)
    // No meio (perto de 512) fica Parado. Nas pontas ele move.
    const int centro = 512;
    const int margem = 150; // Ajuste para sensibilidade (Deadzone)

  public:
    Eixo(byte p, char pos, char neg, char parado) {
      pino = p; cmdPos = pos; cmdNeg = neg; cmdParado = parado;
    }

    void setup() {
      pinMode(pino, INPUT);
      #ifdef ESP32
        analogReadResolution(10); // Escala 0-1023 igual ao Python
      #endif
    }

    char lerEstado() {
      int valor = analogRead(pino);

      // Lógica de carga resistiva absoluta
      if (valor > (centro + margem)) {
        return cmdPos; // Ex: Direita / Cima
      } 
      else if (valor < (centro - margem)) {
        return cmdNeg; // Ex: Esquerda / Baixo
      } 
      else {
        return cmdParado; // Centro (Deadzone)
      }
    }
};

// --- INSTÂNCIAS ---
Eixo eixoX(34, 'D', 'E', 'P'); 
Eixo eixoY(35, 'C', 'B', 'P');

void setup() {
  Serial.begin(115200);
  eixoX.setup();
  eixoY.setup();
}

void loop() {
  // Leitura direta da carga resistiva convertida pelo ADC
  char x = eixoX.lerEstado();
  char y = eixoY.lerEstado();

  // Envio binário para Unity
  byte pacote[2] = {(byte)x, (byte)y};
  Serial.write(pacote, 2);

  // Delay fixo de 50ms para sincronia
  delay(50);
}
