#ifndef MESSDATEN_CPP
#define MESSDATEN_CPP

#include "CMessdaten.h"

Messdaten::Messdaten()
{
    float SetupWert;

    for ( int i = 0; i < WerteMessen_f::TotalWerteMessen_f; i++)
    {
       if (i== LuDru) SetupWert = 980.00;
       else SetupWert = -5.5;

        ActuellerWert_f[i] = 0;
        MittelWert_f[i] = 0;
        MinWert_f[i] =  3.4028235E+38;
        MaxWert_f[i] = -3.4028235E+38;
        for (byte x = 0; x < MaxMessungen; x++)  
        {
          ZeitWerte_f[i][x] = SetupWert; 

        }
    }
    for ( int i = 0; i < WerteMessen_b::TotalWerteMessen_b; i++)
    {
        ActuellerWert_b[i] = 0;
        MittelWert_b[i] = 0;
        MinWert_b[i] =  100;
        MaxWert_b[i] = 0;
        for (byte x = 0; x < MaxMessungen; x++)  {ZeitWerte_b[i][x] = 0; }
    }
    for (int i =0; i < WerteMessen_d::TotalWerteMessen_d;i++)
    {
        for (byte x = 0; x < MaxMessungen; x++)  
        {
          ZeitWerte_d[i][x] = false; 
        }
    }
    HeizenGesmatZeit = 0.0;
}

u_int8_t Messdaten::RechneFeuchte(u_int16_t Messung)
{
   u_int8_t W = (100- (Messung -2695)/14);

   if (W < 100) return W;

  return 100;
}

void Messdaten::Begin(byte Nr, Log_Data_File *SD)
{
    pSD = SD;
    oneWire.begin(ONE_WIRE_BUS);
    Sensor_Dallas.setOneWire(&oneWire);
    Sensor_Dallas.begin();
        
    AnzahlSensorDallas = Sensor_Dallas.getDeviceCount();
    Sensor_Da[Dallas] = (Nr == AnzahlSensorDallas);
    
    Sensor_Da[AHT_au] = Sensor_AHT_AU.begin(&Wire,0,0x38);
    Sensor_Da[AHT_in] = Sensor_AHT_IN.begin(&Wire,1,0x39);
    Sensor_Da[BME] = Sensor_BME.begin(0x76,&Wire);
    if(!Sensor_Da[BME]) Sensor_Da[BME] = Sensor_BME.begin(0x77,&Wire);
    Sensor_Da[Analog] = true;

    Sensor_Da [Power] = Sensor_Power.begin();
    Sensor_Power.setCalibration_32V_1A();
}

void Messdaten::ESP_Log_Data(tm Zeit)
{
  Tmp_File = pSD->speicher_ESPDataFile(TextESPDataFile(Zeit));

  if (Tmp_File)
  {
    // Strom messen
    if(Sensor_Da[Power])
    {
        shuntvoltage = Sensor_Power.getShuntVoltage_mV();
        busvoltage = Sensor_Power.getBusVoltage_V();
        current_mA = Sensor_Power.getCurrent_mA();
        power_mW = Sensor_Power.getPower_mW();
    }    
    Tmp_File.printf(" %2d:%02d ; %8.2f mA; %8.2f mW ; %8.2f mdB  \n",Zeit.tm_hour, Zeit.tm_min, current_mA,power_mW,Feldstaerke_dBm);
  }
}

void Messdaten::SetneuerTag(tm Zeit)
{
  Gestern = Heute;
  Heute = Zeit;
  if (Gestern.tm_mday== 0)
  {
    Gestern = Heute;
    Gestern.tm_mday--;
  }
  
}

void Messdaten::Messen()
{   
    uint16_t Wert;
    // Messung BME
    if (Sensor_Da[BME])
    {
        ActuellerWert_f[In1Tmp] = Sensor_BME.readTemperature();
        ActuellerWert_b[In1Feu] = Sensor_BME.readHumidity();
        ActuellerWert_f[LuDru] = (Sensor_BME.readPressure() /100);
    }
    // Messung AHT aussen
    if (Sensor_Da[AHT_au])
    {
        Sensor_AHT_AU.getEvent(&humidity, &temp);
        ActuellerWert_f[Au1Tmp] = temp.temperature;
        ActuellerWert_b[Au1Feu] = humidity.relative_humidity;
    }
    // Messung AHT aussen
    if (Sensor_Da[AHT_in])
    {
        Sensor_AHT_IN.getEvent(&humidity, &temp);
        ActuellerWert_f[In2Tmp] = temp.temperature;
        ActuellerWert_b[In2Feu] = humidity.relative_humidity;
    }
    // Messung Boden Temp und Feuchte
    if(Sensor_Da[Dallas])
    {
        Sensor_Dallas.requestTemperatures();
        if (AnzahlSensorDallas > 0)  ActuellerWert_f[Bo1Tmp]=Sensor_Dallas.getTempCByIndex(0);
        if (AnzahlSensorDallas > 1)  ActuellerWert_f[Bo2Tmp]=Sensor_Dallas.getTempCByIndex(1);
        if (AnzahlSensorDallas > 2)  ActuellerWert_f[Bo3Tmp]=Sensor_Dallas.getTempCByIndex(2);
        if (AnzahlSensorDallas > 3)  ActuellerWert_f[Bo4Tmp]=Sensor_Dallas.getTempCByIndex(3);
    }
    if(Sensor_Da[Analog])
    {
      const u16_t f1 = 14, f2=2695;
      digitalWrite(PIN_MES_ANG,HIGH); 
      delay(1000);
      AnalogWertA = analogRead(PIN_Analog1);   
      AnalogWertB = analogRead(PIN_Analog2);   
      AnalogWertC = analogRead(PIN_Analog3);
      AnalogWertD = analogRead(PIN_Analog4);   
      
       ActuellerWert_b[Bo1Feu] = RechneFeuchte(AnalogWertA);
       ActuellerWert_b[Bo2Feu] = RechneFeuchte(AnalogWertB);
       ActuellerWert_b[Bo3Feu] = RechneFeuchte(AnalogWertC);
       ActuellerWert_b[Bo4Feu] = RechneFeuchte(AnalogWertD);

        digitalWrite(PIN_MES_ANG,LOW);
        if ((AnalogWertA>= f2)|| (AnalogWertB>= f2) || (AnalogWertC>= f2) || (AnalogWertD>= f2))
        {
          Tmp_File = pSD->speicher_DebugFile(2);
          if (Tmp_File)
          {
            Tmp_File.printf("- No.1 = %3d %; %5d ;|- No.2 = %3d %; %5d ;|- No.3 = %3d %; %5d ;|- No.4 = %3d %; %5d ;|\n ", ActuellerWert_b[Bo1Feu],AnalogWertA, ActuellerWert_b[Bo2Feu],AnalogWertB, ActuellerWert_b[Bo3Feu],AnalogWertC, ActuellerWert_b[Bo4Feu],AnalogWertD );
            Tmp_File.close();
          }
          
          
        }
    }
    // Licht wird später gemessen.
}
void Messdaten::Regelung(tm Zeit)
{
if (RegelungTemper)
  {
    float Temp_ist; 
    if (Sensor_Da[BME])
    {
       Temp_ist = Sensor_BME.readTemperature();
    }  
    Heizen(Zeit,Temp_ist,OnTemp_Heizer,OffTemp_Heizer);
  }
  if(Sensor_Da[Power]) // Strom messsen
  {
      current_mA = Sensor_Power.getCurrent_mA(); 
      if ( current_mA > MaxCurrent); MaxCurrent = current_mA;
   }
}

void Messdaten::WerteSichern(bool start, tm Zeit, float DB)
{
    unsigned int t = ((60  * Zeit.tm_hour) + Zeit.tm_min )/ Abtastzeit;
    bool WritIt =( Zeit.tm_min % Abtastzeit == 0);
    EndZeitKurve = t;
    Messen();
    if (WritIt)
    {
      Tmp_File = pSD->speicher_DataFile(TextLogFile(Zeit));
      if(Tmp_File)  Tmp_File.printf("%3d ;",t);
    }
    // alle float werte
    for ( int i = 0; i < WerteMessen_f::TotalWerteMessen_f; i++)
    {
        if (start) MittelWert_f[i] = ActuellerWert_f[i];
        else 
        {
          MittelWert_f[i]= (MittelWert_f[i] + ActuellerWert_f[i]) / 2.00;
        }
        if(MaxWert_f[i] < ActuellerWert_f[i]) MaxWert_f[i] = ActuellerWert_f[i];
        if(MinWert_f[i] > ActuellerWert_f[i]) MinWert_f[i] = ActuellerWert_f[i];
        if (WritIt)
        {
          ZeitWerte_f[i][t] = MittelWert_f[i];
          MittelWert_f[i] = ActuellerWert_f[i];
          if (Tmp_File)  Tmp_File.printf("%8.2f;",ZeitWerte_f[i][t]);
        }
    } 
    // alle byte werte 
    for ( int i = 0; i < WerteMessen_b::TotalWerteMessen_b; i++)
    {
        if (start) MittelWert_b[i] = ActuellerWert_b[i];
        else 
        {
            MittelWert_b[i]= (MittelWert_b[i] + ActuellerWert_b[i]) / 2.00;
        }
        if(MaxWert_b[i] < ActuellerWert_b[i]) MaxWert_b[i] = ActuellerWert_b[i];
        if(MinWert_b[i] > ActuellerWert_b[i]) MinWert_b[i] = ActuellerWert_b[i];
        if (WritIt)
        {
          ZeitWerte_b[i][t] = MittelWert_b[i];
          MittelWert_b[i] = ActuellerWert_b[i];
          if (Tmp_File) Tmp_File.printf("%3d;",ZeitWerte_b[i][t]);
        }
    } 
    if (WritIt)
    {
        ZeitWerte_d[Relais1][t] = Relais1State;
        ZeitWerte_d[Relais2][t] = Relais2State; 
        ZeitWerte_d[Relais3][t] = Relais3State; 
        ZeitWerte_d[Relais4][t] = Relais4State;
        ZeitWerte_d[Dig_In_1][t] = digitalRead(PIN_IN_1);
        ZeitWerte_d[Dig_In_2][t] = digitalRead(PIN_IN_2);
        ZeitWerte_d[Dig_In_3][t] = digitalRead(PIN_IN_3);
        ZeitWerte_d[Dig_In_4][t] = digitalRead(PIN_IN_4);
        if (Tmp_File)
        {
          Tmp_File.printf("%2d ;%2d; %2d; %2d;",ZeitWerte_d[Relais1][t],ZeitWerte_d[Relais2][t],ZeitWerte_d[Relais3][t],ZeitWerte_d[Relais4][t]);
          Tmp_File.printf("%2d ;%2d; %2d; %2d;",ZeitWerte_d[Dig_In_1][t],ZeitWerte_d[Dig_In_2][t],ZeitWerte_d[Dig_In_3][t],ZeitWerte_d[Dig_In_4][t]);
          Tmp_File.printf("%2d;%02d \n", Zeit.tm_hour,Zeit.tm_min);
          Tmp_File.close();
        }
    }    
    if (start)
    {
      MaxFeldstaerke = DB;
      MinFeldstaerke = DB;
    }
    else
    {
      if (DB != 0)
      {
        Feldstaerke_dBm = DB;
        if (DB > MaxFeldstaerke) MaxFeldstaerke = DB;
        if (DB < MinFeldstaerke) MinFeldstaerke = DB;
      }
    }
    ESP_Log_Data(Zeit);
}

void Messdaten::ResetMinMax()
{
    for ( int i = 0; i < WerteMessen_f::TotalWerteMessen_f; i++)
    {
        MittelWert_f[i] = ActuellerWert_f[i];
        MinWert_f[i] =  3.4028235E+38;
        MaxWert_f[i] = -3.4028235E+38;
    }
    for ( int i = 0; i < WerteMessen_b::TotalWerteMessen_b; i++)
    {
        MittelWert_b[i] = ActuellerWert_b[i];
        MinWert_b[i] =  100;
        MaxWert_b[i] = 0;
    }
}
bool Messdaten::SennsorErr(Sensorentype ty)
{
    return Sensor_Da[ty];
}

void Messdaten::SchalteRelaisState( AsyncWebParameter* p)
{
  String Value = p->value().c_str();
  if (p->name()== "SW")
  {
     switch (Value.toInt())
     {
      case 0:
        break;
      case 1:
        Relais1State = !Relais1State;
        break;
      case 2:
        Relais2State = !Relais2State;
        break;
      case 3:
        Relais3State = !Relais3State;
        break;
      case 4:
        Relais4State = !Relais4State;
        break;
      default:
        break;
      }
      SchalteRelais();
  }
}
void Messdaten::SchalteRelais()
{
        digitalWrite(PIN_RELAIS_1, Relais1State);
        digitalWrite(PIN_RELAIS_2, Relais2State);
        digitalWrite(PIN_RELAIS_3, Relais3State);
        digitalWrite(PIN_RELAIS_4, Relais4State);
}
void Messdaten::SchalteBit( AsyncWebParameter* p)
{
  String Value = p->value().c_str();
  Serial.print("Name = ");  Serial.println(p->name());
  Serial.print("Value = "); Serial.println(Value);

  if (p->name()== "Bit")
  {
    switch (Value.toInt())
    {
      case 0:
        break;
      case 1:
        RegelungTemper = !RegelungTemper;
        break;
      case 2:
        RegelungWasser = !RegelungWasser;
        break;
      case 3:
        AnzeigevollKurve = !AnzeigevollKurve;
        break;
      case 4: // HeizZeit zzurück setzten;
        HeizenGesmatZeit = 0.0;
        break;  
      case 5:
        break;
      case 10:
        Save_Data_Mem();       
        break;
      case 11:
        Load_Data_Mem();       
        break;
      case 12:
        Sensor_Da[Analog] = !Sensor_Da[Analog];
        digitalWrite(PIN_MES_ANG,HIGH);         break;
    }
  } 
}

void Messdaten::Heizen(tm Zeit, float ist, float OnTemp, float OffTemp)
{
  static time_t Zeit_ein;
  static time_t Zeit_aus;
  Tmp_File = pSD->speicher_DebugFile(1);
   if((ist <= OnTemp) &&(!Relais1State)) // Heizen ein || (!RegelungTemper && !Relais1State)
    {
      if(Tmp_File) Tmp_File.printf("%2d:%02D:%02d; %8.f ; on ",Zeit.tm_hour,Zeit.tm_min,Zeit.tm_sec, ist);
      Relais1State = true;
      time(&Zeit_ein);
    }

  if((ist >= OffTemp)&& (Relais1State)   )// Heizen aus || (!RegelungTemper && Relais1State)
    {
      if(Tmp_File) Tmp_File.printf("%2d:%02D:%02d; %8.f ; off ",Zeit.tm_hour,Zeit.tm_min,Zeit.tm_sec, ist);
      Relais1State = false;
      time(&Zeit_aus);
      HeizenGesmatZeit += difftime(Zeit_aus, Zeit_ein);
    }
  if(Tmp_File) Tmp_File.close();  
  SchalteRelais();
}
void Messdaten::Waessern()
{
  
}

void Messdaten::DownloadDatei(AsyncWebServerRequest *p)
{
  AsyncWebParameter *pPar = p->getParam(0);
  String Value = pPar-> value().c_str();
  File file;
  const char* memFineName = "MemData.txt";

  Serial.print("Name = ");  Serial.println(pPar->name());
  Serial.print("Value = "); Serial.println(Value);

  if (pPar->name()== "Datei")
  {
    switch (Value.toInt())
    {
       case 0: // 
        break;
       case 1: // actuelle Logdatei
        pSD->Send_DataFile(p, TextLogFile(Heute));
        break;
       case 2: // gestern Log_Data
        pSD->Send_DataFile(p, TextLogFile(Gestern));
        break;
       case 3: //  letzte MemDatei
        pSD->Send_MemFile(p);
        break;
       case 4: // Debugdatei Regelung
         pSD->Send_DebugFile(p,1);
        break;
       case 5: // Debugdatei Analog Messung
         pSD->Send_DebugFile(p,2);
        break;
       case 6: // actuelle Logdatei zu ESP32
        pSD->Send_DataFile(p, TextESPDataFile(Heute));
        break;
       case 7: // gestern Logdatei zu ESP32
        pSD->Send_DataFile(p, TextESPDataFile(Gestern));
        break;
    }
  }
}
void Messdaten::Save_Data_Mem()
{
  tm Zeit;
  File Tmp_File = pSD->speicher_MemFile();
  u8_t t;

Zeit.tm_hour = 0; Zeit.tm_min = 0;

  if (Tmp_File)
  {
    for ( t = 0; t < MaxMessungen; t++)
    {
      Tmp_File.printf("%03d ;",t);
      Serial.printf("%03d ;\t",t);
      // alle float werte
      for ( int i = 0; i < WerteMessen_f::TotalWerteMessen_f; i++)
      {
        
          Tmp_File.printf("%8.2f;",ZeitWerte_f[i][t]);
          Serial.printf("%8.2f;",ZeitWerte_f[i][t]);
      } 
      // alle byte werte 
      for ( int i = 0; i < WerteMessen_b::TotalWerteMessen_b; i++)
      {
          Tmp_File.printf("%3d;",ZeitWerte_b[i][t]);
          Serial.printf("%3d;",ZeitWerte_b[i][t]);
      } 
      Tmp_File.printf("%2d ;%2d; %2d; %2d;",ZeitWerte_d[Relais1][t],ZeitWerte_d[Relais2][t],ZeitWerte_d[Relais3][t],ZeitWerte_d[Relais4][t]);
      Tmp_File.printf("%2d ;%2d; %2d; %2d;",ZeitWerte_d[Dig_In_1][t],ZeitWerte_d[Dig_In_2][t],ZeitWerte_d[Dig_In_3][t],ZeitWerte_d[Dig_In_4][t]);
      Tmp_File.printf("%2d;%2d \n", Zeit.tm_hour,Zeit.tm_min);

      Serial.printf("%2d ;%2d; %2d; %2d;",ZeitWerte_d[Relais1][t],ZeitWerte_d[Relais2][t],ZeitWerte_d[Relais3][t],ZeitWerte_d[Relais4][t]);
      Serial.printf("%2d ;%2d; %2d; %2d;",ZeitWerte_d[Dig_In_1][t],ZeitWerte_d[Dig_In_2][t],ZeitWerte_d[Dig_In_3][t],ZeitWerte_d[Dig_In_4][t]);
      Serial.printf("%2d;%02d \n", Zeit.tm_hour,Zeit.tm_min);
      
      Zeit.tm_min += 10;
      if (Zeit.tm_min == 60)
      {
        Zeit.tm_hour++;
        Zeit.tm_min = 0;
      }
    }
   Tmp_File.close();
  }
}
void Messdaten::Load_Data_Mem()
{
  char ch;
  u_int8_t Pos_x = 0; // Position in Zeit
  u_int16_t Pos_y = 0; // Position im Array Y-Werte
  String ZahlStr;
  String ZeitStr;
  bool wertepos = false;
  u_int8_t ByteWert;
  float FloatWert;
  File MeM_File= pSD->lese_MemFile();
  if (MeM_File)
  {
    //Error_MemFile = false;
     while(MeM_File.available())
     {
       ch = MeM_File.read();
      switch (ch)
      {
      case ';':
      if(wertepos)
      {
        int n = ZahlStr.lastIndexOf('.');
        if(n == -1)
        { // Zahl u_int
          ByteWert = ZahlStr.toInt();
          if (Pos_x == 0  ) Serial.printf(" >Y= %3d ( %s = %4d)  ",Pos_y, ZahlStr.c_str(), ByteWert);
          if((Pos_y >= TotalWerteMessen_f) && (Pos_y< (TotalWerteMessen_f + TotalWerteMessen_b)))
          {
              ZeitWerte_b[Pos_y-TotalWerteMessen_f][Pos_x] = ZahlStr.toFloat();
          }
          if((Pos_y >= (TotalWerteMessen_f + TotalWerteMessen_b)) && (Pos_y< (TotalWerteMessen_d+TotalWerteMessen_f + TotalWerteMessen_b)))
          {
              ZeitWerte_d[Pos_y -TotalWerteMessen_f -TotalWerteMessen_b][Pos_x] = ZahlStr.toFloat();
          }
        }
        else 
        {// Zahl float
            if((Pos_y >= 0) && (Pos_y< TotalWerteMessen_f))
            {
              ZeitWerte_f[Pos_y][Pos_x] = ZahlStr.toFloat();
            }
        }
        Pos_y++;
      }
      else
      {
        Pos_x = ZahlStr.toInt();
        wertepos = true;
      }
      ZahlStr.clear();
      break;
      case '\n':
          wertepos = false;
          Pos_y = 0;
          ZahlStr.clear();
          break;
      default:
         ZahlStr += ch;
        break;
      }
     }
     MeM_File.close();
  }
  else
  {    // Error Open File
      // Error_MemFile = true; 
  }
}
void Messdaten::Load_Data_Log(tm Uhrzeit)
{
  unsigned int tmax = ((60  * Uhrzeit.tm_hour) + Uhrzeit.tm_min )/ Abtastzeit;
  char ch;
  u_int8_t Pos_x = 0; // Position in Zeit
  u_int8_t Pos_y = 0; // Position im Array Y-Werte
  String ZahlStr;
  bool wertepos = false;

  Tmp_File = pSD->lese_DataFile(TextLogFile(Uhrzeit));
  if (Tmp_File)
  {
     while(Tmp_File.available())
     {
       ch = Tmp_File.read();
      switch (ch)
      {
      case ';':
      if(wertepos)
      {
        int n = ZahlStr.lastIndexOf('.');
        if(n == -1)
        { // Zahl u_int
          if((Pos_y >= TotalWerteMessen_f) && (Pos_y< (TotalWerteMessen_f + TotalWerteMessen_b)))
          {
              ZeitWerte_b[Pos_y-TotalWerteMessen_f][Pos_x] = ZahlStr.toInt();
          }
          if((Pos_y >= (TotalWerteMessen_f + TotalWerteMessen_b)) && (Pos_y< (TotalWerteMessen_d+TotalWerteMessen_f + TotalWerteMessen_b)))
          {
              ZeitWerte_d[Pos_y -TotalWerteMessen_f -TotalWerteMessen_b][Pos_x] = ZahlStr.toInt();
          }
        }
        else 
        {// Zahl float
            if((Pos_y >= 0) && (Pos_y< TotalWerteMessen_f))
            {
              ZeitWerte_f[Pos_y][Pos_x] = ZahlStr.toFloat();
            }
        }
        Pos_y++;
      }
      else
      {
        Pos_x = ZahlStr.toInt();
        wertepos = true;
      }
      ZahlStr.clear();
      break;
      case '\n':
          wertepos = false;
          Pos_y = 0;
          ZahlStr.clear();
          break;
      default:
         ZahlStr += ch;
        break;
      }
     }
     Tmp_File.close();
  }
  else
  {    // Error Open File
      // Error_MemFile = true; 
  }
}

String Messdaten::DebugI2Cscan()
{
    byte addresse, error;
    nDevices=0;
    Text.clear();
    for(addresse = 1; addresse < 127; addresse++ )
    {
        Wire.beginTransmission(addresse);
        error = Wire.endTransmission();
        if (error == 0)
        {
            sprintf(buffer,"  [x%02X] ", addresse);
            Text += buffer;
            nDevices++;
        }
    }
    return Text; 
}
String Messdaten::DebugI2CnDev()
{
    if (nDevices == 0)
        return "I2C devices found";
    else
        sprintf(buffer," %i Geräte gefunden ", nDevices);
    return buffer;
}

String Messdaten::replaceVariable(const String &var)
{
   if (var == "IN1_TEMP_WERT")  return( String(ActuellerWert_f[In1Tmp])+" &#8451;"); //
   if (var == "IN1_TEMP_MAX")   return( String(MaxWert_f[In1Tmp]) +" &#8451;"); //
   if (var == "IN1_TEMP_MIN")   return(String(MinWert_f[In1Tmp])+" &#8451;"); //

   if (var == "IN2_TEMP_WERT")  return(String(ActuellerWert_f[In2Tmp])+" &#8451;"); //
   if (var == "IN2_TEMP_MAX")   return(String(MaxWert_f[In2Tmp])+" &#8451;"); //
   if (var == "IN2_TEMP_MIN")   return(String(MinWert_f[In2Tmp])+" &#8451;"); //

   if (var == "AU1_TEMP_WERT")  return(String(ActuellerWert_f[Au1Tmp])+" &#8451;"); //
   if (var == "AU1_TEMP_MAX")   return(String(MaxWert_f[Au1Tmp])+" &#8451;"); //
   if (var == "AU1_TEMP_MIN")   return(String(MinWert_f[Au1Tmp])+" &#8451;"); //
   
   if (var == "BO1_TEMP_WERT")  return(String(ActuellerWert_f[Bo1Tmp])+" &#8451;"); //
   if (var == "BO1_TEMP_MAX")   return(String(MaxWert_f[Bo1Tmp])+" &#8451;"); //
   if (var == "BO1_TEMP_MIN")   return(String(MinWert_f[Bo1Tmp])+" &#8451;"); //

   if (var == "BO2_TEMP_WERT")  return(String(ActuellerWert_f[Bo2Tmp])+" &#8451;"); //
   if (var == "BO2_TEMP_MAX")   return(String(MaxWert_f[Bo2Tmp])+" &#8451;"); //
   if (var == "BO2_TEMP_MIN")   return(String(MinWert_f[Bo2Tmp])+" &#8451;"); //

   if (var == "BO3_TEMP_WERT")  return(String(ActuellerWert_f[Bo3Tmp])+" &#8451;"); //
   if (var == "BO3_TEMP_MAX")   return(String(MaxWert_f[Bo3Tmp])+" &#8451;"); //
   if (var == "BO3_TEMP_MIN")   return(String(MinWert_f[Bo3Tmp])+" &#8451;"); //

   if (var == "BO4_TEMP_WERT")  return(String(ActuellerWert_f[Bo4Tmp])+" &#8451;"); //
   if (var == "BO4_TEMP_MAX")   return(String(MaxWert_f[Bo4Tmp])+" &#8451;"); //
   if (var == "BO4_TEMP_MIN")   return(String(MinWert_f[Bo4Tmp])+" &#8451;"); //

   if (var == "LU_DRUK_WERT")   return( String(ActuellerWert_f[LuDru])+" hPa"); //
   if (var == "LU_DRUK_MAX")   return(String(MaxWert_f[LuDru])+" hPa"); //
   if (var == "LU_DRUK_MIN")   return(String(MinWert_f[LuDru])+" hPa"); //

   if (var == "IN1_FEUT_WERT")   return(String(ActuellerWert_b[In1Feu])+" &#37;"); //
   if (var == "IN1_FEUT_MAX")   return(String(MaxWert_b[In1Feu])+" &#37;"); //
   if (var == "IN1_FEUT_MIN")   return(String(MinWert_b[In1Feu])+" &#37;"); //

   if (var == "IN2_FEUT_WERT")  return(String(ActuellerWert_b[In2Feu])+" &#37;"); //
   if (var == "IN2_FEUT_MAX")   return(String(MaxWert_b[In2Feu])+" &#37;"); //
   if (var == "IN2_FEUT_MIN")   return(String(MinWert_b[In2Feu])+" &#37;"); //

   if (var == "AU1_FEUT_WERT")   return(String(ActuellerWert_b[Au1Feu])+" &#37;"); //
   if (var == "AU1_FEUT_MAX")   return(String(MaxWert_b[Au1Feu])+" &#37;"); //
   if (var == "AU1_FEUT_MIN")   return(String(MinWert_b[Au1Feu])+" &#37;"); //

   if (var == "BO1_FEUT_WERT")  return(String(ActuellerWert_b[Bo1Feu])+" &#37;"); //
   if (var == "BO1_FEUT_MAX")   return(String(MaxWert_b[Bo1Feu])+" &#37;"); //
   if (var == "BO1_FEUT_MIN")   return(String(MinWert_b[Bo1Feu])+" &#37;"); //

   if (var == "BO2_FEUT_WERT")  return(String(ActuellerWert_b[Bo2Feu])+" &#37;"); //
   if (var == "BO2_FEUT_MAX")   return(String(MaxWert_b[Bo2Feu])+" &#37;"); //
   if (var == "BO2_FEUT_MIN")   return(String(MinWert_b[Bo2Feu])+" &#37;"); //

   if (var == "BO3_FEUT_WERT")  return(String(ActuellerWert_b[Bo3Feu])+" &#37;"); //
   if (var == "BO3_FEUT_MAX")   return(String(MaxWert_b[Bo3Feu])+" &#37;"); //
   if (var == "BO3_FEUT_MIN")   return(String(MinWert_b[Bo3Feu])+" &#37;"); //

   if (var == "BO4_FEUT_WERT")  return(String(ActuellerWert_b[Bo4Feu])+" &#37;"); //
   if (var == "BO4_FEUT_MAX")   return(String(MaxWert_b[Bo4Feu])+" &#37;"); //
   if (var == "BO4_FEUT_MIN")   return(String(MinWert_b[Bo4Feu])+" &#37;"); //

   if (var == "RELAIS1_TEXT") return (Relais1State ?"I" : "O");
   if (var == "RELAIS2_TEXT") return (Relais2State ?"I" : "O");
   if (var == "RELAIS3_TEXT") return (Relais3State ?"I" : "O");
   if (var == "RELAIS4_TEXT") return (Relais4State ?"I" : "O");

   if (var== "CLASS_RELAIS1" ) return (Relais1State ? "button_I" : "button_O");
   if (var== "CLASS_RELAIS2" ) return (Relais2State ? "button_I" : "button_O");
   if (var== "CLASS_RELAIS3" ) return (Relais3State ? "button_I" : "button_O");
   if (var== "CLASS_RELAIS4" ) return (Relais4State ? "button_I" : "button_O");

   if (var== "X_ARRAY" )      return(TextArray());
   if (var == "IN1_TEMP_ARRAY") return(TextArray(In1Tmp));
   if (var == "IN2_TEMP_ARRAY") return(TextArray(In2Tmp));
   if (var == "AU1_TEMP_ARRAY")  return(TextArray(Au1Tmp));
   if (var == "BO1_TEMP_ARRAY") return(TextArray(Bo1Tmp));
   if (var == "BO2_TEMP_ARRAY") return(TextArray(Bo2Tmp));
   if (var == "BO3_TEMP_ARRAY") return(TextArray(Bo3Tmp));
   if (var == "BO4_TEMP_ARRAY") return(TextArray(Bo4Tmp));
  
  if (var == "IN1_FEUT_ARRAY")  return(TextArray(In1Feu));
  if (var == "IN2_FEUT_ARRAY") return(TextArray(In2Feu));
  if (var == "AU1_FEUT_ARRAY")  return(TextArray(Au1Feu));
  if (var == "BO1_FEUT_ARRAY") return(TextArray(Bo1Feu));
  if (var == "BO2_FEUT_ARRAY") return(TextArray(Bo2Feu));
  if (var == "BO3_FEUT_ARRAY") return(TextArray(Bo3Feu));
  if (var == "BO4_FEUT_ARRAY") return(TextArray(Bo4Feu));
  if (var == "LU_DRUK_ARRAY")  return(TextArray(LuDru));

  if(var == "ON_TEMP")        return(String(OnTemp_Heizer)+" &#8451;");
  if(var == "OFF_TEMP")       return(String(OffTemp_Heizer)+" &#8451;");
  if (var =="RELAIS1_ARRAY")  return(TextArray(Relais1,20.0,25.0));
  
  if (var == "I2CDEV")  return(DebugI2Cscan());
  if (var == "I2CANZ")  return(DebugI2CnDev());

  if (var == "SAHTAU")     return(SennsorErr(AHT_au)? "OK": "Error");
  if (var == "SAHTIN")     return(SennsorErr(AHT_in)? "OK": "Error");
  if (var == "SBME")       return(SennsorErr(BME)   ? "OK": "Error");
  if (var == "SNPOW")      return(SennsorErr(Power) ? "OK": "Error");
  if (var == "SNLI")       return(SennsorErr(TSL)   ? "OK": "Error");
  if (var == "SNDALLAS")   return(SennsorErr(Dallas)? "OK": "Error");
  if (var == "SNANALOGT")  return(SennsorErr(Analog)? "OK": "Testen");
  if (var == "ANZ")        return(String(ANZAHL_Dallas));
  if (var == "POWERVOLT")  return(DebugVolt());
  if (var == "POWERSTRO")  return(DebugStrom());
  if (var == "STROMINMA") return(String(current_mA)+" mA ");
  if (var == "MINMAXSTRO") return(String(MaxCurrent)+" mA ");
  if (var == "POWERLEIS")   return(DebugLeistung());
  if (var == "WIFIRSSI")   return(String(Feldstaerke_dBm)+" dBm");
  if (var == "WIFIMAXMINRSSI")   return(String(MinFeldstaerke)+" dBm .... "+ String(MaxFeldstaerke)+" dBm");

  if (var == "CLASS_BIT1")  return( RegelungTemper ? "button_I" : "button_O");
  if (var == "CLASS_BIT2")  return( RegelungWasser ? "button_I" : "button_O");
  if (var == "CLASS_BIT3")  return( !Sensor_Da[Analog] ? "button_I" : "button_O");
  if (var == "BIT1_TEXT")   return( RegelungTemper ? "I" : "O");
  if (var == "BIT2_TEXT")   return( RegelungWasser ? "I" : "O");
  if (var == "BIT3_TEXT")   return( !Sensor_Da[Analog] ? "I" : "O");

  if (var == "ICON_1")      return(IconHochdruck());
  if (var == "ICON_2")      return(IconHeizen());
  if (var == "ICON_3")      return(IconFenster());
  if (var == "ICON_4")      return(IconNeu());

  if (var == "HEITZDAUER")return(TextZeitHeizen());
   //return var;
   return String();
}

String Messdaten::DebugVolt()
{
  Text.clear();
  sprintf(buffer,"Bus Voltage:   %5.2f  V \n",busvoltage);   Text = buffer;
  sprintf(buffer,"Shunt Voltage: %5.2f mV \n",shuntvoltage); Text += buffer;
  sprintf(buffer,"Load Voltage:  %5.2f  V \n",loadvoltage);  Text += buffer;

  loadvoltage = busvoltage + (shuntvoltage / 1000);
  sprintf(buffer,"Voltage:  %5.2f  V ",loadvoltage);
  return buffer;
}
String Messdaten::DebugStrom()
{
  sprintf(buffer,"Current:       %5.2f mA \n",current_mA);   
  return buffer;
}
String Messdaten::DebugLeistung()
{
  sprintf(buffer,"Power:         %5.2f mW \n",power_mW); 
  return buffer;
}
String Messdaten::TextZeitHeizen()
{
  int DiffSec = (int)round(HeizenGesmatZeit);
  int min = DiffSec / 3600;

  sprintf(buffer,"%02d:%02d:%02d", DiffSec % 3600,  (min / 60 ), (min % 60 ));
  return buffer;
}
String Messdaten::TextZeit(tm Uhrzeit)
{
  sprintf(buffer,"%02d:%02d:%02d", Uhrzeit.tm_hour, Uhrzeit.tm_min, Uhrzeit.tm_sec);
  return buffer;
}
String Messdaten::TextTag(tm Uhrzeit)
{
  sprintf(buffer,"%02d:%02d:%04d", Uhrzeit.tm_mday, (Uhrzeit.tm_mon +1), (Uhrzeit.tm_year +1900));
  return buffer;
}
String Messdaten::TextTagZeit(tm Uhrzeit)
{
  String TagJetzt;
  TagJetzt = wochentage[Uhrzeit.tm_wday];
  TagJetzt += "\tder ";
  TagJetzt += String(Uhrzeit.tm_mday);
  TagJetzt += " " + monat[Uhrzeit.tm_mon] + " ";
  TagJetzt += String(Uhrzeit.tm_year + 1900)+" - "+TextZeit(Uhrzeit);;
  return TagJetzt;
}
String Messdaten::TextLogFile(tm Uhrzeit)
{
  sprintf(buffer,"/DataTag%04d%02d%02d.log", (Uhrzeit.tm_year +1900), (Uhrzeit.tm_mon +1), Uhrzeit.tm_mday);
  return buffer;
}

String Messdaten::TextESPDataFile(tm Uhrzeit)
{
  sprintf(buffer,"/ESPDataTag%04d%02d%02d.log", (Uhrzeit.tm_year +1900), (Uhrzeit.tm_mon +1), Uhrzeit.tm_mday);
  return buffer;
}

String Messdaten::TextArray()
{
  u8_t h =0,m = 0;
  Text ="[";
  for(int i =0; i < (MaxMessungen+1); i++)
  {
    sprintf(buffer,"\"%2d:%02d\"", h,m);
    Text +=buffer;
    if (i== (MaxMessungen))   { Text += "];";}
    else                      { Text += "," ;} 
    m += Abtastzeit;
    if (m==60) 
    {
      h++;
      m=0;
    }
  }
  return  Text;
}
String Messdaten::TextArray(WerteMessen_f type)
{
  Text = "[";
  if (AnzeigevollKurve) EndPunktKurve = MaxMessungen+1;
  else  EndPunktKurve = EndZeitKurve+1;
    for(int i =0; i < EndPunktKurve; i++)
    {
        Text += String(ZeitWerte_f[type][i]);
        if (i== (EndPunktKurve-1))           
        {
            if (AnzeigevollKurve) Text +=","+ String(ZeitWerte_f[type][0])+"];";
            else  Text +=  "];";
        }
        else  { Text += "," ;} 
    }
    return Text;
}
String Messdaten::TextArray(WerteMessen_b type)
{
  Text = "[";
  if (AnzeigevollKurve) EndPunktKurve = MaxMessungen+1;
  else  EndPunktKurve = EndZeitKurve+1;
    for(int i =0; i < EndPunktKurve; i++)
    {
        Text += String(ZeitWerte_b[type][i]);
        if (i== (EndPunktKurve-1))         
        {
          if (AnzeigevollKurve) Text +=","+ String(ZeitWerte_b[type][0])+"];";
          else            Text +="];";
        }
        else                               { Text += "," ;} 
    }
    return Text;
}
String Messdaten::TextArray(WerteMessen_d type, float von, float bis)
{
    Text = "[";
    if (AnzeigevollKurve) EndPunktKurve = MaxMessungen+1;
    else  EndPunktKurve = EndZeitKurve+1;

    for(int i =0; i < EndPunktKurve; i++)
    {
        Text += String(von + ZeitWerte_d[type][i]*(bis-von));
        if (i== (EndPunktKurve-1))           
        {
            if (AnzeigevollKurve) Text +=  ","+ String(von + ZeitWerte_d[type][0]*(bis-von))+"];";
            else Text += "];";
        }
        else                                { Text += "," ;} 
    }
    return Text;
}

String Messdaten::IconHochdruck()
{
  float LD = 1015;
  if (Sensor_Da[BME]) LD = Sensor_BME.readPressure() /100;
  if( LD > 1013.25) // Hochdrucklage
    return("<span class='InfoIcon'>&#127774;</span>");
  else
    return("<span class='InfoIcon'>  &#9748;</span>");  
  return String();
}
String Messdaten::IconHeizen()
{
  if( Relais1State) // Heizung ein
    return("<span class='InfoIcon'>&#128308;</span>");
  else
    return("<span class='InfoIcon'>&#128309;</span>");  
  return String();
}
String Messdaten::IconFenster()
{
  if( digitalRead(PIN_IN_1)|| digitalRead(PIN_IN_2)) // Fenster auf
    return("<span class='InfoIcon'>&#128256;</span>");
  else
    return("<span class='InfoIcon'>&#128257;</span>");  
  return String();
}
String Messdaten::IconNeu()
{
  if( digitalRead(PIN_IN_1)|| digitalRead(PIN_IN_2)) // Fenster auf
    return("<span class='InfoIcon'>&#128256;</span>");
  else
    return("<span class='InfoIcon'>&#128257;</span>");  
  return String();
}

/* byte i = 0;
  if (Sensor_DHT != NULL)
   {
    do 
    {
      ActuellerWert = Sensor_DHT->getTempAndHumidity();
      Error = (Sensor_DHT->getStatus() != 0);
      if ((i == 50) & (Error) )
      {
        ErrorNr++;  
        Error = false;
        ActuellerWert = {-8.88,-8.88};
      }
      i++;
    }while (Error);
   }
*/
#endif

