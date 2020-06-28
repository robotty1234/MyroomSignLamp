/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
/*********
 上記のURLにあるサイトのプログラムを基に一部仕様を変えて書き直しました
*********/
// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid = "????";//接続先のSSIDを入力
const char* password = "????";//接続先のパスワードを入力

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String outputLEDState = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int LED = 14;//ランプLED制御ピン
const int wifiLED = 4;//wifi接続確認用LED制御ピン

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(LED, OUTPUT);
  pinMode(wifiLED, OUTPUT);
  // Set outputs to LOW
  digitalWrite(LED, LOW);

  // Connect to Wi-Fi network with SSID and password
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
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            //client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /LED/on") >= 0) {
              Serial.println("LED on");
              outputLEDState = "on";
              digitalWrite(LED, HIGH);
            } else if (header.indexOf("GET /LED/off") >= 0) {
              Serial.println("LED off");
              outputLEDState = "off";
              digitalWrite(LED, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\n");
            client.println("<html lang=\"ja\">\n");
            client.println("<head>\n");
            client.println("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n");
            client.println("<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">\n");
            client.println("<title></title>\n");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: black; border: none; color: white; padding: 32px 80px;");
            client.println("text-decoration: none; font-size: 60px; margin: 4px; cursor: pointer;}");
            client.println(".button2 {background-color: red;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1><span style=font-size:2.5em;>作業中!入室注意ランプ</span></h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p><span style=font-size:3.0em;>ランプ点灯状態</span></p>");
            // If the output5State is off, it displays the ON button       
            if (outputLEDState=="off") {
              //client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
              client.println("<p><a href=\"/LED/on\"><button class=\"button\">入室注意解除</button></a></p>");
            } else if (outputLEDState=="on"){
              //client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
              client.println("<p><a href=\"/LED/off\"><button class=\"button button2\">入室注意中！！！</button></a></p>");
            }

            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
