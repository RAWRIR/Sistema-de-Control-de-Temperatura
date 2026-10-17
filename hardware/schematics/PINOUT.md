# Mapa de Pines y Guía de Conexión Física (PINOUT)

Proyecto: **Sistema de Control de Temperatura con ESP32 y Actuador Fan DC**

---

## 1. Tabla de Interconexión Eléctrica

| Componente Origen | Terminal / Pin | Color Cable en Montaje | Componente Destino | Terminal / Pin | Tipo de Señal / Función |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Fuente 5V Externa** | Positivo `+5V` | Rojo | **Diodo D1** | Ánodo | Alimentación de la etapa de potencia. |
| **Diodo D1** | Cátodo (Banda) | Serie directa | **Diodo D2** | Ánodo | Caída de tensión etapa 1 (~0.7V drop). |
| **Diodo D2** | Cátodo (Banda) | Serie directa | **Diodo D3** | Ánodo | Caída de tensión etapa 2 (~0.7V drop). |
| **Diodo D3** | Cátodo (Banda) | Serie directa | **Diodo D4** | Ánodo | Caída de tensión etapa 3 (~0.7V drop). |
| **Diodo D4** | Cátodo (Banda) | Rojo | **Motor DC** | Borne Positivo (+) | Tensión reducida resultante ($\approx 2.2\,\text{V} - 2.5\,\text{V}$). |
| **Diodo Flyback D5** | Cátodo (Banda) | Directo en borne | **Motor DC** | Borne Positivo (+) | Antiparalelo contra picos inductivos. |
| **Diodo Flyback D5** | Ánodo | Directo en borne | **Motor DC** | Borne Negativo (-) | Conducción de descarga de bobina. |
| **Capacitor C1 (Lenteja)**| Terminales 1 y 2 | Directo en bornes | **Motor DC** | Bornes (+) y (-) | Supresión de ruidos y chispas de escobillas (100nF). |
| **Motor DC** | Borne Negativo (-) | Gris | **Transistor Q1** | Colector (C) | Retorno conmutado por lado bajo (*Low-Side*). |
| **Transistor Q1 (TO-18)** | Emisor (E) | Negro | **GND Común** | GND Fuente / ESP32 | Cierra la corriente del motor a tierra. |
| **Transistor Q1 (TO-18)** | Base (B) | Azul | **Resistor R1** | Terminal 2 | Control de saturación del transistor. |
| **Resistor R1 (1 kΩ)** | Terminal 1 | Azul | **ESP32 DevKit** | Pin `GPIO 19` (D19) | Señal de salida digital / PWM desde ESP32. |
| **ESP32 DevKit** | Pin `3V3` (3.3V Out) | Rojo | **Sensor DHT11** | Pin `VCC` (+) | Alimentación regulada para el sensor ambiental. |
| **ESP32 DevKit** | Pin `GPIO 4` (D4) | Naranja | **Sensor DHT11** | Pin `DATA` (S) | Protocolo 1-Wire de lectura de temperatura y humedad. |
| **ESP32 DevKit** | Pin `GND` (Derecho) | Negro | **Sensor DHT11** | Pin `GND` (-) | Referencia negativa del sensor. |
| **Fuente 5V Externa** | Negativo `GND` | Negro | **ESP32 DevKit** | Pin `GND` (Izquierdo)| **Masa común unificada obligatoria**. |

---

## 2. Diagrama de Pines del Transistor 2N2222A (Encapsulado Metálico TO-18)

Mirando la cápsula desde la **parte inferior (lado de las patillas)** con la pequeña pestaña metálica de referencia orientada hacia la izquierda:

```text
             VISTA INFERIOR (BOTTOM VIEW)
                     [Cápsula Metálica]

                           (2) BASE
                              o
                             / \
           PESTAÑA          /   \
           METÁLICA        /     \
              [===] o (1)         o (3) COLECTOR
                   EMISOR
```

* **Pin 1 (Junto a la pestaña metálica):** **Emisor (E)** $\rightarrow$ Conectar a GND Común.
* **Pin 2 (Terminal central):** **Base (B)** $\rightarrow$ Conectar a un extremo del resistor $R_1$ ($1\,\text{k}\Omega$).
* **Pin 3 (Terminal derecha):** **Colector (C)** $\rightarrow$ Conectar al borne negativo del motor (cable gris). *Nota: En el encapsulado TO-18, el colector suele tener continuidad con la carcasa metálica.*

---

## 3. Diagrama de Pines del Módulo DHT11 (3 Pines)

Mirando el módulo de frente (rejilla plástica azul hacia el usuario):

```text
                 +-------------------+
                 |    [DHT11 AZUL]   |
                 |  |||||||||||||||  |
                 |  |||||||||||||||  |
                 +-------------------+
                 |     [PCB NEGRA]   |
                 |   R_pullup [===]  |
                 |   S             - |
                 +---+-----+-----+---+
                     |     |     |
                    (1)   (2)   (3)
                    DATA   VCC   GND
                   (o VCC según fabricante)
```

> **Verificación obligatoria:**  
> Revise la serigrafía impresa en el circuito impreso del sensor:
> * Pin `+` / `VCC`: Al pin `3V3` del ESP32.
> * Pin `S` / `OUT` / `DAT`: Al pin `GPIO 4` del ESP32.
> * Pin `-` / `GND`: Al pin `GND` del ESP32.
