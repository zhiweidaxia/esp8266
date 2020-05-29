
#include <ESP8266WiFi.h>
#include <Ticker.h> 
#ifndef STASSID

#endif
//192.168.43.1 8080
//"183.230.40.39" 6002
const char* ssid     = "你的wifi";//wifi名
const char* password = "你的密码";//wifi密码
const char* host = "183.230.40.39";//ONENET服务器ip
const uint16_t port = 6002;//端口
const char* deviceId = "设备ID";//设备ID
const char* productId = "产品ID";//产品ID
const char* API_key = "Master key";//APIkey


/* 0x10 设置连接模式为Connect模式
 * 0x3B 为指定数据长度，是一个16进制数据，0x3B在十进制下为59 
 * 0x00 0x04 协议名称长度，这里确认协议名称长度为4
 * 0x4D 0x51 0x54 0x54 前面的0x00 0x04字节确定协议名称长度，后面的四个0x4D , 0x51 , 0x54 , 0x54在ASCII码分别代表“M”，“Q”，“T”，“T”；
 * 0x04 为MQTT的版本号，本次的MQTT协议版本号为MQTT v3.1.1，根据MQTT中对它的定义，它是0x04 
 * 0xC0 参考onenet MQTT协议参考手册为需要账户和密码登入
 * 0x00 0x10 用户自定义连接时间，此处的0x10根据十进制算就是16秒；
*/
unsigned int oneNet_head[12]={0x10,0x3B,0x00,0x04,0x4D,0x51,0x54,0x54,0x04,0xC0,0x00,0x20};
/*
 * 0x00 0x09 设备ID的长度为9
 * 0x36 0x30 0x30 0x30 0x31 0x38 0x31 0x33 0x39 ASCII表示设备ID：600018139
 */
unsigned int oneNet_device_len[]={0x00,0x09};
/*
 * 0x00 0x06 产品ID长度为6
 * 0x33 0x34 0x38 0x37 0x34 0x39 代表产品ID：348749
 */
unsigned int oneNet_product_len[]={0x00,0x06};
/*
 * 0x00 0x1C APIKEY长度
 * 0x41 0x69 0x59 0x35 0x53 0x6D 0x69 0x49 0x59 0x4B 0x6D 0x55 0x35 0x7A 0x46 0x74 0x48 0x74 0x63 0x70 0x4A 0x36 0x55 0x65 0x54 0x65 0x67 0x3D 
 * 代表APIkey：AiY5SmiIYKmU5zFtHtcpJ6UeTeg=
 */
unsigned int oneNet_APIkey_len[]={0x00,0x1C};
/*
{
"datastreams":[
{"id": "h","datapoints" : [{"value":   1}]},
{"id": "t","datapoints" : [{"value":  03}]},
{"id": "Intelligentfloor","datapoints" : [{"value":  2}]}
]
}
 */
unsigned int oneNet_Pubish_head[9]={
  0x30,//模式位publish
  0xB7,//暂定183位数据
  0x01,//一个组发送
  0x00,0x03,//三个head头文件
  0x24,0x64,0x70,//字符串形式为$dp，onenet上传命令符号
  0x01,//表示一个单元
};
String target = "     ";
String datastreams = "{\"datastreams\":[";
String hdata = "{ \"id\":  \"h\", \"datapoints\" : [{ \"value\":     }]},";
String tdata = "{ \"id\":  \"t\", \"datapoints\" : [{ \"value\":     }]},";
String idata = "{ \"id\":  \"Intelligentfloor\", \"datapoints\" : [{\"value\":     }]}";
String ended = "]}";

WiFiClient client;
Ticker flipper;
void ConnectOneNet();
void flip();
void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  //delay(5000);
  ConnectOneNet();
  delay(5000);
  flipper.attach(10, flip);
}

void ConnectOneNet(){
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }
  int jj;
  Serial.println("sending data to server");

  if (client.connected()) {
    /*设备头*/
    //先发送连接头
    for(jj=0;jj<12;jj++)
      client.write(oneNet_head[jj]);

     /*设备ID*/ 
     //发送设备ID长度
    for(jj=0;jj<2;jj++)
      client.write(oneNet_device_len[jj]);
      //发送设备ID
      client.print(deviceId);

      /*产品ID*/ 
     //发送产品ID长度
     for(jj=0;jj<2;jj++)
      client.write(oneNet_product_len[jj]);
    //发送产品ID
    client.print(productId);

    /*APIkey*/ 
    //发送APIkey长度
    for(jj=0;jj<2;jj++)
      client.write(oneNet_APIkey_len[jj]);
    //发送APIkey
    client.print(API_key);

    
    Serial.println("over");
      delay(1000);
  }
  else{
    Serial.println("OneNET connect Fall");
  }

}
String ht = "99";
String tt = "99";
String it = "99";
void oneNetSend(String hdd,String tdd,String idd,String xh,String xt,String xi){
  int jj;
  Serial.println("\n\nString ht,String tt,String it");
  Serial.println(xh);
  Serial.println(xt);
  Serial.println(xi);
  hdd.replace(target,xh);
  tdd.replace(target,xt);
  idd.replace(target,xi);
  
  String buff=datastreams+hdd+tdd+idd+ended;
  //buff.trim();
  int jsonlen = buff.length();


  
  oneNet_Pubish_head[1]=jsonlen+9-3+2;
  if (client.connected()) {
    /*设备头*/
    //先发送连接头
    for(jj=0;jj<9;jj++){
      client.write(oneNet_Pubish_head[jj]);
      Serial.write(oneNet_Pubish_head[jj]);
    }
    if(jsonlen<=255){
      client.write(0x00);
      client.write(jsonlen);
      Serial.println("\n\ndata len");
      Serial.write(0x00);
      Serial.write(jsonlen);
    }else{
      client.write(jsonlen);
      Serial.println("\n\ndata len");
      Serial.println(jsonlen);
      Serial.write(jsonlen);
    }
    client.print(buff);
    Serial.print(buff);
    Serial.println("Over");
  }
}
int count=0;
void flip() {     //回调函数
  int state = digitalRead(LED_BUILTIN);  
  digitalWrite(LED_BUILTIN, !state);  
  ++count;
  Serial.print("Timer-------------------:");
  Serial.println(count);
  oneNetSend(hdata,tdata,idata,ht,tt,it);
}



void loop() {
 

  String serialget=Serial.readString();
  if(!serialget.equals("")){
      Serial.print("\r\nGetting Data");
      Serial.println(serialget.indexOf('>'));
      if(serialget.indexOf('>')!=0){
        Serial.println(serialget);
        ht = serialget.substring(serialget.indexOf('h')+1,serialget.indexOf('t'));
        Serial.println(ht);
        tt = serialget.substring(serialget.indexOf('t')+1,serialget.indexOf('f'));
        Serial.println(tt);
        it = serialget.substring(serialget.indexOf('f')+1,serialget.indexOf('>'));
        Serial.println(it);
      }else{Serial.println("\r\nData is error");}
  }else{Serial.println("\r\nNothing");}
  delay(100);
}
