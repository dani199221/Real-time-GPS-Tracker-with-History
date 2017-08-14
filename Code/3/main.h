#include <18F45K22.h>
#device ADC=16

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES WDT128                   //Watch Dog Timer uses 1:128 Postscale
#FUSES INTRC                    //Internal RC Osc
#FUSES NOBROWNOUT               //No brownout reset
#FUSES WDT_SW                   //No Watch Dog Timer, enabled in Software
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOXINST                  //Extended set extension and Indexed Addressing mode disabled (Legacy mode)

#use delay(internal=16MHz)

