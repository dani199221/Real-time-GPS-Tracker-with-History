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

//Function Prototypes
void getGPS(char*);
void initGPS();
void parseGPS(char*,char*);
unsigned int32 getInt(char*);
short delay(unsigned int16);
void syncGSM();
void turnOnGSM();
void turnOffGSM();

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

void main() {
   output_float(gsmPKey);
   enable_interrupts (INT_RDA); // Enables UART 1 Interrupts
   enable_interrupts (INT_RDA2); //Enables UART 2 Interrupts
   enable_interrupts(GLOBAL);
   
   clearBuffer(buffer1, buffer1Size);
   fprintf(pc,"_________________\n\n\rStarting Up!\n\r");
   while(TRUE){
   for(int i=0;i<25;i++){
      turnOnGSM();
      fprintf(pc,"Powering Up\n\r");
      delay_ms(6000);//Wait for the device to Power Up;
      initGSM();
      if(checkCom()){
         fprintf(pc,"Coms OK\n\r");
      }else{
         fprintf(pc,"ERR\n\r");
      }
      delay_ms(5000);
      fprintf(pc,"Turning Off\n\r");
      turnOffGSM();
      delay_ms(5000);
   }
      while(1);
   }

}

void initGPS(){
   fprintf(gps,"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
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

void parseGPS(char* raw, char* parsed){
   static char* p = 0;
   static unsigned int32 time;
   static unsigned int32 date;
   static float longitude;
   static char NS;
   static float latitude;
   static char EW;
   static float speed;
   static float heading;
   
   
   //Time
   p = strchr(raw, ',')+1;
   time = getInt(p);
   
   //Validity
   p = strchr(p, ',')+1;
   if (p[0] != 'A') return;
   
   //Latitude
   p = strchr(p, ',')+1;
   latitude = atof(p);
   
   //North-South
   p = strchr(p, ',')+1;
   if (p[0] == 'N') NS = 'N';
   else if (p[0] == 'S') NS = 'S';
   else return;
   
   //Longitude
   p = strchr(p, ',')+1;
   longitude = atof(p);
   
   //East-West
   p = strchr(p, ',')+1;
   if (p[0] == 'W') EW = 'W';
   else if (p[0] == 'E') EW = 'E';
   else return;
   
   //Speed
   p = strchr(p, ',')+1;
   speed = atof(p);
   
   //Heading
   p = strchr(p, ',')+1;
   heading = atof(p);
   
   //Date
   p = strchr(p, ',')+1;
   date = getInt(p);
   
   
   sprintf(parsed,"GPS:%Lu,%f,%c,%f,%c,%f,%f",time,latitude,NS,longitude,EW,heading,speed);
}

unsigned int32 getInt(char* p){
   float r = atof(p);
   return r;
}
