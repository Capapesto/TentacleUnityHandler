// ==========================================
// CONTROLE DE EIXOS X/Y - LÓGICA SOMATIVA (DELTA)
// ==========================================

class EixoPotenciometro {
  private:
    byte pinoAnalogico;
    char cmdSobe;    // Direita / Cima
    char cmdDesce;   // Esquerda / Baixo
    char cmdParado; 
    
    int valorAnterior;
    unsigned long ultimoTempoMovimento;
    char estadoAtual;
    
    const int threshold = 3;           // Sensibilidade do movimento
    const unsigned long tempoMovimento = 300; // Quanto tempo o sinal persiste (ms)

  public:
    EixoPotenciometro(byte pino, char comandoSobe, char comandoDesce, char comandoParado) {
      pinoAnalogico = pino;
      cmdSobe = comandoSobe;
      cmdDesce = comandoDesce;
      cmdParado = comandoParado;
    }

    void iniciar() {
      pinMode(pinoAnalogico, INPUT);
      valorAnterior = analogRead(pinoAnalogico);
      ultimoTempoMovimento = 0;
      estadoAtual = cmdParado;
    }

    char lerEstado() {
      int valorAtual = analogRead(pinoAnalogico);
      unsigned long agora = millis();

      // LÓGICA SOMATIVA: Detecta a mudança (Delta)
      // Se a diferença for maior que o threshold, define a direção
      if (abs(valorAtual - valorAnterior) > threshold) {
        if (valorAtual > valorAnterior) {
          estadoAtual = cmdSobe;
        } else {
          estadoAtual = cmdDesce;
        }
        
        valorAnterior = valorAtual;
        ultimoTempoMovimento = agora; // Reseta o cronômetro de persistência
      }

      // Se parou de girar e passou o tempo de persistência, volta a ficar parado
      if (agora - ultimoTempoMovimento > tempoMovimento) {
        estadoAtual = cmdParado;
      }

      return estadoAtual;
    }
};

// ==========================================
// Instanciando os Eixos
// ==========================================
// Pot 34 -> Eixo X (D=Direita, E=Esquerda)
// Pot 35 -> Eixo Y (C=Cima, B=Baixo)
EixoPotenciometro eixoX(34, 'D', 'E', 'P'); 
EixoPotenciometro eixoY(35, 'C', 'B', 'P');

void setup() {
  Serial.begin(115200); 
  eixoX.iniciar();
  eixoY.iniciar();
}

void loop() {
  // Coleta os estados baseados no movimento (Delta)
  char stX = eixoX.lerEstado();
  char stY = eixoY.lerEstado();

  // Pacote de 2 bytes para a Unity
  byte pacote[2] = {(byte)stX, (byte)stY};
  
  // Envio simultâneo via Serial Write
  Serial.write(pacote, 2);

  // Intervalo de 50ms para bater com a lógica do seu Python original
  delay(50); 
}
