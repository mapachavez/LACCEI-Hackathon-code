#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

// Crear objeto de la clase
Adafruit_ADS1115 ads;

// Factor de escala. Por defecto el valor de referencia es 6,144V
// El factor de escala de esta referencia es de 0,1875mV
float factorEscala = 0.1875F;

String ssid     = "SSID";  // SSID to connect tu wifi
String password = "Password"; // Password

String host     = "api.thingspeak.com"; // Open Weather Map API
const int httpPort   = 80;
String apiKey     = "0A12L0L8XGUP39OP";

float voltios;
float voltios1;


String eventName = "Voltage Monitoring ";



int setupESP8266(void) {
  // Start our ESP8266 Serial Communication
  Serial.begin(115200);   // Serial connection over USB to computer
  Serial.println("AT");   // Serial connection on Tx / Rx port to ESP8266
  delay(10);        // Wait a little for the ESP to respond
  if (!Serial.find("OK")) return 1;
    
  // Connect to 123D Circuits Simulator Wifi
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  delay(10);        // Wait a little for the ESP to respond
  if (!Serial.find("OK")) return 2;
  
  
  return 0;
}

void setup(void)
{
  Serial.begin(9600);

  // Factor de escala
  ads.setGain(GAIN_TWOTHIRDS);

  // Iniciar el ADS1115
  ads.begin();
//setupESP8266();
   Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Se conectó !!!");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}



void loop()
{  
  WiFiClient client;
  const int httpPort= 80;

  if(!client.connect(host,httpPort)){
    Serial.println("Connection Failed");
    delay(300);
    return;
  }

  // Obtener el diferencial
  short diferencia_0_1 = ads.readADC_Differential_0_1();
  // Convertir en voltios 
  float voltios1 = (diferencia_0_1 * factorEscala)/1000.0;
  float voltios = voltios1 * 8.30;
  Serial.print("Diferencia 0-1 = "); 
  Serial.println(diferencia_0_1); 
  Serial.print("Voltage = ");
  Serial.println(voltios1,4);
   Serial.print("Voltage que se trasformó = ");
  Serial.println(voltios,4);
  Serial.println();
  
  String Link="GET /update?api_key="+apiKey+"&field1=";
  Link= Link + voltios;
  Link= Link + "HTTP/1.1\r\n"+"Host: "+host+"\r\n"+"Connection: close\r\n\r\n";
  client.print(Link);
  delay(5000);

  int timeout=0;
  while ((!client.available())&& (timeout <1000))
  {
    delay(10);
    timeout++;
  }

  if (timeout <500)
  {
    while(client.available()){
      Serial.println(client.readString());
    }

  }
  else
  {
    Serial.println("Request timeout..");
  }
  delay(5000);
}
