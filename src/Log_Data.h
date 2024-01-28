#ifndef LOG_DATA_H
#define LOG_DATA_H

#include "Hardware.h"
//#include "CMessdaten.h"
#include <SD.h>
#include <ESPAsyncWebServer.h>



class Log_Data_File
{
  private:
    bool Error;
    u_int64_t SD_Size;
    u_int64_t total_Size;
    u_int64_t used_Size;
    sdcard_type_t SD_Type;
    bool SD_Error;
    bool Error_MemFile;
    bool Error_LogFile;
    bool Error_DebugFile;
    bool Exist_MemFile;
    File Temp_File;
    const  String Debug1FileName = "/Debug1Data.txt";
    const  String Debug2FileName = "/Debug2Data.txt";
    const String memFileName   = "/MemData.txt";
    String LogDateiESP_data;
    String LogDateiHeute;
    String LogDateiGestern;

  public:
  void Begin();
  bool Get_SD_Size();
  File speicher_MemFile();
  File speicher_DataFile(String DateiName);
  File speicher_ESPDataFile(String DateiName);
  File speicher_DebugFile(u_int8_t Nr);
  File lese_MemFile();
  File lese_DataFile(String DateiName);
  File lese_ESPDataFile(String DateiName);
  void Send_MemFile(AsyncWebServerRequest* p);
  void Send_DataFile(AsyncWebServerRequest* p,String DateiName );
  void Send_DebugFile(AsyncWebServerRequest* p,u_int8_t Nr);
  
  String replaceVariable(const String& var);
  void downloadMemFile( AsyncWebServerRequest* p);
};


#endif