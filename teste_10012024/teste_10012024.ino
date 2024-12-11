
   // --------------------- formula ---------------------------------------
   // ----------------------EXEMPLO----------------------------------------
   // peso = 810gr, diametro 1,518mm,alça= 1,483, diametro do tubo= 0,65
   // 3,14/4*{ (Ø bobina)²- (Ø tubo)² }*alça
   // resutadoVolume = 3,14/4*{ (valor1*valor1)- (valor2*valor2) }*valor3 
   // resultadoVolume = (peso)/(resultadoVolume em decimetros cubicos) 
   // resultado volume = 2,1907
   // densidade = peso/volume dm3
   // desidade = 810 gr/ 2,1907  
   // resultado densidade = 369,74 g/dm3

//-------------- VARIANTES GLOBAIS ---------------------------------------


#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <HX711.h>
#include <PushButton.h>
#include "LiquidCrystal_I2C.h"

// Definições de pinos
#define pinDT A1
#define pinSCK A0
#define pinBotao 8
#define pinLed1 7 // LED verde
#define pinLed2 6 // LED amarelo
#define pinLed3 5 // LED vermelho

// Limites de densidade
const float DENSIDADE_MIN3 = 100; // vermelho baixo
const float DENSIDADE_MAX3 = 329; // vermelho baixo
const float DENSIDADE_MIN4 = 330; // amarelo baixo
const float DENSIDADE_MAX4 = 339; // amarelo baixo
const float DENSIDADE_MIN1 = 340; // verde
const float DENSIDADE_MAX1 = 380; // verde
const float DENSIDADE_MIN2 = 381; // amarelo alto
const float DENSIDADE_MAX2 = 390; // amarelo alto
const float DENSIDADE_MIN5 = 391; // vermelho alto
const float DENSIDADE_MAX5 = 600; // vermelho alto

// Instanciando objetos
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
LiquidCrystal_I2C lcd(0x3F, 16, 2);
HX711 scale;
PushButton botao(pinBotao);

// Variáveis globais
float valorPi = 3.14159;
float alca = 148; // altura da bobina (em mm)
float diametroTubo = 64; // Diâmetro do tubo (em mm)
float peso = 0; // Peso
float centro = 123.5; // Distancia do centro até o sensor (em mm)
float diametroBobina = 0; // Diâmetro da bobina (em mm)
float resultadoVolume = 0, resultadoDensidade = 0, distanciaEncontrada = 0; 
const int numReadings = 10; // Número de leituras do sensor
int readings[numReadings];
bool lastButtonState = LOW;
int leituraCount = 0; // Contador para a numeração sequencial das leituras

//========================== SETUP =========================
void setup() {
    Serial.begin(9600);
    if (!lox.begin()) {
        Serial.println(F("Falha ao encontrar o chip VL53L0X"));
        while (1);  // Se não encontrar o sensor, entra em loop infinito
    }
    Serial.println("DENSITÔMETRO INICIADO!");

    pinMode(pinBotao, INPUT_PULLUP);
    pinMode(pinLed1, OUTPUT);
    pinMode(pinLed2, OUTPUT);
    pinMode(pinLed3, OUTPUT);

    lcd.init();
    lcd.backlight();
    scale.begin(pinDT, pinSCK);
    scale.set_scale(-374250); // Calibração do HX711 373000
    scale.tare();
    Serial.println("Setup Finalizado!");

    // Inicializa o array de leituras
    for (int i = 0; i < numReadings; i++) {
        readings[i] = 0;
    }
}

//========================== FUNÇÕES =========================

// Função para atualizar LEDs
void updateLEDs(float densidade) {
    if (densidade >= DENSIDADE_MIN1 && densidade <= DENSIDADE_MAX1) {
        digitalWrite(pinLed1, HIGH); // Verde
        digitalWrite(pinLed2, LOW);
        digitalWrite(pinLed3, LOW);
    } else if ((densidade >= DENSIDADE_MIN2 && densidade <= DENSIDADE_MAX2) || 
               (densidade >= DENSIDADE_MIN4 && densidade <= DENSIDADE_MAX4)) {
        digitalWrite(pinLed1, LOW);
        digitalWrite(pinLed2, HIGH); // Amarelo
        digitalWrite(pinLed3, LOW);
    } else if ((densidade >= DENSIDADE_MIN3 && densidade <= DENSIDADE_MAX3) || 
               (densidade >= DENSIDADE_MIN5 && densidade <= DENSIDADE_MAX5)) {
        digitalWrite(pinLed1, LOW);
        digitalWrite(pinLed2, LOW);
        digitalWrite(pinLed3, HIGH); // Vermelho
    } else {
        digitalWrite(pinLed1, LOW);
        digitalWrite(pinLed2, LOW);
        digitalWrite(pinLed3, LOW); // Desliga todos os LEDs
    }
}

// Função para calcular o volume e a densidade
void calcularDensidade() {
    // Cálculo do volume
    resultadoVolume = (valorPi / 4) * (pow(diametroBobina, 2) - pow(diametroTubo, 2)) * alca;
    
    // Convertendo o volume de mm³ para dm³ (1 dm³ = 1000 cm³ = 1000000 mm³)
    resultadoVolume = resultadoVolume / 1000000.0;  // Volume em dm³

    // Exibição do volume no Serial
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Volume:");
    lcd.print(resultadoVolume, 4); // Exibe o volume no LCD

    delay(2000); // Pausa para visualizar o volume antes de exibir a densidade

    // Cálculo da densidade
    resultadoDensidade = (peso / resultadoVolume)*1000;

    // Exibição do peso e da densidade no LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Peso: ");
    lcd.print(peso, 3);
    lcd.print(" kg");   
    lcd.setCursor(9, 1);
    lcd.print("Dia.");
    lcd.print(int(diametroBobina));
    lcd.setCursor(0, 1);
    lcd.print("De:");
    lcd.print(int(resultadoDensidade));

    // Atualiza LEDs
    updateLEDs(resultadoDensidade);

    // Exibição da densidade no Serial
    Serial.print(leituraCount);
    Serial.print(" | Peso: ");
    Serial.print(peso, 3);
    Serial.print(" g | Diâmetro: ");
    Serial.print(diametroBobina, 2);
    Serial.print(" mm | Volume: ");
    Serial.print(resultadoVolume, 6);
    Serial.print(" dm³ | Densidade: ");
    Serial.print(resultadoDensidade, 2);
    Serial.println(" g,dm³ | ");
}

// Função para calcular a média da distância, descartando as 2 maiores e as 2 menores leituras
float calcularMediaDistancia() {
    float soma = 0;
    
    // Coleta as 10 leituras do sensor
    for (int i = 0; i < numReadings; i++) {
        VL53L0X_RangingMeasurementData_t measure;
        lox.rangingTest(&measure, false);
        if (measure.RangeStatus != 4) {
            readings[i] = measure.RangeMilliMeter; // Salva a distância medida em milímetros
        } else {
            readings[i] = 0; // Em caso de erro, armazena 0
        }
        delay(50); // Atraso entre as leituras
    }

    // Ordena as leituras (bubble sort)
    for (int i = 0; i < numReadings - 1; i++) {
        for (int j = i + 1; j < numReadings; j++) {
            if (readings[i] > readings[j]) {
                int temp = readings[i];
                readings[i] = readings[j];
                readings[j] = temp;
            }
        }
    }

    // Descarta as 2 maiores e as 2 menores leituras
    for (int i = 2; i < numReadings - 2; i++) {
        soma += readings[i];
    }

    // Calcula a média das 6 leituras restantes
    return soma / 6;
}

//========================== LOOP =========================
void loop() {
    bool currentButtonState = digitalRead(pinBotao);
    if (currentButtonState == LOW && lastButtonState == HIGH) {
        // Coleta de medidas da balança e cálculo da densidade
        scale.power_up();
        peso = scale.get_units(20);
        scale.power_down();

        // Coleta da média da distância
        distanciaEncontrada = calcularMediaDistancia();

        // Corrigindo erro de 14% de ajuste no diâmetro
        //diametro = diametroBobina - porcentagem;

        // Calcula o diâmetro da bobina
        diametroBobina = (centro - distanciaEncontrada) * 2;
        
        // Incrementa o contador de leituras
        leituraCount++;
       
        // Chama a função para calcular e exibir a densidade
        calcularDensidade();
    }
    lastButtonState = currentButtonState;
    delay(50); // Atraso para o botão não ser pressionado repetidamente
}


  

 


 

   
   
    
 
