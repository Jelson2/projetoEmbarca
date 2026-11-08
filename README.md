# 🔇 Alerta de Silêncio com Matriz de LEDs

Projeto de sistemas embarcados em **C** para o **Raspberry Pi Pico (RP2040)**. Um microfone monitora o nível de som do ambiente. Quando o barulho passa de um limite, a matriz de LEDs 5x5 exibe a palavra **SILÊNCIO**, letra por letra, e os botões permitem ajustar a sensibilidade.

## ✨ Como funciona

1. O microfone é lido continuamente pelo conversor analógico-digital (ADC).
2. Se o valor passa do **limiar de som** (padrão: `2500`), a matriz mostra uma letra de cada vez, **S-I-L-E-N-C-I-O**, a cada 500 ms.
3. Ao terminar, a matriz apaga e o sistema volta a ouvir.
4. Os botões ajustam a sensibilidade:

| Botão | Ação |
|-------|------|
| **A** | Volta o limiar para o padrão (2500) e toca um beep agudo |
| **B** | Diminui o limiar em 200 (mínimo 1000), deixando mais sensível, e toca um beep grave |

## 🧰 Hardware

| Componente | Pino (GPIO) |
|------------|-------------|
| Matriz de LEDs 5x5 (WS2812B) | 7 |
| Botão A | 5 |
| Botão B | 6 |
| Buzzer | 21 |
| Microfone (ADC) | 28 |

Os pinos correspondem à placa **BitDogLab**. Em outra placa, ajuste os `#define` no início do `neopixel_pio.c`.

## 📁 Estrutura

```
├── neopixel_pio.c      # Código principal (LEDs, botões, buzzer e microfone)
├── ws2818b.pio         # Programa PIO que gera o sinal dos LEDs WS2812B
├── CMakeLists.txt      # Configuração de compilação
└── pico_sdk_import.cmake
```

## 🚀 Como compilar e gravar

**Requisitos:** [Pico SDK](https://github.com/raspberrypi/pico-sdk) 1.5.1, CMake e toolchain ARM. O jeito mais simples é usar a extensão **Raspberry Pi Pico** no VS Code.

1. Clone o repositório:
   ```bash
   git clone https://github.com/Jelson2/projetoEmabarca.git
   ```
2. Abra a pasta no VS Code e compile o projeto (**Compile**).
3. Segure o botão **BOOTSEL** da placa e conecte o cabo USB.
4. Copie o arquivo `neopixel_pio.uf2`, gerado na pasta `build`, para o disco que aparecer.

A placa reinicia sozinha e o programa começa a rodar.

## ⚙️ Personalização

| O que mudar | Onde |
|-------------|------|
| Sensibilidade inicial | `soundThreshold` e `defaultTheshold` |
| Passo do botão B | `thresholdStep` |
| Cor das letras | valores `{25, 135, 223}` (RGB) nas matrizes |
| Tempo de cada letra | `sleep_ms(500)` dentro do `switch` |
| Número de LEDs e pino | `LED_COUNT` e `LED_PIN` |

## 🧠 Conceitos usados

- **PIO** do RP2040 para falar o protocolo dos LEDs WS2812B
- Leitura analógica com **ADC**
- Entradas digitais com *pull-up* (botões)
- Som com onda quadrada no buzzer, sem PWM

## 👤 Autor

Feito por [Jelson](https://github.com/Jelson2).
