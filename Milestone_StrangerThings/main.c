#include <msp430.h>

void UARTSetup(void)
{
    P3SEL |= (BIT4|BIT3); //RX|TX peripheral mode
    UCA0CTL1 |= UCSWRST; // reset state machine
    UCA0CTL1 |= UCSSEL_2; // SMCLK
    UCA0BR0 = 6; // 9600 baud
    UCA0BR1 = 0;
    UCA0MCTL |= UCBRS_0|UCBRF_13|UCOS16; // modulation UCBRSx=0, UCBRFx=0
    UCA0CTL1 &= ~UCSWRST; // initialize state machine
    UCA0IE |= UCRXIE; // enable UART interrupt
}

void LEDSetup(void)
{
    P1DIR |= BIT0; // status LED
    P1OUT &= ~BIT0; // off
}

void PWMSetup(void)
{
    TA0CCR0 = 0xFF; // reset register
    TA0CCR1 = 0x00; // red
    TA0CCR2 = 0x00; // green
    TA0CCR3 = 0x00; // blue

    TA0CCTL1 = OUTMOD_7; // set/reset mode
    TA0CCTL2 = OUTMOD_7;
    TA0CCTL3 = OUTMOD_7;

    TA0CTL = MC_3 | TASSEL_2 | ID_1; // up down | SMCLK | 2^1 division
}

void RGBLEDSetup(void)
{
    P1DIR |= (BIT2|BIT3|BIT4); // P1.(2-4) RGB LED output pins
    P1SEL |= (BIT2|BIT3|BIT4); // P1.(2-4) = CCR(1-3)
}

void initSetup(void)
{
    UARTSetup();
    LEDSetup();
    PWMSetup();
    RGBLEDSetup();
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog
    initSetup();
    __bis_SR_register(LPM0_bits + GIE); // low power mode + enable global interrupt
}

volatile int byteCount = 0;
volatile int byteLn = 0;
#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0 (void)
{
    P1OUT |= BIT0; // status on
    // If there is data (i.e. not Null)
    if(__even_in_range(UCA0IV, USCI_UCTXIFG) == USCI_UCRXIFG) // UCA0IV == USCI_UCRXIFG
    {
        switch(byteCount)
        {
            case 0:
                while(!(UCA0IFG & UCTXIFG)); // wait while transmitting
                byteLn = UCA0TXBUF; // byte length
                UCA0TXBUF = UCA0RXBUF - 0x03; // transmit all but first 3 bytes
                break;
            case 1:
                TA0CCR1 = (UCA0RXBUF); // red
                break;
            case 2:
                TA0CCR2 = (UCA0RXBUF); // green
                break;
            case 3:
                TA0CCR3 = (UCA0RXBUF); // blue
                break;
            default:
                while(!(UCA0IFG & UCTXIFG));
                UCA0TXBUF = UCA0RXBUF; //Just transmit the incoming byte on to the next board
        }
        if(UCA0RXBUF == byteLn) // reset counter if last byte
        {
            byteCount = 1;
        }
        else
        {
            byteCount += 1;
        }
    }
    P1OUT &= ~BIT0; // status off
}
