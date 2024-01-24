   //----------------- TESTE COM A MÉDIA ----------------------------------
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

#include <Wire.h>               // COMUCICAÇÃO I2C BIBLIOTECA BALANÇA                   
#include "Adafruit_VL53L0X.h"   // BIBLIOTECA SENSOR VL5310X
#include "LiquidCrystal_I2C.h"; / /BIBLIOTECA DISPLAY
#include "HX711.h"              // BIBLIOTECA BALANÇA 
LiquidCrystal_I2C lcd(0x3F, 16, 2);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
HX711 escala;
#define DT A1                   // DEFINIÇÃO DO PINO MODULO PESO
#define SCK A0                  // DEFINIÇÃO DO PINO MODULO PESO

const int ANALOG_PIN = SCL; //A0
float valor1;                   // 3,14 constante
float valor2;                   // 4 constante
float valor3;                   // diametro da bobina em 166mm
float valor4;                   // diametro do tubo em mm
float valor5;                   // alça em mm
float valor6;                   // peso 932
float resultado1, resultado2, resultado3, resultadoVolume, resultadoDensidade, distanciaEncontrada, centro, diametroBobina;
float media = 0;
float valor = 0;
int x = 0;

//DEFINICAO DOS PINOS

#define pinLed1  7  //definição de pino VERDE
#define pinLed2  6  //definição de pino AMARELO
#define pinLed3  5  //definição de pino VERMELHO

//-------------------------- INICIO SETUP --------------------------------
void setup() {

  if (!lox.begin()) {
    while(1);
  }
 
 escala.begin (DT, SCK);
 Serial.begin(9600);
 Serial.print("Leitura do Valor ADC: ");
 Serial.println(escala.read()); // Aguada até o dispositivo estar pronto
 Serial.println("Nao coloque nada na balanca!");
 Serial.println("Iniciando...");
 escala.set_scale(-373000); // Substituir o valor encontrado para escala //387000   889
 escala.tare(20); // O peso é chamado de Tare.
 Serial.println("Insira o item para Pesar");
 
 pinMode(pinLed1, OUTPUT); //VERDE
 pinMode(pinLed2, OUTPUT); //AMARELO
 pinMode(pinLed3, OUTPUT); //VERMELHO
 
 lcd.init();
 lcd.backlight();
 
 // valores fixos
 valor1 = 3.14;  // constante
 valor2 = 4;    // constante
 //valor3 = 1.520;  //diam. da bobina 1518
 valor4 = 0.65;  // diametro do tubo
 valor5 = 1.483;//alça
 centro = 135; // centralização 1.250

  //resultadoVolume = valor1/valor2*(valor3*valor3) - (valor4*valor4)*valor5; 
resultado1 = valor1/valor2;

resultado2 = valor3*valor3-valor4*valor4;

resultadoVolume = resultado1*resultado2*valor5;

resultadoDensidade = valor6 / resultadoVolume; 


  Serial.begin(9600);
  Serial.print("Valor1: ");
  Serial.println(valor1);

  Serial.print("Valor2: ");
  Serial.println(valor2);

   Serial.print("Valor3: ");
  Serial.println(valor3);

  Serial.print("Valor4: ");
  Serial.println(valor4);

  Serial.print("Valor5: ");
  Serial.println(valor5);

  Serial.print("valor6: ");
  Serial.println(valor6);

  Serial.print("resultado1: ");
  Serial.println(resultado1);

  Serial.print("resultado2: ");
  Serial.println(resultado2);

  Serial.print("resultadoVolume: ");
  Serial.println(resultadoVolume);

  Serial.print("resultadoDensidade: ");
  Serial.println(resultadoDensidade);

  
}
// ------------------FIM  DO  SETUP -------------------------------

//-------------------- INICIO LOOP ---------------------------------
void loop() {
//analog.update();// MEDIA MOVEL

VL53L0X_RangingMeasurementData_t measure;

   x = 0;
valor = 0;


while(x < 10)

{ 

valor = (valor + measure.RangeMilliMeter );
delay(400); 
x++;

//==========================================
Serial.print("Reading a measurement... ");
     lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

       if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
     } else {
      Serial.println(" out of range ");
    
    //========================================

    }
media = valor/10; 

}
     Serial.begin(9600);// repetiu
     Serial.print("Reading a measurement... ");
     lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  
lox.rangingTest(&measure, false); // MEDIA MOVEL
  
 distanciaEncontrada =  measure.RangeMilliMeter;

 diametroBobina = centro - distanciaEncontrada ;
 media = 2 *  diametroBobina/100;  //tem que ser 2   valor3
 valor6 = escala.get_units(20);

 resultado1 = valor1/valor2;

 resultado2 = media*media-valor4*valor4;// valor3

 resultadoVolume = resultado1*resultado2*valor5;

 resultadoDensidade = (valor6 / resultadoVolume)*1000; 

 int valorLido = digitalRead (8);
 
 lcd.setCursor(0, 0);   // 0ºlinha 0ºcoluna
 lcd.print ("P");
 lcd.setCursor(0, 1);   // 0ºlinha 1ºcoluna
 lcd.print ("DENS.");
 lcd.setCursor(10, 0);   // 6ºlinha 0ºcoluna
 lcd.print ("D");
 lcd.setCursor(12, 0);   // 6ºlinha 0ºcoluna
 lcd.print (media); // valor3
 lcd.setCursor(8, 1);   // 8ºlinha 1ºcoluna
 lcd.print (resultadoDensidade);
 lcd.println("dm");
 lcd.setCursor(1, 0);
 lcd.print(escala.get_units(20), 3);
 lcd.println(" kg ");

  Serial.begin(9600); //repetiu
  Serial.print("valor6: ");
  Serial.println(valor6);

  Serial.print("valor3: ");
  Serial.println(valor3);

  Serial.print("Densidade: ");
  Serial.println(resultadoDensidade);

  Serial.print("distanciaEncontrada "); //(distanciaEncontrada);
  Serial.println( measure.RangeMilliMeter);//(media);;

  Serial.print(" media ");
  Serial.println(media); 

   Serial.println("");
   delay(50);
  
  //-------------  led verde para boas condições -------------------

  // criando as condicoes se a densidade for entre 340 e 360
  if (resultadoDensidade <= 360 and resultadoDensidade>=340)  {
    digitalWrite(pinLed1, HIGH); //liga o LED verde
    digitalWrite(pinLed2, LOW); //liga o LED amarelo
    digitalWrite(pinLed3, LOW); //liga o LED verde
  }
  // criando as condicoes se a densidade for entre 361 e 370
  if (resultadoDensidade <= 370 and resultadoDensidade>=361)  {
    digitalWrite(pinLed2, HIGH); //liga o LED amarelo
    digitalWrite(pinLed1, LOW); //liga o LED verde
    digitalWrite(pinLed3, LOW); //liga o LED vermelho
  }
    
  
  // criando as condicoes se a densidade for entre 330 e 339
  if (resultadoDensidade <= 339 and resultadoDensidade>=330)  {
    digitalWrite(pinLed2, HIGH); //liga o LED amarelo
    digitalWrite(pinLed1, LOW); //liga o LED verde
    digitalWrite(pinLed3, LOW); //liga o LED vermelho
  }
  
  // criando as condicoes se a densidade for entre 320 e 329
  if (resultadoDensidade <= 329 and resultadoDensidade>=320)  {
    digitalWrite(pinLed3, HIGH); //liga o LED vermelho
    digitalWrite(pinLed2, LOW); //liga o LED amarelo
    digitalWrite(pinLed1, LOW); //liga o LED verde
  
  }

  // criando as condicoes se a densidade for entre 371 e 380
  if (resultadoDensidade <= 380 and resultadoDensidade>=371)  {
    digitalWrite(pinLed3, HIGH); //liga o LED vermelho
    digitalWrite(pinLed2, LOW); //liga o LED amarelo
    digitalWrite(pinLed1, LOW); //liga o LED verde

    
  }
   {
   Serial.println("");
   delay(50);
  }
   }

   
