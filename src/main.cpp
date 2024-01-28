

/* ESP32 HTTP IoT Server 
 *
 *
 * 
 * 
 * 
 * 
 * 
 * 
*/
#include "Hardware.h"
#include "Log_Data.h"
#include "CMessdaten.h"

#include <WiFi.h>
#include <WiFiAP.h>
#include <ESPmDNS.h>

#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include <SPIFFS.h>
#include <time.h>

const char* MacGlaspalst   = "E0:5A:1B:C7:F6:64";
const char* MacLokschuppen = "E0:5A:1B:C9:81:F4";
const char* Version = "1.11.b";
const char* NTP_SERVER  = "de.pool.ntp.org";
const char* TZ_INFO     = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";


Geraete ESPDevise;
String HostName;
String TitelName;

u_int16_t WifiAbbruchAnzahl = 0;
u_int16_t letzteMesMin = 0;

bool ErrorSPIFFS;

String MacAdress;
String ErrorWifi;
String SpiffDir;
time_t now;
tm Time;

Messdaten Daten;
Log_Data_File SpeicherKarte;
AsyncWebServer server(80);


void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void ListFilesSpiffs();
String replaceVariable(const String& var);



void setup(void) {
  Serial.begin(115200);
  // Relais 1 - 4
  pinMode(PIN_RELAIS_1, OUTPUT);
  pinMode(PIN_RELAIS_2, OUTPUT);
  pinMode(PIN_RELAIS_3, OUTPUT);
  pinMode(PIN_RELAIS_4, OUTPUT);
  // OutPut Messung BodenFeuche
  pinMode(PIN_MES_ANG , OUTPUT);
  pinMode(PIN_LED,OUTPUT);
  digitalWrite(PIN_LED, HIGH);
  // SW 1- 4
  pinMode(PIN_IN_1, INPUT) ; 
  pinMode(PIN_IN_2, INPUT) ; 
  pinMode(PIN_IN_3, INPUT) ; 
  pinMode(PIN_IN_4, INPUT) ; 

  SpeicherKarte.Begin();
  Daten.Begin(ANZAHL_Dallas, &SpeicherKarte);
  ErrorSPIFFS = SPIFFS.begin(true);
  if(ErrorSPIFFS)     ListFilesSpiffs();
    
  MacAdress = WiFi.macAddress();
  if(MacAdress == MacGlaspalst) 
  { 
    ESPDevise = Geraete::Glaspalast;
    TitelName = "Glaspalast";
    HostName  = "Glaspalast";
    Serial.println(" ESP32 ist Glaspalast");
  }
  if(MacAdress == MacLokschuppen) 
  {
    ESPDevise = Geraete::Lokschuppen;
    TitelName = "Lokschuppen";
    HostName  = "Lokschuppen";
    Serial.println(" ESP32 ist Lokschuppen");
  }
  WiFi.disconnect(true);
  
  delay(1000);

  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);

  delay(1000);
  while (WiFi.status() != WL_CONNECTED) {delay (10);  }
  Serial.println("WiFi Verbunden !");
  Serial.print(" - WLan Netz    = ");  Serial.println(WIFI_SSID);
  Serial.print(" - IP  addreses = ");  Serial.println(WiFi.localIP());
  Serial.print(" - MAC addreses = ");  Serial.println(MacAdress);
  WiFi.setHostname(HostName.c_str()); 
  if(MDNS.begin(HostName))
  {
    Serial.print(" - Responder gestartet.  = http:\\\\");
    Serial.println(HostName);
  }
  Serial.print(" - Hostname: ");  Serial.println(WiFi.getHostname());

  configTime(0, 0, NTP_SERVER);
  setenv("TZ", TZ_INFO, 1);

 //  AsyncElegantOTA.begin(&server); 
  server.begin();
  
  server.on("/espstyle.css", HTTP_GET, [](AsyncWebServerRequest* request)  
  {
        request->send(SPIFFS, "/espstyle.css");  

  });
  server.on("/Chart.js", HTTP_GET, [](AsyncWebServerRequest* request)  
  { 
       request->send(SPIFFS, "/Chart.js");  
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest* request)  
  { 
     Serial.println("on Server favicon.ico ");

       switch (ESPDevise)
       {
       case Glaspalast:
        request->send(SPIFFS, "/favicon(1).ico");
        break;
       case Lokschuppen:
         Serial.println("Send favicon Lookschuppen ");
        request->send(SPIFFS, "/favicon(2).ico");
        break;
       
       default:
        break;
       }
  });
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) 
  {
        request->send(SPIFFS, "/SeiteHome.html", String(), false, replaceVariable);  
  });
  server.on("/out", HTTP_GET, [](AsyncWebServerRequest* request) 
  {
    if (request->args() > 0)  Daten.SchalteRelaisState(request->getParam(0));
    Daten.Messen();  
    request->send(SPIFFS, "/SeiteSchalten.html", String(), false, replaceVariable);  
  });
  server.on("/boden", HTTP_GET, [](AsyncWebServerRequest* request) 
  {
     request->send(SPIFFS, "/SeiteBoden.html", String(), false, replaceVariable);  
  });
  server.on("/luft", HTTP_GET, [](AsyncWebServerRequest* request) 
  {
   request->send(SPIFFS, "/SeiteLuft.html", String(), false, replaceVariable);  
  });
  server.on("/info", HTTP_GET, [](AsyncWebServerRequest* request) 
  {
    if (request->args() > 0)  Daten.SchalteBit(request->getParam(0));
    request->send(SPIFFS, "/SeiteInfo.html", String(), false, replaceVariable);  
  });
  server.on("/download", HTTP_GET,[](AsyncWebServerRequest* request)
  {
    if (request->args() > 0) Daten.DownloadDatei(request);
  });  
  
  do
  {
    time(&now);                     
    localtime_r(&now, &Time);
    delay(50);
  }
  while(Time.tm_year == 70);
  Serial.print(" - "); Serial.print(Daten.TextTag(Time)); 
  Serial.print(" | "); Serial.println(Daten.TextZeit(Time)); 
  Daten.SetneuerTag(Time);
  Daten.Load_Data_Log(Time);
  Daten.WerteSichern(true, Time,WiFi.RSSI()); 

  letzteMesMin = Time.tm_min;
}

void loop(void) {
    
  // 1 min rum
  if(Time.tm_min != letzteMesMin)
  {
    Daten.Regelung(Time);
    Daten.WerteSichern(false, Time, WiFi.RSSI());
    if((Time.tm_hour== 0)&&(Time.tm_min== 0))// Neuer Tag
    {
      Daten.SetneuerTag(Time);
      Daten.ResetMinMax();
    }
    letzteMesMin = Time.tm_min;
  }
  getLocalTime(&Time);  

}
//=================================================================================================================================

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Zum Router Verbunden !");
}
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
   digitalWrite(PIN_LED, LOW);
}
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  WifiAbbruchAnzahl++;
  digitalWrite(PIN_LED, HIGH);
  ErrorWifi = "Grund des Abruchs = " + String(info.wifi_sta_disconnected.reason);
  if (info.wifi_sta_disconnected.reason > 199)
  {
    ErrorWifi = WiFi_Error200[info.wifi_sta_disconnected.reason - 200]+" ["+ String(info.wifi_sta_disconnected.reason)+"]" ;
  }
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
}
void ListFilesSpiffs()
{
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  SpiffDir ="";
  Serial.println("List file ===================");
  while(file){
      Serial.print("FILE: ");
      Serial.println(file.name());
      SpiffDir += String("<br><li>")+file.name()+String("</li>\n");
      file = root.openNextFile();
  }
    Serial.println("===========================");
}

String replaceVariable(const String& var)
{
  String V_Text;
 
  if (var == "TITELDATUM")  return (Daten.TextTagZeit(Time)); //
  if (var == "TITELNAME")   return (TitelName); //
  V_Text.clear();
  V_Text =  Daten.replaceVariable(var);
  if (!V_Text.isEmpty())
  {
    return V_Text;
  }
  
  V_Text =  SpeicherKarte.replaceVariable(var);
  if (!V_Text.isEmpty())
  {
    return V_Text;
  }
  if(var == "WIFINAME")       return(WIFI_SSID);
  if(var == "WIFIHOST")       return( HostName);
  if(var == "WIFIMAC")        return( MacAdress);
  if(var == "WIFIKANAL")      return(String( WIFI_CHANNEL));
  if(var == "WIFIERROR")     return( String(WifiAbbruchAnzahl));
  if(var == "WIFIERR_GR")    return( ErrorWifi);

  if(var == "ESPIP")         return( String(WiFi.localIP().toString().c_str()));
  if(var == "NTPTIME")       return(NTP_SERVER);
  if(var == "SPIFFSDIR")     return(SpiffDir);
  return String();
}
