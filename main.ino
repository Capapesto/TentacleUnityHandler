// ==========================================
// CONTROLE X/Y - LÓGICA DELTA PURA (JOYSTICK SOMATIVO)
// ==========================================

class Eixo {
  private:
    byte pino;
    char cmdPos, cmdNeg, cmdParado;
    
    int lastVal;              // Último valor estável para comparação
    unsigned long lastMove;   // Timer para o timeout de 300ms
    char estado;              // Estado atual (D, E, C, B ou P)
    
    // AJUSTE DE SENSIBILIDADE AQUI:
    const int threshold = 8;        // Se não mexer, diminua. Se tremer sozinho, aumente.
    const unsigned long timeout = 300; // Tempo de persistência do comando (ms)

  public:
    Eixo(byte p, char pos, char neg, char parado) {
      pino = p; cmdPos = pos; cmdNeg = neg; cmdParado = parado;
    }

    void setup() {
      pinMode(pino, INPUT);
      #ifdef ESP32
        analogReadResolution(10); // Escala 0-1023 (Igual ao Python)
      #endif
      lastVal = analogRead(pino);
      estado = cmdParado;
      lastMove = 0;
    }

    char atualizar() {
      int currentVal = analogRead(pino);
      unsigned long agora = millis();
      int diff = currentVal - lastVal;

      // 1. DETECÇÃO DE MOVIMENTO (A lógica do seu update_x / update_y)
      if (abs(diff) > threshold) {
        // Define a direção baseada no delta
        estado = (diff > 0) ? cmdPos : cmdNeg;
        
        // Atualiza referência e reseta o timer
        lastVal = currentVal;
        lastMove = agora;
      } 
      
      // 2. TIMEOUT DE PARADA (A lógica do seu processar_logica)
      // Se o tempo passou e não houve novo movimento, volta a ser 'P'
      if (agora - lastMove > timeout) {
        estado = cmdParado;
      }

      return estado;
    }
};

// --- INSTÂNCIAS ---
// Pino, Positivo, Negativo, Parado
Eixo eixoX(34, 'D', 'E', 'P'); 
Eixo eixoy(35, 'C', 'B', 'P');

void setup() {
  Serial.begin(115200);
  eixoX.setup();
  eixoy.setup();
}

void loop() {
  // Coleta os estados dos eixos
  char x = eixoX.atualizar();
  char y = eixoy.atualizar();

  // Envia EXATAMENTE 2 bytes via Serial Write para a Unity
  byte pacote[2] = {(byte)x, (byte)y};
  Serial.write(pacote, 2);

  // Delay de 50ms para manter a taxa de atualização do Python (20Hz)
  delay(50);
}
