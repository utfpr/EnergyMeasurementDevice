//Authors: Lucas Beluomini, Ivanilton Polato, Pedro Bertelli

#include <U8glib.h>

//------------------------OLED-----------------------------
#include <U8glib.h>
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

//------------------------TENSÃO---------------------------

//DECLARAÇÕES USADAS PARA CAUCULO DE TENSÃO

double sensorTensao = 0;

double sensorTensao1 = 0;

int crosscount = 0;

int climbhill = 0;

double VmaxD = 0;

double VeffD;

double Veff = 0;

//------------------------AUXILIARES-----------------------

// DECLARAÇÃO DA VARIÁVEL DA MEDIA DE AMOSTRAGEM A
int amostragem = 1000;

// DECLARAÇÃO DA VARIÁVEL DA CONSTANTE DO ADC 5/1023 A
float voltsporUnidade = 0.004887586;// 5%1023

//DECLARAÇÃO DA VARIÁVEL DE SENSIBILIDADE DO ACS712 A
float sensibilidade = 0.100; //0.185 para sensor 5A

//FIM CORRENTE
float somaFinalCorrente = 0;


//----------------------DECLARAÇÕES CORRENTE A----------------------

//PINO DO SENSOR ACS712 A
int sensorACS712A = A0;

// DECLARAÇÃO DA VARIÁVEL AUXILIAR A
int sensorValue_auxA = 0;

// DECLARAÇÃO DA VARIÁVEL QUE FARÁ LEIUTURA DIRETO NO SENSOR ACS712 A
float valorSensorACS712A = 0;

// DECLARAÇÃO DA VARIÁVEL QUE FARÁ LEIUTURA DE CORRENTE A
float valorCorrenteA = 0;


//----------------------DECLARAÇÕES CORRENTE B----------------------


//PINO DO SENSOR ACS712 B
int sensorACS712B = A1;

// DECLARAÇÃO DA VARIÁVEL AUXILIAR B
int sensorValue_auxB = 0;

// DECLARAÇÃO DA VARIÁVEL QUE FARÁ LEIUTURA DIRETO NO SENSOR ACS712 B
float valorSensorACS712B = 0;

// DECLARAÇÃO DA VARIÁVEL QUE FARÁ LEIUTURA DE CORRENTE B
float valorCorrenteB = 0;

//---------------------DECLARAÇÕES POTENCIA------------------------

// DECLARAÇÃO VRIAVEL DE POTENCIA A
float potenciaA = 0;

// DECLARAÇÃO VRIAVEL DE POTENCIA B
float potenciaB = 0;

void setup() {

 //ZERA OS VALORES
  Veff = 0;
  valorCorrenteA = 0;
  valorCorrenteB = 0;

 //VALOR REFERENCIAL
  analogReference(DEFAULT);

 // INICIALIZA A PORTA SERIAL
  Serial.begin(9600);

 //PINO SENSOR DE CORRENTE ACS712 A
  pinMode(A0, INPUT);

 //PINO SENSOR DE CORRENTE ACS712 B
  pinMode(A1, INPUT);

 //PINO SENSOR DE TENSÃO
  pinMode(A2, INPUT);

 //DISPLAY OLED
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }

  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }

  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }

  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
}

void loop() {

  u8g.firstPage();

  //------------------------------------TENSÃO---------------------------------------
    sensorTensao1=sensorTensao;
    delay(100);
    sensorTensao = 0;
    sensorTensao = analogRead(A2);

    if (sensorTensao>=sensorTensao1 && sensorTensao>511 && sensorTensao != 0){
      climbhill=1;
      VmaxD=sensorTensao;
    }

    if (sensorTensao<sensorTensao1 && climbhill==1 && sensorTensao != 0){
      climbhill=0;
      VmaxD=sensorTensao1;
      VeffD=VmaxD/sqrt(2);
      Veff = (VmaxD*0.1357);
      //Veff=(((VeffD-420.76)/-90.24)*-210.2)+210.2;
      VmaxD=0;
    }

    if (sensorTensao == 0) {
      Veff=0;
    }


// ----------------------------------CORRENTE---------------------------------------

 //REINICIA O VALOR ATUAL E ATUALIZA NA PRÓXIMA LEITURA
    valorSensorACS712A = 0;
    valorSensorACS712B = 0;
    sensorValue_auxA = 0;
    sensorValue_auxB = 0;

 //INICIA A AQUISIÇÃO DOS VALORES PARA MEDIR A CORRENTE CONSUMIDA------------------


    for(int i=0; i < amostragem ; i++){
      // a saída do sensor é (1023)vcc/2 para corrente =0
      sensorValue_auxA = (analogRead(sensorACS712A) -511.08);
      sensorValue_auxB = (analogRead(sensorACS712B) -511.08);

      // somam os quadrados das leituras (Deiaxar positivo).
      valorSensorACS712A += pow(sensorValue_auxA,2);
      valorSensorACS712B += pow(sensorValue_auxB,2);

      //INTERVALO ENTRE LEITURAS
      delay(1);
    }

    //ajusta o valor lido para ampere
    valorSensorACS712A = (valorSensorACS712A/ amostragem) * voltsporUnidade;
    valorSensorACS712B = (valorSensorACS712B/ amostragem) * voltsporUnidade;

    /*
    if(valorSensorACS712A<=0.02){
      valorSensorACS712A = 0;
    }
    if(valorSensorACS712B<=0.02){
      valorSensorACS712B = 0;
    }
    */


    // calcula a corrente considerando a sensibilidade do sernsor
    valorCorrenteA = (valorSensorACS712A/sensibilidade)*0.3125;
    valorCorrenteB = (valorSensorACS712B/sensibilidade)*0.3125;

    if(valorCorrenteA>20){
      valorCorrenteA = 0;
    }


    if(valorCorrenteB>20){
      valorCorrenteB = 0;
    }

    delay(1);

    somaFinalCorrente = valorCorrenteB + valorCorrenteA;


    potenciaA = valorCorrenteA * Veff;
    potenciaB = valorCorrenteB * Veff;
  do
  {
    draw();
  } while( u8g.nextPage() );

 delay(100);
}

void draw() {

//---------------------------PRINTS--------------------------------
/*
   Serial.print("TENSAO: ");
   Serial.print(Veff);
   Serial.print(" V");
   Serial.print("CORRENTE 1:");
   Serial.print(valorCorrenteA);
   Serial.println(" A");
   Serial.print("CORRENTE 2:");
   Serial.print(valorCorrenteB);
   Serial.println(" A");
*/

  //ESCREVE-------------------------
  //Seleciona a fonte de texto
  u8g.setFont(u8g_font_8x13B);

  u8g.drawStr( 55, 30, "v");
  u8g.setPrintPos(0, 30);
  u8g.print(Veff, 1);

  u8g.drawStr( 55, 45, "A");
  u8g.setPrintPos(0, 45);
  u8g.print(valorCorrenteA);

  u8g.drawStr( 55, 60, "w");
  u8g.setPrintPos(0, 60);
  if (potenciaA>=10000){
    u8g.print(potenciaA, 0);
  }else{
  u8g.print(potenciaA, 1);
  }

  u8g.drawLine( 64, 0, 64, 64);
  u8g.drawLine( 0, 15, 128, 15);

  u8g.drawStr( 120, 30, "v");
  u8g.setPrintPos(70, 30);
  u8g.print(Veff, 1);

  u8g.drawStr( 120, 45, "A");
  u8g.setPrintPos(70, 45);
  u8g.print(valorCorrenteB);

  u8g.drawStr( 120, 60, "w");
  u8g.setPrintPos(70, 60);
  if (potenciaB>=10000){
    u8g.print(potenciaB, 0);
  }else{
  u8g.print(potenciaB, 1);
  }

  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 5, 10, "Input A");
  u8g.drawStr( 69, 10, "Input B");
}
