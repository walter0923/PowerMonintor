#include "msp430.h"

#define over_ptr 0x1080
#define lower_ptr 0x1000

void PORT_Ini(void);
void ADC_Ini(void);
void Timer_Ini(void);
void Flash_Ini(void);
void Proc(void);
void Over_4V(void);
void Lower_8V(void);
void Flash_Erase(void);
int Flash_read(int *ptr);

unsigned char a = 0;
unsigned char b =1;
unsigned char cc = 1;
float gg;
float aa[4]= {0};
unsigned int systick = 0,secTick = 0;;

void main(void)
{
  int* over = 0;
  int* lower = 0;
  WDTCTL = WDTPW + WDTHOLD; 
  
  PORT_Ini();
  ADC_Ini();
  Timer_Ini();
  Flash_Ini();
  __bis_SR_register(GIE);
  
  while(1)
  {
    switch(cc)
    {
    case 1:
      //Flash_Erase();
      if(!(P3IN & BIT4))
      {
        Flash_Erase();
      }
      //Lower_8V();
      //Over_4V();
      
      cc = 2;
      break;
      
    case 2:
      over = (int *)over_ptr;
      if(*over == 0xFFAA)
      {
        cc = 4; //jump to over step
      }
      
      else if(*over == 0xFFFF)
      {
        cc = 3; //go to read lower ercord
      }
      break;
      
    case 3:
      lower = (int *)lower_ptr;
      if(*lower == 0xFF11)
      {
        cc = 5; //jump to lower step
      }
      
      else if((*lower == 0xFFFF) && ((*over == 0xFFFF)))
      {
        cc = 6; //go to normal step
      }
      break;
      
    case 4: //over step
      __delay_cycles(600000);
      P2OUT |= BIT4;
      __delay_cycles(50000);
      P2OUT &= ~BIT4;
      __delay_cycles(50000);
      P2OUT |= BIT4;
      __delay_cycles(50000);
      P2OUT &= ~BIT4;
      __delay_cycles(50000);
      P2OUT |= BIT4;
      __delay_cycles(50000);
      P2OUT &= ~BIT4;
      __delay_cycles(50000);
      P2OUT |= BIT4;
      __delay_cycles(50000);
      P2OUT &= ~BIT4;
      __delay_cycles(50000);
      P2OUT |= BIT4;
      __delay_cycles(50000);
      P2OUT &= ~BIT4;
      __delay_cycles(50000);
      P2OUT |= BIT4;
      __delay_cycles(50000);
      P2OUT &= ~BIT4;
      __delay_cycles(50000);
      P2OUT |= BIT4;
      __delay_cycles(50000);
      P2OUT &= ~BIT4;
      __delay_cycles(50000);
      P2OUT |= BIT4;
      __delay_cycles(50000);
      P2OUT &= ~BIT4;
      __delay_cycles(50000);
      P2OUT |= BIT4;
      __delay_cycles(50000);
      P2OUT &= ~BIT4;
      __delay_cycles(50000);
      P2OUT |= BIT4;
      __delay_cycles(50000);
      P2OUT &= ~BIT4;
      __delay_cycles(50000);
      P2OUT |= BIT4;
      __delay_cycles(50000);
      P2OUT &= ~BIT4;
      __delay_cycles(50000);
      P2OUT |= BIT4;
      __delay_cycles(50000);
      P2OUT &= ~BIT4;
      __delay_cycles(600000);
      cc = 3;
      break;
      
    case 5:     //lower step
      P2OUT |= BIT4;
      __delay_cycles(800000);
      P2OUT &= ~BIT4;
      __delay_cycles(100000);
      P2OUT |= BIT4;
      __delay_cycles(800000);
      P2OUT &= ~BIT4;
      __delay_cycles(100000);
      P2OUT |= BIT4;
      __delay_cycles(800000);
      P2OUT &= ~BIT4;
      __delay_cycles(600000);
      cc = 7;
      break;
      
    case 6:     //normal step
      __delay_cycles(100000);
      P2OUT |= BIT4;
      __delay_cycles(100000);
      P2OUT &= ~BIT4;
      __delay_cycles(100000);
      P2OUT |= BIT4;
      __delay_cycles(100000);
      P2OUT &= ~BIT4;
      __delay_cycles(100000);
      P2OUT |= BIT4;
      __delay_cycles(100000);
      P2OUT &= ~BIT4;
      __delay_cycles(100000);
      cc = 7;
      break;
      
    case 7:
      __bis_SR_register(LPM0_bits);
      cc = 8;
      break;
      
      case 8:
        Proc();
        cc = 7;
        break;
    }
  }
}

void ADC_Ini(void)
{
  P2SEL |= BIT0;
  ADC10CTL1 = INCH_0 + ADC10DIV_3;         // Temp Sensor ADC10CLK/4
  ADC10CTL0 = ADC10SHT_0 + SREF_0 + ADC10ON + ADC10IE;
  //ADC10CTL0 = ADC10SHT_3 + REF2_5V + REFON + ADC10ON + ADC10IE;
  __delay_cycles(1000);                    // Wait for ADC Ref to settle
}

void PORT_Ini(void)
{
  P1DIR |= BIT0 | BIT1 | BIT2 | BIT3;
  P2DIR |= BIT2 | BIT4; 
  P3DIR |= BIT0 | BIT1;
  P3DIR &= ~BIT4;
  
  P1OUT = BIT0 | BIT1 | BIT2 | BIT3; 
  P2OUT |= BIT2; 
  P2OUT &= ~BIT4;
  P3OUT |= BIT0 | BIT1;
  
}

void Timer_Ini(void)
{
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  CCR0 = 800;
  TACTL = TASSEL_2 + MC_1;                  // SMCLK, upmode
}

void Flash_Ini(void)
{
  FCTL2 = FWKEY + FSSEL0 + FN0;             // MCLK/2 for Flash Timing Generator
}

void Over_4V(void)
{
  char *Flash_ptr;                          // Flash pointer

  Flash_ptr = (char *) 0x1080;              // Initialize Flash pointer
  _DINT();
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  FCTL3 = FWKEY;                            // Clear Lock bit
  while(FCTL3 & BUSY);
  *Flash_ptr = 0;                           // Dummy write to erase Flash segment

  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  *Flash_ptr = 0xAA;                   // Write value to flash

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
  _EINT();
}

void Lower_8V(void)
{
  char *Flash_ptr;                          // Flash pointer

  Flash_ptr = (char *) 0x1000;              // Initialize Flash pointer
  _DINT();
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  FCTL3 = FWKEY;                            // Clear Lock bit
  while(FCTL3 & BUSY);
  *Flash_ptr = 0;                           // Dummy write to erase Flash segment

  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  *Flash_ptr = 0x11;                   // Write value to flash

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
  _EINT();
}

void Flash_Erase(void)
{
  char *Flash_ptrA;                         // Segment A pointer
  char *Flash_ptrB;                         // Segment B pointer

  Flash_ptrA = (char *) 0x1080;             // Initialize Flash segment A pointer
  Flash_ptrB = (char *) 0x1000;             // Initialize Flash segment B pointer
  _DINT();
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  FCTL3 = FWKEY;                            // Clear Lock bit
  while(FCTL3 & BUSY);
  *Flash_ptrA = 0;                          // Dummy write to erase Flash segment B
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  FCTL3 = FWKEY;  
  while(FCTL3 & BUSY);
  *Flash_ptrB = 0;                          // Dummy write to erase Flash segment B
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
  _EINT();
}

int Flash_read(int *ptr)
{
  return *ptr;
}

void Proc(void)
{
      P1OUT |= (BIT0 | BIT1 | BIT2 | BIT3);
      P2OUT |= BIT2 | BIT4;
      P3OUT |= BIT0 | BIT1;
      
      for(int i = 0; i < 4; i ++)
      {
        __delay_cycles(2000);
        switch(a)
        {
          case 0:
            P3OUT |= BIT0 |BIT1;
            P2OUT &= ~(BIT2);
            break;
        
          case 1:
            P2OUT |= BIT2;
            P3OUT |= BIT1;
            P3OUT &= ~(BIT0);
            break;
      
          case 2:
            P3OUT |= BIT1;
            P3OUT &= ~BIT0;
            P2OUT &= ~BIT2;
            break;
     
          case 3:
            P2OUT |= BIT2;
            P3OUT |= BIT0;
            P3OUT &= ~(BIT1);
            break;
      
          default:
            break;
        }
        __delay_cycles(5000); 
        ADC10CTL0 |= ENC + ADC10SC;
        while(ADC10CTL1 & ADC10BUSY){;}
        P2OUT |= BIT2;
        P3OUT |= BIT0 | BIT1;
      }
      
      P2OUT |= BIT2;
      P3OUT |= BIT0 | BIT1;
      __delay_cycles(1000);
      
      if(aa[0] > 3.65)
      {
        if((aa[0] > aa[1]) && (aa[0] > aa[2]) && (aa[0] > aa[3]))
        {
          P1OUT |= BIT0 | BIT1 | BIT2 | BIT3;
          P1OUT &= ~(BIT0);
        }
        if(aa[0] > 4.00)
        {
          Over_4V();
        }
      }
      
      if(aa[1] > 3.65)
      {
        if((aa[1] > aa[0]) && (aa[1] > aa[2]) && (aa[1] > aa[3]))
        {
          P1OUT |= BIT0 | BIT1 | BIT2 | BIT3;
          P1OUT &= ~(BIT1);
        }
        if(aa[1] > 4.00)
        {
          Over_4V();
        }
      }
      
      if(aa[2] > 3.65)
      {
        if((aa[2] > aa[0]) && (aa[2] > aa[1]) && (aa[2] > aa[3]))
        {
          P1OUT |= BIT0 | BIT1 | BIT2 | BIT3;
          P1OUT &= ~(BIT2);
        }
        if(aa[2] > 4.00)
        {
          Over_4V();
        }
      }
      
      if(aa[3] > 3.65)
      {
        if((aa[3] > aa[0]) && (aa[3] > aa[1]) && (aa[3] > aa[2]))
        {
          P1OUT |= BIT0 | BIT1 | BIT2 | BIT3;
          P1OUT &= ~(BIT3);
        }
        if(aa[3] > 4.00)
        {
          Over_4V();
        }
      }
      
      if((aa[0] + aa[1] + aa[2] + aa[3]) < 9)
      {
        Lower_8V();
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
