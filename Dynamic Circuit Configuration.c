/******************************************************************************************************
 * @file                    main.c
 * @author                  Chandrika Sethumadhavan
 * @Exercise number         4 Dynamic Circuit Configuration
 *


 * All Pin Connections:
    * - JP5 is set to VF0
    * - Left (links - CON4)  pin of relay - P3.6
    * - Right (rechts - CON4) pin of relay - DAC_IN (X2)
    * - COMP_OUT - P1.0
    * - Mittel pin of relay - PIEZO BUZZER (X1)
    * - REL_STAT (X5) - P1.7

 * - The program is designed to play two different melodies based on the number of knocks detected.
 * - IMPORTANT : Please make the second knock within half a second of making the first knock to play melody 2. else the program will play melody 1  (The knocks should be done in an instant (continuously).
 *******************************************************************************************************/

#include <templateEMP.h>
#include <msp430.h>


#define melody1_size 20
#define melody2_size 40
#define NOTE_LENGTH_SCALE 2
#define STATE_WAITING_FOR_KNOCK 0
#define STATE_DETECTING_KNOCKS  1
#define STATE_DEBOUNCING        2


volatile int currentState = STATE_WAITING_FOR_KNOCK;
volatile unsigned int k_count = 0;
volatile int flagmonitor = 0;
int pause = 0;
int i=0;


//Mary Had a Lamb
int maryHadALamb[melody1_size] = {
    330, 294, 262, 294, 330, 330, 330,
    294, 294, 294,
    330, 392, 392,
    330, 294, 262, 294, 330, 330, 330
};

//Twinkle Twinkle
int twinkleStar[melody2_size] = {
    262, 262, 392, 392, 440, 440, 392, 392,
    349, 349, 330, 330, 294, 294, 262, 262,
    392, 392, 349, 349, 330, 330, 294, 294,
    392, 392, 349, 349, 330, 330, 294, 294,
    262, 262, 392, 392, 440, 440, 392, 392
};

//delay function
void WDT(unsigned int milliseconds) {
    WDTCTL = WDT_MDLY_0_5; // Setting WDT to interval mode (0.5ms)
    unsigned int counter = milliseconds * 2;

    while (counter > 0) {
        if (IFG1 & WDTIFG) { //WDT interrupt flag
            IFG1 &= ~WDTIFG; //Clearing the interrupt flag
            counter--;       // Decrementing the counter
        }
    }

    WDTCTL = WDTPW | WDTHOLD;
}



//functions for setting up and handling buzzers, interrupts, and playback

void pwm() {
    P3SEL |= BIT6;
    P3DIR |= BIT6;
    TA0CCTL2 = OUTMOD_3;
    TA0CTL = TASSEL_2 + MC_1;
    P3REN &= ~BIT6;
}

void function_for_interrupts() {
    P1DIR &= BIT0;
    P1OUT |= BIT0;
    P1IE |= BIT0;
    P1IES |= BIT0;
    P1IFG &= BIT0;
    __bis_SR_register(GIE);
}

void function_for_relstat() {
    P1DIR |= BIT7;
    P1OUT &= ~BIT7;
}

void combined_func_for_tune_play(int k_count) {
    const int* store_m_data;
    unsigned int size_of_m;

    //Select melodies based on k_count value
    if (k_count == 1) {
        store_m_data = maryHadALamb;
        size_of_m = melody1_size;
    } else if ((k_count != 1) && (k_count != 0)) {
        store_m_data = twinkleStar;
        size_of_m = melody2_size;
    } else {
        return;
    }

    P1OUT |= BIT7; // Enable the relay
    pwm();

    //function to play the melody corresponding to the knock count
    for (i = 0; i < size_of_m; i += 2) {
        if (!pause) {
            int frequency = store_m_data[i];
            int duration = store_m_data[i + 1] * NOTE_LENGTH_SCALE;

            // Calculate frequency
            int timePerCycle = (frequency > 0) ? (1000000 / frequency) : 0;
            TA0CCR0 = (timePerCycle > 0) ? (timePerCycle - 1) : 0;
            TA0CCR2 = (timePerCycle > 0) ? (timePerCycle / 2) : 0;

            //Wait for the note duration
            WDT(duration);


            TA0CCR0 = 0;
            TA0CCR2 = 0;


            WDT(100);
        }
        else {

                    while (pause) {
                        __no_operation();
                    }
                }

    }
    P1OUT &= ~BIT7; // Disable the relay after playing
}


int main(void) {
    initMSP();
    function_for_interrupts();
    pwm();
    function_for_relstat();

    __bis_SR_register(GIE);

    while (1) {
        P1OUT &= ~BIT7;
        if (flagmonitor) {
            int delayCounter = 1000;
            while (delayCounter-- > 0) {
                __delay_cycles(1000); // delay
            }
            flagmonitor = 0;
            if (k_count) {
                combined_func_for_tune_play(k_count);
            }
            k_count = 0; //Reset
        }

    }

}

void clearInterruptFlag() {
    P1IFG &= ~BIT0;
}


void handleFirstKnock() {
    k_count = 1; //First knock detected
    currentState = STATE_DETECTING_KNOCKS;
}


void trackKnocks() {
    int remainingTime = 40;
    while (remainingTime > 0) {
        __delay_cycles(50);

        if (P1IFG & BIT0) { //Detect additional knock
            clearInterruptFlag();
            k_count++;
            remainingTime = 40;
        } else {
            remainingTime--;
        }
    }

    currentState = STATE_DEBOUNCING;
}


void applyDebounce() {
    __delay_cycles(1000);
    flagmonitor = 1;
    currentState = STATE_WAITING_FOR_KNOCK;
}

// Interrupt Service Routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    if (P1IFG & BIT0) {
        clearInterruptFlag();


        if (currentState == STATE_WAITING_FOR_KNOCK) {
            handleFirstKnock();
        } else if (currentState == STATE_DETECTING_KNOCKS) {
            trackKnocks();
        } else if (currentState == STATE_DEBOUNCING) {
            applyDebounce();
        }
    }
}

