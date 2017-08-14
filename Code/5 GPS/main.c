#include <main.h>
#include <string.h>
#INCLUDE <stdlib.h>

#define gpsTX PIN_A7
#define gpsRX PIN_A5

#define gsmPKey PIN_C0

/*
UART1 = RX=RC7, TX=RC6
UART2 = RX=RD7, TX=RD6
*/
/*1200 to 115200*/
#use rs232(baud=9600,parity=N,bits=8,uart1,stream = gsm,ERRORS)
#use rs232(baud=9600,parity=N,bits=8,uart2,stream = pc,ERRORS)
#use rs232(baud=4800,parity=N,xmit=gpsTX,rcv=gpsRX,bits=8,stream=gps,timeout=500,errors)

//Global Variables
char t[25];

//Function Prototypes
void getGPS(char*);
void initGPS();
short parseGPS(char*,char*);
unsigned int32 getInt(char*);
short delay(unsigned int16);
void syncGSM();
void turnOnGSM();
void initGPRS();
void initHTTP();
void turnOffGSM();
void gpsDegrees(float& x, char& dir);
void gpsTime(char* t);
void gpsDate(char* d);

struct GPS_DATA{
   float latitude;
   float longitude;
   char time[9];
   float speedF;
   // char speedC[];
   char date[11];
} gpsData;


#define buffer1Size 100
unsigned int buffer1Indx=0;
char buffer1[buffer1Size];
short recieveFlag1=0;
char tChar1=0;

void clearBuffer(char* buffer, unsigned int bufferSize){
   memset(buffer,NULL,bufferSize);
}

#INT_RDA
void gsmInt(VOID) {
   static short preRecieveFlag=0;
   tChar1=getc(gsm);
   //putc(tChar1,pc);
   buffer1[buffer1Indx]=tChar1;
   if(tChar1=='\n'/*||tChar1=='\r'*/){
      if(preRecieveFlag==1){
         recieveFlag1=1;
         preRecieveFlag=0;
      }else{
         preRecieveFlag=1;
      }
   }
   if(buffer1Indx++==buffer1Size)
      buffer1Indx=0;
}
char OK[3]="OK";
char ERROR[]="ERROR";

short checkCom(){
   buffer1Indx=0;
   recieveFlag1=0;
   fprintf(gsm,"AT\r");
   if(delay(1000))
      return strstr(buffer1, OK);
   else
      return 0;
}

short delay(unsigned int16 delayCount){
   while(recieveFlag1!=1 && delayCount>0){
      delayCount--;
      delay_ms(1);
   }
   return(delayCount!=0);//returns 0 if nothing received(delay==0)
}

void syncGSM(){
   for(int i=0;i<5;i++){
      putc('A',gsm);
      delay_ms(100);
   }
   putc('\r',gsm);
   fprintf(gsm,"ATE0\r");
   buffer1Indx=0;
}

void turnOnGSM(){
   if(checkCom()) return; //Do not Turn On If Already On
   output_low(gsmPKey);
   delay_ms(1500);//Keeping Pwr Key low for > 1s
   output_float(gsmPKey);//Its internally pulled up to 3V
}

void turnOffGSM(){
   output_low(gsmPKey);
   delay_ms(1500);//Keeping Pwr Key low for > 1s
   output_float(gsmPKey);//Its internally pulled up to 3V
   delay_ms(1500);//Waiting for Shutdown
}

void initGSM(){
   fprintf(gsm,"ATE0\r");
   delay_ms(100);
   recieveFlag1=0;
   buffer1Indx=0;
}

char tChar2;
#INT_RDA2
void pcInt(VOID) {
   tChar2=getc(pc);
}

// void sendATCommand(){
   // fprintf()
// }
void newATCommand(){
   buffer1Indx=0;
   recieveFlag1=0;
}

short confirmATResponse(char* expected, unsigned int16 timeOut=3000){
   while(timeOut>0){
      if(recieveFlag1){
         if(strstr(buffer1,expected)){
            delay_ms(200);//To be on the save side
            return 1;
         }else if(strstr(buffer1,ERROR)){
            return 0;
         }
      }
      timeOut--;
      delay_ms(1);
   }
   return 0;
}


void initHTTP(){
   fprintf(pc,"4\n\r");
   newATCommand();
   fprintf(gsm,"AT+HTTPINIT\r");
   if(!confirmATResponse(OK)) fprintf(pc,"False4\n\r");
   delay_ms(500);
   // delay_ms(10000);
   
   fprintf(pc,"5\n\r");
   newATCommand();
   fprintf(gsm,"AT+HTTPPARA=\"CID\",1\r");
   if(!confirmATResponse(OK)) fprintf(pc,"False5\n\r");
   delay_ms(500);
}


void initGPRS(){
   fprintf(pc,"1\n\r");
   newATCommand();
   fprintf(gsm,"AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r");
   if(!confirmATResponse(OK)) fprintf(pc,"False1\n\r");
   delay_ms(500);
   
   fprintf(pc,"2\n\r");
   newATCommand();
   fprintf(gsm,"AT+SAPBR=3,1,\"APN\",\"WAP.WARID\"\r");
   if(!confirmATResponse(OK)) fprintf(pc,"False2\n\r");\
   delay_ms(500);
   
   fprintf(pc,"3\n\r");
   newATCommand();
   fprintf(gsm,"AT+SAPBR=1,1\r");//can take a lot of time + wait after
   if(!confirmATResponse(OK)) fprintf(pc,"False3\n\r");
   delay_ms(1000);
}

// void connectToServer(){
   // newATCommand();
   // fprintf(gsm,"");
   // confirmATResponse(OK);
// }

char httpAction[]="+HTTPACTION:0,";
unsigned int16 httpErrorCode(){
   char* p = strstr(buffer1,httpAction);
   if(p==NULL) return 0;
   // fprintf(pc,"%s\n\r",p+14);
   // fprintf(pc,"%Lu\n\r",getInt(p+14));
   // while(1);
   return(getInt(p+14));
}

void httpOff(){
   newATCommand();
   fprintf(gsm,"AT+HTTPTERM\r");
   confirmATResponse(OK);
   
   newATCommand();
   fprintf(gsm,"AT+SAPBR=0,1\r");
   confirmATResponse(OK);
}

void main() {
   // output_float(gsmPKey);
   // enable_interrupts (INT_RDA); // Enables UART 1 Interrupts
   // enable_interrupts (INT_RDA2); //Enables UART 2 Interrupts
   // enable_interrupts(GLOBAL);
   fprintf(pc,"Starting uP!\n\r");
         output_high(PIN_A0);
      output_high(PIN_A1);
      delay_ms(1000);
      output_low(PIN_A0);
      output_low(PIN_A1);
      output_high(PIN_B0);
      output_high(PIN_B1);
      delay_ms(1000);
      output_low(PIN_B0);
      output_low(PIN_B1);
   
   char bbb[100]="0";
   char ooo[100];
   while(1){
      getGPS(bbb);
      fprintf(pc,"%s\n\r",bbb);
      parseGPS(bbb,ooo);
      fprintf(pc,"%s\n\r",ooo);
   }

}

void initGPS(){
   //RMC Sentence Only
   fprintf(gps,"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
   //Increase Frequency
   fprintf(gps,"$PMTK220,200*2C");
}

void getGPS(char* buffer){
   static char c;
   static unsigned int indx=0;
   static char gprmc[8]="$GPRMC";
   static short fail = 0;
   fail=0;
   do{
      if(fail){initGPS();}
      indx=0;
      do{
         c=getc(gps);
      }while(c!='$');
      while(c!='\r'){
         buffer[indx]=c;
         c=getc(gps);
         indx++;
      }
      buffer[indx]=0;
      fail=1;
   }while(!strstr(buffer, gprmc));
}

short parseGPS(char* raw, char* parsed){
   static char* p = 0;
   static short valid=0;
	char time[10];
	char date[20];
   static float longitude;
   static char NS;
   static float latitude;
   static char EW;
   static float speed;
   static float heading;
   
   
   //Time
   p = strchr(raw, ',')+1;
	strncpy(time,p,6);
   
   //Validity
   p = strchr(p, ',')+1;
   if (p[0] == 'A'){
      valid=1;
   }else{
      valid=0;
   }
   
   //Latitude
   p = strchr(p, ',')+1;
   latitude = atof(p);
   
   //North-South
   p = strchr(p, ',')+1;
   if (p[0] == 'N') NS = 'N';
   else if (p[0] == 'S') NS = 'S';
   
   //Longitude
   p = strchr(p, ',')+1;
   longitude = atof(p);
   
   //East-West
   p = strchr(p, ',')+1;
   if (p[0] == 'W') EW = 'W';
   else if (p[0] == 'E') EW = 'E';
   
   //Speed
   p = strchr(p, ',')+1;
   speed = atof(p);
   
   //Heading
   p = strchr(p, ',')+1;
   heading = atof(p);
   
   //Date
   p = strchr(p, ',')+1;
	strncpy(date,p,6);
   
   
   //validity test
   if(valid&&longitude!=0&&latitude!=0){
   
   }else{
   
   }
   
   gpsDegrees(latitude,NS);
   gpsDegrees(longitude,EW);
   gpsTime(time);
   gpsDate(date);
   
   strcpy(gpsData.time,time);
   // strcpy(gpsData.date,date);
   gpsData.longitude=longitude;
   gpsData.latitude=latitude;
   
   
   
   sprintf(parsed,"La=%6.4f&Lo=%6.4f&T=%s&D=%s",latitude,longitude,time,date);
	return 1;
}

void gpsDegrees(float& x, char& dir){
   unsigned int deg = (int)(x/100);
	// fprintf(pc,"Deg: %u\n\r",deg);
   float min=x-((long)deg*100);
   // fprintf(pc,"Min: %f\n\r",min);
	
   x=(min/60)+deg;
   if(dir=='S'||dir=='W') x=-x;
}

void gpsTime(char* t){
   t[8]=0;
   t[7]=t[5];
   t[6]=t[4];
   t[5]=':';
   t[4]=t[3];
   t[3]=t[2];
   t[2]=':';
}

void gpsDate(char* t){
	//ddmmyy to yyyy-mm-dd
   t[8]=t[0];
   t[9]=t[1];
   t[6]=t[3];
   t[3]=t[5];//
   t[5]=t[2];//
   t[2]=t[4];
	t[4]='-';
   t[7]='-';
   t[0]='2';
   t[1]='0';
   t[10]=0;
}

unsigned int32 getInt(char* p){
   float r = atof(p);
   return r;
}
