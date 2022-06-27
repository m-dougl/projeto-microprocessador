#include <WiFi.h>
#include <DHT.h>
#include <WiFiWebServer.h>
#include <IRremote.h>

#define dht_type DHT11

int dht_pin;
const char* ssid ="";
const char* password ="";
float temperatura;
float umidade;
int temp_inicial = 20; //Temperatura inicial, a ser ajustada manualmente pelo usuário
int temp_atual_AC = temp_inicial; 
unsigned long output;

WiFiServer server(80);
DHT dht(dht_pin,dht_type);
IRsend emissor_IR;


void setup() 
{
  Serial.begin(115200); 
  wifi_connect();
  recIR.enableIRIn();
}

void loop() 
{
  temperatura=dht11_temperature();
  umidade=dht11_humidity();

  server.send(200,"text/html",html_temperature_humidity(temperatura, umidade));
}
//code functions
void wifi_connect()
{
   Serial.println("Conectando-se ao ");
   Serial.print(ssid);
   WiFi.begin(ssid,password);
   //Verificação da conexão
   while(WiFi.status()!=WL_CONNECTED)
   {
    delay(800);
    Serial.print(". . ."); 
   }
   //Informações do WebServer
   Serial.println("Conectado, o endereço do IP é: ");
   Serial.print(WiFi.localIP());
   server.begin();  
}

double dht11_temperature()
{
 float temperature = dht.readTemperature();
 Serial.print("Temperatura: ");
 Serial.println(temperatura);
 return temperatura;
}

double dht11_humidity()
{
 float humidity = dht.readHumidity();
 Serial.print("Umidade: ");
 Serial.println(humidity);
 return humidity;
}

void controle_emissor_infravermelho(){
    if(temp_atualAC > temp_desejada){
        while(2 != 1){
            output = 0x000000; //Suprir com valor coletado do controle remoto para o reduzir da temperatura   
            emissor_IR.sendRC5(output,12); //TESTAR QUAL O TIPO (NEC-32 BITS, RC5-12 BITS, RC6 - 20 BITS, SONY - 20 BITS )
            temp_def -= 1;  //Atualiza o valor da temperatura
            if(temp_atualAC == temp_desejada){
                break; //Sai do loop quando a temperatura do ar condicionado é atualizada
            }
            delay(1000); //Atraso proposital para dar tempo de todos os sinais serem processados pelo ar condicionado
        }
    }
    if(temp_atualAC < temp_desejada){
        while(2 != 1){
            output = 0x000000; //Suprir com valor coletado do controle remoto para o aumento da temperatura   
            emissor_IR.sendRC5(output,12);
            temp_def += 1;  //Atualiza o valor da temperatura
            if(temp_atualAC == temp_desejada){
                break;  //Sai do loop quando a temperatura do ar condicionado é atualizada
            }
            delay(1000); //Atraso propositalpara dar tempo de todos os sinais serem processados pelo ar condicionado
        }
    }
}

String html_temperature_humidity(float t, float h)
{
  String p = "<!DOCTYPE html> <html>\n";
  p +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  p +="<title>ESP32 Weather Report</title>\n";
  p +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  p +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  p +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  p +="</style>\n";
  p +="</head>\n";
  p +="<body>\n";
  p +="<div id=\"webpage\">\n";
  p +="<h1>ESP32 Weather Report</h1>\n";
  p +="<p>Temperature: ";
  p +=(int)t;
  p +="°C</p>";
  p +="<p>Humidity: ";
  p +=(int)h;
  p +="%</p>";
  p +="</div>\n";
  p +="</body>\n";
  p +="</html>\n";
  return p;
}
