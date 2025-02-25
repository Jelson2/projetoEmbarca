#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7
#define BUTTON_A 5
#define BUTTON_B 6
#define BUZZER_PIN 21
#define MIC_PIN 28

// Definição de pixel GRB
struct pixel_t {
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

// Inicializa a máquina PIO para controle da matriz de LEDs.
void npInit(uint pin) {
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0) {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true);
  }
  ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
  for (uint i = 0; i < LED_COUNT; ++i) {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

// Atribui uma cor RGB a um LED.
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}

// Limpa o buffer de pixels.
void npClear() {
  for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED(i, 0, 0, 0);
}

// Escreve os dados do buffer nos LEDs.
void npWrite() {
  for (uint i = 0; i < LED_COUNT; ++i) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(50);
}

int getIndex(int x, int y) {
  if (y % 2 == 0) {
      return 24-(y * 5 + x); // Linha par (esquerda para direita).
  } else {
      return 24-(y * 5 + (4 - x)); // Linha ímpar (direita para esquerda).
  }
}

void setupButtons() {
  gpio_init(BUTTON_A);
  gpio_set_dir(BUTTON_A, GPIO_IN);
  gpio_pull_up(BUTTON_A);
  
  gpio_init(BUTTON_B);
  gpio_set_dir(BUTTON_B, GPIO_IN);
  gpio_pull_up(BUTTON_B);
}

bool isButtonPressed(uint button) {
  return !gpio_get(button);
}

void buzzerInit() {
  gpio_init(BUZZER_PIN);
  gpio_set_dir(BUZZER_PIN, GPIO_OUT);
}

void playTone(uint frequency, uint duration_ms){
  uint32_t delay = 1000000 / frequency / 2;
  uint32_t cycles = duration_ms * frequency /1000;
    for (uint32_t i = 0; i < cycles; i++){
      gpio_put(BUZZER_PIN,1);
      sleep_us(delay);
      gpio_put(BUZZER_PIN, 0);
      sleep_us(delay);
    }
}

void micInit(){
  adc_gpio_init(MIC_PIN);
  adc_init();
  adc_select_input(2);
}

uint16_t readMicrophone(){
  return adc_read();
}


int main() {
  stdio_init_all();
  npInit(LED_PIN);
  npClear();
  setupButtons();
  buzzerInit();
  micInit();
  
  int patternIndex = 0;
  const int patternCount = 8;
  uint16_t soundThreshold = 2500; //define um limiar para o som
  uint16_t defaultTheshold = 2500;
  bool soundDetected = false;
  int thresholdStep = 200;

  while (true) {
    if (isButtonPressed(BUTTON_A)) {
    soundThreshold = defaultTheshold;
    
      playTone(1500, 200);
      sleep_ms(200);
    }
    if (isButtonPressed(BUTTON_B)) {
      soundThreshold -= thresholdStep;
      if(soundThreshold < 1000){
        soundThreshold = 1000;
      }
      playTone(1000, 200);
      sleep_ms(200);

    }

    uint16_t micValue = readMicrophone();
    if (micValue > soundThreshold){
      if (!soundDetected){
      patternIndex = (patternIndex + 1) % patternCount; //muda o padrão da animação
      soundDetected = true;
    
for (int patternIndex = 0; patternIndex < patternCount; patternIndex++){
    switch (patternIndex) {
      case 0:
        // Padrão 1
        int matriz[5][5][3] ={
            {{0, 0, 0}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}},
            {{0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
            {{0, 0, 0}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}},
            {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {25, 135, 223}},
            {{0, 0, 0}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}}
        };
        for (int linha = 0; linha < 5; linha++) {
          for (int coluna = 0; coluna < 5; coluna++) {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, matriz[coluna][linha][0], matriz[coluna][linha][1], matriz[coluna][linha][2]);
          }
        }
        break;
      case 1:
      int matriz2[5][5][3] ={
        {{0, 0, 0}, {0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}}
    };
  
    for(int linha = 0; linha < 5; linha++)
    {
      for(int coluna = 0; coluna < 5; coluna++)
      {
        int posicao = getIndex(linha, coluna);
        npSetLED(posicao, matriz2[coluna][linha][0], matriz2[coluna][linha][1], matriz2[coluna][linha][2]);
      }
    }
  
        break;
      case 2:
    
        // Padrão 3
        // Adicione seu código para o padrão 3 aqui
        int matriz3[5][5][3] = {
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
          {{25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {0, 0, 0}}
      };
      
      for(int linha = 0; linha < 5; linha++)
      {
        for(int coluna = 0; coluna < 5; coluna++)
        {
          int posicao = getIndex(linha, coluna);
          npSetLED(posicao, matriz3[coluna][linha][0], matriz3[coluna][linha][1], matriz3[coluna][linha][2]);
        }
      }
      
        break;
      case 3:
        // Padrão 4
        // Adicione seu código para o padrão 4 aqui
        int matriz4[5][5][3] ={
          {{0, 0, 0}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}},
          {{0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
          {{0, 0, 0}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {0, 0, 0}},
          {{0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
          {{0, 0, 0}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}}
      };
        
        for(int linha = 0; linha < 5; linha++)
        {
          for(int coluna = 0; coluna < 5; coluna++)
          {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, matriz4[coluna][linha][0], matriz4[coluna][linha][1], matriz4[coluna][linha][2]);
          }
        }
        
        
        break;
      case 4:
        // Padrão 5
        // Adicione seu código para o padrão 5 aqui
        int matriz5[5][5][3] = {
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {25, 135, 223}},
          {{25, 135, 223}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {25, 135, 223}},
          {{25, 135, 223}, {0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {25, 135, 223}},
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {25, 135, 223}, {25, 135, 223}},
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}
      };
        
        for(int linha = 0; linha < 5; linha++)
        {
          for(int coluna = 0; coluna < 5; coluna++)
          {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, matriz5[coluna][linha][0], matriz5[coluna][linha][1], matriz5[coluna][linha][2]);
          }
        }
        
        break;
      case 5:
        // Padrão 6
        // Adicione seu código para o padrão 6 aqui
        int matriz6[5][5][3] = {
          {{25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {0, 0, 0}},
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
          {{25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {0, 0, 0}}
      };
        
        for(int linha = 0; linha < 5; linha++)
        {
          for(int coluna = 0; coluna < 5; coluna++)
          {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, matriz6[coluna][linha][0], matriz6[coluna][linha][1], matriz6[coluna][linha][2]);
          }
        }
        
        break;
        case 6:
        // Padrão 6
        // Adicione seu código para o padrão 6 aqui
        int matriz7[5][5][3] = {
          {{0, 0, 0}, {0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}},
          {{0, 0, 0}, {0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}},
          {{0, 0, 0}, {0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}},
          {{0, 0, 0}, {0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}},
          {{0, 0, 0}, {0, 0, 0}, {25, 135, 223}, {0, 0, 0}, {0, 0, 0}}
      };
        
        for(int linha = 0; linha < 5; linha++)
        {
          for(int coluna = 0; coluna < 5; coluna++)
          {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, matriz7[coluna][linha][0], matriz7[coluna][linha][1], matriz7[coluna][linha][2]);
          }
        }
          
        
        break;
        case 7:
      
        int matriz8[5][5][3] ={
          {{25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}},
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {25, 135, 223}},
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {25, 135, 223}},
          {{25, 135, 223}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {25, 135, 223}},
          {{25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}, {25, 135, 223}}
      };
        
        for(int linha = 0; linha < 5; linha++)
        {
          for(int coluna = 0; coluna < 5; coluna++)
          {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, matriz8[coluna][linha][0], matriz8[coluna][linha][1], matriz8[coluna][linha][2]);
          }
        }
      break;
    }
    npWrite();
    sleep_ms(500); 
  }
    npClear();
    npWrite();
    soundDetected = false;
}else {
  if(soundDetected){
    npClear();
    npWrite();
    soundDetected = false;

  }

  }
  sleep_ms(100); 
} 
  }
}

