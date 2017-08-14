
AT+CREG? // answer should be +CREG: 0,1, +CREG: 1,1, +CREG: 0,5, +CREG: 1,5
AT+CIPMUX=0 // OK
AT+CIPSTATUS // STATE: IP INITIAL
AT+CSTT="WAP.WARID","","" // OK
AT+CIPSTATUS // STATE: IP START
AT+CIICR // OK
AT+CIPSTATUS //STATE: IP GPRSACT
AT+CIFSR // Your ip
AT+CIPSTATUS // IP STATUS
AT+CIPSTART="UDP","182.178.188.143","9494" // AT+CIPSTART="UDP","haxan7.dyndns.info","9494"
AT+CIPSEND=22
Muhammad Hassan Nadeem
AT+CIPCLOSE



AT+CIPSTART="UDP","haxan7.dyndns.info","9494"


_______Initializations_____
AT+CIURC=0;
AT+IPR=9600;//Baudrate;
AT+CGDCONT=1,"IP","WAP.WARID"
AT+CIPCSGP=1,"WAP.WARID","",""
AT+SAPBR

AT&W //Save Settings



AT+SAPBR=3,1,"Contype","GPRS"
AT+SAPBR=3,1,"APN","WAP.WARID"
AT+SAPBR=3,1,"USER",""
AT+SAPBR=3,1,"PWD",""



____POST______________
http://www.dsscircuits.com/forum/index.php?topic=82.0

AT+SAPBR=3,1,"Contype","GPRS"
OK

AT+SAPBR=3,1,"APN","WAP.WARID"
OK

AT+SAPBR=1,1
OK// Tries to connect GPRS 

AT+HTTPINIT
OK

AT+HTTPPARA="CID",1
OK

AT+HTTPPARA="URL","haxan7.dyndns.info:8080/p/post.php" //web address to send data to
OK

AT+HTTPDATA=5,10000 //100 refers to how many bytes you're sending.  You'll probably have to tweak or just put a large #
DOWNLOAD

fn=ii //ie latitude,longitude,etc...
OK

AT+HTTPACTION=1 //POST the data 
ACTION: /// +HTTPACTION:1,200,117

AT+HTTPTERM //terminate http
OK

AT+SAPBR=0,1// Disconnect GPRS
OK
DEACT


____GET_______________
http://www.edaboard.com/thread281314.html
AT+SAPBR=5

AT+CGATT?
AT+SAPBR=3,1,"CONTYPE","GPRS"
AT+SAPBR=3,1,"APN","WAP.WARID"
// AT+SAPBR=3,1,"USER","ctigprs"
// AT+SAPBR=3,1,"PWD","ctigprs"

AT+SAPBR=1,1
AT+HTTPINIT
AT+HTTPPARA="CID",1
AT+HTTPPARA="URL","haxan7.dyndns.info:8080/p/get.php?fn=KONG"
AT+HTTPACTION=0 // +HTTPACTION:0,200,14
AT+HTTPTERM

AT+HTTPREAD

AT+HTTPPARA="URL","haxan7.dyndns.info:8080/p/get.php?fn=123"
AT+HTTPPARA="URL","haxan7.dyndns.info:8080/p/get.php?fn=456"
AT+HTTPPARA="URL","haxan7.dyndns.info:8080/p/get.php?fn=789"
AT+HTTPPARA="URL","haxan7.dyndns.info:8080/p/get.php?fn=101112"
________________________________________
AT+HTTPPARA="URL","haxan7.dyndns.info:8080/p/get.php?fn=0"
AT+HTTPPARA="URL","haxan7.dyndns.info:8080/p/get.php?fn=1"

http://haxan7.dyndns.info:8080/p/get.php?fn=99999


haxan7.dyndn80/p/get.php?fn=101112

http://m2msupport.net/m2msupport/module-tester/