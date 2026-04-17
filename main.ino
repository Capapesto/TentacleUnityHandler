// ==========================================
// CONTROLE SOMATIVO - VERSÃO ESTÁVEL (ESP32)
// ==========================================

class EixoPotenciometro {
  private:
    byte pinoAnalogico;
    char cmdSobe, cmdDesce, cmdParado;
    
    int valorAnterior;
    unsigned long ultimoTempoMovimento;
    char estadoAtual;
    
    // Threshold maior para ignorar o "jitter" do ESP32
    const int threshold = 25; 
    const unsigned long tempoPersistencia = 200; 

    // Função interna para ler o valor sem ruído
    int lerSuave() {
      long soma = 0;
      for(int i = 0; i < 10; i++) {
        soma += analogRead(pinoAnalogico);
      }
      return soma / 10;
    }

  public:
    EixoPotenciometro(byte pino, char sobe, char desce, char parado) {
      pinoAnalogico = pino;
      cmdSobe = sobe;
      cmdDesce = desce;
      cmdParado = parado;
    }

    void iniciar() {
      // Configurações específicas para estabilizar o ADC do ESP32
      #ifdef ESP32
        analogReadResolution(10); // 0 a 1023
        analogSetAttenuation(ADC_11db); // Permite ler até 3.3V perfeitamente
      #endif
      
      pinMode(pinoAnalogico, INPUT);
      delay(10);
      valorAnterior = lerSuave();
      estadoAtual = cmdParado;
      ultimoTempoMovimento = 0;
    }

    char lerEstado() {
      int valorAtual = lerSuave();
      unsigned long agora = millis();
      int diferenca = valorAtual - valorAnterior;

      // DETECÇÃO DE MOVIMENTO REAL
      if (abs(diferenca) > threshold) {
        estadoAtual = (diferenca > 0) ? cmdSobe : cmdDesce;
        valorAnterior = valorAtual; // Só atualiza se moveu de verdade
        ultimoTempoMovimento = agora;
      } 
      // VOLTA PARA PARADO
      else if (agora - ultimoTempoMovimento > tempoPersistencia) {
        estadoAtual = cmdParado;
      }

      return estadoAtual;
    }
};

// Configuração dos Eixos
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

  // Envio binário de 2 bytes para a Unity
  byte pacote[2] = {(byte)stX, (byte)stY};
  Serial.write(pacote, 2);

  delay(40); // Frequência de 25Hz (Estável para Unity)
}
