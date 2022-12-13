const int pino_microfone = A0; // Pino onde o potenciometro está conectado
int leitura = 0; // Variável para armazenar o valor lido pelo ADC

#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11

int corAtual = 0;
char cores[7][3] = {{255,255,255},  // Branco
                    {255,255,  0},  // Amarelo
                    {255,  0,  0},  // Vermelho
                    {255,  0,255},  // Roxo
                    {  0,255,255},  // Ciano
                    {  0,255,  0},  // Verde
                    {  0,  0,255}}; // Azul

float amplitude = 0.0; // Armazenará o valor de pico a pico da onda
unsigned int valor_max = 0; // Armazenará o valor máximo lido pelo sensor
unsigned int valor_min = 1024; // Armazenará o valor mínimo lido pelo sensor
float valor_limite = 4.5; // Valor mínimo para considerar uma palma [0.0, 5.0]
unsigned long inicio_amostragem; // Armazena o instante que começou a amostragem
const unsigned long tempo_amostragem = 250; // Tempo de amostragem em ms
bool ligado = true; // Define se a luz está acesa ou apagada

int palmas = 0; // Contador de palmas
unsigned long tempo_palmas; // Armazena o instante que começou a amostragem
const unsigned long tempo_palmas_max = 500; // Período de amostragem de palmas

void setup() {
  // Inicia e configura a Serial
  Serial.begin(9600); // 9600bps

  pinMode(pino_microfone, INPUT);
  pinMode(RED_PIN,OUTPUT);
  pinMode(GREEN_PIN,OUTPUT);
  pinMode(BLUE_PIN,OUTPUT);
  analogWrite(RED_PIN, 255);
  analogWrite(BLUE_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  
  inicio_amostragem = millis(); // Reseta o tempo inicial da amostragem
}

void loop() {
  // Coleta dados durante o tempo de amostragem
  if (millis() - inicio_amostragem < tempo_amostragem) {

    // Lê o valor de tensão no pino do microfone
    leitura = analogRead(pino_microfone);

    if (leitura > valor_max) {
      valor_max = leitura;
    }
    else if (leitura < valor_min) {
      valor_min = leitura; // Atualiza valor mínimo
    }
  }
  else { // Processa os dados
    inicio_amostragem = millis(); // reseta o tempo inicial da amostragem

    // Calcula, converte e imprime o valor de pico-a-pico em tensão elétrica
    amplitude = valor_max - valor_min;
    amplitude = (amplitude * 5.0) / 1023.0; // Converte para tensão (V)

    Serial.println(amplitude);

    // Reseta os valores máximos e mínimos
    valor_max = 0;
    valor_min = 1024;

    // Se o valor de amplitude superar o valor limite, inverte o estado da carga
    if (amplitude > valor_limite) {
      if (palmas == 0){ // se for a primeira palma daquele período
        tempo_palmas = millis(); // reinicia a amostragem de palmas
      }

      palmas++;
    }
  }

  // Se o tempo de amostragem de palmas é atingido
  if (millis() - tempo_palmas > tempo_palmas_max) {

    tempo_palmas = millis(); // Reseta tempo inicial da amostragem de palmas
    if(palmas == 2) {
      if(ligado) {
        analogWrite(RED_PIN,0);
        analogWrite(GREEN_PIN,0);
        analogWrite(BLUE_PIN,0);
        ligado = false;
      }
      else {
        analogWrite(RED_PIN, cores[corAtual][0]);
        analogWrite(GREEN_PIN, cores[corAtual][1]);
        analogWrite(BLUE_PIN, cores[corAtual][2]);
        ligado = true;
      }
    }

    if (palmas == 1 && ligado)
    {
      corAtual++;
      if (corAtual > 6) corAtual = 0;
      
      analogWrite(RED_PIN, cores[corAtual][0]);
      analogWrite(GREEN_PIN, cores[corAtual][1]);
      analogWrite(BLUE_PIN, cores[corAtual][2]);
    }

    if(palmas > 0) palmas = 0;
  }
}
