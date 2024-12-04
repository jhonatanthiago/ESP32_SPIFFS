#include <FS.h>       // Biblioteca para manipulação de sistemas de arquivos
#include <SPIFFS.h>   // Biblioteca SPIFFS para ESP32

const int ledPin = 2;      // Pino onde o LED está conectado (D2)
const int buttonPin = 12;  // Pino onde o botão está conectado (D12)

bool ledState = false;     // Variável para armazenar o estado atual do LED
bool buttonState = false;  // Estado atual do botão
bool lastButtonState = false;  // Último estado do botão (para detectar mudanças)

// Nome do arquivo no SPIFFS
const char* filename = "/ledState.txt";

// Função para salvar o estado do LED no SPIFFS
void saveLedState(bool state) {
  File file = SPIFFS.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Falha ao abrir o arquivo para escrita!");
    return;
  }
  file.println(state ? "1" : "0");
  file.close();
  Serial.println("Estado do LED salvo!");
}

// Função para carregar o estado do LED do SPIFFS
bool loadLedState() {
  if (!SPIFFS.exists(filename)) {
    Serial.println("Arquivo não encontrado. Estado padrão: desligado.");
    return false; // Retorna o estado desligado como padrão
  }

  File file = SPIFFS.open(filename, FILE_READ);
  if (!file) {
    Serial.println("Falha ao abrir o arquivo para leitura!");
    return false; // Retorna o estado desligado como padrão
  }

  String state = file.readStringUntil('\n');
  file.close();

  Serial.println("Estado do LED carregado: " + state);
  return state.toInt() == 1;
}

void setup() {
  // Inicializa comunicação serial
  Serial.begin(115200);

  // Configuração dos pinos
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Inicializa o SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Falha ao inicializar o SPIFFS!");
    return;
  }

  // Carrega o estado do LED do arquivo e ajusta o LED
  ledState = loadLedState();
  digitalWrite(ledPin, ledState ? HIGH : LOW);
}

void loop() {
  // Leitura do estado do botão
  buttonState = digitalRead(buttonPin);

  // Detecta a transição do botão (pressionado -> solto)
  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(50); // Debounce
    ledState = !ledState;                 // Alterna o estado do LED
    digitalWrite(ledPin, ledState ? HIGH : LOW); // Atualiza o LED
    saveLedState(ledState);              // Salva o novo estado no SPIFFS
  }

  lastButtonState = buttonState; // Atualiza o último estado do botão
}
