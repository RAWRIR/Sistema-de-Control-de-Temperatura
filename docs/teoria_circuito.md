# Fundamentos Teóricos y Análisis de Ingeniería del Circuito

Este documento detalla los principios físicos, cálculos y criterios de diseño aplicados en el circuito eléctrico del **Sistema de Control de Temperatura con ESP32**.

---

## 1. Reductor de Voltaje por Diodos de Silicio en Serie (D1 - D4)

### 1.1 El Problema
Los motores miniatura de corriente continua (tipo 130 o motores de proyectos escolares) tienen una tensión nominal de operación de **$1.5\,\text{V}$ a $3.0\,\text{V}$**. 
Si se alimentan directamente con los **$5.0\,\text{V}$** provenientes de un puerto o cargador USB:
1. La velocidad de giro se eleva a niveles excesivos que provocan vibraciones mecánicas destructivas.
2. La corriente consumida aumenta de forma desproporcionada.
3. Se produce sobrecalentamiento en las bobinas del rotor y desgaste acelerado de los bujes y escobillas metálicas.

### 1.2 La Solución por Diodos
En lugar de emplear un regulador lineal voluminoso (como un LM317 o un convertidor reductor DC-DC buck), se utiliza una serie de **4 diodos rectificadores de silicio (1N4007)**:

* Cada diodo de silicio presenta una tensión umbral directa de conducción típica de:
  $$V_F \approx 0.65\,\text{V} - 0.75\,\text{V}$$
* Al conectar 4 diodos en polarización directa en serie:
  $$\Delta V_{\text{total}} = 4 \times V_F \approx 4 \times 0.70\,\text{V} = 2.80\,\text{V}$$
* La tensión efectiva aplicada al motor resulta en:
  $$V_{\text{motor}} = V_{\text{fuente}} - \Delta V_{\text{total}} = 5.00\,\text{V} - 2.80\,\text{V} \approx 2.20\,\text{V} - 2.50\,\text{V}$$

Este valor se encuentra en el rango seguro de trabajo del motor, asegurando par suficiente para mover la hélice sin sobrecalentamiento.

---

## 2. Protección Contra Sobretensiones Inductivas: Diodo Flyback (D5)

### 2.1 Principio Físico de la Fuerza Contraelectromotriz (f.c.e.m.)
El motor DC está constituido internamente por devanados de alambre de cobre que presentan una inductancia $L$. 
Cuando el transistor de conmutación conduce, la bobina almacena energía magnética proporcional a la corriente:
$$E = \frac{1}{2} L I^2$$

Al cortar súbitamente la corriente (cuando el GPIO 19 pasa a nivel `LOW` y el transistor se bloquea), la derivada de la corriente en el tiempo es fuertemente negativa ($\frac{di}{dt} \to -\infty$). Por la Ley de Faraday-Lenz:
$$V_L = -L \frac{di}{dt}$$

Esto genera un pulso de tensión inversa (*inductive kickback*) en el colector del transistor que puede superar fácilmente los **$50\,\text{V} - 100\,\text{V}$**, superando la tensión de ruptura colector-emisor $V_{CEO}$ del 2N2222A ($40\,\text{V}$), perforando la unión de silicio y quemando el transistor.

### 2.2 Acción del Diodo Flyback
El diodo **D5 (1N4007)** se suelda en **antiparalelo** respecto a la corriente normal de trabajo:
* Cátodo conectado al borne positivo ($V+$).
* Ánodo conectado al colector del transistor / borne negativo.

En régimen normal, D5 permanece en corte inverso (no conduce).  
En el instante del corte, el pico inductivo polariza en directo a D5, cerrando el lazo inductivo y permitiendo que la energía se disipe suavemente en la resistencia parásita interna del devanado del motor, protegiendo al transistor.

---

## 3. Supresión de Ruido Electromagnético: Capacitor Lenteja (C1)

### 3.1 Origen del Ruido de Conmutación (EMI)
Los motores de corriente continua con escobillas dependen de un conmutador mecánico de delgas de cobre. A medida que el rotor gira, las escobillas tocan y desacoplan secuencialmente los diferentes devanados. Este desacople brusco genera pequeños arcos voltaicos (chispas microscópicas).

Estas chispas radian interferencia electromagnética de alta frecuencia (**EMI / RFI**) y transitorios de voltaje a través de los cables de alimentación. En microcontroladores sensibles como el ESP32, esto suele provocar:
* Corrupción de lecturas digitales en el protocolo 1-Wire del DHT11.
* Reinicios espontáneos del microcontrolador por caídas de tensión (*Brownout Reset*).
* Falsos disparos en interrupciones de hardware.

### 3.2 Acción del Capacitor Cerámico
Un capacitor cerámico tipo disco ("lenteja") de **$100\,\text{nF}$ (0.1 µF, código 104)** soldado directamente en las orejetas del motor:
* Ofrece una impedancia extremadamente baja a las altas frecuencias ($X_C = \frac{1}{2\pi f C}$), actuando como un cortocircuito local para el ruido de radiofrecuencia.
* Extingue inmediatamente las micro-chispas de las escobillas en el propio punto de origen.

---

## 4. Driver de Conmutación BJT Low-Side (Q1 + R1)

### 4.1 Elección de la Topología Low-Side
Se utiliza un transistor NPN actuando como conmutador de lado bajo (*Low-Side*):
* El emisor se conecta a GND común.
* La carga (motor) se coloca entre la línea positiva y el colector.
* Esto permite activar el transistor con una tensión de compuerta/base referenciada a tierra ($0\,\text{V}$ a $3.3\,\text{V}$), compatible con el nivel lógico de los pines del ESP32 sin necesidad de convertidores de nivel (*level shifters*).

### 4.2 Cálculo de la Resistencia de Base ($R_1 = 1\,\text{k}\Omega$)
Para que el transistor actúe como un interruptor cerrado ideal, debe operar en **zona de saturación** ($V_{CE} \approx 0.1\,\text{V} - 0.2\,\text{V}$), no en zona activa:

* Tensión de salida en ALTO del ESP32: $V_{OH} \approx 3.3\,\text{V}$.
* Tensión base-emisor en saturación: $V_{BE(sat)} \approx 0.7\,\text{V}$.
* Corriente de base inyectada:
  $$I_B = \frac{V_{OH} - V_{BE(sat)}}{R_1} = \frac{3.3\,\text{V} - 0.7\,\text{V}}{1000\,\Omega} = 2.6\,\text{mA}$$

Para una ganancia forzada de saturación $\beta_{sat} \approx 10$ a $20$:
$$I_{C(sat)} = \beta_{sat} \times I_B = 20 \times 2.6\,\text{mA} = 52\,\text{mA}$$
Dado que el motor en régimen libre consume típicamente entre $80\,\text{mA}$ y $180\,\text{mA}$, una corriente de base de $2.6\,\text{mA}$ proporciona una conducción eficiente. La potencia disipada en el transistor es mínima ($P_D = V_{CE(sat)} \times I_C \approx 0.2\,\text{V} \times 0.15\,\text{A} = 30\,\text{mW}$), manteniéndose frío al tacto dentro de su cápsula metálica TO-18.

---

## 5. GND Común Unificado

Un requisito fundamental de la electrónica analógica y digital es la **referencia común de potencial (0V)**:
* La señal lógica enviada por el GPIO 19 del ESP32 solo tiene sentido si la tensión se mide respecto al emisor del transistor.
* Si el GND de la fuente externa de 5V y el GND del ESP32 no estuvieran conectados físicamente, la unión base-emisor del transistor no vería ninguna diferencia de potencial, impidiendo la activación del motor y generando ruido flotante impredecible.
