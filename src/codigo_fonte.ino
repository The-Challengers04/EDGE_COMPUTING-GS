#include <LiquidCrystal.h>

//Definindo os pinos dos sensores
#define UmidAirPin A0
#define TempAirPin A1
#define LuzPin A2
#define UmidSoloPin A3

//Definindo os pinos do atuadores
#define Bulb 11
#define Ventilador 10
#define Aquecedor 9
#define UmidificadorAr 8
#define Irrigador 7

//Iniciando Variaveis do Sistema
int seconds = 0;

int umidadeSolo = 0;
int umidadeAir = 0;
float tempAir = 0;
int valueLDR = 0;

int optionSelect = 1; // Tipo de informação a imprimir
long lastTime = millis(); // Ultima alteração do tipo de informação

// TempoSys = [Horas, Minutos, Segundos]

int TempoSys[3]; // Tempo do sistema
int TempoSol[3]; // Tempo do sol registrado pelo sistema
int TempoIdeal[3]; // Tempo ideal de sol para crescimento da planta
int TempoNascerSol[3]; // Horário que o sol nasce no local
int TempoPorSol[3]; // Horário que o sol se poem no local

long lastTimeSecondRecorded = 0;


boolean HighAirTemp = false;
boolean LowAirTemp = false;
boolean LowSoloUmid = false;
boolean LowAirUmid = false;
boolean LowSun = false;

boolean showSunTime = false; // Variável para definir se sera apresentado os lumens a receber ou o tempo de Sol
// O Tempo de Sol vai aparecer quando o sol se por, dessa forma vai ser a lampada auxiliar 



// PARAMETROS:
float umidadeSoloMin = 10;

float umidadeAirMin = 10;

float tempAirMax = 20;
float tempAirMin = 5;



long wait = 3 * 1000; // Intervalo entre as informações




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
  pinMode(Aquecedor,OUTPUT);
  pinMode(UmidificadorAr,OUTPUT);
  pinMode(Irrigador,OUTPUT);
  
  imprimir(optionSelect);

  for(int i = 0; i < 3; i++)
  {
    TempoSol[i] = 0;
  }
  /*
  EXEMPLO DE ARRAYS DE TEMPO
  array[0] = Horas
  array[1] = Minutos
  array[2] = Segundos
*/

  // Sistema foi iniciado as 13:00
  TempoSys[0] = 17;
  TempoSys[1] = 59;
  TempoSys[2] = 0;

  // Sol nasce as 6:00
  TempoNascerSol[0] = 6;
  TempoNascerSol[1] = 0;
  TempoNascerSol[2] = 0;

  // Sol se poem as 18:00
  TempoPorSol[0] = 18;
  TempoPorSol[1] = 0;
  TempoPorSol[2] = 0;

  // Tempo ideal de sol para a planta é 4hrs e 30min
  TempoIdeal[0] = 0;
  TempoIdeal[1] = 2;
  TempoIdeal[2] = 0;

}

void loop()
{
  // ATUALIZANDO VALORES DOS SENSORES
  umidadeSolo = readUmidadeSolo();
  umidadeAir = readUmidadeAir();
  tempAir = readTemp();
  valueLDR = readLuz();

  boolean isSunny = false;
  if(valueLDR > 300)
    isSunny = true;

  // Atualizando Atuadores
  HighAirTemp = (tempAir > tempAirMax);
  LowAirTemp = (tempAir < tempAirMin);
  LowSoloUmid = (umidadeSolo < umidadeSoloMin);
  LowAirUmid = (umidadeAir < umidadeAirMin);
  
  ativarAtuadores();
  
  if(TempoMaior(TempoSys, TempoPorSol)){ // O Sol se Pos
    if(TempoMaior(TempoIdeal, TempoSol))
    {
      isSunny = true;
      LowSun = true;
    }
    else
    {
      isSunny = false;
      LowSun = false;
    }
    showSunTime = true;
  }
  
  
  // ATUALIZANDO VALORES DOS TEMPOS
  AtualizarRelogio(isSunny);
  
  
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

void AtualizarRelogio(boolean isSunny){
	if(millis() - lastTimeSecondRecorded >= 1000)
    {
      TempoSys[2] += 1; // + Segundos
      if(TempoSys[2] >= 60)
      {
        TempoSys[2] = 0;
        TempoSys[1] = TempoSys[1] + 1; // + Minutos
        if(TempoSys[1] >= 60)
        {
          TempoSys[1] = 0;
          TempoSys[0] = TempoSys[0] + 1; // + Horas
          if(TempoSys[0] >= 24)
          {
            TempoSys[0] = 0;
            // Reiniciando tempo de sol
            TempoSol[2] = 0;
            TempoSol[1] = 0;
            TempoSol[0] = 0;
          }
        }
      }
      // Atualizando tempo de sol
      if(isSunny)
      {
        TempoSol[2] = TempoSol[2] + 1; // + Segundos
        if(TempoSol[2] >= 60)
        {
          TempoSol[2] = 0;
          TempoSol[1] = TempoSol[1] + 1; // + Minutos
          if(TempoSol[1] >= 60)
          {
            TempoSol[1] = 0;
            TempoSol[0] = TempoSol[0] + 1; // + Horas
            if(TempoSol[0] >= 24)
            {
              TempoSol[0] = 0;
            }
          }
        }
      }

      lastTimeSecondRecorded += 1000; // Adiciona mais um segundo, dessa forma se entrar com o tempo quebrado é possível ajustar na proxima
      Serial.print(TempoSys[0]);
      Serial.print("hrs ");
      Serial.print(TempoSys[1]);
      Serial.print("min ");
      Serial.print(TempoSys[2]);
      Serial.println("sec ");

    }
}

void ativarAtuadores(){
  if(HighAirTemp){
    digitalWrite(Ventilador, HIGH);
  }
  else if(LowAirTemp){
    digitalWrite(Aquecedor, HIGH);
  }

  if(LowSoloUmid){
    digitalWrite(Irrigador, HIGH);
  }
  if(LowAirUmid){
    digitalWrite(UmidificadorAr, HIGH);
  }
  if(LowSun){
    digitalWrite(Bulb, HIGH);
  }

  if(!HighAirTemp){
    digitalWrite(Ventilador, LOW);
  }
  else if(!LowAirTemp){
    digitalWrite(Aquecedor, LOW);
  }
  
  if(!LowSoloUmid){
    digitalWrite(Irrigador, LOW);
  }
  if(!LowAirUmid){
    digitalWrite(UmidificadorAr, LOW);
  }
  if(!LowSun){
    digitalWrite(Bulb, LOW);
  }
}

boolean TempoMaior(int Tempo1[], int Tempo2[])
{
  if(Tempo1[0] > Tempo2[0])
    return true; // Hora é maior
  else if(Tempo1[0] == Tempo2[0])
  {
    if(Tempo1[1] > Tempo2[1])
      return true; // Hora igual e minuto maior
    else if(Tempo1[1] == Tempo2[1])
    {
      if(Tempo1[2] > Tempo2[2])
        return true; // Hora e minutos iguais com segundos maiores
      else
        return false; // Hora, Minutos e Segundos iguais ou menores
    }
    else
      return false; // Hora igual mas minutos menores
  }
  else
    return false; // Hora menor
}

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
    // Verificar se vai ser apresentado o lumens ou o tempo de sol registrado ate o momento
      if(!showSunTime)
      {
    	lcd_1.setCursor(0, 0);
    	lcd_1.print("LUZ:");
    	atualizarValorLCD(option);
    	break;
      }else
      {
        lcd_1.setCursor(0, 0);
        lcd_1.print("Tempo Sol:");
        atualizarValorLCD(option);
      }
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
      if(!showSunTime){
        lcd_1.setCursor(0, 1);
        lcd_1.print(valueLDR);
        lcd_1.print(" lux");
        break;
      }else
      {
        lcd_1.setCursor(0, 1);
        lcd_1.print(TempoSol[0]);
        lcd_1.print("hrs ");
        lcd_1.print(TempoSol[1]);
        lcd_1.print("mins");
      }
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


