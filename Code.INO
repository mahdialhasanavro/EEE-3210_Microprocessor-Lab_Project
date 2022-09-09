//Project-AGRISTAND
//MAHDI HASAN

#include <SoftwareSerial.h>
#include <DHT.h>
#include <SFE_BMP180.h>

char status;
double T , P ;


#define dhtpin A0
#define dhttype DHT11
#define rl 10
#define gl 9

#define moisturepin A1
#define cmoisturepin A2
#define rainpin A3

#define RX 2
#define TX 3

String AP = "Mehedi";       // Wifi NAME
String PASS = "mehedihasan"; // Wifi PASSWORD
String API = "WLOI9GTSM35PX4QJ";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;

SoftwareSerial esp8266(RX,TX); 

DHT dht(dhtpin,dhttype);

SFE_BMP180 bmp;
 
  
void setup() 
{
  Serial.begin(9600); 
  esp8266.begin(115200); 
  dht.begin();
  bmp.begin();
  
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  pinMode (rl, OUTPUT);
  pinMode(gl,OUTPUT);

  if(sendCommand)
  {
    digitalWrite(rl, HIGH);
  }
  else 
  {
    digitalWrite(rl, LOW);
  }
}

void loop()
{
 
 String getData = "GET /update?api_key="+ API +"&field1="+getTemperaturedata()+"&field2="+getHumiditydata()+"&field3="+getMoisturedata()+"&field4="+getCmoisturedata()+"&field5="+getRaindata()+"&field6="+getPressuredata();
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");

 if (sendCommand)
 {
  digitalWrite(gl,HIGH);
  delay(100);
  digitalWrite(gl,LOW);
 }
}

String getTemperaturedata()
{

  int temp = dht.readTemperature();
  //Serial.println (temp);
 // delay(50);
  return String(temp); 
}


String getHumiditydata()
{
  int hume= dht.readHumidity();
  //Serial.println (hume);
  //delay(50);
  return String(hume);
}



String getMoisturedata()
{
  int moistureval= analogRead(moisturepin);
  //Serial.println(moistureval);
  //delay(50);
  int moistureval2 = constrain (moistureval , 370 , 1013);
  int moistureval3 = map (moistureval2, 370, 1013 , 100 , 0);
  return String(moistureval3);
}

String getCmoisturedata()
{
  int cmoistureval = analogRead (cmoisturepin);
  //Serial.println (cmoistureval);
  //delay(50);
  int cmoistureval2 = constrain (cmoistureval, 960,1000);
  int cmoistureval3 = map (cmoistureval2 , 960 , 1000 , 100 ,0);
  return String (cmoistureval3);
}

String getRaindata()
{
  int rainval = analogRead (rainpin);
  //Serial.println(rainval);
  //delay(50);
  int rainval2 = constrain (rainval,200,1023);
  int rainval3 = map (rainval2, 200 ,1023 , 100 , 0);
  return String (rainval3);
}

String getPressuredata()
{

status = bmp.startTemperature();
delay(status);
bmp.getTemperature(T);
status = bmp.startPressure(3);
delay(status);
bmp.getPressure(P,T);

int pres = P * 0.1 ; // for KiloPasskel
//Serial.println(pres);
//delay(50);

return String (pres);

}

  void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
