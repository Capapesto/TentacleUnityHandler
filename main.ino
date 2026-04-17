
class EixoPotenciometro {
  private:
    byte pinoAnalogico;
    char cmdSobe;
    char cmdDesce;
    char cmdParado; 
    
    int valorAnterior;
    unsigned long ultimoTempoMovimento;
    char estadoAtual;
    
    const int threshold = 4; // Margem de ruído
    const unsigned long tempoEstatico = 300; // 300ms parado

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
      ultimoTempoMovimento = millis();
      estadoAtual = cmdParado;
    }

    
    char lerEstado() {
      int valorAtual = analogRead(pinoAnalogico);
      unsigned long tempoAtual = millis();

      // se variação > ruido
      if (abs(valorAtual - valorAnterior) > threshold) {
        
        if (valorAtual > valorAnterior) {
          estadoAtual = cmdSobe;
        } else if (valorAtual < valorAnterior) {
          estadoAtual = cmdDesce;
        }

        valorAnterior = valorAtual;
        ultimoTempoMovimento = tempoAtual;
      }

      // Se passou o tempo sem mudanças, "Parado"
      if (estadoAtual != cmdParado && (tempoAtual - ultimoTempoMovimento >= tempoEstatico)) {
        estadoAtual = cmdParado;
      }

      return estadoAtual;
    }
};

// ==========================================
// Eixos
// (Pino, Sobe, Desce, Parado)
// P = Parado, E = Esquerda, D = Direita, B = Baixo, C = Cima
// ==========================================
EixoPotenciometro eixoX(D34, 'D', 'E', 'P'); 
EixoPotenciometro eixoY(D35, 'C', 'B', 'P');

//67
// anti flood 
char ultimoEstadoX = '-';
char ultimoEstadoY = '-';

void setup() {
  
  Serial.begin(115200); 
  
  eixoX.iniciar();
  eixoY.iniciar();
}

void loop() {
  // coleta de estados
  char estadoX = eixoX.lerEstado();
  char estadoY = eixoY.lerEstado();

  //comparação do tultimo estado
  if (estadoX != ultimoEstadoX || estadoY != ultimoEstadoY) {
    
    
    byte pacote[2] = {estadoX, estadoY};
    
    // envia os negocio pra usb
    Serial.write(pacote, 2);

    //atualiza a memora
    ultimoEstadoX = estadoX;
    ultimoEstadoY = estadoY;
  }
  
  delay(10); 
}
