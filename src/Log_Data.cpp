#include "Log_Data.h"

void Log_Data_File::Begin()
{
  Error = !SD.begin(PIN_SD_SC);
  Get_SD_Size();
}
bool Log_Data_File::Get_SD_Size()
{
  SD_Size = SD.totalBytes();
  SD_Size = SD.cardSize();
  total_Size = SD.totalBytes();
  used_Size = SD.usedBytes();
  
  return Error;
};

File Log_Data_File::speicher_MemFile()
{
   Exist_MemFile = SD.exists(memFileName);
   Temp_File = SD.open(memFileName,FILE_WRITE);
   return (Temp_File);
}

File Log_Data_File::speicher_DataFile(String DateiName)
{
  if (SD.exists(DateiName))
  {    Temp_File =  SD.open(DateiName,FILE_APPEND); }
  else
  {    Temp_File =  SD.open(DateiName,FILE_WRITE);  }
  return (Temp_File);
}

File Log_Data_File::speicher_ESPDataFile(String DateiName)
{
  if (SD.exists(DateiName))
  {    Temp_File =  SD.open(DateiName,FILE_APPEND); }
  else
  {    Temp_File =  SD.open(DateiName,FILE_WRITE);  }
  return (Temp_File);
}

File Log_Data_File::speicher_DebugFile(u_int8_t Nr)
{
  if (Nr == 1)   
  {
    if (SD.exists(Debug1FileName))
    {     Temp_File = SD.open(Debug1FileName.c_str(), FILE_APPEND);}
    else
    {     Temp_File = SD.open(Debug1FileName.c_str(), FILE_WRITE);}
  }
  if (Nr == 2)   
  {
    if (SD.exists(Debug2FileName))
    {     Temp_File = SD.open(Debug2FileName.c_str(), FILE_APPEND);}
    else
    {     Temp_File = SD.open(Debug2FileName.c_str(), FILE_WRITE);}
  }
  return (Temp_File);
}

File Log_Data_File::lese_MemFile()
{
    return SD.open(memFileName,FILE_READ);
}

File Log_Data_File::lese_DataFile(String DateiName)
{
    return SD.open(DateiName,FILE_READ);
}

File Log_Data_File::lese_ESPDataFile(String DateiName)
{
    return SD.open(DateiName,FILE_READ);
}

void Log_Data_File::Send_MemFile(AsyncWebServerRequest *p)
{
   Temp_File = SD.open(memFileName,FILE_READ);
   if (Temp_File)
   {
      p->send(SD, memFileName, "application/octet-stream", true);
      Temp_File.close();
  } 
}

void Log_Data_File::Send_DataFile(AsyncWebServerRequest *p, String DateiName)
{
   Temp_File = SD.open(DateiName,FILE_READ);
    if (Temp_File)
    {
      p->send(SD, DateiName, "application/octet-stream", true);
      Temp_File.close();
    } 

}

void Log_Data_File::Send_DebugFile(AsyncWebServerRequest *p, u_int8_t Nr)
{
  if (Nr == 1)
  {   
    Temp_File = SD.open(Debug1FileName,FILE_READ); 
           p->send(SD, Debug1FileName	, "application/octet-stream", true);  }
  if (Nr == 2)
  {
     Temp_File = SD.open(Debug2FileName,FILE_READ);
    p->send(SD, Debug2FileName, "application/octet-stream", true);
  }
 if (Temp_File)  Temp_File.close();
}

String Log_Data_File::replaceVariable(const String &var)
{
  
  if (var == "SD_ERROR")    return(SD_Error ? "Error": "OK");
  if (var == "ERROR_MEMFILE") return(Error_MemFile ? "OK": "Error");
  {
    /* code */
  }
  
  if (var == "SD_TYPE")
  {
    switch (SD_Type)
    {
      case CARD_MMC:
        return ("SD Card Type: MMC");
        break;
      case CARD_SD:
        return ("SD Card Type: SDSC");
        break;
      case CARD_SDHC:
        break;
        return ("SD Card Type: SDHC");
      case CARD_UNKNOWN:
        break;
        return ("SD Card Type: unknown");
      case CARD_NONE:
        return ("SD Card Type: none");
        break;
      default:
        break;
    }
  }
  if (var == "SD_GROSS")    return( String(SD_Size    / (1024*1024))+" Mb;"); 
  if (var == "SD_TOTAL_BY") return( String(total_Size / (1024*1024))+" Mb;");
  if (var == "SD_USED_BY")  return( String(used_Size  / (1024*1024))+" Mb;");
    return String();
}

void Log_Data_File::downloadMemFile(AsyncWebServerRequest *p)
{
    const char* memFineName = "MemData.txt";

  File file = SD.open(memFineName,FILE_READ);
  if (file)
  {
      p->send(file, file.name(), "application/octet-stream", true);
      file.close();
  }
  
  /* HTML
   <a href = "/download"> Download memFile.txt </a>
   or
   <button onclick§location.href='download' " type="button"> Download memFile.txt </button>
  
  */
}
