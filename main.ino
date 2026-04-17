class Eixo {
  private:
    byte pino;
    char cmdPos, cmdNeg, cmdParado;
    const int centro = 512;
    const int margem = 150; 

  public:
    Eixo(byte p, char pos, char neg, char parado) : 
      pino(p), cmdPos(pos), cmdNeg(neg), cmdParado(parado) {}

    void setup() {
      pinMode(pino, INPUT);
      #ifdef ESP32
        analogReadResolution(10); 
      #endif
    }

    char lerEstado() {
      int valor = analogRead(pino);
      if (valor > (centro + margem)) return cmdPos;
      if (valor < (centro - margem)) return cmdNeg;
      return cmdParado;
    }
};

Eixo eixoX(34, 'D', 'E', 'P'); 
Eixo eixoY(35, 'C', 'B', 'P');

void setup() {
  // 115200 é alto para o .NET Framework instável. 
  // Se não funcionar, tente baixar para 9600 em ambos os lados.
  Serial.begin(115200);
  
  eixoX.setup();
  eixoY.setup();
  
  // Aguarda a porta estabilizar
  while (!Serial) { ; } 
}

void loop() {
  char x = eixoX.lerEstado();
  char y = eixoY.lerEstado();

  // Envio binário
  byte pacote[2] = {(byte)x, (byte)y};
  
  Serial.write(pacote, 2);
  
  // Força o envio imediato do buffer para o Windows
  Serial.flush(); 

  delay(50);
}
