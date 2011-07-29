#include "msp430g2211.h"
int toggler = 0x0;
volatile unsigned int i; 

void main(void)
{
WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
P1OUT &= ~0x41;
P1DIR |= 0x41; // Set P1.0 and P1.6 to output direction
P1IE |= 0x08; // P1.3 interrupt enabled
P1IES |= 0x08; // P1.3 Hi/lo edge
P1IFG &= ~0x08; // P1.3 IFG cleared


_BIS_SR(LPM4_bits + GIE); // Enter LPM4 w/interrupt
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{

toggler ^= 0x1;
switch(toggler)
	{
	case 0x1:
	P1OUT ^= 0x40; // P1.6 = toggle
	break;
	case 0x0:
	P1OUT ^= 0x01; // P1.0 = toggle    
    break;
    default:
    P1OUT |= 0x41;
	}

    i = 30000; // SW Delay
    do i--;
    while (i != 0);
    P1OUT &= ~0x41; //turn off the leds
P1IFG &= ~0x08; // P1.3 IFG cleared
}
