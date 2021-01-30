#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>

unsigned long startup_microsecs;
char daysOfTheWeek[7][5] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
char monthOfYear[12][5]  =  {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

MCP_CAN CAN0(9);                               // Set CS to pin 4
RTC_DS1307 rtc;

void setup()
{
  SPI.setClockDivider(SPI_CLOCK_DIV2);//Use 8MHz for full speed data transfer for SD card and CAN BUS
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.begin();
  
  Serial.begin(115200);
  CAN0.begin(CAN_500KBPS,MCP_8MHz);                       // init can bus : baudrate = 500k
  
  CAN0.init_Mask(0,0,0x3FF);                // Init first mask...
  CAN0.init_Mask(1,0,0x3FF);                // Init second mask...
  CAN0.init_Filt(0,0,0x17c);                // Init first filter...RPM BRAKE - POWERTRAIN DATA
  CAN0.init_Filt(1,0,0x188);                // Init second filter...GEAR - GEARBOX

  CAN0.init_Filt(2,0,0x13a);                // Init third filter...
  CAN0.init_Filt(3,0,0x158);                // Init fourth filter...
  CAN0.init_Filt(4,0,0x1d0);                // Init fifth filter...
  CAN0.init_Filt(5,0,0x374);                // Init sixth filter...
  pinMode(2, INPUT);                            // Setting pin 2 for /INT input

  if (!SD.begin(10)) {
    return;
  }
  if (! rtc.begin()) {
    return;
  }
   if (! rtc.isrunning()) {
     rtc.adjust(DateTime(2018, 1, 21, 11, 30, 00));
   }
         File dataFile2 = SD.open("datalog.txt", FILE_WRITE);
      if (dataFile2) {
        DateTime now = rtc.now();
            String dateTime = "date ";
            dateTime += String(daysOfTheWeek[now.dayOfTheWeek()]);
            dateTime += " ";
            dateTime += String(monthOfYear[now.month()-1]);
            dateTime += " ";
            dateTime += String(now.day(), DEC);
            dateTime += " ";
            dateTime += String(now.hour(), DEC);
            dateTime += ":";
            dateTime += String(now.minute(), DEC);
            dateTime += ":";
            dateTime += String(now.second(), DEC);
            dateTime += " ";
            dateTime += String(now.year(), DEC);
        dataFile2.println(dateTime);
        dataFile2.println("base hex timestamps absolute");
        dataFile2.close();
      }
      startup_microsecs=micros();

      //attachInterrupt(0, MCP2515_ISR, FALLING);
}
void Parse(String &dataString, uint32_t time,unsigned char* rxBuf,long unsigned int rxId,unsigned char len,File* dataFile)
{
      if(len>8) len=8;
      if(len<1) return;
      
      unsigned long diff_time = (time-startup_microsecs);

      uint32_t seconds = diff_time / 1000000;
      uint32_t microseconds = diff_time % 1000000;
       dataString += seconds;
      dataString +=".";
      if (microseconds<100000)dataString +="0";
      if (microseconds<10000)dataString +="0";
      if (microseconds<1000)dataString +="0";
      if (microseconds<100)dataString +="0";
      if (microseconds<10)dataString +="0";
      dataString+=microseconds;

      dataString+="\t1\t";

      dataString+= String(rxId, HEX);
      dataString+="\tRx\td\t";
      dataString+=len;
      dataString+=" ";
      for(int i = 0; i<len; i++)                // Process each byte of the data
      {
        /*if(rxBuf[i] < 0x10)                     // If data byte is less than 0x10, add a leading zero
        {
          dataString += "0";
        }*/
        dataString+=String(rxBuf[i], HEX);
        dataString+= " ";
      }
      while(len<8)
      {
        dataString+=" 0";
        len++;
      }
        
      dataString+="\r\n";
}

void loop()
{
   long unsigned int rxId;
  unsigned char len;
  unsigned char rxBuf[8];
 
  long unsigned int rxId2;
  unsigned char len2;
  unsigned char rxBuf2[8];

  long unsigned int rxId3;
  unsigned char len3;
  unsigned char rxBuf3[8];
 
 /* long unsigned int rxId4;
  unsigned char len4;
  unsigned char rxBuf4[8];*/

    while(CAN_MSGAVAIL == CAN0.checkReceive())
    {
      unsigned long time=micros();
      uint32_t seconds = time / 1000000;
      unsigned long time2;
      unsigned long time3;
      //unsigned long time4;
      len=0;len2=0;len3=0;//len4=0;
    
      //each read to readMsgBufID takes 96 microseconds for full 8 bytes data
      CAN0.readMsgBufID(&rxId,&len, rxBuf);
      time2=micros();
      if(len>0)CAN0.readMsgBufID(&rxId2,&len2, rxBuf2);
      time3=micros();
      if(len2>0)CAN0.readMsgBufID(&rxId3,&len3, rxBuf3);
//      time4=micros();
      //if(len3>0)CAN0.readMsgBufID(&rxId4,&len4, rxBuf4);
      File dataFile = SD.open("datalog.txt", FILE_WRITE);
      if(dataFile)
      {
        String dataString = "";
        if(len>0)Parse(dataString,time,rxBuf,rxId,len,&dataFile);
        if(len2>0)Parse(dataString,time2,rxBuf2,rxId2,len2,&dataFile);
        //Serial.print(dataString);
        if(len3>0)
        {
          Parse(dataString,time3,rxBuf3,rxId3,len3,&dataFile);
          //if(len4>0)Parse(dataString,time4,rxBuf4,rxId4,len4,&dataFile);

        }
        dataFile.print(dataString);//90 ms minimum to write 50bytes of 4 batches data
        //Serial.print(dataString);

        if(seconds%2==0)digitalWrite(6, HIGH);
        else digitalWrite(6, LOW);
        
        dataFile.close();
      }
      
    }
}
