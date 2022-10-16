#include <ESP8266WiFi.h>

#include <PubSubClient.h>
int closeAll = 1000;
const int openAll = 999;
const int closeNum = 100;
const int LED1 = 16;
const int LED2 = 5;
const int LED3 = 14;
const int LED4 = 0;
const int LED5 = 2;
const int LED6 = 4;
const int lightsLenght = 6;
const int lights[lightsLenght] = { LED1, LED2, LED3, LED4, LED5, LED6 };

const char* ssid = "MI_Chester";            //连接的路由器的名字
const char* password = "abcd35873";         //连接的路由器的密码
const char* mqtt_server = "192.168.2.192";  //服务器的地址
const int port = 1883;                      //服务器端口号
const char* TOPIC = "/led/light";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {  //自动连WIFI接入网络
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    // 没连上一直重试

    digitalWrite(LED1, HIGH);

    delay(500);

    digitalWrite(LED1, LOW);

    delay(500);

    Serial.println("wifi connecting...");
  }
  Serial.println("wifi connected success !");
  //常亮

  digitalWrite(LED1, LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {  //用于接收数据

  int l = 0;

  int p = 1;

  for (int i = length - 1; i >= 0; i--) {

    l += (int)((char)payload[i] - '0') * p;

    p *= 10;
  }

  Serial.print("\nreceived -> ");
  Serial.println(l);
  controlLights(l);
}

void controlLights(int l) {


  for (int i = 0; i < lightsLenght; i++) {

    if (l == lights[i]) {
      Serial.print("open gpio");
      Serial.print(l);
      Serial.print(" light");
      digitalWrite(lights[i], LOW);
    }

    if (l - closeNum == lights[i]) {
      Serial.print("close gpio");
      Serial.print(l - closeNum);
      Serial.print(" light");
      digitalWrite(lights[i], HIGH);
    }
  }
  if (l == closeAll) {
     Serial.print("close all gpio lights");
    closeAllLight();
  }
  if (l == openAll) {
    Serial.print("open all gpio lights");
    openAllLight();
  }
}

void closeAllLight() {
  for (int i = 0; i < lightsLenght; i++) {
    digitalWrite(lights[i], HIGH);
  }
}
void openAllLight() {
  for (int i = 0; i < lightsLenght; i++) {
    digitalWrite(lights[i], LOW);
  }
}

void reconnect() {  //等待，直到连接上服务器

  while (!client.connected()) {  //如果没有连接上

    digitalWrite(LED2, HIGH);

    delay(500);

    digitalWrite(LED2, LOW);

    delay(500);

    if (client.connect("jiahao_first_8266", "zjh", "Jjhjjh35873@")) {  //接入时的用户名，尽量取一个很不常用的用户名
      Serial.print("mqtt connected success!");
      client.subscribe(TOPIC);  //接收外来的数据时的intopic
      digitalWrite(LED2, LOW);

      digitalWrite(LED1, HIGH);

      digitalWrite(LED2, HIGH);
    } else {

      Serial.print("mqtt failed, rc=");  //连接失败

      Serial.print(client.state());  //重新连接
    }
  }
}

void setupLight() {
  for (int i = 0; i < lightsLenght; i++) {
    pinMode(lights[i], OUTPUT);
    digitalWrite(lights[i], HIGH);
  }
}

void setup() {  //初始化程序，只运行一遍

  setupLight();
  Serial.begin(9600);  //设置串口波特率（与烧写用波特率不是一个概念）

  setup_wifi();  //自动连WIFI接入网络

  client.setServer(mqtt_server, port);  //端口号

  client.setCallback(callback);  //用于接收服务器接收的数据
}

void loop() {  //主循环

  reconnect();  //确保连上服务器，否则一直等待。

  client.loop();  //MUC接收数据的主循环函数。
}