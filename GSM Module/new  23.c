AT+SAPBR=3,1,"CONTYPE","GPRS"
AT+SAPBR=3,1,"APN","WAP.WARID"
AT+SAPBR=1,1
AT+HTTPINIT
AT+HTTPPARA="CID",1

AT+HTTPPARA="URL","haxan7.dyndns.info:8080/p/get.php?fn=df"
AT+HTTPACTION=0

The above AT response code (601) for HTTP session start indicates that there is a network error. Then make sure that the PDP context is setup properly.

AT+SAPBR=5,1

AT+CIFSR //get ip

AT+HTTPTERM
AT+SAPBR=0,1



pdp context 157




AT+HTTPPARA="URL","haxan7.dyndns.info/p/get.php?fn=sdf:8080"

AT+CGDCONT?