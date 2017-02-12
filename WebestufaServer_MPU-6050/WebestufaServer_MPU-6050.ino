
//Programa : Arduino Ethernet Shield W5100 e GY-521 (MPU-6050)
//Código Fonte de Referência: https://www.embarcados.com.br/arduino-acelerometro-giroscopio/
//Adaptação: Marcelo | Webestufa


#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>


//Endereco I2C do MPU6050
const int MPU=0x68;  //pino aberto 0X68 , pino ligado em 3,3V 0x69
 
//Variaveis globais
int acelX,acelY,acelZ,temperatura,giroX,giroY,giroZ;


//Definicoes de IP, mascara de rede e gateway
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0,99);          //Define o endereco IP
IPAddress gateway(192,168,0,1);      //Define o gateway
IPAddress subnet(255, 255, 255, 0); //Define a máscara de rede
//Inicializa o servidor web na porta 80
EthernetServer server(80);


void setup() {

  Serial.begin(9600); //inicia a comunicação serial
  Wire.begin();                 //inicia I2C
  Wire.beginTransmission(MPU);  //Inicia transmissão para o endereço do MPU
  Wire.write(0x6B);             
   
  //Inicializa o MPU-6050
  Wire.write(0); 
  Wire.endTransmission(true);
  
  //Inicializa a interface de rede
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();

  
}


  void loop() {

  Wire.beginTransmission(MPU);      //transmite
  Wire.write(0x3B);                 // Endereço 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);     //Finaliza transmissão
  
  Wire.requestFrom(MPU,14,true);   //requisita bytes
   
  //Armazena o valor dos sensores nas variaveis correspondentes
  acelX=Wire.read()<<8|Wire.read();  //0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  acelY=Wire.read()<<8|Wire.read();  //0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  acelZ=Wire.read()<<8|Wire.read();  //0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  temperatura=Wire.read()<<8|Wire.read();  //0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  giroX=Wire.read()<<8|Wire.read();  //0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  giroY=Wire.read()<<8|Wire.read();  //0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  giroZ=Wire.read()<<8|Wire.read();  //0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
    
  //Aguarda conexao do browser  
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == 'n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 2"); //Recarrega a pagina a cada 2seg
          client.println();
          client.println("<!DOCTYPE HTML>");
          //INICIO DO CODIGO HTML

          client.println("<HTML>");
          client.println("<head><Webestufa</title></head>");
          client.println("<BODY>");
          client.println("<br><br><center><a href=\"http://www.webestufa.com.br\"> <img align=center width=350px heigh=150px  src=https://lh3.googleusercontent.com/-GGIuiX7uaG4/V5frN70j5cI/AAAAAAAAAh8/h7rBqeX_d_UEqC_EDjHDqFUOU_OcX6sLgCCo/s128/logowebestufa.gif></center></br></br>");
          client.println("<H1><center>Arduino + Ethernet Shield W5100 + Acelerometro e Giroscopio</center></H1>");
          client.println("<br />");
          client.println("<br />");
          client.println("<h2><center>");
          
          //Valores lidos do Acelerômetro
          client.print("<br><br><center>Acelerometro :");
          client.print("   X: ");client.print(acelX);
          client.print("\tY: ");client.print(acelY);
          client.print("\tZ: ");client.print(acelZ);
          client.println("<br />");
          client.println("<br />");
                   
          //Envia valores lidos do giroscópio
          client.print("\tGiroscopio:"); 
          client.print("  X:");client.print(giroX);
          client.print("\tY:");client.print(giroY);
          client.print("\tZ:");client.print(giroZ);

              
          //Temperatura em graus Celsius
          client.print("\tTemperatura: ");client.println(temperatura/340.00+36.53);client.print("*C");
                  
          //Aguarda 500 ms
          delay(500);
          
          client.println("</center></h2>");
          client.println("<br>");

          break;
        }
        if (c == 'n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != 'r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();

    
    }
}

