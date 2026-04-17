// ==========================================
// CONTROLE DE EIXOS X/Y - MODO JOYSTICK
// ==========================================

class EixoPotenciometro {
  private:
    byte pinoAnalogico;
    char cmdPositivo; // Direita ou Cima
    char cmdNegativo; // Esquerda ou Baixo
    char cmdParado; 
    
    // Ajuste esses valores se o seu potenciômetro for sensível demais
    const int limiteInferior = 400; 
    const int limiteSuperior = 600; 

  public:
    EixoPotenciometro(byte pino, char positivo, char negativo, char parado) {
      pinoAnalogico = pino;
      cmdPositivo = positivo;
      cmdNegativo = negativo;
      cmdParado = parado;
    }

    void iniciar() {
      pinMode(pinoAnalogico, INPUT);
    }

    char lerEstado() {
      int valorAtual = analogRead(pinoAnalogico);

      // Lógica de posição absoluta (Estilo Joystick)
      if (valorAtual > limiteSuperior) {
        return cmdPositivo;
      } 
      else if (valorAtual < limiteInferior) {
        return cmdNegativo;
      } 
      else {
        return cmdParado;
      }
    }
};

// ==========================================
// Configuração dos Eixos
// ==========================================

// Pinos 34 e 35 (ESP32). Se for Arduino Uno, mude para A0 e A1
EixoPotenciometro eixoX(34, 'D', 'E', 'P'); // Direita, Esquerda, Parado
EixoPotenciometro eixoY(35, 'C', 'B', 'P'); // Cima, Baixo, Parado

void setup() {
  // A Unity geralmente trabalha bem com 9600 ou 115200. 
  // Garanta que no C# esteja igual!
  Serial.begin(115200); 
  
  eixoX.iniciar();
  eixoY.iniciar();
}

void loop() {
  // Pega o estado baseado na POSIÇÃO do potenciômetro
  char estadoX = eixoX.lerEstado();
  char estadoY = eixoY.lerEstado();

  // Cria o pacote de 2 bytes
  byte pacote[2] = {(byte)estadoX, (byte)estadoY};
  
  // Envia para a Unity
  Serial.write(pacote, 2);

  // Um delay curto para não sobrecarregar o buffer da Unity
  // 20ms = 50 FPS de atualização de dados
  delay(20); 
}
