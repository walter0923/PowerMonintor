#include "msp430.h"
void PORT_Ini(void);
void ADC_Ini(void);
void Timer_Ini(void);
void Proc(void);

unsigned char a = 0;
unsigned char b =1;
unsigned char cc = 1;
float gg;
float aa[4]= {0};
unsigned int systick = 0,secTick = 0;;

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD; 
  if (CALBC1_1MHZ==0xFF)					// If calibration constants erased
  {											
    while(1);                               // do not load, trap CPU!!	
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set range
  DCOCTL = CALDCO_1MHZ;                     // Set DCO step + modulation */

  PORT_Ini();
  ADC_Ini();
  Timer_Ini();
  __bis_SR_register(GIE);
  
  while(1)
  {
    switch(cc)
    {
    case 1:
      P2OUT &= ~BIT3;
      __delay_cycles(500000);
      P2OUT |= BIT3;
      __delay_cycles(500000);
      P2OUT &= ~BIT3;
      __delay_cycles(500000);
      P2OUT |= BIT3;
      __delay_cycles(500000);
      P2OUT &= ~BIT3;
      __delay_cycles(500000);
      P2OUT |= BIT3;
      __delay_cycles(500000);
      cc = 2;
      break;
      
    case 2:
      __bis_SR_register(LPM0_bits);
      cc = 3;
      break;
      
      case 3:
        Proc();
        cc = 2;
        break;
    }
  }
}

void ADC_Ini(void)
{
  P1SEL |= BIT0;
  ADC10CTL1 = INCH_0 + ADC10DIV_3;         // Temp Sensor ADC10CLK/4
  ADC10CTL0 = ADC10SHT_0 + SREF_0 + ADC10ON + ADC10IE;
  //ADC10CTL0 = ADC10SHT_3 + REF2_5V + REFON + ADC10ON + ADC10IE;
  __delay_cycles(1000);                    // Wait for ADC Ref to settle
}

void PORT_Ini(void)
{
  P2SEL = 0x00;
  P2DIR |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7;
  P2OUT |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7;
  //P2OUT &= ~(BIT4 | BIT5 | BIT6 | BIT7);
}

void Timer_Ini(void)
{
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  CCR0 = 1000;
  TACTL = TASSEL_2 + MC_1;                  // SMCLK, upmode
}

void Proc(void)
{
      P2OUT |= (BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
      P2OUT |= BIT0 | BIT1 | BIT2;
      
      for(int i = 0; i < 4; i ++)
      {
        __delay_cycles(2000);
        switch(a)
        {
          case 0:
            P2OUT |= BIT1 |BIT2;
            P2OUT &= ~(BIT0);
            break;
        
          case 1:
            P2OUT |= BIT0 | BIT2;
            P2OUT &= ~(BIT1);
            break;
      
          case 2:
            P2OUT |= BIT2;
            P2OUT &= ~(BIT1 | BIT0);
            break;
     
          case 3:
            P2OUT |= BIT0 | BIT1;
            P2OUT &= ~(BIT2);
            break;
      
          default:
            break;
        }
        __delay_cycles(5000); 
        ADC10CTL0 |= ENC + ADC10SC;
        while(ADC10CTL1 & ADC10BUSY){;}
        P2OUT |= BIT0 | BIT1 | BIT2;
      }
      
      P2OUT |= BIT0 | BIT1 | BIT2;
      __delay_cycles(1000);
      
      if(aa[0] > 3.65)
      {
        if((aa[0] > aa[1]) && (aa[0] > aa[2]) && (aa[0] > aa[3]))
        {
          P2OUT |= BIT4 | BIT5 | BIT6 | BIT7;
          P2OUT &= ~(BIT4);
        }
      }
      
      if(aa[1] > 3.65)
      {
        if((aa[1] > aa[0]) && (aa[1] > aa[2]) && (aa[1] > aa[3]))
        {
          P2OUT |= BIT4 | BIT5 | BIT6 | BIT7;
          P2OUT &= ~(BIT5);
        }
      }
      
      if(aa[2] > 3.65)
      {
        if((aa[2] > aa[0]) && (aa[2] > aa[1]) && (aa[2] > aa[3]))
        {
          P2OUT |= BIT4 | BIT5 | BIT6 | BIT7;
          P2OUT &= ~(BIT6);
        }
      }
      
      if(aa[3] > 3.65)
      {
        if((aa[3] > aa[0]) && (aa[3] > aa[1]) && (aa[3] > aa[2]))
        {
          P2OUT |= BIT4 | BIT5 | BIT6 | BIT7;
          P2OUT &= ~(BIT7);
        }
      }
      
      for(int j = 0; j < 4; j ++)
      {
        aa[j] = 0;
      }
}
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
  gg = (float)ADC10MEM / 1024;
  gg *= 3.3; // 3.3 * 4
  switch(a)
  {
      case 0:
        aa[0] = gg * 5;
        a = 1;
        break;
        
      case 1:
        aa[1] = gg * 5 - aa[0];    
        a = 2;
        break;
        
      case 2:
        aa[2] = gg * 5  - (aa[0] + aa[1]);
        a = 3;
        break;
        
      case 3:
        aa[3] = gg * 5 - (aa[0] + aa[1] + aa[2]);
        a = 0;
        break;
      
      default:
        break;
  }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  
  systick ++;
  if(systick >= 1000)
  {
    secTick ++;
    systick = 0;
  }
  
  if(secTick >= 5)
  {
    secTick = 0;
    //cc = 3;
    __bic_SR_register_on_exit(LPM0_bits);
  }
}
