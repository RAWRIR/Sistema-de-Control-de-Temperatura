# Lista de Materiales (BOM - Bill of Materials)

Proyecto: **Sistema de Control de Temperatura con ESP32 y Actuador Fan DC**  
Repositorio: `RAWRIR/Sistema-de-Control-de-Temperatura`

| Referencia | Componente | Valor / Modelo | Encapsulado | Cantidad | Descripción Técnica y Función en el Circuito |
| :--- | :--- | :--- | :--- | :---: | :--- |
| **U1** | Microcontrolador | **ESP32 DevKit V1** (ESP-WROOM-32) | Módulo 30 Pines | 1 | Controlador principal. Procesa las lecturas del sensor DHT11 y conmuta el actuador mediante señal lógica/PWM en GPIO 19. |
| **SEN1** | Sensor Ambiental | **DHT11** (Módulo Breakout 3 Pines) | Módulo PCB con carcasa azul | 1 | Sensor digital de temperatura (0–50 °C) y humedad relativa (20–90%). Incorpora resistencia de pull-up interna. |
| **M1** | Actuador / Motor | **Motor DC 130 con Hélice** | Motor DC estándar de juguete | 1 | Carga inductiva principal (1.5V - 3.0V DC). Genera flujo de aire para la refrigeración forzada. |
| **Q1** | Transistor BJT NPN | **2N2222A** | TO-18 Metálico | 1 | Interruptor electrónico de lado bajo (*Low-Side Switch*). Conduce la corriente del motor a tierra cuando se satura. $I_C(max) = 800\,\text{mA}$, $V_{CEO} = 40\,\text{V}$. |
| **R1** | Resistor | **1 kΩ (1/4 W, 5%)** | Axial Through-Hole (Marrón-Negro-Rojo-Oro) | 1 | Resistencia limitadora de corriente de base. Fija $I_B \approx 2.6\,\text{mA}$ garantizando saturación profunda sin sobrecargar el GPIO 19. |
| **D1, D2, D3, D4** | Diodos Rectificadores | **1N4007** (o 1N4001 - 1N4004) | DO-41 Through-Hole | 4 | Cadena reductora de tensión en serie. Cada diodo genera una caída directa $V_F \approx 0.7\,\text{V}$ ($\Sigma \approx 2.8\,\text{V}$), bajando los 5V de la fuente a $\approx 2.2\,\text{V}-2.5\,\text{V}$ para proteger el motor de 3V. |
| **D5** | Diodo Flyback | **1N4007** (o 1N4148 rápido) | DO-41 Through-Hole | 1 | Diodo de rueda libre conectado en antiparalelo con el motor. Disipa los picos de fuerza contraelectromotriz (f.c.e.m.) al desconectar el transistor, evitando destruir a Q1. |
| **C1** | Capacitor Cerámico | **100 nF (0.1 µF / código 104)** | Disco cerámico ("tipo lenteja") | 1 | Filtro de alta frecuencia soldado en bornes del motor. Suprime interferencias electromagnéticas (EMI) y chispas de las escobillas mecánicas. |
| **PS1** | Fuente de Alimentación | **Cargador USB / Fuente 5V DC** | Adaptador de pared / USB | 1 | Suministra 5V DC (mínimo 1A recomendado) para alimentar la rama motora y el sistema. |
| **W1..Wn** | Cables de Conexión | Jumpers DuPont / Cables unifilares | Varios calibres (AWG 22–24) | 1 set | Interconexión de componentes, asegurando una línea de **tierra común unificada (GND)**. |
