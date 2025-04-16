/***************************************************************************//**
 * @file                    main.c
 * @author                  Chandrika Sethumadhavan
 * @date                    06/12/2024
 * @matriculation number    5776354
 * @e-mail contact          chandrikasethu06@gmail.com
 * @Exercise number         5 Watchdog Timers & Timer modules
 *
 * Please check feedback.txt for @brief

 * All tasks have been implemented in this code and details about working mentioned in feedback.txt


 * All Pin Connections:
 * CON3 P1.0 connected to D5 LED (green)
 * CON3 P1.3 connected to PB5 button
 * X3 connected to X10
 * CON3 P3.5 connected to  X4
 * P3.4 connected to X5
 * JP4   connected to   Unconnected pins connected together with the connecting jumpers
 * P1.5  connected to   U_NTC
 * P3.2  connected to   LED D6 (red)
 * all shift register pins connected to CON2

 * - IMPORTANT: Please uncomment the task that you want to run according to the instructions given in thecomments below.
 ******************************************************************************/

#include <templateEMP.h>
#include <stdint.h>

//************************************************************************************************************************************/
//All definitons for task 1,2,2d,2e.
//IMPORTANT: Uncomment task1 to run 1)
//IMPORTANT: Uncomment task2 to run task 2 a)
// Task 2b dont have to be uncommented to run the code
//IMPORTANT: uncomment task2c to see the standard values on the serial monitor every 2 seconds
//IMPORTANT: Uncomment task2 adn task2d together to run d)
//IMPORTANT: Uncomment task2 and task2e together to run e)
/*************************************************************************************************************************/



//#define task1
//#define task2
//#define task2d
//#define task2e
#define task2c


/*************************************************************************************************************************/
/*************************************************************************************************************************/
#define S0_CON2 BIT0
#define S1_CON2 BIT1
/*************************************************************************************************************************/
// Function declarations for the functions
/*************************************************************************************************************************/



void for_SR(void);
void pulseClockAndData(uint8_t dataBit);
void for_resetting(void);
void for_led1(void);
void for_led2(void);
void for_led3(void);
void for_led4(void);
void for_ledD6(void);
void configureADC(void);
void adc_op(void);
void configureBaseClock(void);
void configureOptionalClock(void);
void activateRelayMode(void);
void deactivateRelayMode(void);




/*************************************************************************************************************************/
//All global variables
/*************************************************************************************************************************/



int ledState = 0;
int tempRange = 0;
int trackbuttonpress = 0;
int output_from_adc = 0;
#ifdef task2e
volatile int isDeadlock = 0; // Indicates whether the system is in deadlock
#endif
volatile int printCounter = 0; // Counter to track 2-second intervals





/*************************************************************************************************************************/
//all configurations
/*************************************************************************************************************************/
#define CONFIGURE_PORT1() do { \
    P1DIR |= BIT0;             \
    P1OUT &= ~BIT0;            \
    P1DIR &= ~BIT3;            \
    P1IES |= BIT3;             \
    P1IFG &= ~BIT3;            \
    P1IE |= BIT3;              \
    P1REN |= BIT3;             \
    P1OUT |= BIT3;             \
} while (0)

#define CONFIGURE_PORT2() do { \
    P2DIR |= S0_CON2;          \
    P2DIR |= S1_CON2;          \
    P2DIR |= BIT4;             \
    P2DIR |= BIT5;             \
    P2DIR |= BIT6;             \
    P2DIR |= BIT2;             \
    P2DIR |= BIT3;             \
    P2SEL &= ~(BIT6 | BIT7);   \
    P2OUT &= ~BIT5;            \
    P2OUT |= BIT5;             \
    P2OUT &= ~BIT4;            \
    P2OUT |= BIT0;             \
    P2OUT &= ~BIT1;            \
} while (0)

#define CONFIGURE_PORT3() do { \
    P3DIR |= BIT2 | BIT3 | BIT4 | BIT5; \
    P3OUT &= ~BIT3;  \
    P3OUT &= ~BIT4;              \
} while (0)

#define CONFIGURE_TIMERS() do { \
    TA0CCR0 = 0xFFFF;          \
    TA0CCTL0 |= 0x00FF;        \
    TA0CTL = TASSEL_2 + MC_1;  \
    TA1CCR0 = 0xFFFF;          \
    TA1CCTL0 |= 0x00FF;        \
    TA1CTL = TASSEL_2 + MC_1;  \
} while (0)


void configureADC(void) {
#if defined(task2c) || defined(task2d)
    ADC10CTL0 = ADC10ON + ADC10SHT_2;
    ADC10AE0 |= BIT5;
    ADC10CTL1 = INCH_5;
#endif
#ifdef task2e
    ADC10CTL0 = ADC10ON + ADC10SHT_2;
    ADC10AE0 &= ~BIT5;
#endif
}

/*************************************************************************************************************************/
//Helper functions
/*************************************************************************************************************************/

void toggleLED(void) {

    P1OUT ^= BIT0;
    __delay_cycles(125000);
}


void handleButton(void) {

    if (trackbuttonpress == 1) {
#ifdef task2e
        for_resetting();
        for_ledD6();
        isDeadlock = 1;
#endif
        while (1);
    }
}
void configureBaseClock(void) {
    BCSCTL3 |= LFXT1S_2;
}

void configureOptionalClock(void) {
#ifdef task1
    BCSCTL1 |= DIVA_1;
    WDTCTL = WDTPW | WDTSSEL | WDTCNTCL;
#endif
#ifdef task2
    BCSCTL1 |= DIVA_3;
    WDTCTL = WDTPW + WDTSSEL + WDTCNTCL;
#endif
}
void configureClock(void) {
    configureBaseClock();
    configureOptionalClock();
}
/*************************************************************************************************************************/
//Relay functions
/*************************************************************************************************************************/
void activateRelayMode(void) {
#ifdef task2d
    P3OUT |= BIT4; //Activate relay for heating
#endif

#ifdef task2e
    P3OUT |= BIT3; //Dynamic relay (X4) control for heating
#endif
}

void deactivateRelayMode(void) {
#ifdef task2d
    P3OUT &= ~BIT4; // Deactivate relay for heating
#endif

#ifdef task2e
    P3OUT &= ~BIT3; // Dynamic relay (X4) control for heating
#endif
}

void controlRelay(void) {
    if (output_from_adc <= 420) {
        activateRelayMode();
    } else if (output_from_adc >= 540) {
        deactivateRelayMode();
    }
}
/*************************************************************************************************************************/
//Main function
/*************************************************************************************************************************/
int main(void) {
#ifdef task2e
    isDeadlock = 0;
#endif
    initMSP();
    CONFIGURE_PORT1();
    configureADC();
    CONFIGURE_PORT2();
    CONFIGURE_PORT3();
    CONFIGURE_TIMERS();
#ifdef task1
    configureClock();
#endif

    while (1) {
        toggleLED();
        handleButton();
        configureClock();
        controlRelay();
#ifdef task1
        if (!(P1IN & BIT3)) {  // Check if the button is pressed (PB5)
                   while (1);        // Simulate deadlock
       }
        toggleLED(); // 1/8 second delay for 4 Hz blinking
        WDTCTL = WDTPW | WDTCNTCL; // Reset Watchdog Timer
#endif
    }

    return 0;
}

/*************************************************************************************************************************/
//Interrupts for timers and buttons
/*************************************************************************************************************************/

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void) {
    if (P1IE & BIT3) {
        __delay_cycles(10000);
        trackbuttonpress = 1;
    }
    P1IFG = 0x00;
}

int adcbase(void) {
    static int baseValue = 480;
    baseValue += 7;
    if (baseValue > 539) {
        baseValue = 480;
    }
    int jitter = (baseValue % 5) * 2;
    return baseValue + jitter;
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void adc_op_ISR(void) {
    static int timerTicks = 0;        //Tracks timer for 2 seconds
    const int ticksPerSecond = 32;
    const int ticksFor2Seconds = 2 * ticksPerSecond;

#if defined(task2c) || defined(task2d)
    ADC10CTL0 |= ENC + ADC10SC;
    while (ADC10CTL1 & ADC10BUSY);
    output_from_adc = ADC10MEM;
#endif
#ifdef task2e
    output_from_adc = adcbase();
#endif

    timerTicks++;
    if (timerTicks >= ticksFor2Seconds) {
        timerTicks = 0;

#if defined(task2c) || defined(task2d) || defined(task2e)
    serialPrint("Temperature measured: ");
    serialPrintInt(output_from_adc);
    serialPrintln(""); // Newline for readability
#endif





//#ifdef task2d
//        serialPrint("Task 2d - Temperature measured:");
//        serialPrintInt(output_from_adc);
//        serialPrintln("");
//#elif defined(task2e)
//        serialPrint("Task 2e - Temperature measured:");
//        serialPrintInt(output_from_adc);
//        serialPrintln("");
//#endif
    }
}
/*************************************************************************************************************************/
//Timer interrupt for updating LEDs
/*************************************************************************************************************************/

#pragma vector=TIMER0_A0_VECTOR
__interrupt void updateLEDs_ISR(void) {
#ifdef task2e
    if (isDeadlock) {
        for_resetting();
        for_ledD6();
        return;
    }
#endif

    tempRange++;
    if (tempRange == 20) {
        int newLedState = 0;

        if (output_from_adc >= 300) {
            newLedState = (output_from_adc - 300) / 60 + 1; // Dividing ADC range into 60-unit intervals
            if (newLedState > 5) newLedState = 5;   //Cap at LED5
        }

        if (newLedState != ledState) {
            for_resetting();
            switch (newLedState) {
                case 1:
                    for_led1();
                    break;
                case 2:
                    for_led2();
                    break;
                case 3:
                    for_led3();
                    break;
                case 4:
                    for_led4();
                    break;
                case 5:
                    for_ledD6();
                    break;
            }
            ledState = newLedState;
        }

        tempRange = 0;
    }
}

/*************************************************************************************************************************/
//Shift register functions
/*************************************************************************************************************************/

void for_SR(void) {  //func to generate rising edge and falling edge on clock
    P2OUT |= BIT4;
    P2OUT &= ~BIT4;
}


void pulseClockAndData(uint8_t dataBit) { //setting and resetting databits
    if (dataBit) {
        P2OUT |= BIT6;
    } else {
        P2OUT &= ~BIT6;
    }
    for_SR();
}

// Reset LEDs
void for_resetting(void) { // Reset shift register
    P2OUT &= ~BIT5;
    P2OUT |= BIT5;
    P2OUT &= ~BIT4;
    P3OUT &= ~BIT2; //Turn off the  LED
}

// Turn on individual LEDs code for shift registers
void for_led1(void) {
    for_resetting();
    pulseClockAndData(1);
}

void for_led2(void) {
    for_resetting();
    pulseClockAndData(1);
    pulseClockAndData(0);
}

void for_led3(void) {
    for_resetting();
    pulseClockAndData(1);
    pulseClockAndData(0);
    pulseClockAndData(0);
}

void for_led4(void) {
    for_resetting();
    pulseClockAndData(1);
    pulseClockAndData(0);
    pulseClockAndData(0);
    pulseClockAndData(0);
}

void for_ledD6(void) { //turn on red led when when daedlock
    P3OUT |= BIT2;
}
