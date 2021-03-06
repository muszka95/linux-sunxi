/* AUTO GENERATED on 2006-05-12 14:04:55 */

/*****************************************************************************

Copyright (c) 2004 by Nanoradio AB

This software is copyrighted by and is the sole property of Nanoradio AB.
All rights, title, ownership, or other interests in the
software remain the property of Nanoradio AB.  This software may
only be used in accordance with the corresponding license agreement.  Any
unauthorized use, duplication, transmission, distribution, or disclosure of
this software is expressly forbidden.

This Copyright notice may not be removed or modified without prior written
consent of Nanoradio AB.

Nanoradio AB reserves the right to modify this software without
notice.

Nanoradio AB
Torshamnsgatan 39                       info@nanoradio.se
164 40 Kista                            http://www.nanoradio.se
SWEDEN

Module Description :
==================
This module is autogenerated and implements accessfunction to the registry.

*****************************************************************************/
#include "driverenv.h"
#include "registry.h"
#include "registryAccess.h"
#include "hicWrapper.h"

/*****************************************************************************
T E M P O R A R Y   T E S T V A R I A B L E S
*****************************************************************************/

/*****************************************************************************
C O N S T A N T S / M A C R O S
*****************************************************************************/

#define VALIDATE_CACHE(_name, _pos) \
    do\
    {\
        char* value_name_pos;\
        DE_BUG_ON(_pos == NULL, "Bad registry entry encountered while looking for \"%s\"\n",_name);\
        value_name_pos = _pos - DE_STRLEN(_name) - 2;\
        if(DE_STRNCMP(value_name_pos, _name, DE_STRLEN(_name)))\
        {\
            char buffer[128];\
            DE_STRNCPY(buffer, value_name_pos, DE_STRLEN(_name));\
            buffer[DE_STRLEN(_name)] = 0;\
            DE_BUG_ON(1,"Registry parse failed. Expected \"%s\", found \"%s\"\n",_name,buffer);\
        }\
    }while(0)

/*****************************************************************************
L O C A L   D A T A T Y P E S
*****************************************************************************/

/*****************************************************************************
L O C A L   F U N C T I O N   P R O T O T Y P E S
*****************************************************************************/

/*****************************************************************************
 M O D U L E   V A R I A B L E S
*****************************************************************************/
/************************************************
** Cache of registry objects.
************************************************/
extern rRegistry registry;

const  char  TABS[21] = "                    ";
static int   nrTabs = 0;

const char  HEX_DIGIT[] = "0123456789ABCDEF";
const char  SINGLE_NUMBER[] = "";

int IsSpaceNR(char c)
{
   if ( (c == ' ') 
     || (c == '\t') )
   {
      return TRUE;
   }
   return FALSE;
}

char* AscciHex2bin(char* text, char* bin, const char* delimiter)
{
   int val;
   int len = (int)DE_STRLEN(delimiter);
   while( ((*text >= '0') && (*text <= '9'))
       || ((*text >= 'A') && (*text <= 'F'))
       || ((*text >= 'a') && (*text <= 'f')) )
   {
      
      val = text[0] - ( (text[0] > '9') ? ( text[0] > 'F' ? ('a' - 10) : ('A' - 10) ): '0');

      val = val << 4;
      val += text[1] - ( (text[1] > '9') ? ( text[1] > 'F' ? ('a' - 10) : ('A' - 10) ): '0');

      text  += 2;
      *bin++ = (uint8_t)val;
      if ( (delimiter == SINGLE_NUMBER)
        || (DE_STRNCMP(text, delimiter, len) != 0) )
      {
         while(IsSpaceNR(*text)) text++;
         break; /* Break the while loop. */
      }
      else
      {
         text += len;
      }
   }
   return text;
}

char* AscciHex2binN(char* text, char* bin, const char* delimiter, int *n)
{
   int val, nmax;
   int len = (int)DE_STRLEN(delimiter);
   nmax = *n;
   *n = 0;
   while( ((*text >= '0') && (*text <= '9'))
       || ((*text >= 'A') && (*text <= 'F'))
       || ((*text >= 'a') && (*text <= 'f')) )
   {
      if (0 == nmax)
      {
         break;
      }
      val = DE_STRTOL(text, &text, 16);
      (*n)++;
      nmax--;
      *bin++ = val;
      if ( (delimiter == SINGLE_NUMBER)
        || (DE_STRNCMP(text, delimiter, len) != 0) )
      {
         while(IsSpaceNR(*text)) text++;
         break; /* Break the while loop. */
      }
      else
      {
         text += len;
      }
   }
   return text;
}

char* Bin2HexAscii(char* text, char* bin, int len, const char* delimiter)
{
   int val;
   while (len)
   {
      val = *bin;
      text[1] = HEX_DIGIT[val & 0x0f];
      val = val >> 4;
      text[0] = HEX_DIGIT[val & 0x0f];

      bin++;
      len--;
      text += 2;
      if (len)
      {
         DE_STRCPY(text, delimiter);
         text += DE_STRLEN(text);
      }
   }
   /* Null terminate the string. */
   *text = 0x00;
   return text;
}

int AscciDec2Int(char** text_ref)
{
   int val = 0;
   int neg = 0;
   char* pos = *text_ref;

   if (*pos == '-')
   {
      neg = 1;
      pos++;
   }
   while((*pos >= '0') && (*pos <= '9'))
   {
      val = val * 10;
      val += *pos - '0';
      pos++;
   }
   *text_ref = pos;
   return (neg ? -val : val);
}

char* Bin2DecAscii(char* text, int bin)
{
   int val = bin;
   if (bin < 0)
   {
      *text = '-';
      text++;
   }
   while (val)
   {
      *text = (uint8_t)('0' + (val % 10));
      val = val / 10;
      text++;
   }
   /* Null terminate the string. */
   *text = 0x00;
   return text;
}


/*****************************************************************************
G L O B A L   C O N S T A N T S / V A R I A B L E S
*****************************************************************************/
#define CACHE_INTEGER_TYPE(storage, action, object_p, type, name)    \
         {                                                           \
            int value = (int)*object_p;                              \
            Cache_BasicType(storage, action, &value, name);          \
            *object_p = (type)value;                                 \
         }

void Cache_BasicType(PersistentStorage_t* storage, Action_t action, int* object_p, char* name)
{
   char* pos; 
   switch (action)
   {
      case CACHE_ACTION_READ:
      {
         pos = DE_STRCHR(storage->ptr, ':') + 2;
         VALIDATE_CACHE(name, pos);         
         *object_p = AscciDec2Int(&pos);
         break;
      }
      case CACHE_ACTION_WRITE:
         DE_SPRINTF(storage->ptr, "%s%s: %d\n", &TABS[20-nrTabs], name, *object_p);
         break;
   }
   storage->ptr += DE_STRLEN(storage->ptr) + 1;
}

void Cache_TreeStructure(PersistentStorage_t* storage, Action_t action, char* name)
{
   switch (action)
   {
      case CACHE_ACTION_READ:
         break;

      case CACHE_ACTION_WRITE:
         DE_SPRINTF(storage->ptr, "%s%s\n", &TABS[20-nrTabs], name);
         break;
   }
   storage->ptr += DE_STRLEN(storage->ptr) + 1;
}

/*****************************************************************************
G L O B A L   F U N C T I O N S (Hand Coded)
*****************************************************************************/

void Cache_m80211_ie_ssid_t(PersistentStorage_t* storage, Action_t action, m80211_ie_ssid_t* object_p, char* name)
{
   char* pos; 

   switch (action)                       
   {                                                  
      case CACHE_ACTION_READ:
         pos = DE_STRCHR(storage->ptr, ':') + 2;
         VALIDATE_CACHE(name, pos);         
         pos = AscciHex2bin(pos, (char*)&object_p->hdr.id, SINGLE_NUMBER);
         if (object_p->hdr.id == M80211_IE_ID_NOT_USED)
         {
            object_p->hdr.len = 0;
         }
         else
         {
            DE_STRNCPY(object_p->ssid, pos, M80211_IE_MAX_LENGTH_SSID);
            object_p->hdr.len = (m80211_ie_len_t)DE_STRLEN(object_p->ssid);
         }
         break;                                       

      case CACHE_ACTION_WRITE:                        
         if (object_p->hdr.id == M80211_IE_ID_NOT_USED
             || object_p->hdr.len == 0) /* XXX zero-size SSID is ok */
         {
            DE_SPRINTF(storage->ptr, "%s%s: %02X\n", &TABS[20-nrTabs], name, M80211_IE_ID_NOT_USED);
         }
         else
         {
            char ssid[M80211_IE_MAX_LENGTH_SSID + 1];
            DE_MEMCPY(ssid, object_p->ssid, object_p->hdr.len);
            ssid[object_p->hdr.len] = '\0';
            DE_SPRINTF(storage->ptr, "%s%s: %02X %s\n", &TABS[20-nrTabs], name,
                       object_p->hdr.id, ssid);  
         }
         break;                                       
   } 
   storage->ptr += DE_STRLEN(storage->ptr) + 1;
} /* Cache_m80211_ie_ssid_t */

void Cache_m80211_mac_addr_t(PersistentStorage_t* storage, Action_t action, m80211_mac_addr_t* object_p, char* name)
{
   char* pos; 
   const char* Delimiter = ":";
   int size = M80211_ADDRESS_SIZE;

   switch (action)                       
   {                                                  
      case CACHE_ACTION_READ:
      {    
         pos = DE_STRCHR(storage->ptr, ':') + 2;
         VALIDATE_CACHE(name, pos);         
         AscciHex2bin(pos, (char*)&(object_p->octet[0]), Delimiter);
         break;                                       
      }                                               
      case CACHE_ACTION_WRITE:                        
         DE_SPRINTF(storage->ptr, "%s%s: ", &TABS[20-nrTabs], name);  
         pos = &storage->ptr[DE_STRLEN(storage->ptr)];
         pos = Bin2HexAscii(pos, (char*)&(object_p->octet[0]), size, Delimiter);
         DE_STRCPY(pos, "\n");
         break;                                       
   } 
   storage->ptr += DE_STRLEN(storage->ptr) + 1;
} /* Cache_m80211_mac_addr_t */



void Cache_channel_list_t(PersistentStorage_t* storage, Action_t action, channel_list_t* object_p, char* name)
{
   char* pos; 
   const char* Delimiter = ",";
   int no_channels;

   switch (action) 
   { 
      case CACHE_ACTION_READ:
         pos = DE_STRCHR(storage->ptr, ':') + 2;
         VALIDATE_CACHE(name, pos);
         no_channels = AscciDec2Int(&pos);
         pos++;
         AscciHex2bin(pos, (char*)&object_p->channelList[0], Delimiter);
         object_p->no_channels = (uint8_t)no_channels;
         break;

      case CACHE_ACTION_WRITE:
         no_channels = object_p->no_channels;
         DE_SPRINTF(storage->ptr, "%s%s: %02d ", &TABS[20-nrTabs], name, no_channels);  
         pos = &storage->ptr[DE_STRLEN(storage->ptr)];
         pos = Bin2HexAscii(pos, (char*)&object_p->channelList[0], no_channels, Delimiter);
         DE_STRCPY(pos, "\n");
         break;
   } 
   storage->ptr += DE_STRLEN(storage->ptr) + 1;
} /* Cache_channel_list_t */

void Cache_rSupportedRates(PersistentStorage_t* storage, Action_t action, rSupportedRates* object_p, char* name)
{
   char* pos; 
   const char* Delimiter = ",";

   switch (action)                                    
   {                                                  
      case CACHE_ACTION_READ:                         
         pos = DE_STRCHR(storage->ptr, ':') + 2;
         VALIDATE_CACHE(name, pos);
         pos = AscciHex2bin(pos, (char*)&object_p->hdr.id, SINGLE_NUMBER);
         if (object_p->hdr.id == M80211_IE_ID_NOT_USED)
         {
            object_p->hdr.len = 0;
         }
         else
         {
            object_p->hdr.len = (m80211_ie_len_t)AscciDec2Int(&pos);
            if (object_p->hdr.len <= sizeof(object_p->rates))
            {
               pos = DE_STRCHR(pos, ' ') + 1;
               DE_ASSERT(pos != NULL);
               AscciHex2bin(pos, (char*)object_p->rates, Delimiter);          
            }
            else
            {
               object_p->hdr.id  = M80211_IE_ID_NOT_USED;
               object_p->hdr.len = 0;
            }
         }
         break;                                       

      case CACHE_ACTION_WRITE:                        
         if ( (object_p->hdr.id  == M80211_IE_ID_NOT_USED)
           || (object_p->hdr.len == 0) 
           || (object_p->hdr.len > sizeof(object_p->rates)) )
         {
            DE_SPRINTF(storage->ptr, "%s%s: %02X\n", &TABS[20-nrTabs], name, M80211_IE_ID_NOT_USED);  
         }
         else
         {
            DE_SPRINTF(storage->ptr, "%s%s: %02X %03d ", &TABS[20-nrTabs], name, object_p->hdr.id, object_p->hdr.len);  
            pos = &storage->ptr[DE_STRLEN(storage->ptr)];
            DE_STRCPY(Bin2HexAscii(pos, (char*)object_p->rates, object_p->hdr.len, Delimiter), "\n");
         }
         break;                                       
   } 
   storage->ptr += DE_STRLEN(storage->ptr) + 1;
} /* Cache_rSupportedRates */

void Cache_rExtSupportedRates(PersistentStorage_t* storage, Action_t action, rExtSupportedRates* object_p, char* name)
{
   char* pos; 
   const char* Delimiter = ",";

   switch (action)                                    
   {                                                  
      case CACHE_ACTION_READ:                         
         pos = DE_STRCHR(storage->ptr, ':') + 2;
         VALIDATE_CACHE(name, pos);
         pos = AscciHex2bin(pos, (char*)&object_p->hdr.id, SINGLE_NUMBER);
         if (object_p->hdr.id == M80211_IE_ID_NOT_USED)
         {
            object_p->hdr.len = 0;
         }
         else
         {
            object_p->hdr.len = AscciDec2Int(&pos);
            if (object_p->hdr.len <= sizeof(object_p->rates))
            {
               pos = DE_STRCHR(pos, ' ') + 1;
               DE_ASSERT(pos != NULL);
               AscciHex2bin(pos, (char*)object_p->rates, Delimiter);          
            }
            else
            {
               object_p->hdr.id  = M80211_IE_ID_NOT_USED;
               object_p->hdr.len = 0;
            }
         }
         break;                                       

      case CACHE_ACTION_WRITE:                        
         if ( (object_p->hdr.id  == M80211_IE_ID_NOT_USED)
           || (object_p->hdr.len == 0) 
           || (object_p->hdr.len > sizeof(object_p->rates)) )
         {
            DE_SPRINTF(storage->ptr, "%s%s: %02X\n", &TABS[20-nrTabs], name, M80211_IE_ID_NOT_USED);  
         }
         else
         {
            DE_SPRINTF(storage->ptr, "%s%s: %02X %03d ", &TABS[20-nrTabs], name, object_p->hdr.id, object_p->hdr.len);  
            pos = &storage->ptr[DE_STRLEN(storage->ptr)];
            DE_STRCPY(Bin2HexAscii(pos, (char*)object_p->rates, object_p->hdr.len, Delimiter), "\n");
         }
         break;                                       
   } 
   storage->ptr += DE_STRLEN(storage->ptr) + 1;
} /* Cache_rExtSupportedRates */



void Cache_rATIMSet(PersistentStorage_t* storage, Action_t action, rATIMSet* object_p, char* name)
{
   char* pos; 

   switch (action)                                    
   {                                                  
      case CACHE_ACTION_READ:                         
         pos = DE_STRCHR(storage->ptr, ':') + 2;
         VALIDATE_CACHE(name, pos);
         pos = AscciHex2bin(pos, (char*)&object_p->hdr.id, SINGLE_NUMBER);
         if (object_p->hdr.id == M80211_IE_ID_NOT_USED)
         {
            object_p->hdr.len = 0;
         }
         else
         {
            object_p->hdr.len = (m80211_ie_len_t)AscciDec2Int(&pos);
            pos = DE_STRCHR(pos, ' ') + 1;
            DE_ASSERT(pos != NULL);
            object_p->atim_window = (uint16_t)AscciDec2Int(&pos);
         }
         break;                                       

      case CACHE_ACTION_WRITE:                        
         if ( (object_p->hdr.id  == M80211_IE_ID_NOT_USED)
           || (object_p->hdr.len == 0) )
         {
            DE_SPRINTF(storage->ptr, "%s%s: %02X\n", &TABS[20-nrTabs], name, M80211_IE_ID_NOT_USED);  
         }
         else
         {
            DE_SPRINTF(storage->ptr, "%s%s: %02X %03d ", &TABS[20-nrTabs], name, object_p->hdr.id, object_p->hdr.len);  
            pos = &storage->ptr[DE_STRLEN(storage->ptr)];
	    DE_STRCPY(Bin2DecAscii(pos, object_p->atim_window), "\n");
         }
         break;                                       
   } 
   storage->ptr += DE_STRLEN(storage->ptr) + 1;
} /* Cache_rATIMSet */


void Cache_rChannelSet(PersistentStorage_t* storage, Action_t action, rChannelSet* object_p, char* name)
{
   char* pos; 

   switch (action)                                    
   {                                                  
      case CACHE_ACTION_READ:                         
         pos = DE_STRCHR(storage->ptr, ':') + 2;
         VALIDATE_CACHE(name, pos);
         pos = AscciHex2bin(pos, (char*)&object_p->hdr.id, SINGLE_NUMBER);
         if (object_p->hdr.id == M80211_IE_ID_NOT_USED)
         {
            object_p->hdr.len = 0;
         }
         else
         {
            object_p->hdr.len = (m80211_ie_len_t)AscciDec2Int(&pos);
            pos = DE_STRCHR(pos, ' ') + 1;
            DE_ASSERT(pos != NULL);
            object_p->channel = (uint8_t)AscciDec2Int(&pos);
         }
         break;                                       

      case CACHE_ACTION_WRITE:                        
         if ( (object_p->hdr.id  == M80211_IE_ID_NOT_USED)
           || (object_p->hdr.len == 0) )
         {
            DE_SPRINTF(storage->ptr, "%s%s: %02X\n", &TABS[20-nrTabs], name, M80211_IE_ID_NOT_USED);  
         }
         else
         {
            DE_SPRINTF(storage->ptr, "%s%s: %02X %03d ", &TABS[20-nrTabs], name, object_p->hdr.id, object_p->hdr.len);  
            pos = &storage->ptr[DE_STRLEN(storage->ptr)];
	    DE_STRCPY(Bin2DecAscii(pos, object_p->channel), "\n");
         }
         break;                                       
   } 
   storage->ptr += DE_STRLEN(storage->ptr) + 1;
} /* Cache_rChannelSet */


void Cache_rVersionId(PersistentStorage_t* storage, Action_t action, rVersionId* object_p, char* name)
{
   char* pos; 
   switch (action)                       
   {                                                  
      case CACHE_ACTION_READ:
         pos = DE_STRCHR(storage->ptr, ':') + 2;
         VALIDATE_CACHE(name, pos);
         *object_p = (rVersionId)DE_STRTOUL(pos, NULL, 16);
/*          DE_ASSERT(Registry_VerifyVersion(*object_p)); */
         break;                                       

      case CACHE_ACTION_WRITE:                        
         DE_SPRINTF(storage->ptr, "%s%s: %08lX\n", &TABS[20-nrTabs], name,
                 Registry_VERSION_ID);  
         break;                                       
   } 
   storage->ptr += DE_STRLEN(storage->ptr) + 1;
} /* Cache_rVersionId */

#define Cache_rPowerSaveMode(_s, _a, _o, _n) CACHE_INTEGER_TYPE((_s),(_a),(_o),rPowerSaveMode,(_n))

#define Cache_rBSS_Type(_s, _a, _o, _n) CACHE_INTEGER_TYPE((_s),(_a),(_o),rBSS_Type,(_n))

#define Cache_rNetworkType(_s, _a, _o, _n) CACHE_INTEGER_TYPE((_s),(_a),(_o),rNetworkType,(_n))

#define Cache_rSTA_WMMSupport(_s, _a, _o, _n) CACHE_INTEGER_TYPE((_s),(_a),(_o),rSTA_WMMSupport,(_n))

#define Cache_sNetworkPresence(_s, _a, _o, _n) CACHE_INTEGER_TYPE((_s),(_a),(_o),sNetworkPresence,(_n))

#define Cache_sNetworkStatus(_s, _a, _o, _n) CACHE_INTEGER_TYPE((_s),(_a),(_o),sNetworkStatus,(_n))

void Cache_rRateList(PersistentStorage_t* storage, Action_t action, rRateList* object_p, char* name)
{
   char* pos; 
   const char* Delimiter = ",";
   int len;

   switch (action)                                    
   {                                                  
      case CACHE_ACTION_READ:                         
         pos = DE_STRCHR(storage->ptr, ':') + 2;
         VALIDATE_CACHE(name, pos);
         len = 0;
         if (STRLEN(pos) > 1)
         {
            len = sizeof object_p->rates;
            AscciHex2binN(pos, (char*)object_p->rates, Delimiter, &len);
         }
         object_p->len = len;
         break;                                       

      case CACHE_ACTION_WRITE:                        
         if ((object_p->len == 0) 
           || (object_p->len > sizeof(object_p->rates)) )
         {
            DE_SPRINTF(storage->ptr, "%s%s: \n", &TABS[20-nrTabs], name);  
         }
         else
         {
            DE_SPRINTF(storage->ptr, "%s%s: ", &TABS[20-nrTabs], name);  
            pos = &storage->ptr[DE_STRLEN(storage->ptr)];
            DE_STRCPY(Bin2HexAscii(pos, (char*)object_p->rates, object_p->len, Delimiter), "\n");
         }
         break;                                       
   } 
   storage->ptr += DE_STRLEN(storage->ptr) + 1;
} /* Cache_rRateList */

/*****************************************************************************
G L O B A L   C A C H E   F U N C T I O N S
*****************************************************************************/
void Cache_int(PersistentStorage_t* storage, Action_t action, int* object_p, char* name)
{
   CACHE_INTEGER_TYPE(storage, action, object_p, int, name);
} /* Cache_int */

void Cache_uint(PersistentStorage_t* storage, Action_t action, unsigned int* object_p, char* name)
{
   CACHE_INTEGER_TYPE(storage, action, object_p, unsigned int, name);
} /* Cache_int */

void Cache_rBool(PersistentStorage_t* storage, Action_t action, rBool* object_p, char* name)
{
   CACHE_INTEGER_TYPE(storage, action, object_p, rBool, name);
} /* Cache_rBool */

void Cache_rTimeout(PersistentStorage_t* storage, Action_t action, rTimeout* object_p, char* name)
{
   CACHE_INTEGER_TYPE(storage, action, object_p, rTimeout, name);
} /* Cache_rTimeout */

void Cache_rInfo(PersistentStorage_t* storage, Action_t action, rInfo* object_p, char* name)
{
   CACHE_INTEGER_TYPE(storage, action, object_p, rInfo, name);
} /* Cache_rInfo */
#define Cache_rSSID Cache_m80211_ie_ssid_t

#define Cache_rBSSID Cache_m80211_mac_addr_t

#define Cache_rChannelList Cache_channel_list_t

void Cache_rInterval(PersistentStorage_t* storage, Action_t action, rInterval* object_p, char* name)
{
   CACHE_INTEGER_TYPE(storage, action, object_p, rInterval, name);
} /* Cache_rInterval */

void Cache_rScanTimeouts(PersistentStorage_t* storage, Action_t action, rScanTimeouts* object_p, char* name)
{
   Cache_TreeStructure(storage, action, name);
   nrTabs++;
   Cache_rTimeout(storage, action, &object_p->minChannelTime, "minChannelTime");
   Cache_rTimeout(storage, action, &object_p->maxChannelTime, "maxChannelTime");
   nrTabs--;
} /* Cache_rScanTimeouts */

void Cache_rScanPolicy(PersistentStorage_t* storage, Action_t action, rScanPolicy* object_p, char* name)
{
   Cache_TreeStructure(storage, action, name);
   nrTabs++;
   Cache_rSSID(storage, action, &object_p->ssid, "ssid");
   Cache_rBSSID(storage, action, &object_p->bssid, "bssid");
   Cache_rBSS_Type(storage, action, &object_p->bssType, "bssType");
   nrTabs--;
} /* Cache_rScanPolicy */

void Cache_rConnectionPolicy(PersistentStorage_t* storage, Action_t action, rConnectionPolicy* object_p, char* name)
{
   Cache_TreeStructure(storage, action, name);
   nrTabs++;
   Cache_rBSS_Type(storage, action, &object_p->defaultBssType, "defaultBssType");
   Cache_rTimeout(storage, action, &object_p->joinTimeout, "joinTimeout");
   Cache_rTimeout(storage, action, &object_p->probeDelay, "probeDelay");
   Cache_int(storage, action, &object_p->probesPerChannel, "probesPerChannel");
   Cache_rScanTimeouts(storage, action, &object_p->passiveScanTimeouts, "passiveScanTimeouts");
   Cache_rScanTimeouts(storage, action, &object_p->activeScanTimeouts, "activeScanTimeouts");
   Cache_rScanTimeouts(storage, action, &object_p->connectedScanTimeouts, "connectedScanTimeouts");
   Cache_rTimeout(storage, action, &object_p->connectedScanPeriod, "connectedScanPeriod");
   Cache_rTimeout(storage, action, &object_p->max_connectedScanPeriod, "max_connectedScanPeriod");
   Cache_rTimeout(storage, action, &object_p->scanResultLifetime, "scanResultLifetime");
   Cache_rTimeout(storage, action, &object_p->disconnectedScanInterval, "disconnectedScanInterval");
   Cache_rTimeout(storage, action, &object_p->max_disconnectedScanInterval, "max_disconnectedScanInterval");
   Cache_int(storage, action, &object_p->periodicScanRepetition, "periodicScanRepetition");
   nrTabs--;
} /* Cache_rConnectionPolicy */

void Cache_rGeneralWiFiProperties(PersistentStorage_t* storage, Action_t action, rGeneralWiFiProperties* object_p, char* name)
{
   Cache_TreeStructure(storage, action, name);
   nrTabs++;
   Cache_rBSSID(storage, action, &object_p->macAddress, "macAddress");
   nrTabs--;
} /* Cache_rGeneralWiFiProperties */

void Cache_rSTA_QoSInfo(PersistentStorage_t* storage, Action_t action, rQoSInfoElements* object_p, char* name)
{
   Cache_TreeStructure(storage, action, name);
   nrTabs++;
   Cache_rInfo(storage, action, &object_p->ac_vo, "ac_vo");
   Cache_rInfo(storage, action, &object_p->ac_vi, "ac_vi");
   Cache_rInfo(storage, action, &object_p->ac_bk, "ac_bk");
   Cache_rInfo(storage, action, &object_p->ac_be, "ac_be");
   nrTabs--;
} /* Cache_rSTA_QoSInfo */

void Cache_rLinkSupervision(PersistentStorage_t* storage, Action_t action, rLinkSupervision* object_p, char* name)
{
   Cache_TreeStructure(storage, action, name);
   nrTabs++;
   Cache_rBool(storage, action, &object_p->enable, "enable");
   Cache_uint(storage, action, &object_p->beaconFailCount, "beaconFailCount");
   Cache_uint(storage, action, &object_p->beaconWarningCount, "beaconWarningCount");   
   Cache_uint(storage, action, &object_p->beaconTimeout, "beaconTimeout");
   Cache_uint(storage, action, &object_p->TxFailureCount, "TxFailureCount");
   Cache_uint(storage, action, &object_p->roundtripCount, "roundtripCount");
   Cache_uint(storage, action, &object_p->roundtripSilent, "roundtripSilent");
   nrTabs--;
} /* Cache_rLinkSupervision */

void Cache_rBasicWiFiProperties(PersistentStorage_t* storage, Action_t action, rBasicWiFiProperties* object_p, char* name)
{
   Cache_TreeStructure(storage, action, name);
   nrTabs++;
   Cache_int(storage, action, &object_p->enableCoredump, "enableCoredump");
   Cache_int(storage, action, &object_p->restartTargetAfterCoredump, "restartTargetAfterCoredump");
   Cache_rTimeout(storage, action, &object_p->timeout, "timeout");
   Cache_rConnectionPolicy(storage, action, &object_p->connectionPolicy, "connectionPolicy");
   Cache_rRateList(storage, action, &object_p->supportedRateSet, "supportedRateSet");
   Cache_uint(storage, action, &object_p->ht_capabilities, "ht_capabilities");
   Cache_uint(storage, action, &object_p->ht_rates, "ht_rates");
   Cache_rBool(storage, action, &object_p->multiDomainCapabilityEnabled, "multiDomainCapabilityEnabled");
   Cache_rBool(storage, action, &object_p->multiDomainCapabilityEnforced, "multiDomainCapabilityEnforced");
   Cache_rChannelList(storage, action, &object_p->regionalChannels, "regionalChannels");
   Cache_int(storage, action, &object_p->maxPower, "maxPower");
   Cache_rSSID(storage, action, &object_p->desiredSSID, "desiredSSID");
   Cache_rBSSID(storage, action, &object_p->desiredBSSID, "desiredBSSID");
   Cache_int(storage, action, &object_p->txRatePowerControl, "txRatePowerControl");
   Cache_rSTA_WMMSupport(storage, action, &object_p->enableWMM, "enableWMM");
   Cache_rBool(storage, action, &object_p->enableWMMPs, "enableWMMPs");
   Cache_int(storage, action, &object_p->wmmPsPeriod, "wmmPsPeriod");   
   Cache_rSTA_QoSInfo(storage, action, &object_p->QoSInfoElements, "QoSinfo");
   Cache_int(storage, action, &object_p->qosMaxServicePeriodLength, "qosMaxServicePeriodLength");
   Cache_rLinkSupervision(storage, action, &object_p->linkSupervision, "linkSupervision");
   Cache_int(storage, action, &object_p->activeScanMode, "activeScanMode");
   Cache_rBool(storage, action, &object_p->defaultScanJobDisposition, "defaultScanJobDisposition");
   Cache_int(storage, action, &object_p->scanNotificationPolicy, "scanNotificationPolicy");
   Cache_rScanPolicy(storage, action, &object_p->scanPolicy, "scanPolicy");
   Cache_rBool(storage, action, &object_p->DHCPBroadcastFilter, "DHCPBroadcastFilter");
   Cache_rBool(storage, action, &object_p->enableBTCoex, "enableBTCoex");
   Cache_rInterval(storage, action, &object_p->activityTimeout, "activityTimeout");
   Cache_rInterval(storage, action, &object_p->cmdTimeout, "cmdTimeout");
   nrTabs--;
} /* Cache_rBasicWiFiProperties */

void Cache_rIBSSBeaconProperties(PersistentStorage_t* storage, Action_t action, rIBSSBeaconProperties* object_p, char* name)
{
   Cache_TreeStructure(storage, action, name);
   nrTabs++;
   Cache_rInterval(storage, action, &object_p->dtim_period, "dtim_period");
   Cache_rInterval(storage, action, &object_p->beacon_period, "beacon_period");
   Cache_rChannelSet(storage, action, &object_p->tx_channel, "tx_channel");
   Cache_rATIMSet(storage, action, &object_p->atim_set, "atim_set");
   Cache_rRateList(storage, action, &object_p->supportedRateSet, "supportedRateSet");
   nrTabs--;
} /* Cache_rIBSSBeaconProperties */

void Cache_rNetworkProperties(PersistentStorage_t* storage, Action_t action, rNetworkProperties* object_p, char* name)
{
   Cache_TreeStructure(storage, action, name);
   nrTabs++;
   Cache_rBasicWiFiProperties(storage, action, &object_p->basic, "basic");
   Cache_rIBSSBeaconProperties(storage, action, &object_p->ibssBeacon, "ibssBeacon");
   nrTabs--;
} /* Cache_rNetworkProperties */

void Cache_rPowerManagementProperties(PersistentStorage_t* storage, Action_t action, rPowerManagementProperties* object_p, char* name)
{
   Cache_TreeStructure(storage, action, name);
   nrTabs++;
   Cache_rPowerSaveMode(storage, action, &object_p->mode, "mode");
   Cache_rBool(storage, action, &object_p->enablePsPoll, "enablePsPoll");   
   Cache_rBool(storage, action, &object_p->receiveAll_DTIM, "receiveAll_DTIM");
   Cache_rInterval(storage, action, &object_p->listenInterval, "listenInterval");
   Cache_rTimeout(storage, action, &object_p->psTrafficTimeout, "psTrafficTimeout");
   Cache_rTimeout(storage, action, &object_p->psTxTrafficTimeout, "psTxTrafficTimeout");
   Cache_rTimeout(storage, action, &object_p->psDelayStartOfPs, "psDelayStartOfPs");
   nrTabs--;
} /* Cache_rPowerManagementProperties */

void Cache_rHostDriverProperties(PersistentStorage_t* storage, Action_t action, rHostDriverProperties* object_p, char* name)
{
   Cache_TreeStructure(storage, action, name);
   nrTabs++;
   Cache_rTimeout(storage, action, &object_p->delayAfterReset, "delayAfterReset");
   Cache_rBool(storage, action, &object_p->automaticFWLoad, "automaticFWLoad");
   Cache_rBool(storage, action, &object_p->enable20MHzSdioClock, "enable20MHzSdioClock");
   Cache_int(storage, action, &object_p->txPktWinSize, "txPktWinSize");
   Cache_rBool(storage, action, &object_p->hmgAutoMode, "hmgAutoMode");
   nrTabs--;
} /* Cache_rHostDriverProperties */

void Cache_rRegistry(PersistentStorage_t* storage, Action_t action, rRegistry* object_p, char* name)
{
   Cache_TreeStructure(storage, action, name);
   nrTabs++;
   Cache_rVersionId(storage, action, &object_p->version, "version");
   Cache_rGeneralWiFiProperties(storage, action, &object_p->general, "general");
   Cache_rNetworkProperties(storage, action, &object_p->network, "network");
   Cache_rPowerManagementProperties(storage, action, &object_p->powerManagement, "powerManagement");
   Cache_rHostDriverProperties(storage, action, &object_p->hostDriver, "hostDriver");
   nrTabs--;
} /* Cache_rRegistry */


/*****************************************************************************
G L O B A L   A C C E S S   F U N C T I O N S
*****************************************************************************/
bool_t Registry_VerifyVersion(rVersionId version)
{
   return (Registry_VERSION_ID == version);
}


void* Registry_GetProperty(PropertyId_t id)
{
   switch (id)
   {
      case ID_version /*[rRegistry]*/:
         return (void*)&registry.version;

      case ID_general /*[rRegistry]*/:
         return (void*)&registry.general;

      case ID_network /*[rRegistry]*/:
         return (void*)&registry.network;

      case ID_basic /*[rNetworkProperties]*/:
         return (void*)&registry.network.basic;

      case ID_connectionPolicy /*[rBasicWiFiProperties]*/:
         return (void*)&registry.network.basic.connectionPolicy;

      case ID_passiveScanTimeouts /*[rConnectionPolicy]*/:
         return (void*)&registry.network.basic.connectionPolicy.passiveScanTimeouts;

      case ID_activeScanTimeouts /*[rConnectionPolicy]*/:
         return (void*)&registry.network.basic.connectionPolicy.activeScanTimeouts;

      case ID_connectedScanTimeouts /*[rConnectionPolicy]*/:
         return (void*)&registry.network.basic.connectionPolicy.connectedScanTimeouts;

      case ID_QoSInfoElements /*[rBasicWiFiProperties]*/:
         return (void*)&registry.network.basic.QoSInfoElements;

      case ID_linkSupervision /*[rBasicWiFiProperties]*/:
         return (void*)&registry.network.basic.linkSupervision;

      case ID_scanPolicy /*[rBasicWiFiProperties]*/:
         return (void*)&registry.network.basic.scanPolicy;

      case ID_ibssBeacon /*[rNetworkProperties]*/:
         return (void*)&registry.network.ibssBeacon;

      case ID_powerManagement /*[rRegistry]*/:
         return (void*)&registry.powerManagement;

      case ID_hostDriver /*[rRegistry]*/:
         return (void*)&registry.hostDriver;

   }

   DE_ASSERT(FALSE); // Unknown Property in call to Registry_GetProperty
   return NULL;
}



/******************************* END OF FILE ********************************/


