/**
 * ==============================================================================
 * Proyecto: Sistema de Control de Temperatura con ESP32 y Actuador Fan DC
 * Repositorio: RAWRIR / Sistema-de-Control-de-Temperatura
 * Autor: RAWRIR
 * Microcontrolador: ESP32 DevKit V1 (ESP-WROOM-32)
 * Sensor: DHT11 (Humedad y Temperatura Digital)
 * Actuador: Motor DC 3V con Hélice (Ventilador) controlado por Transistor 2N2222A
 * ==============================================================================
 */

#include <Arduino.h>
#include <DHT.h>

// ==============================================================================
// 1. CONFIGURACIÓN DE PINES (HARDWARE MAP)
// ==============================================================================
#define DHT_PIN         4    // GPIO 4 (D4): Línea de Datos (DATA/S) del sensor DHT11
#define DHT_TYPE        DHT11// Tipo de sensor utilizado (DHT11 o DHT22)

#define FAN_PIN         19   // GPIO 19 (D19): Control de base del transistor Q1 (2N2222A)

// ==============================================================================
// 2. PARÁMETROS DEL CONTROL DE TEMPERATURA
// ==============================================================================
// Control tipo termostato con histéresis (evita conmutaciones rápidas / chattering)
const float TEMP_UMBRAL_ACTIVACION = 28.0; // Encender ventilador si T >= 28.0 °C
const float TEMP_UMBRAL_APAGADO    = 26.0; // Apagar ventilador si T <= 26.0 °C

// Opcional: Modo de operación (TRUE: PWM proporcional, FALSE: Digital ON/OFF con histéresis)
const bool USAR_CONTROL_PWM = false;

// Configuración de PWM en ESP32 (LEDC)
const int PWM_CANAL      = 0;     // Canal PWM (0 a 15)
const int PWM_FRECUENCIA = 5000;  // Frecuencia en Hz (5 kHz ideal para motor DC)
const int PWM_RESOLUCION = 8;     // Resolución en bits (0 a 255)

// Intervalo de lectura del sensor (mínimo 1000-2000 ms para DHT11)
const unsigned long INTERVALO_LECTURA_MS = 2000;

// ==============================================================================
// 3. VARIABLES GLOBALES E INSTANCIAS
// ==============================================================================
DHT dht(DHT_PIN, DHT_TYPE);

unsigned long ultimaLectura = 0;
bool ventiladorActivo = false;
int velocidadPWM = 0;

// ==============================================================================
// 4. INICIALIZACIÓN (SETUP)
// ==============================================================================
void setup() {
  // Inicialización de la consola serie
  Serial.begin(115200);
  delay(1000);

  Serial.println(F("\n======================================================="));
  Serial.println(F("     SISTEMA DE CONTROL DE TEMPERATURA - ESP32"));
  Serial.println(F("======================================================="));

  // Inicializar sensor DHT
  Serial.print(F("Inicializando sensor DHT11 en GPIO "));
  Serial.print(DHT_PIN);
  Serial.println(F("..."));
  dht.begin();

  // Configuración del actuador (Ventilador)
  if (USAR_CONTROL_PWM) {
    Serial.print(F("Configurando actuador en modo PWM (Canal "));
    Serial.print(PWM_CANAL);
    Serial.println(F(")..."));
    ledcAttachChannel(FAN_PIN, PWM_FRECUENCIA, PWM_RESOLUCION, PWM_CANAL);
    ledcWrite(FAN_PIN, 0); // Iniciar apagado
  } else {
    Serial.print(F("Configurando actuador en modo Digital ON/OFF en GPIO "));
    Serial.print(FAN_PIN);
    Serial.println(F("..."));
    pinMode(FAN_PIN, OUTPUT);
    digitalWrite(FAN_PIN, LOW); // Iniciar apagado
  }

  Serial.println(F("Sistema iniciado correctamente. Comenzando monitoreo...\n"));
}

// ==============================================================================
// 5. BUCLE PRINCIPAL (LOOP)
// ==============================================================================
void loop() {
  unsigned long tiempoActual = millis();

  // Ejecutar muestreo de temperatura según el intervalo definido
  if (tiempoActual - ultimaLectura >= INTERVALO_LECTURA_MS) {
    ultimaLectura = tiempoActual;

    // Lectura de humedad y temperatura en grados Celsius
    float humedad = dht.readHumidity();
    float temperatura = dht.readTemperature();

    // Verificación de validez de las lecturas
    if (isnan(humedad) || isnan(temperatura)) {
      Serial.println(F("⚠️ Error: No se pudo obtener lectura del sensor DHT11. Verifica cables y alimentación."));
      return;
    }

    // --- ALGORITMO DE CONTROL ---
    if (USAR_CONTROL_PWM) {
      // Control de velocidad proporcional entre 25°C y 35°C
      if (temperatura < 25.0) {
        velocidadPWM = 0;
      } else if (temperatura >= 35.0) {
        velocidadPWM = 255; // 100% duty cycle
      } else {
        // Mapeo lineal: 25°C -> ~100 PWM (mínimo para vencer inercia del motor), 35°C -> 255
        velocidadPWM = map((long)(temperatura * 10), 250, 350, 100, 255);
      }
      ledcWrite(FAN_PIN, velocidadPWM);
      ventiladorActivo = (velocidadPWM > 0);
    } else {
      // Control tipo termostato con histéresis de 2°C
      if (!ventiladorActivo && temperatura >= TEMP_UMBRAL_ACTIVACION) {
        ventiladorActivo = true;
        digitalWrite(FAN_PIN, HIGH);
        Serial.println(F(">> [ALERTA] Temperatura alta superada. VENTILADOR ENCENDIDO."));
      } else if (ventiladorActivo && temperatura <= TEMP_UMBRAL_APAGADO) {
        ventiladorActivo = false;
        digitalWrite(FAN_PIN, LOW);
        Serial.println(F(">> [NORMAL] Temperatura normalizada. VENTILADOR APAGADO."));
      }
    }

    // Telemetría formateada por puerto serie
    imprimirTelemetria(temperatura, humedad, ventiladorActivo, velocidadPWM);
  }
}

// ==============================================================================
// 6. FUNCIONES AUXILIARES
// ==============================================================================
void imprimirTelemetria(float temp, float hum, bool estadoFan, int pwmVal) {
  Serial.print(F("[TELEMETRIA] Temp: "));
  Serial.print(temp, 1);
  Serial.print(F(" °C | Humedad: "));
  Serial.print(hum, 1);
  Serial.print(F(" % | Ventilador: "));
  
  if (estadoFan) {
    if (USAR_CONTROL_PWM) {
      Serial.print(F("ENCENDIDO (PWM: "));
      Serial.print(map(pwmVal, 0, 255, 0, 100));
      Serial.println(F("%)"));
    } else {
      Serial.println(F("ENCENDIDO (HIGH)"));
    }
  } else {
    Serial.println(F("APAGADO (LOW)"));
  }
}
