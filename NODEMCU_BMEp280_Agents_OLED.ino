#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

//BME/P 280
#define HPA_PRESSAO (1013.25)
Adafruit_BME280 bme;
float temperatura, umidade, pressao_atm;

//OLED
Adafruit_SSD1306 display(-1);

//------------------------------------------SETUP----------------------------
void setup() {
  Serial.begin(9600);

  //BME/P 280
  bme.begin(0x76); 

  //OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,1);
  display.println("Starting..");
  display.display();
  delay(500);

  //WIFI
  conectarWiFi("xxxx","xxxx");
  exibirOLED("Aguardando...");
}
//------------------------------------------ LOOP ----------------------------
void loop() {
  delay(5000);
  captarSinaisAmbiente();
  exibirOLED(temperatura,umidade,pressao_atm,0);
  enviarDadosWebService(temperatura,umidade,pressao_atm);
  exibirOLED(temperatura,umidade,pressao_atm,1);
  delay(120000);  //2minutos
}

//------------------------------------------FUNCOES----------------------------
void captarSinaisAmbiente(){
  float t[5], u[5], p[5];
  for(int i=0; i<5; i++){
    t[i] = bme.readTemperature();
    u[i] = bme.readHumidity();
    p[i] = bme.readPressure() / 100.0F;
    delay(300);
  }
  temperatura = (t[0]+t[1]+t[2]+t[3]+t[4])/5;
  umidade = (u[0]+u[1]+u[2]+u[3]+u[4])/5;
  pressao_atm = (p[0]+p[1]+p[2]+p[3]+p[4])/5;
}
//----------------------------------------------------
void exibirOLED(float t, float u, float p){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,1);
  display.println("Temperatura: "+String(t)+"oC");
  display.setCursor(0,13);
  display.println("Umidade: "+String(u)+"%");  
  display.setCursor(0,25);
  display.println("P. Atm.: "+String(p)+"hPa");    
  display.display(); 
  delay(5000);
}
//----------------------------------------------------
void exibirOLED(float t, float u, float p, int i){
  if(i==0){
    exibirOLED(temperatura,umidade,pressao_atm);
  }else{
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,1);
      display.println("T:"+String(t)+"oC");
      display.setCursor(65,1);
      display.println("U:"+String(u)+"%");  
      display.setCursor(1,13);
      display.println("P:"+String(p)+"hPa");   
      display.setCursor(1,25);
      display.println("Aguardando...");              
      display.display();    
  }
  
}
//----------------------------------------------------
void exibirOLED(String msg){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,1);
  display.println(msg); 
  display.display(); 
  delay(3000);
}
//----------------------------------------------------
void conectarWiFi(String user, String pass){
 WiFi.begin(user,pass);
  while (WiFi.status() != WL_CONNECTED) {  
    exibirOLED("Conectando...");
  }
  if(WiFi.status() == WL_CONNECTED){
      exibirOLED("ConectadoWiFi!");
  }    
}
//----------------------------------------------------
void enviarDadosWebService(float t, float u, float p){
  if(WiFi.status()== WL_CONNECTED){ 
     exibirOLED("Enviando...");
     HTTPClient http;

     http.begin("http://balthazar83.jelastic.saveincloud.net/ROOT-xxx/GravarRegistroService.do?p="+String(p)+"&u="+String(u)+"&t="+String(t));
     http.addHeader("Content-Type", "text/plain");
   
     int httpCode = http.POST("Message from ESP8266"); 
     String payload = http.getString();                
     http.end();  
     exibirOLED("Enviado!");
   }else{
      Serial.println("Error in WiFi connection");   
      exibirOLED("Erro no envio!");
   } 
}
