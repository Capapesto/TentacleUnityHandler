// ==========================================
// CONTROLE SOMATIVO COM FILTRO DE RUÍDO
// ==========================================

class EixoPotenciometro {
  private:
    byte pinoAnalogico;
    char cmdSobe, cmdDesce, cmdParado;
    
    int valorAnterior;
    unsigned long ultimoTempoMovimento;
    char estadoAtual;
    
    // Aumentamos o threshold. No ESP32, 3 é muito pouco (causa movimento infinito)
    // 30 é um valor mais seguro para ignorar o "tremido" do sensor.
    const int threshold = 30; 
    const unsigned long tempoPersistencia = 250; // Tempo que o comando dura após girar

  public:
    EixoPotenciometro(byte pino, char sobe, char desce, char parado) {
      pinoAnalogico = pino;
      cmdSobe = sobe;
      cmdDesce = desce;
      cmdParado = parado;
    }

    void iniciar() {
      pinMode(pinoAnalogico, INPUT);
      // Forçamos o ESP32 a ler de 0 a 1023 (igual ao seu código Python)
      #ifdef ESP32
        analogReadResolution(10); 
      #endif
      valorAnterior = analogRead(pinoAnalogico);
      estadoAtual = cmdParado;
      ultimoTempoMovimento = 0;
    }

    char lerEstado() {
      int valorAtual = analogRead(pinoAnalogico);
      unsigned long agora = millis();
      int diferenca = valorAtual - valorAnterior;

      // Se a variação for real (maior que o ruído)
      if (abs(diferenca) > threshold) {
        estadoAtual = (diferenca > 0) ? cmdSobe : cmdDesce;
        valorAnterior = valorAtual;
        ultimoTempoMovimento = agora;
      } 
      // Se parou de girar, espera o tempo de persistência para mandar 'P'
      else if (agora - ultimoTempoMovimento > tempoPersistencia) {
        estadoAtual = cmdParado;
      }

      return estadoAtual;
    }
};

// Instanciação (X=34, Y=35)
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

  // Envio binário exato para Unity
  byte pacote[2] = {(byte)stX, (byte)stY};
  Serial.write(pacote, 2);

  // 50ms de delay para não engasgar o buffer da Unity
  delay(50);
}
