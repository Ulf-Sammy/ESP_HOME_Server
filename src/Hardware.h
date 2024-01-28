#ifndef HARDWARE_H
#define HARDWARE_H

#include <WString.h>


// Hardware Zuordung

// GPIO 0
// GPIO 1 = Tx Serial
// GPIO 3 = Rx Serial

// GPIO 6
// GPIO 7
// GPIO 8
// GPIO 9
// GPIO 10
// GPIO 11

#define PIN_LED 2       // GPIO 2 = LED 
#define PIN_MES_ANG 4   // GPIO 4 Select Messung Bodenfeuchte einschalten
#define ONE_WIRE_BUS 13 // GPIO 13
#define PIN_IN_1 27      // GPIO 27
#define PIN_IN_2 14     // GPIO 14
#define PIN_IN_3 12     // GPIO 12
#define PIN_IN_4 15     // GPIO 15
#define PIN_RELAIS_1 16 // GPIO 16
#define PIN_RELAIS_2 17 // GPIO 17
#define PIN_SD_SC 5     // GPIO 5 SD Schip Select 
#define PIN_SD_CLK 18   // GPIO 18
#define PIN_SD_MISO 19  // GPIO 19
#define PIN_SD_MOSI 23   // GPIO 23 

#define PIN_I2C_SCA  21  // GPIO 21 I2C: SCA
#define PIN_I2C_SCL  22  // GPIO 22 I2C: SCL
#define PIN_RELAIS_3 25  // GPIO 25
#define PIN_RELAIS_4 26  // GPIO 26
#define PIN_DHT22_1 32   // GPIO 32
#define PIN_DHT22_2 33   // GPIO 33

#define PIN_Analog1 36   // GPIO 36
#define PIN_Analog2 39   // GPIO 39
#define PIN_Analog3 34   // GPIO 34
#define PIN_Analog4 35   // GPIO 35
//
#define ANZAHL_Dallas 2
// ende der Hardware
// Wifi Daten
#define WIFI_SSID "ThePeanuts"
#define WIFI_PASSWORD "Schulz1965#Snoopy"
#define WIFI_CHANNEL 1
// gespeicherte Daten
#define Abtastzeit 10 //10 // 10min
#define MaxMessungen 24*60 / Abtastzeit // 144 = 24 *60/10 alle 10 min

enum Geraete {Glaspalast,Lokschuppen,Pavillon,Grill,Heizung,PCPlatz };

const char wochentage[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
const String monat[12] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
const String WiFi_Error200[5] = {"Beacon Timeout"," no AP found","authentication failed","Assoc failed","handshake timeout" };
const String WiFi_Error000[23] = {"UNSPECIFIED","AUTH_EXPIRE", "REASON_AUTH_LEAVE","REASON_ASSOC_EXPIRE","REASON_ASSOC_TOOMANY","REASON_NOT_AUTHED","REASON_NOT_ASSOCED","REASON_ASSOC_LEAVE","REASON_ASSOC_NOT_AUTHED",  "REASON_DISASSOC_PWRCAP_BAD","REASON_DISASSOC_SUPCHAN_BAD", "REASON_IE_INVALID" ,"REASON_MIC_FAILURE","REASON_4WAY_HANDSHAKE_TIMEOUT","REASON_GROUP_KEY_UPDATE_TIMEOUT", "REASON_IE_IN_4WAY_DIFFERS","REASON_GROUP_CIPHER_INVALID","REASON_PAIRWISE_CIPHER_INVALID", "REASON_AKMP_INVALID","REASON_UNSUPP_RSN_IE_VERSION","REASON_INVALID_RSN_IE_CAP","REASON_802_1X_AUTH_FAILED", "REASON_CIPHER_SUITE_REJECTED"};


struct Mess_Daten
{ // temperature
   float Innen_1T;
   float Innen_2T;
   float Aussen_1T;
   float Boden_1T;
   float Boden_2T;
   float Boden_3T;
   float Boden_4T;
   // diverse
   float Luftdruck_1;
   float LichtLux_1;
   // Feuchtigkiet
   uint8_t Innen_1F;
   uint8_t Innen_2F;
   uint8_t Aussen_1F;
   uint8_t Boden_1F;
   uint8_t Boden_2F;
   uint8_t Boden_3F;
   uint8_t Boden_4F;
};

struct Flag_Daten
{
   bool Relais_1;
   bool Relais_2;
   bool Relais_3;
   bool Relais_4;
   bool Input_1;
   bool Input_2;
   bool Input_3;
   bool Input_4;
   bool Bit_1;
   bool Bit_2;
   bool Bit_3;
   bool Bit_4;
   bool Bit_5;
   bool Bit_6;
   bool Bit_7;
   bool Bit_8;
   bool Bit_9;
};

struct Spez_Daten
{
   uint8_t RSSI;
   uint8_t Strom;
};
/*

   REASON_UNSPECIFIED              = 1,
   REASON_AUTH_EXPIRE              = 2,
   REASON_AUTH_LEAVE               = 3,
   REASON_ASSOC_EXPIRE             = 4,
   REASON_ASSOC_TOOMANY            = 5,
   REASON_NOT_AUTHED               = 6,
   REASON_NOT_ASSOCED              = 7,
   REASON_ASSOC_LEAVE              = 8,
   REASON_ASSOC_NOT_AUTHED         = 9,
   REASON_DISASSOC_PWRCAP_BAD      = 10,  
   REASON_DISASSOC_SUPCHAN_BAD     = 11,  
   REASON_IE_INVALID               = 13,  
   REASON_MIC_FAILURE              = 14,  
   REASON_4WAY_HANDSHAKE_TIMEOUT   = 15,  
   REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,  
   REASON_IE_IN_4WAY_DIFFERS       = 17,  
   REASON_GROUP_CIPHER_INVALID     = 18,  
   REASON_PAIRWISE_CIPHER_INVALID  = 19,  
   REASON_AKMP_INVALID             = 20,  
   REASON_UNSUPP_RSN_IE_VERSION    = 21,  
   REASON_INVALID_RSN_IE_CAP       = 22,  
   REASON_802_1X_AUTH_FAILED       = 23,  
   REASON_CIPHER_SUITE_REJECTED    = 24,  
*/
/*
   REASON_BEACON_TIMEOUT       = 200,
   REASON_NO_AP_FOUND          = 201,
   REASON_AUTH_FAIL            = 202,
   REASON_ASSOC_FAIL           = 203,
   REASON_HANDSHAKE_TIMEOUT    = 204,
*/
#endif
