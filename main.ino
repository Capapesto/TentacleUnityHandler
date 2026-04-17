// ==========================================
// CONTROLE DE EIXOS X/Y COM POTENCIÔMETROS
// ==========================================

class EixoPotenciometro {
  private:
    byte pinoAnalogico;
    char cmdSobe;
    char cmdDesce;
    char cmdParado; 
    
    int valorAnterior;
    unsigned long ultimoTempoMovimento;
    char estadoAtual;
    
    const int threshold = 3; // Margem de ruído (igual ao > 3 do Python)
    const unsigned long tempoEstatico = 300; // 300ms para voltar ao repouso

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

      // Verifica se a variação foi maior que o ruído
      if (abs(valorAtual - valorAnterior) > threshold) {
        
        if (valorAtual > valorAnterior) {
          estadoAtual = cmdSobe;
        } else if (valorAtual < valorAnterior) {
          estadoAtual = cmdDesce;
        }

        valorAnterior = valorAtual;
        ultimoTempoMovimento = tempoAtual;
      }

      // Se passou o tempo estático sem novas variações, volta para "Parado"
      if (estadoAtual != cmdParado && (tempoAtual - ultimoTempoMovimento >= tempoEstatico)) {
        estadoAtual = cmdParado;
      }

      return estadoAtual;
    }
};

// ==========================================
// Instanciando os Eixos
// (Pino, Sobe/Direita, Desce/Esquerda, Parado)
// P = Parado, E = Esquerda, D = Direita, B = Baixo, C = Cima
// ==========================================

// Nota: D34 e D35 são pinos típicos de ESP32. Se for Arduino Uno, use A0 e A1.
EixoPotenciometro eixoX(34, 'D', 'E', 'P'); 
EixoPotenciometro eixoY(35, 'C', 'B', 'P');

void setup() {
  Serial.begin(115200); 
  
  eixoX.iniciar();
  eixoY.iniciar();
}

void loop() {
  // Coleta dos estados atuais
  char estadoX = eixoX.lerEstado();
  char estadoY = eixoY.lerEstado();

  // Monta o pacote no mesmo formato do Python (ex: "PP", "DP", "EC")
  char pacote[3];
  pacote[0] = estadoX;
  pacote[1] = estadoY;
  pacote[2] = '\0'; // Terminador de string (opcional, mas bom para debug)
  
  // Envia os "negócio pra usb"
  Serial.print(pacote);

  // Aguarda 50ms para replicar o self.root.after(50) do Tkinter
  delay(50); 
}
