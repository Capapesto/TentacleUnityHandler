// ==========================================
// CONTROLE SOMATIVO - SENSIBILIDADE AJUSTADA
// ==========================================

class EixoPotenciometro {
  private:
    byte pinoAnalogico;
    char cmdPos, cmdNeg, cmdParado;
    
    int valorReferencia;      // O último valor "estável" registrado
    unsigned long tempoUltimoMovimento;
    char estadoAtual;
    
    const int threshold = 7;         // Sensibilidade (se não mexer, aumente; se mexer sozinho, diminua)
    const unsigned long timeout = 300; // Tempo para parar (ms)

  public:
    EixoPotenciometro(byte pino, char p, char n, char parado) {
      pinoAnalogico = pino;
      cmdPos = p;
      cmdNeg = n;
      cmdParado = parado;
    }

    void iniciar() {
      pinMode(pinoAnalogico, INPUT);
      #ifdef ESP32
        analogReadResolution(10); // 0 a 1023 para bater com seu Python
      #endif
      delay(10);
      valorReferencia = analogRead(pinoAnalogico);
      estadoAtual = cmdParado;
      tempoUltimoMovimento = 0;
    }

    char lerEstado() {
      int valorLido = analogRead(pinoAnalogico);
      unsigned long agora = millis();
      int delta = valorLido - valorReferencia;

      // Se a mão moveu o pot mais que o ruído (threshold)
      if (abs(delta) >= threshold) {
        estadoAtual = (delta > 0) ? cmdPos : cmdNeg;
        
        // AQUI ESTÁ O SEGREDO: Atualiza a referência para o novo ponto
        valorReferencia = valorLido; 
        tempoUltimoMovimento = agora;
      } 
      // Se parou de girar por mais de 300ms, volta pro repouso
      else if (agora - tempoUltimoMovimento > timeout) {
        estadoAtual = cmdParado;
      }

      return estadoAtual;
    }
};

// Configuração (X=34, Y=35)
EixoPotenciometro eixoX(34, 'D', 'E', 'P'); 
EixoPotenciometro eixoY(35, 'C', 'B', 'P');

void setup() {
  Serial.begin(115200);
  eixoX.iniciar();
  eixoY.iniciar();
}

void loop() {
  char stX = eixoX.lerEstado();
  char stY = eixoY.lerEstado();

  // Pacote de 2 bytes para a Unity
  byte pacote[2] = {(byte)stX, (byte)stY};
  Serial.write(pacote, 2);

  // 50ms de intervalo (mesmo do seu script Python)
  delay(50);
}
