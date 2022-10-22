#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
const int LED1 = 16;
const int LED2 = 5;
const int recv_pin = 4;

DHT dht(recv_pin, DHT11);
const char* ssid = "MI_Chester";            //连接的路由器的名字
const char* password = "abcd35873";         //连接的路由器的密码
const char* mqtt_server = "192.168.2.192";  //服务器的地址
const int port = 1883;                      //服务器端口号
const char* TOPIC = "/8266/publish";
const char* publish_topic = "/8266/dht/recv";
const char* mqtt_id = "jiahao_second_8266";
const char* mqtt_username = "zjh";
const char* mqtt_password = "Jjhjjh35873@";

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

  String s = "";

  for (int i = 0; i < length; i++) {
    s.concat((char)payload[i]);
  }
  for (int i = length - 1; i >= 0; i--) {
    l += (int)((char)payload[i] - '0') * p;
    p *= 10;
  }
  Serial.println(s);
  if (s == "read_dht") {

    String h = "[";
    h.concat(dht.readHumidity(true));
    h.concat(",");
    h.concat(dht.readTemperature());
    h.concat("]");
    Serial.println(h.c_str());
    client.publish(publish_topic, h.c_str());
  }
  Serial.println("received -> ");
  Serial.println(l);
}

void reconnect() {  //等待，直到连接上服务器

  while (!client.connected()) {  //如果没有连接上

    digitalWrite(LED2, HIGH);

    delay(500);

    digitalWrite(LED2, LOW);

    delay(500);


    if (client.connect(mqtt_id, mqtt_username, mqtt_password)) {
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

void setup() {  //初始化程序，只运行一遍
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(recv_pin, INPUT);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  dht.begin();
  
  Serial.begin(9600);  //设置串口波特率（与烧写用波特率不是一个概念）

  setup_wifi();  //自动连WIFI接入网络

  client.setServer(mqtt_server, port);  //端口号

  client.setCallback(callback);  //用于接收服务器接收的数据
}

void loop() {
  reconnect();    //确保连上服务器，否则一直等待。
  client.loop();  //MUC接收数据的主循环函数。
}

char* appendChar(char* prefix, char suffix) {

  char* dest;
  dest = new char[strlen(prefix) + 2];  //因为加了一个字符，还要有一个结束符
  strcpy(dest, prefix);
  dest[strlen(prefix)] = suffix;
  dest[strlen(prefix) + 1] = 0;
  return dest;
}