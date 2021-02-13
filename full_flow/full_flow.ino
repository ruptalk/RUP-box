#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Servo.h>

#define SERVER_IP "192.168.219.106:1000"

/*
#ifndef STASSID
#define STASSID "SK_WiFiGIGA32C0"
#define STAPSK  "2004073078"
#endif
*/

#ifndef STASSID
#define STASSID "U+NetAC43"
#define STAPSK  "4000020797"
#endif

int servoPin = 5;
int trigPin = 12;
int echoPin = 13;
long duration, distance;

Servo servo; 
int angle = 0; // servo position in degree

void setup()
{
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  Serial.begin(115200);       // 시리얼 속도 설정      
  pinMode(echoPin, INPUT);   // echoPin 입력    
  pinMode(trigPin, OUTPUT);  // trigPin 출력     
}
void loop(){
  digitalWrite(trigPin, HIGH);  // trigPin에서 초음파 발생(echoPin도 HIGH)        
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);    // echoPin 이 HIGH를 유지한 시간을 저장 한다.
  distance = ((float)(340 * duration) / 1000) / 2; 

  if(distance<750){
    //wait for WIFI connection
  if ((WiFi.status() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    http.begin(client, "http://" SERVER_IP "/php/Classification.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    Serial.print("[HTTP] POST...\n");
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");

        DynamicJsonDocument doc(32);
        deserializeJson(doc, payload);

        String check = doc["success"];
        Serial.println(check);
        if(check.equals("[2]")){
          servo.attach(servoPin);
          servo.write(0);
          delay(500);
          servo.detach();

          delay(3000);

          servo.attach(servoPin);
          servo.write(180);
          delay(500);
          servo.detach();
        }
      }
    } 
    else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  }
 
  Serial.print("Duration:");   //시리얼모니터에 Echo가 HIGH인 시간을 표시
  Serial.print(duration);
  Serial.print("\nDIstance:"); // 물체와 초음파 센서간 거리를 표시        
  Serial.print(distance);
  Serial.println("mm\n");
  delay(2000);

}
