#ifndef MESSDATEN_H
#define MESSDATEN_H
#include "Hardware.h"
#include "Log_Data.h"
#include <OneWire.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>

#include <DHTesp.h>
#include <DallasTemperature.h>
#include <Adafruit_BME280.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_TSL2591.h>
#include <Adafruit_INA219.h>
#include <ESPAsyncWebServer.h>

enum WerteMessen_f {In1Tmp,In2Tmp,Au1Tmp,LuDru,LiLux,Bo1Tmp,Bo2Tmp,Bo3Tmp,Bo4Tmp,TotalWerteMessen_f };
enum WerteMessen_b {In1Feu,In2Feu,Au1Feu,Bo1Feu,Bo2Feu,Bo3Feu,Bo4Feu,TotalWerteMessen_b};
enum WerteMessen_d {Relais1,Relais2,Relais3,Relais4,Dig_In_1,Dig_In_2,Dig_In_3,Dig_In_4,TotalWerteMessen_d};
enum Sensorentype {Dallas,AHT_in,AHT_au,BME,TSL,Power,Analog,TotalWerteSensoren};

class Messdaten 
{
  private:
    Log_Data_File *pSD;
    OneWire oneWire;
    DallasTemperature Sensor_Dallas;
    Adafruit_AHTX0 Sensor_AHT_AU; 
    Adafruit_AHTX0 Sensor_AHT_IN; 
    Adafruit_BME280 Sensor_BME;
    Adafruit_INA219 Sensor_Power;
    Adafruit_TSL2591 Sensor_Licht = Adafruit_TSL2591(2591); 
// Messung mit Wert float
    sensors_event_t humidity, temp;
    byte AnzahlSensorDallas =0;
    // byte nDevice;
    float ActuellerWert_f[WerteMessen_f::TotalWerteMessen_f];
    float MittelWert_f[WerteMessen_f::TotalWerteMessen_f];
    float MinWert_f[WerteMessen_f::TotalWerteMessen_f];
    float MaxWert_f[WerteMessen_f::TotalWerteMessen_f];
    float ZeitWerte_f[WerteMessen_f::TotalWerteMessen_f][MaxMessungen];
// Messung mit Wert byte 
    u_int8_t ActuellerWert_b[WerteMessen_b::TotalWerteMessen_b];
    u_int8_t MittelWert_b[WerteMessen_b::TotalWerteMessen_b];
    u_int8_t MinWert_b[WerteMessen_b::TotalWerteMessen_b];
    u_int8_t MaxWert_b[WerteMessen_b::TotalWerteMessen_b];
    u_int8_t ZeitWerte_b[WerteMessen_b::TotalWerteMessen_b][MaxMessungen];
    u_int8_t ZeitWert_Wifi[MaxMessungen];
    
    bool ZeitWerte_d[WerteMessen_d::TotalWerteMessen_d][MaxMessungen]; // Relais und Digi Eingänge 
    bool Sensor_Da [Sensorentype::TotalWerteSensoren];

    float shuntvoltage = 0;
    float busvoltage = 0;
    float current_mA = 0;
    float loadvoltage = 0;
    float power_mW = 0;
    float MaxCurrent = 0;
    float MaxFeldstaerke = 0;
    float MinFeldstaerke = 0;
    float OnTemp_Heizer = 0.00;
    float OffTemp_Heizer = 1.00;
    bool Relais1State  = false;
    bool Relais2State  = false;
    bool Relais3State  = false;
    bool Relais4State  = false;
    bool RegelungTemper = true;
    bool RegelungWasser = false;
    bool RestHeizzeit = false;
    bool AnzeigevollKurve = false;

    //bool AnalogTest = false;
    tm Heute;
    tm Gestern;
    double HeizenGesmatZeit;
    
    u_int16_t AnalogWertA;
    u_int16_t AnalogWertB;
    u_int16_t AnalogWertC;
    u_int16_t AnalogWertD;
    
    File Tmp_File;
    byte nDevices = 0;
    byte EndPunktKurve;
    byte EndZeitKurve;
    String Text;
    char buffer[50];
    u_int8_t RechneFeuchte(u_int16_t Messung);
  public:
    Messdaten();
    void Begin(byte Nr, Log_Data_File *pSD);
    void ESP_Log_Data(tm Zeit,  u_int8_t DB);
    void SetneuerTag(tm Zeit);
    void Messen();
    void Regelung(tm Zeit); //Heizung und Pumpe
    void WerteSichern(bool start,tm Zeit, float DB);

    void ResetMinMax();
    bool SennsorErr(Sensorentype ty);
    void SchalteBit( AsyncWebParameter* p);
    void SchalteRelaisState( AsyncWebParameter* p);
    void SchalteRelais();

    void Heizen(tm Zeit,float ist,float OnTemp, float OffTemp);
    void Waessern();

    void DownloadDatei(AsyncWebServerRequest* p);
    void Save_Data_Mem(); // alles auf mal speichern zu Sicherung bei update 
    void Load_Data_Mem();
    void Load_Data_Log(tm Uhrzeit); // alles in den Speiche laden
    
    String DebugI2Cscan();
    String DebugI2CnDev();

    String replaceVariable(const String& var);

    String DebugVolt();
    String DebugStrom();
    String DebugLeistung();

    String TextZeitHeizen();
    String TextZeit(tm Uhrzeit);
    String TextTag( tm Uhrzeit);
    String TextTagZeit(tm Uhrzeit);
    String TextLogFile(tm Uhrzeit);
    String TextESPDataFile(tm Uhrzeit);
    String TextArray(); // X- 
    String TextArray(WerteMessen_f type); //Y - Werte float
    String TextArray(WerteMessen_b type); //Y - Werte byte
    String TextArray(WerteMessen_d type, float von, float bis); //Y - Werte bool
    String IconHochdruck();
    String IconHeizen();
    String IconFenster();
    String IconNeu();
};

#endif