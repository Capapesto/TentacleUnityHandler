class EixoPotenciometro {

  private:
    byte pinoAnalogico;
    char cmdSobe;
    char cmdDesce;
    char cmdParado; 
    
    int valorAnterior;
    unsigned long ultimoTempoMovimento;
    char estadoAtual;
    
    const int threshold = 4; 
    const unsigned long tempoEstatico = 300;

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

      // Detecta movimento
      if (abs(valorAtual - valorAnterior) > threshold) {

        if (valorAtual > valorAnterior) {
          estadoAtual = cmdSobe;
        } else {
          estadoAtual = cmdDesce;
        }

        valorAnterior = valorAtual;
        ultimoTempoMovimento = tempoAtual;
      }

      // Timeout → parado
      if (tempoAtual - ultimoTempoMovimento > tempoEstatico) {
        estadoAtual = cmdParado;
      }

      return estadoAtual;
    }
};


// ==========================================
// Eixos
// ==========================================
EixoPotenciometro eixoX(D34, 'D', 'E', 'P'); 
EixoPotenciometro eixoY(D35, 'C', 'B', 'P');

// Timer de envio (igual ao after do Python)
unsigned long ultimoEnvio = 0;
const int intervaloEnvio = 50; // ms

void setup() {
  Serial.begin(115200);

  eixoX.iniciar();
  eixoY.iniciar();
}

void loop() {

  unsigned long agora = millis();

  // equivalente ao root.after(50)
  if (agora - ultimoEnvio >= intervaloEnvio) {

    char estadoX = eixoX.lerEstado();
    char estadoY = eixoY.lerEstado();

    byte pacote[2] = {estadoX, estadoY};

    Serial.write(pacote, 2);

    ultimoEnvio = agora;
  }
}
