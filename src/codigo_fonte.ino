
#include <LiquidCrystal.h>

//Definindo os pinos dos sensores
#define UmidAirPin A0
#define TempAirPin A1
#define LuzPin A2
#define UmidSoloPin A3

//Definindo os pinos do atuadores
#define Bulb 11
#define Ventilador 10
#define Aquecedor
#define UmidificadorAr
#define Irrigador

//Iniciando Variaveis do Sistema
int seconds = 0;

int umidadeSolo = 0;
int umidadeAir = 0;
float tempAir = 0;
int valueLDR = 0;


// PARAMETROS:
//float umidadeSoloMax = 0;
float umidadeSoloMin = 0;

float umidadeAirMax = 60;
float umidadeAirMin = 0;

float tempAirMax = 0;
float tempAirMin = 0;


long tempOfSun = 0;


long lastTime = millis(); // Ultima alteração do tipo de informação
long wait = 3 * 1000; // Intervalo entre as informações
int optionSelect = 1; // Tipo de informação a imprimir



LiquidCrystal lcd_1(13, 12, 5, 4, 3, 2);
void setup()
{
  lcd_1.begin(16, 2); // Set up the number of columns and rows on the LCD.
  
  Serial.begin(9600);
  
  
  pinMode(UmidAirPin,INPUT);
  pinMode(TempAirPin,INPUT);
  pinMode(LuzPin,INPUT);
  pinMode(UmidSoloPin,INPUT);
  
  pinMode(Bulb,OUTPUT);
  pinMode(Ventilador,OUTPUT);
  //pinMode(Aquecedor,OUTPUT);
  //pinMode(UmidificadorAr,OUTPUT);
  //pinMode(Irrigador,OUTPUT);
  
  imprimir(optionSelect);
}

void loop()
{
  // ATUALIZANDO VALORES DOS SENSORES
  umidadeSolo = readUmidadeSolo();
  umidadeAir = readUmidadeAir();
  tempAir = readTemp();
  valueLDR = readLuz();
  digitalWrite(Ventilador, LOW);
  
  // AÇÕES DO SISTEMA
  if(umidadeSolo < umidadeSoloMin)
  {
    //Ligue a mangueira
  }
  
  if(umidadeAir < umidadeAirMin)
  {
    //Ligue o Umidificador
  }
  else if(umidadeAir > umidadeAirMax)
  {
    //Ligue o ventilador
    digitalWrite(Ventilador, HIGH);
  }
  
  if(tempAir < tempAirMin)
  {
    //Ligue o Aquecedor
  }
  else if(tempAir > tempAirMax)
  {
    //Ligue o ventilador
    digitalWrite(Ventilador, HIGH);
  }
  
  /*FAZER LOGICA PARA LIGAR A LAMPADA SUPLEMENTAR COM O TEMPO DE SOL*/
  
  
  
  
  //ATUALIZANDO INFORMAÇÕES DO LCD
  delay(100);
  atualizarValorLCD(optionSelect);
  
  /*Mudando o tipo de informação*/
  if(millis() - lastTime >= wait)
  {
    LimparLCD();
    optionSelect += 1;
    if(optionSelect > 4)
      optionSelect = 1;
    imprimir(optionSelect);
    lastTime = millis();
  }
}


//================================= METODOS DE LEITURA ===============================
float readTemp()
{
  int sensorValue = analogRead(TempAirPin);  // Lê o valor analógico do sensor
  float voltage = sensorValue * (5.0 / 1023.0);  // Converte o valor lido para tensão
  float temperature = (voltage - 0.5) * 100.0;  // Calcula a temperatura em graus Celsius
  return temperature;
}

int readLuz(){return converterParaLumens(analogRead(LuzPin));}

int readUmidadeSolo(){return (int)((long)analogRead(UmidSoloPin) * 100 / 876);}

int readUmidadeAir(){return (int)((long)analogRead(UmidAirPin) * 100 / 1023);}


// Função para converter o valor do LDR em lumens
float converterParaLumens(int valorLDR){
    // Mapeamento do valor do LDR para a faixa de lumens desejada
    // Esse mapeamento pode variar de acordo com as características do seu LDR
    float lumensMin = 0.0; // Valor mínimo em lumens
    float lumensMax = 1000.0; // Valor máximo em lumens

    // Valor máximo que o LDR pode atingir (depende da resolução do ADC do Arduino)
    int valorMaxLDR = 1023;

    // Cálculo da conversão
    float lumens = ((float)valorLDR / valorMaxLDR) * (lumensMax - lumensMin) + lumensMin;

    return lumens;
}



//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* Metodos de ação =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*



// ============================== METODOS DE IMPRESSÃO ================================

/*IMPRIMIR NOVA INFORMAÇÃO*/
void imprimir(int option)
{
  /*OPTIONS VALUES:
  1 = temperatura
  2 = umidade do ar
  3 = umidade do solo
  4 = luminosidade
  5 = tempo de sol*/
  int position = 1;
  switch(option)
  {
    case 1: // TEMPERATURA
    	lcd_1.setCursor(0, 0);
    	lcd_1.print("TEMP:");
    	atualizarValorLCD(option);
    	break;
    
    case 2: // UMIDADE DO AR
    	lcd_1.setCursor(0, 0);
    	lcd_1.print("AIR UMID:");
    	atualizarValorLCD(option);
    	break;
    
    case 3: // UMIDADE DO SOLO
    	lcd_1.setCursor(0, 0);
    	lcd_1.print("SOLO UMID:");
    	atualizarValorLCD(option);
    	break;
    
    case 4: // LUZ
    	lcd_1.setCursor(0, 0);
    	lcd_1.print("LUZ:");
    	atualizarValorLCD(option);
    	break;
    
    case 5: // TEMPO DE SOL
    	break;
  }
}


/*ATUALIZAR INFORMAÇÃO DO SENSOR APRESENTADO*/
void atualizarValorLCD(int option)
{
  /*OPTIONS VALUES:
  1 = temperatura
  2 = umidade do ar
  3 = umidade do solo
  4 = luminosidade
  5 = tempo de sol*/
  //Limpando a linha de baixo do LCD
  lcd_1.setCursor(0, 1);
  lcd_1.print("                ");
  
  
  int position = 1;
  switch(option)
  {
    case 1: // TEMPERATURA
    	lcd_1.setCursor(0, 1);
    	lcd_1.print(tempAir);
    	position+=2;
    	if(tempAir < 0)
          position+=1; //Vai apresentar o sinal -
        if(tempAir/10 >= 1 || tempAir/-10 >= 1)
          position+=1; 
        if(tempAir/100 >= 1 || tempAir/-100 >= 1)
          position+=1;
        lcd_1.setCursor(position, 1);
        lcd_1.write(B10110010);
        lcd_1.print("C  ");
    	break;
    
    case 2: // UMIDADE DO AR
    	lcd_1.setCursor(0, 1);
    	lcd_1.print(umidadeAir);
    	if(umidadeAir/10 >= 1)
          position+=1; 
        if(umidadeAir/100 >= 1)
          position+=1;
    	lcd_1.setCursor(position, 1);
        lcd_1.print("%   ");
    	break;
    
    case 3: // UMIDADE DO SOLO
    	lcd_1.setCursor(0, 1);
    	lcd_1.print(umidadeSolo);
    	if(umidadeSolo/10 >= 1)
          position+=1; 
        if(umidadeSolo/100 >= 1)
          position+=1;
    	lcd_1.setCursor(position, 1);
        lcd_1.print("%   ");
    	break;
    
    case 4: // LUZ
    	lcd_1.setCursor(0, 1);
    	lcd_1.print(valueLDR);
    	lcd_1.print(" lux");
    	break;
    
    case 5: // TEMPO DE SOL
    	break;
  }
}


/*LIMPAR O CONTEUDO DO LCD*/
void LimparLCD() // Metodos usado para limpar a escrita do LCD para escrever um novo conteudo
{
  //Imprimindo no LCD
    lcd_1.setCursor(0, 0);
  	lcd_1.print("                ");
  	lcd_1.setCursor(0, 1);
  	lcd_1.print("                ");
}


