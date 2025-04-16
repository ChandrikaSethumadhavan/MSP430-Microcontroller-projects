/***************************************************************************//**
 * @file                    main.c
 * @author                  Chandrika Sethumadhavan
 * @date                    22/11/2024
 * @matriculation number    5776354
 * @e-mail contact          chandrikasethu06@gmail.com
 * @Exercise number         3 Pulse Width Modulation
 *
 * Please check feedback.txt for @brief


 * All Pin Connections:
    * - P3.6: Piezo Buzzer
    * - P1.4: pb6_button
    * - P1.3: pb5_button

 * - Only one task can be active at a time. The active task is defined by the preprocessor directives ( Task A, Task B and Task C).
     IMPORTANT: Please uncomment the task that you want to run.
 ******************************************************************************/


#include <msp430.h>
#include <templateEMP.h>

//#define Task_A
//#define Task_B
#define Task_C


#define NOTE_LENGTH_SCALE 2

//Pin connections
#define Piezo_buzzer BIT6

#define pb6_button BIT4

#define pb5_button BIT3

//Melody data
#define melody1_size 22
#define melody2_size 40

/************************************************************************************/
//Melodies
int maryHadALamb[melody1_size] = {
    330, 294, 262, 294, 330, 330, 330,
    294, 294, 294,
    330, 392, 392,
    330, 294, 262, 294, 330, 330, 330
};


int twinkleStar[melody2_size] = {
    262, 262, 392, 392, 440, 440, 392, 392,
    349, 349, 330, 330, 294, 294, 262, 262,
    392, 392, 349, 349, 330, 330, 294, 294,
    392, 392, 349, 349, 330, 330, 294, 294,
    262, 262, 392, 392, 440, 440, 392, 392
};

/************************************************************************************/
unsigned int i = 0;
int j = 0;
int k_count = 0;
int count = 0;
int m_count = 0;
int pause = 0; // 0 for playing, 1 for paused


//Melody 1
void playMaryHadALambMelody() {
    play(maryHadALamb, melody1_size);
}

//Melody 2
void playTwinkleStarMelody() {
    play(twinkleStar, melody2_size);
}

//watchdog timer delay
void WDT(unsigned int milliseconds) {
    WDTCTL = WDT_MDLY_0_5;  //Set WDT to interval mode (0.5ms)
    unsigned int counter = milliseconds * 2;

    while (counter > 0) {
        if (IFG1 & WDTIFG) { //Check the WDT interrupt flag
            IFG1 &= ~WDTIFG; //Clear the WDT interrupt flag
            counter--;       //Decrementing counter
        }
    }

    WDTCTL = WDTPW | WDTHOLD;
}

//Setting frequency and duration of the melody
void play(const int* melodyData, unsigned int dataLength) {
    for (i = 0; i < dataLength; i += 2) {
        if (!pause) {
            int frequency = melodyData[i];
            int duration = melodyData[i + 1] * NOTE_LENGTH_SCALE;

            //Calculate frequency
            int timepercycle = (frequency > 0) ? (1000000 / frequency) : 0;
            TA0CCR0 = (timepercycle > 0) ? (timepercycle - 1) : 0;
            TA0CCR2 = (timepercycle > 0) ? (timepercycle / 2) : 0;

            //Wait for the note duration
            WDT(duration);

            // turn off the buzzer
            TA0CCR0 = 0;
            TA0CCR2 = 0;

            //delay
            WDT(100);
        } else {
            //Wait until pause is toggled off
            while (pause) {
                __no_operation();
            }
        }
    }
}

/**************************************************************************************************************************************/
/*Initialisations*/


//Configuring the piezo buzzer as input
void configurebuzzerasinput() {
    P3SEL &= ~Piezo_buzzer;
    P3SEL2 &= ~Piezo_buzzer;
    P3DIR &= ~Piezo_buzzer;
    P3REN |= Piezo_buzzer;
    P3OUT |= Piezo_buzzer;
}

//Configuring the piezo buzzer as output
void configurebuzzerasoutput() {
    P3SEL |= Piezo_buzzer;
    P3DIR |= Piezo_buzzer;
    TA0CCTL2 = OUTMOD_3;
    TA0CTL = TASSEL_2 + MC_1;
    P3REN &= ~Piezo_buzzer;
}

//PWM configuration
void pwm(void) {
    P3DIR |= Piezo_buzzer;
    P3SEL |= Piezo_buzzer;
    TA0CCTL2 = OUTMOD_3;
    TA0CTL = TASSEL_2 + MC_1;
}

//Configuring the buttons
void buttons(void) {
    P1DIR &= ~(pb5_button | pb6_button);
    P1REN |= pb5_button | pb6_button;
    P1OUT |= pb5_button | pb6_button;
}

//Configuring the buttons with interrupts
void buttons_interrupts(void) {
    P1DIR &= ~(pb5_button | pb6_button);
    P1OUT |= pb5_button | pb6_button;
    P1REN |= pb5_button | pb6_button;
    P1IE |= pb5_button | pb6_button;
    P1IES |= pb5_button | pb6_button;
    P1IFG &= ~(pb5_button | pb6_button);
    __bis_SR_register(GIE);
}


/**************************************************************************************************************************************/
//Debouncing the button press
int handleButtonPress(int pin) {
    __delay_cycles(100000); // Debounce delay
    if (!(P1IN & pin)) {
        return 1;
    }
    return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void) {
    if (P1IFG & pb5_button) {
        if (handleButtonPress(pb5_button)) {
            count++;
        }
        P1IFG &= ~pb5_button; // Clear interrupt flag
    } else if (P1IFG & pb6_button) {
        if (handleButtonPress(pb6_button)) {
            togglePauseState(); // Use a function to handle pause state
        }
        P1IFG &= ~pb6_button; // Clear interrupt flag
    }
}

// Function to toggle the pause state
void togglePauseState(void) {
    static int lastState = 0; // Stores the previous pause state
    pause = !lastState;       // Toggle the state
    lastState = pause;        // Update the last state
}

/************************************************************************************************************************************************************************/
//Main function
void main(void) {
    initMSP();

    pwm();
    buttons();
    buttons_interrupts();
    TA0CTL = TASSEL_2 + MC_1;

    while (1) {
#ifdef Task_C
    configurebuzzerasinput();

    // Configure Timer A1 for 1 ms intervals
    TA1CTL = TASSEL_2 | MC_1 | TACLR; // Use SMCLK, up mode, clear timer
    TA1CCR0 = 1000;                   // Set timer period

    if (!(P3IN & BIT6)) {  // Check for the first knock
        __delay_cycles(50000); // Debounce delay for first knock
        k_count = 1;

        int detectionWindow = 1000; // Monitor time window for second knock
        int elapsedMillis = 0;

        while (elapsedMillis < detectionWindow) {
            if (TA1CCTL0 & CCIFG) {  // Timer overflow occurred
                TA1CCTL0 &= ~CCIFG; // Clear the interrupt flag
                elapsedMillis++;    // Increment elapsed time

                if (!(P3IN & BIT6)) { // Check for second knock
                    __delay_cycles(50000); // Debounce delay for second knock
                    k_count++;
                }
            }
        }

        // Decide which melody to play based on knock count
        configurebuzzerasoutput();
        switch (k_count) {
            case 1:
                playMaryHadALambMelody();
                break;
            case 2:
                playTwinkleStarMelody();
                break;
            default:
                // No action if no valid knock detected
                break;
        }
    }

    TA1CTL = MC_0; // Stop Timer A1
#endif


        #ifdef Task_B
        __delay_cycles(1000000);
        __disable_interrupt();
        if (count == 1) {
            m_count = 1;
        } else if (count == 2) {
            m_count = 2;
        }
        count = 0;
        __enable_interrupt();

        if (m_count == 1) {
            configurebuzzerasoutput();
            playMaryHadALambMelody();
            m_count = 0;
        } else if (m_count == 2) {
            configurebuzzerasoutput();
            playTwinkleStarMelody();
            m_count = 0;
        }
        #endif


        #ifdef Task_A
        playMaryHadALambMelody();
        __delay_cycles(1000000); //1 second delay
        playTwinkleStarMelody();
        __delay_cycles(1000000); //1 second delay
        #endif
    }
}

