// ==========================================
// CONTROLE DE EIXOS X/Y - LÓGICA DELTA (PYTHON)
// ==========================================

class EixoPotenciometro {
  private:
    byte pinoAnalogico;
    char cmdPositivo; // D ou C
    char cmdNegativo; // E ou B
    char cmdParado;   // P
    
    int lastVal;
    unsigned long lastMoveTime;
    char estadoAtual;
    
    // Seguindo seus parâmetros do Python:
    const int threshold = 5;         // Aumentei de 3 para 5 por causa de ruído elétrico
    const unsigned long timeout = 300; // 300ms para voltar a 'P'

  public:
    EixoPotenciometro(byte pino, char pos, char neg, char parado) {
      pinoAnalogico = pino;
      cmdPositivo = pos;
      cmdNegativo = neg;
      cmdParado = parado;
    }

    void iniciar() {
      pinMode(pinoAnalogico, INPUT);
      #ifdef ESP32
        analogReadResolution(10); // Normaliza para 0-1023 igual ao Python
      #endif
      lastVal = analogRead(pinoAnalogico);
      lastMoveTime = millis();
      estadoAtual = cmdParado;
    }

    char lerEstado() {
      int currentVal = analogRead(pinoAnalogico);
      unsigned long agora = millis();

      // EQUIVALENTE AO: if abs(v - self.last_val_x) > 3
      if (abs(currentVal - lastVal) > threshold) {
        // EQUIVALENTE AO: 'D' if v > self.last_val_x else 'E'
        estadoAtual = (currentVal > lastVal) ? cmdPositivo : cmdNegativo;
        
        lastVal = currentVal;   // self.last_val_x = v
        lastMoveTime = agora;   // self.last_move_x = time.time()
      }

      // EQUIVALENTE AO: if agora - self.last_move_x > 0.3
      if (agora - lastMoveTime > timeout) {
        estadoAtual = cmdParado;
      }

      return estadoAtual;
    }
};

// ==========================================
// SETUP E LOOP PRINCIPAL
// ==========================================

EixoPotenciometro eixoX(34, 'D', 'E', 'P'); 
EixoPotenciometro eixoY(35, 'C', 'B', 'P');

void setup() {
  Serial.begin(115200);
  eixoX.iniciar();
  eixoY.iniciar();
}

void loop() {
  // Coleta os estados
  char stX = eixoX.lerEstado();
  char stY = eixoY.lerEstado();

  // MONTA O PACOTE (Exatamente 2 bytes)
  byte pacote[2] = {(byte)stX, (byte)stY};

  // ENVIO SERIAL WRITE (Para Unity)
  Serial.write(pacote, 2);

  // EQUIVALENTE AO: self.root.after(50, ...)
  delay(50); 
}
