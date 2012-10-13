//******************************************************************************
//  MSP430F20xx Demo - WDT, Toggle P1.0, Interval Overflow ISR, 32kHz ACLK
//
//  Description: Toggle P1.0 using software timed by WDT ISR. Toggle rate is
//  exactly 250ms based on 32kHz ACLK WDT clock source.  In this example the
//  WDT is configured to divide 32768 watch-crystal(2^15) by 2^13 with an ISR
//  triggered @ 4Hz.
//  ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = default DCO
//  //* External watch crystal installed on XIN XOUT is required for ACLK *//	
//
//		  MSP430F20xx
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  M. Buccini / L. Westlund
//  Texas Instruments Inc.
//  September 2005
//  Built with CCE Version: 3.2.0 and IAR Embedded Workbench Version: 3.40A
//******************************************************************************

#include "msp430g2211.h"

//WDT-interval times [1ms] coded with Bits 0-2

//WDT is clocked by fSMCLK (assumed 1MHz) 
//#define WDT_MDLY_32         (WDTPW+WDTTMSEL+WDTCNTCL)                         /* 32ms interval (default) */
//#define WDT_MDLY_8          (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS0)                  /* 8ms     " */
//#define WDT_MDLY_0_5        (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS1)                  /* 0.5ms   " */
//#define WDT_MDLY_0_064      (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS1+WDTIS0)           /* 0.064ms " */
//WDT is clocked by fACLK (assumed 32KHz)
//#define WDT_ADLY_1000       (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL)                 /* 1000ms  " */
//#define WDT_ADLY_250        (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS0)          /* 250ms   " */
//#define WDT_ADLY_16         (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS1)          /* 16ms    " */
//#define WDT_ADLY_1_9        (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS1+WDTIS0)   /* 1.9ms   " */
unsigned short countIt = 0;
unsigned short mc_flag = 0;
unsigned short CA_LED = BIT5;
void WD_intervalTimerInit(unsigned char interval, unsigned short delay);

void WD_intervalTimerInit(unsigned char interval, unsigned short delay){
unsigned int currentDelay =0;
  	switch(delay)
	{
	case 1 :
	currentDelay = WDT_ADLY_1_9;
	//currentDelay = WDT_MDLY_0_064;
	break;
	case 2 :
	currentDelay = WDT_ADLY_16;
	//currentDelay = WDT_MDLY_0_5;
	break;
	case 3 :
	currentDelay = WDT_ADLY_250;
	//currentDelay = WDT_MDLY_8;
	break;
	case 4 :
	currentDelay = WDT_ADLY_1000;
	//currentDelay = WDT_MDLY_32;
	break;
	default :
	__no_operation();
	}
	
	unsigned char i = 0;
	WDTCTL = currentDelay;
	for(i=0;i<=interval;i++){
  	//WDTCTL = WDT_ADLY_250;                    // WDT 250ms, ACLK, interval timer
  	//WDTCTL = currentDelay;
  	//IE1 |= WDTIE;
	_low_power_mode_0();
	}
	WDTCTL = WDTPW + WDTHOLD;
}

	unsigned char foo = 0;
	unsigned short bar = 1;	
	
	//soft serial
void serial_setup(unsigned out_mask, unsigned in_mask, unsigned duration);
void putc(unsigned);
void puts(char *);
unsigned getc(void);

void main(void)
{
WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT	
	DCOCTL = 0;
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	serial_setup(BIT1, BIT2, 1000000 / 9600);
	//use vloclk
	//IFG1 &= ~OFIFG;	
	//WDTCTL = WDT_ADLY_16;                    // WDT 1000ms, ACLK, interval timer
	//BCSCTL3 |= LFXT1S_2; 
  	IE1 |= WDTIE;                             // Enable WDT interrupt
  	P1DIR |= BIT7 + BIT6;                            // Set P1.0 to output direction
	
	P1IE = BIT3 + BIT4; // P1.3 interrupt enabled
	P1IES |= BIT3 + BIT4; // P1.3 Hi/lo edge
	P1IFG &= ~BIT3 + BIT4; // P1.3 IFG cleared
 
	CCTL1 = CCIE;                             // CCR1 interrupt enabled
	CCR0 = 50000;
	TACTL = TASSEL_2 + MC_3 + ID_1;// + TAIE;                  // SMCLK, Contmode
	
	//CompA
	CACTL1 = CAREF1 + CARSEL + CAIE;	// 0.5 Vcc ref on - pin, enable
										// interrupts on rising edge.
	CACTL2 = P2CA4 + P2CA0 + CAF;		// Input CA2 on + pin, filter output.
//	CAPD = AIN1;				// disable digital I/O on P1.1 (technically
								// this  step is redundant) CAPD1

	CACTL1 |= CAON;				// turn on comparator
	
  _BIS_SR(GIE);
  while(1){
  	
  	WD_intervalTimerInit(foo,bar);
  	P1OUT |= BIT6;
  	WDTCTL = WDT_ADLY_16;
  	_BIS_SR(LPM1_bits);
  	P1OUT &= ~BIT6;
  	WDTCTL = WDTPW + WDTHOLD; 
  	if (mc_flag ==0)
  		{TACTL &= ~MC_3;
  		//CCR0 = 0;
  		puts("blink off");
  		putc(13);
  		putc(10);
  		}
  	if (mc_flag ==1)
  		{TACTL |= MC_3;
  		//CCR0 = 50000;
  		puts("blink on");
  		putc(13);
  		putc(10);
  		}

  }
	//P1OUT ^= BIT6;
  //_BIS_SR(LPM1_bits + GIE);                 // Enter LPM3 w/interrupt
}

// Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
	/*
	if(countIt ==15)
	{P1OUT ^= BIT6;
	countIt = 0;}
	
	if (countIt ==2)
	{P1OUT ^= BIT6;}          // Toggle P1.0 using exclusive-OR
	countIt++;
	*/
	
   	_low_power_mode_off_on_exit();
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{	
 //if((P1IFG & 0x20)==0x20)
 if((P1IFG & BIT3)==BIT3){
	P1IFG &= ~BIT3; // P1.3 IFG cleared
	if (foo >=5){
		foo = 0;
		if (bar >= 4){
			bar = 1;}
			else{bar++;}
		}
	else {foo++;}
//	_low_power_mode_off_on_exit();
 }
 
 //TODO add int for bit4
 if((P1IFG & BIT4)==BIT4){
	P1IFG &= ~BIT4; // P1.3 IFG cleared
	mc_flag ^=1;
	//_low_power_mode_off_on_exit();
 }
 
}

// Timer_A3 Interrupt Vector (TAIV) handler
#pragma vector=TIMERA1_VECTOR
__interrupt void Timer_A(void)
{
  switch( TAIV )
  {
  case  2: P1OUT ^= BIT7;
  break;                           // CCR1
  case  4: __no_operation(); 
  break;                           // CCR2 not used
  case 10: P1OUT ^= BIT7;
  break;                           // overflow not used
 }
}

#pragma vector = COMPARATORA_VECTOR
__interrupt void COMPA_ISR(void) {
	if ((CACTL2 & CAOUT)==0x01) {
		CACTL1 |= CAIES;		// value high, so watch for falling edge
		//flash = LED1;			// let LED flash
	}
	else {
		CACTL1 &= ~CAIES;		// value low, so watch for rising edge
		//flash = 0;				// turn LED off
		//P1OUT = 0;
	}		
} // COMPA_ISR

