/**************************************************************************************************************************************************
* Name : Chandrika Sethumadhavan | Matriculation nr: 5776354                                                                                       *
* Exercise 1 : Digital I/O and Interrupts                                                                                                          *
* Description: 1. I was not sure if the state 1 should work only once per press or keep working with a certain delay between the lighting up part  *
*                 since I could not find more information about it on the exercise sheet,                                                           *
*                 so I have enabled it to look like it is turning on with a delay more than once per press.                                        *
*              2. The polling was a little hard for me to judge if it was working properly because of my hardware buttons being very hard to press.*
*                 Because of this, I have made the button hard to press.                                                                           *
*                 Please press the button harder. :)
*              3. uncomment line 52 for polling
*
*Connections:   P1.3 connected to PB5 pin
*               P1.4 connected to PB6 pin
*               Red led connected to P1.5 pin
*               Green led connected to P1.6 pin
*               Blue led connected to P1.0 pin
*               Yellow led connected to P1.7 pin
*                                                                                                                                                  *
****************************************************************************************************************************************************/
// Required Libraries
#include <msp430.h>
#include <templateEMP.h>
#include <stdint.h>
#include <stdbool.h>

#define pb5 BIT3   // p1.3 and button pb5 connection
#define pb6 BIT4 // p1.4 and button pb6 connection
#define Red  BIT5  //p1.5 and red led connection
#define Green BIT6  // p1.6 and green led connection
#define Blue  BIT0  //p1.0 and blue led connection
#define Yellow BIT7  // p1.7 and yellow led connection


#define BUTTON_UNPRESSED 0 // switch case constants that shows button states
#define BUTTON_5_PRESSED 1
#define BUTTON_6_PRESSED 2
#define BOTH_BUTTONS_PRESSED 3


void stateTwoFunction();              //all Function prototype definitions
void configureButtonPB5();
void configureButtonPB6();
void configureButtonInputs();
void configureLEDOutputs();
int checkButtonStatus(int btn, int DB);
int getButtonState(uint8_t PB5_2, uint8_t PB6_2);
void mainLedCode(uint8_t PB5_2, uint8_t PB6_2);


volatile int PB5_2, PB6_2; // Declaring both the buttons

//comment this to use Polling or else ISR will work by default
#define InterruptServiceRoutine


// delay function for the given duration
void durationOfDelay(unsigned int duration) {
    volatile unsigned int count = duration;
    while (count != 0) {
        count--;
    }
}

                                                                     // TASK B state 2 just for red and yellow led
void stateTwoFunction() {
    P1OUT |= Red;      // Turn on Red LED
    P1OUT &= ~Yellow;  // Ensure Yellow LED is off
    durationOfDelay(25000); // Delay for 250 ms at 1MHz
    P1OUT &= ~Red;     // Turn off Red LED
    P1OUT |= Yellow;   // Turn on Yellow LED
}

// Configuring PB5 button
void configureButtonPB5() {
    P1DIR &= ~pb5;   // setingt PB5 as input
    P1OUT |= pb5;    // Enabling the pull-up resistor for PB5
    P1REN |= pb5;    // Enablling the resistor function for PB5
    P1IE |= pb5;     // Enabling interrupt for PB5
    P1IES |= pb5;    //  high-to-low transition trigger
    P1IFG &= ~pb5;   // Clear the interrupt flag
}

// Configuring PB6 button
void configureButtonPB6() {
    P1DIR &= ~pb6;   // Setting PB6 as input
    P1OUT |= pb6;    // Enabling the pull-up resistor for PB6
    P1REN |= pb6;    // Enabling the  resistor function for PB6
}

// functions to configure both PB5 and PB6 buttons
void configureButtonInputs() {
    configureButtonPB5();  // for button PB5
    configureButtonPB6(); // for button PB6
}

// Configuring Red, Green, Blue, and Yellow LEDs as output pins
void configureLEDOutputs() {
    P1DIR |= Red;   //Setting Red LED as output
    P1DIR |= Green; //Setting Green LED as output
    P1DIR |= Blue;  //  Setting Blue LED as output
    P1DIR |= Yellow; //Setting Yellow LED as output
}

int checkButtonStatus(int btn, int DB) { // Function to check button status for debouncing using a running average
    float averagePress = 0.0;  //Initialize
    float smoothingFactor = 0.1;  //Smoothing factor
    int checkIterations = 0;
    while (checkIterations < DB) {
        int currentPress = !(P1IN & btn); //Read the button state (1 if pressed, 0 if not)

        //Update the running average
        averagePress = (smoothingFactor * currentPress) + ((1 - smoothingFactor) * averagePress);

        volatile int waitCount = 1000;  //  Small wait time to stabilize
        while (waitCount--);

        checkIterations++;
    }

    // Bbutton will be considered pressed if only the average press level exceeds threshold
    int isPressed = (averagePress > 0.8);
    return isPressed;

}

// Function to determine the button state based on inputs                           TASK A BEHAVIOUR OF 4 DIFFERENT STATES
int getButtonState(uint8_t PB5_2, uint8_t PB6_2) {
    if (PB5_2 == 0 && PB6_2 == 0) {
        return BUTTON_UNPRESSED; //Both buttons are unpressed
    } else if (PB5_2 == 1 && PB6_2 == 0){
            return BUTTON_5_PRESSED;
        }
        //Button 5 is pressed
      else if (PB5_2 == 0 && PB6_2 == 1) {
        return BUTTON_6_PRESSED; //Button 6 is pressed
    } else if (PB5_2 == 1 && PB6_2 == 1) {
        return BOTH_BUTTONS_PRESSED; //Both buttons pressed
    }
      return BUTTON_UNPRESSED;
}

void mainLedCode(uint8_t PB5_2, uint8_t PB6_2) {
    int state = getButtonState(PB5_2, PB6_2); // Get current button state

    switch (state) {
        case BUTTON_UNPRESSED:
            P1OUT |= Yellow;                    //Turn on the yellow LED
            break;
        case BUTTON_5_PRESSED:
            stateTwoFunction();                    //Call the function to handle LED states
            break;
        case BUTTON_6_PRESSED:
            P1OUT |= Green;                            //Turn on the green LED
            P1OUT |= Yellow;                          //Turn on the yellow LED
            break;
        case BOTH_BUTTONS_PRESSED:
            P1OUT |= Blue;                            //Turn on the blue LED
            P1OUT |= Yellow;                              // Turning on yellow LED
            break;
    }
}


// Interrupt Service Routine for PB5 button
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    //Button PB5 interrupt service routine
    if (P1IFG & pb5) { //Check if the interrupt was caused by PB5
        PB5_2 = 1; //Update PB5 state to pressed
        durationOfDelay(1000);
        P1IFG &= ~pb5; // interrupt flag clear
    }
}


/********************* MAIN FUNCTION ********************************************/
void main() {
    initMSP();                  //For the template.h header file
    WDTCTL = WDTPW | WDTHOLD;   //Stop watchdog timer
    configureButtonInputs();    // config buttons as inputs
    configureLEDOutputs();      // config LEDs as outputs

    while (1) {
        #ifndef InterruptServiceRoutine
        if (PB5_2 == 1) { //Check the state from the interrupt
            mainLedCode(PB5_2, PB6_2); //call mainLedCode or handle the state directly
            PB5_2 = 0; //reseting PB5_2 after processing
            }
        #else
            PB5_2 = checkButtonStatus(pb5, 30);               //polling PB5 state if no interrupt
        #endif


        PB6_2 = checkButtonStatus(pb6, 30); //Poll the PB6 state
        P1OUT &= ~Red; //Turn off Red LED  initially
        P1OUT &= ~Green; //Turn off Green LED  initially
        P1OUT &= ~Blue;  // Turn off Blue LED initially
        P1OUT &= ~Yellow;  // Turn off Yellow LED  initially
        mainLedCode(PB5_2, PB6_2);                                                                       //function to handle TASK A)
    }
}




















































