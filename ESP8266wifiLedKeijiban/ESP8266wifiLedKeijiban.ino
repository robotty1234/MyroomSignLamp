#include <ESP8266WiFi.h>

#define LED 14
#define wifiLED 4

const char* ssid = "????";//接続先のSSIDを入力
const char* password = "????";//接続先のパスワードを入力

WiFiServer server(80);

String header;

String outputLEDState = "off";
String output4State = "off";

unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(wifiLED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int overe = 0;
  while (WiFi.status() != WL_CONNECTED && overe < 20) {
    delay(500);
    digitalWrite(wifiLED,HIGH);
    delay(250);
    digitalWrite(wifiLED,LOW);
    delay(250);
    Serial.print(".");
    overe++;
  }
  while(overe >= 20){
    Serial.println("not conect");
    delay(1000);
  }
  digitalWrite(wifiLED,HIGH);
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();         
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            
            if (header.indexOf("GET /LED/on") >= 0) {
              Serial.println("LED on");
              outputLEDState = "on";
              digitalWrite(LED, HIGH);
            } else if (header.indexOf("GET /LED/off") >= 0) {
              Serial.println("LED off");
              outputLEDState = "off";
              digitalWrite(LED, LOW);
            }

            client.println("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\n");
            client.println("<html lang=\"ja\">\n");
            client.println("<head>\n");
            client.println("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n");
            client.println("<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">\n");
            client.println("<title></title>\n");
            
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: black; border: none; color: white; padding: 32px 80px;");
            client.println("text-decoration: none; font-size: 60px; margin: 4px; cursor: pointer;}");
            client.println(".button2 {background-color: red;}</style></head>");
            
            client.println("<body><h1><span style=font-size:2.5em;>作業中!入室注意ランプ</span></h1>");
            client.println("<p><span style=font-size:3.0em;>ランプ点灯状態</span></p>");   
            if (outputLEDState=="off") {
              client.println("<p><a href=\"/LED/on\"><button class=\"button\">入室注意解除</button></a></p>");
            } else if (outputLEDState=="on"){
              client.println("<p><a href=\"/LED/off\"><button class=\"button button2\">入室注意中！！！</button></a></p>");
            }

            client.println("</body></html>");
            
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
