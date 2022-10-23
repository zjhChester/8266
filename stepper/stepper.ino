#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <Stepper.h>

DHT dht(14, DHT11);

Stepper stepper(32, 16, 4, 5, 0);  //1,3,2,4

const char* ssid = "MI_Chester";            //连接的路由器的名字
const char* password = "abcd35873";         //连接的路由器的密码
const char* mqtt_server = "192.168.2.192";  //服务器的地址
const int port = 1883;

const char* TOPIC = "/8266/publish";
const char* RECV_TOPIC = "/8266/dht/recv";
const char* mqtt_id = "jiahao_third_8266";
const char* mqtt_username = "zjh";
const char* mqtt_password = "Jjhjjh35873@";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {  //自动连WIFI接入网络
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    // 没连上一直重试
    delay(500);
    Serial.println("wifi connecting...");
  }
  Serial.println("wifi connected success !");
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
  controlStepper(s);
  getDht(s);
}

void getDht(String s){
    if (s == "read_dht"){
      String str ="[";
      str.concat(dht.readHumidity(true));
      str.concat(",");
      str.concat(dht.readTemperature());
      str.concat("]");
      client.publish(RECV_TOPIC, str.c_str());
      String result = "采集温湿度成功: ";
      result.concat(str);
      Serial.println(result);
    }
}

void controlStepper(String s){
  if (s == "stepper+") {
    Serial.println("stepper + running ... ");
    for(int i=0; i<55;i++){
      delay(50);
    stepper.step(128);
    }
  }
   if (s == "stepper-") {
    Serial.println("stepper - running ... ");
    for(int i=0; i<55;i++){
      delay(50);
    stepper.step(-128);
    }
  }
}

void reconnect() {  //等待，直到连接上服务器

  while (!client.connected()) {  //如果没有连接上

    if (client.connect(mqtt_id, mqtt_username, mqtt_password)) {  //接入时的用户名，尽量取一个很不常用的用户名
      client.subscribe(TOPIC); 
      Serial.println("mqtt connected success!");
      stepper.setSpeed(500);
      Serial.println("stepper setting success!");
      dht.begin();
      Serial.println("dht setting success!");
    } else {
      Serial.print("mqtt failed, rc=");  //连接失败
      Serial.print(client.state());      //重新连接
    }
  }
}


void setup() {                          //初始化程序，只运行一遍
  Serial.begin(9600);                   //设置串口波特率（与烧写用波特率不是一个概念）
  setup_wifi();                         //自动连WIFI接入网络
  client.setServer(mqtt_server, port);  //端口号
  client.setCallback(callback);         //用于接收服务器接收的数据
}

void loop() {  //主循环
  reconnect();    //确保连上服务器，否则一直等待。
  client.loop();  //MUC接收数据的主循环函数。
}