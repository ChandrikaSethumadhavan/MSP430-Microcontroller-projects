/*************************************************************************************************************************************************************************
 * @file                    main.c
 * @author                  Chandrika Sethumadhavan


 * All Pin Connections:

 * ldr to P1.5,
 * pb5 to P3.2,
 * pb6 to P3.3,
 * heater / blue led to P3.4 ,
 * u_ntc to P1.6,
 * u_pot to P1.7,
 * led red to P3.1,
 * led green to P3.0
 * buzzer to P3.6

//No need to modify anything in the code. can be directly used with a serial monitor as a command line tool.
 ****************************************************************************************************************************************************************************/

#include <templateEMP.h>
#include <string.h>
#include <stdlib.h>

//LED and button identifiers
#define rotLedd5 0
#define rotLedd6 1
#define shiftLedd1 2
#define shiftLedd2 3
#define shiftLedd3 4
#define shiftLedd4 5
#define invalidLed -1


//Global variables
void Handlerfcn(const char* command);
void get_Serial_Data(void);
int i =0;
int heaterFlag =0;
int potFlag, LDRFlag, ntcFlag;
int checkbuttonpb1, checkbuttonpb2, checkbuttonpb3, checkbuttonpb4;
int var_for_d1 = 0, var_for_d2 = 0, var_for_d3 = 0, var_for_d4 = 0;
const int d5_led = BIT0, d6_led = BIT1;


//handles buzzer functionality
void turnonbuzzer(int frequency, int duration) {
    static int checkbuzzer = 0;

    if (checkbuzzer) {
        return;
    }

    checkbuzzer = 1;

    P3DIR |= BIT6;
    TA0CCTL0 = CCIE;
    TA0CTL = TASSEL_2 + MC_1;

    if (frequency > 0 && duration > 0) {

        TA0CCR0 = 1000000 / frequency;
        TA0CCTL1 = OUTMOD_7;
        TA0CCR1 = TA0CCR0 / 2;


        int i;
        for ( i = 0; i < duration; i++) {
            __delay_cycles(1000);
        }
    } else {

        int count;
        const int melody2_size = 38;
        int twinkleStar[melody2_size] = {
            262, 262, 392, 392, 440, 440, 392, 392,
            349, 349, 330, 330, 294, 294, 262, 262,
            392, 392, 349, 349, 330, 330, 294, 294,
            392, 392, 349, 349, 330, 330, 294, 294,
            262, 262, 392, 392, 440, 440, 392, 392
        };

        for (count = 0; count < melody2_size; count++) {
            TA0CCR0 = 1000000 / twinkleStar[count];
            TA0CCTL1 = OUTMOD_7;
            TA0CCR1 = TA0CCR0 / 2;
            __delay_cycles(300000);
        }
    }


    TA0CTL = MC_0;
    TA0CCTL0 = 0;
    P3OUT &= ~BIT6;

    checkbuzzer = 0;
}


#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void) {
    P3OUT ^= BIT6;
    TA0CCTL0 &= ~CCIFG;
}


void clockpulse()
{
    P2OUT |= BIT4;
    P2OUT &= ~BIT4;
}

//Activating   the shift register based on LED variables
void Shift_register_function_activation() {
    P2OUT &= ~(BIT2 | BIT3);
    P2OUT |= BIT0;
    P2OUT &= ~BIT1;
    int part[4] = {var_for_d1, var_for_d2, var_for_d3, var_for_d4};
    int i;
    for (i = 0; i < 4; i++) {
        P2OUT = (P2OUT & ~BIT6) | (part[3 - i] << 6);
        clockpulse();
    }
}


// Mapping commands to LED identifiers
int commands_for_leds(const char* command) {
    if (strstr(command, "d5")) return rotLedd5;
    else if (strstr(command, "d6")) return rotLedd6;
    else if (strstr(command, "d1")) return shiftLedd1;
    else if (strstr(command, "d2")) return shiftLedd2;
    else if (strstr(command, "d3")) return shiftLedd3;
    else if (strstr(command, "d4")) return shiftLedd4;
    else return invalidLed;
}

//processes the LED commands upon on input
void Process_commands_for_leds(const char* var_to_get_ip) {
    int var_led = commands_for_leds(var_to_get_ip);

    switch (var_led) {
        case rotLedd5:
            Process_commands_for_D5(var_to_get_ip);
            break;
        case rotLedd6:
            Process_commands_for_D6(var_to_get_ip);
            break;
        case shiftLedd1:
            Process_commands_for_D1(var_to_get_ip);
            break;
        case shiftLedd2:
            Process_commands_for_D2(var_to_get_ip);
            break;
        case shiftLedd3:
            Process_commands_for_D3(var_to_get_ip);
            break;
        case shiftLedd4:
            Process_commands_for_D4(var_to_get_ip);
            break;
        default:
            serialPrintln("Unknown LED command");
            break;
    }
}



//functions to control specific Leds
void Process_commands_for_D5(const char* var_to_get_ip) {
    const char* getoncommand = "on_led_d5";
    const char* getoffCommand = "off_led_d5";


    if (strncmp(var_to_get_ip, getoncommand, strlen(getoncommand)) == 0) {
        P3OUT |= BIT0;
        if (strstr(var_to_get_ip, "-v")) {
            serialPrint("Turning on green led...");
        }
    }

    else if (strncmp(var_to_get_ip, getoffCommand, strlen(getoffCommand)) == 0) {
        P3OUT &= ~BIT0;
        if (strstr(var_to_get_ip, "-v")) {
            serialPrint("Turning off green led...");
        }
    }
}



void Process_commands_for_D6(const char* var_to_get_ip) {
    const char* getoncommand = "on_led_d6";
    const char* getoffCommand = "off_led_d6";


    if (strncmp(var_to_get_ip, getoncommand, strlen(getoncommand)) == 0) {
        P3OUT |= BIT1;
        if (strstr(var_to_get_ip, "-v")) {
            serialPrint("Turning on red led...");
        }
    }

    else if (strncmp(var_to_get_ip, getoffCommand, strlen(getoffCommand)) == 0) {
        P3OUT &= ~BIT1;
        if (strstr(var_to_get_ip, "-v")) {
            serialPrint("Turning off red led...");
        }
    }
}



void Process_commands_for_D1(const char* var_to_get_ip) {
    const char* getoncommand = "on_led_d1";
    const char* getoffCommand = "off_led_d1";


    if (strncmp(var_to_get_ip, getoncommand, strlen(getoncommand)) == 0) {
        var_for_d1 = 1;
        Shift_register_function_activation();
        if (strstr(var_to_get_ip, "-v")) {
            serialPrint("Turning on D1 led...");
        }
    }

    else if (strncmp(var_to_get_ip, getoffCommand, strlen(getoffCommand)) == 0) {
        var_for_d1 = 0;
        Shift_register_function_activation();
        if (strstr(var_to_get_ip, "-v")) {
            serialPrint("Turning off D1 led...");
        }
    }
}



void Process_commands_for_D2(const char* var_to_get_ip) {
    const char* getoncommand = "on_led_d2";
    const char* getoffCommand = "off_led_d2";


    if (strncmp(var_to_get_ip, getoncommand, strlen(getoncommand)) == 0) {
        var_for_d2 = 1;
        Shift_register_function_activation();
        if (strstr(var_to_get_ip, "-v")) {
            serialPrint("Turning on D2 led...");
        }
    }

    else if (strncmp(var_to_get_ip, getoffCommand, strlen(getoffCommand)) == 0) {
        var_for_d2 = 0;
        Shift_register_function_activation();
        if (strstr(var_to_get_ip, "-v")) {
            serialPrint("Turning off D2 led...");
        }
    }
}




void Process_commands_for_D3(const char* var_to_get_ip) {
    const char* getoncommand = "on_led_d3";
    const char* getoffCommand = "off_led_d3";


    if (strncmp(var_to_get_ip, getoncommand, strlen(getoncommand)) == 0) {
        var_for_d3 = 1;
        Shift_register_function_activation();
        if (strstr(var_to_get_ip, "-v")) {
            serialPrint("Turning on D3 led...");
        }
    }

    else if (strncmp(var_to_get_ip, getoffCommand, strlen(getoffCommand)) == 0) {
        var_for_d3 = 0;
        Shift_register_function_activation();
        if (strstr(var_to_get_ip, "-v")) {
            serialPrint("Turning off D3 led...");
        }
    }
}





void Process_commands_for_D4(const char* var_to_get_ip) {
    const char* getoncommand = "on_led_d4";
    const char* getoffCommand = "off_led_d4";


    if (strncmp(var_to_get_ip, getoncommand, strlen(getoncommand)) == 0) {
        var_for_d4 = 1;
        Shift_register_function_activation();
        if (strstr(var_to_get_ip, "-v")) {
            serialPrint("Turning on D4 led...");
        }
    }

    else if (strncmp(var_to_get_ip, getoffCommand, strlen(getoffCommand)) == 0) {
        var_for_d4 = 0;
        Shift_register_function_activation();
        if (strstr(var_to_get_ip, "-v")) {
            serialPrint("Turning off D4 led...");
        }
    }
}



//Initializes LEDs
void initLEDs() {
    P3DIR |= BIT1;
    P3OUT &= ~BIT1;
    P3DIR |= BIT0;
    P3OUT &= ~BIT0;
}


//for SR
void initHeater() {
    P3DIR |= BIT4;
    P3OUT &= ~BIT4;
}


//control SR
void Shift_reg_control() {
    P2SEL &= ~(BIT6 | BIT7);
    P2SEL2 &= ~(BIT6 | BIT7);

    P2DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6);
    P2DIR &= ~BIT7;
}

//reset SR
void Shift_reg_reset() {
    P2OUT &= ~BIT5;
    P2OUT |= BIT5;
    P2OUT &= ~BIT4;
}

//operating SR
void Shift_reg_op() {
    P2OUT &= ~BIT0;
    P2OUT &= ~BIT1;
    P2OUT &= ~BIT2;
    P2OUT &= ~BIT3;
}

//configuring buttons
void Buttons_control() {
    P3DIR &= ~BIT2;
    P3REN |= BIT2;
    P3OUT |= BIT2;

    P3DIR &= ~BIT3;
    P3REN |= BIT3;
    P3OUT |= BIT3;
}


//*******************************************************************************************************************//
//Main function
void main()
{
    initMSP();
    initLEDs();
    initHeater();
    Shift_reg_control();
    Shift_reg_reset();
    Shift_reg_op();
    Buttons_control();


    while(1)
    {
        get_Serial_Data();
    }
}

//********************************************************************************************************************//

////a circular buffer for serial input
#define BUFFER_SIZE 100

char Serial_buffer[BUFFER_SIZE];
int head = 0, tail = 0;
char commandBuffer[BUFFER_SIZE];

void addToBuffer(char c) {  /* Add a character to the buffer */
    Serial_buffer[head] = c;
    head = (head + 1) % BUFFER_SIZE;


    if (head == tail) {
        tail = (tail + 1) % BUFFER_SIZE;
    }
}

int isDataAvailable() {  /* Check if data is  available */
    return head != tail;
}


char readFromBuffer() { /* Read a ch aracter from the buffer */
    if (!isDataAvailable()) {
        return -1;
    }

    char c = Serial_buffer[tail];
    tail = (tail + 1) % BUFFER_SIZE;
    return c;
}

const char terminationChars[] = {'\r', '\n', ' ', ',', ';', '|', '\t', ':', '/'};


int isTerminationCharacter(char c) { /* Check if a character is  termination character */
    int i;
    for (i = 0; i < sizeof(terminationChars); i++) {
        if (c == terminationChars[i]) {
            return 1;
        }
    }
    return 0;
}

// function to Handle serial input and processes commands
void get_Serial_Data(void) {
    while (serialAvailable()) {
        char c = serialRead();
        addToBuffer(c);

        if (isTerminationCharacter(c)) {
            int index = 0;

            while (isDataAvailable()) {
                char temp = readFromBuffer();
                if (isTerminationCharacter(temp)) {
                    break;
                }
                commandBuffer[index++] = temp;
            }
            commandBuffer[index] = '\0';
            Handlerfcn(commandBuffer);
        }
    }
}


// function to Handle commands
void Handlerfcn(const char* command)
{
    char *var_to_get_ip;
    var_to_get_ip = (char*)command;
    serialPrintln(var_to_get_ip);
    int debugFlag = 0;


    if (strstr(var_to_get_ip, "on") || strstr(var_to_get_ip, "off")) {

        const char* ledNames[] = {"d5", "d6", "d1", "d2", "d3", "d4"};
        void (*ledHandlers[])(const char*) = {
            Process_commands_for_D5,
            Process_commands_for_D6,
            Process_commands_for_D1,
            Process_commands_for_D2,
            Process_commands_for_D3,
            Process_commands_for_D4
        };
        int numLeds = sizeof(ledNames) / sizeof(ledNames[0]);


        if (strstr(var_to_get_ip, "led")) {
            int checker_var = 0;
            int i;
            for ( i = 0; i < numLeds; i++) {
                if (strstr(var_to_get_ip, ledNames[i])) {
                    ledHandlers[i](var_to_get_ip);
                    checker_var = 1;
                    break;
                }
            }
            if (!checker_var) {
                serialPrintln("this command is not valid, try again");
            }
            debugFlag = 1;
        }

        //Heater
        if (strstr(var_to_get_ip, "heater")) {
            int command_verbose = strstr(var_to_get_ip, "-v") != NULL;
            static int counter = 0;
            static int heater_state = 0;

            if (strstr(var_to_get_ip, "on")) {
                if (heaterFlag == 0) {

                    P3OUT |= BIT4;
                    var_for_d1 = 1;
                    var_for_d2 = 1;
                    var_for_d3 = 1;
                    var_for_d4 = 1;
                    Shift_register_function_activation();
                    heaterFlag = 1;
                    heater_state = 1;
                    counter = 0;

                    if (command_verbose) {
                        serialPrintln("Heater and blue LED are enabled");
                    }
                } else if (command_verbose) {
                    serialPrintln("Heater is already enabled");
                }
            } else if (strstr(var_to_get_ip, "off")) {
                if (heaterFlag == 1) {

                    P3OUT &= ~BIT4;
                    var_for_d1 = 0;
                    var_for_d2 = 0;
                    var_for_d3 = 0;
                    var_for_d4 = 0;
                    Shift_register_function_activation();
                    heaterFlag = 0;
                    heater_state = 0;

                    if (command_verbose) {
                        serialPrintln("Heater and blue LED are manually disabled");
                    }
                } else if (command_verbose) {
                    serialPrintln("Heater is already disabled");
                }
            }


            if (heater_state == 1) {
                counter++;
                __delay_cycles(1000000);

                if (counter >= 7) {

                    P3OUT &= ~BIT4;
                    var_for_d1 = 0;
                    var_for_d2 = 0;
                    var_for_d3 = 0;
                    var_for_d4 = 0;
                    Shift_register_function_activation();
                    heaterFlag = 0;
                    heater_state = 0;
                    counter = 0;

                    if (command_verbose) {
                        serialPrintln("Heater and blue LED are disabled after 7 seconds");
                    }
                }
            }
        }




          else if (strstr(var_to_get_ip, "buzzer")) {
            int frequency = 0;
            int duration = 0;


            sscanf(var_to_get_ip, "buzzer_%d_%d", &frequency, &duration);


            int command_verbose = strstr(var_to_get_ip, "-v") != NULL;


            turnonbuzzer(frequency, duration);


            if (command_verbose) {
                if (frequency > 0 && duration > 0) {
                    serialPrint("Playing buzzer with frequency: ");
                    serialPrintInt(frequency);
                    serialPrint(" Hz for duration: ");
                    serialPrintInt(duration);
                    serialPrintln(" ms");
                } else {
                    serialPrintln("Playing pre-defined melody (twinkle).");
                }
            }

            debugFlag = 1;
        }



    }

    //int mem = 0;


    else if(strstr(var_to_get_ip, "get"))
    {



        if (strstr(var_to_get_ip, "u_pot") || strstr(var_to_get_ip, "ldr") || strstr(var_to_get_ip, "ntc")) {
            int channel, adcAE0Bit;
            const char* snsr_chosen;
            int* snsr_value;

            if (strstr(var_to_get_ip, "u_pot")) {
                channel = INCH_7;
                adcAE0Bit = BIT7;
                snsr_chosen = "Potentiometer";
                snsr_value = &potFlag;
            } else if (strstr(var_to_get_ip, "ldr")) {
                channel = INCH_5;
                adcAE0Bit = BIT5;
                snsr_chosen = "ldr";
                snsr_value = &LDRFlag;
                __delay_cycles(250000);
            } else {
                channel = INCH_6;
                adcAE0Bit = BIT6;
                snsr_chosen = "ntc";
                snsr_value = &ntcFlag;
            }

            ADC10CTL0 = ADC10ON + ADC10SHT_2;
            ADC10AE0 |= adcAE0Bit;
            ADC10CTL1 = channel;

            ADC10CTL0 |= ENC + ADC10SC;
            while (ADC10CTL1 & ADC10BUSY);

            *snsr_value = ADC10MEM;

            ADC10CTL0 &= ~(ENC + ADC10SC);

            if (strstr(var_to_get_ip, "-v")) {
                serialPrint("Fetching values ");
                serialPrintln(snsr_chosen);
                serialPrintln(" ");
            }

            serialPrint(snsr_chosen);
            serialPrint(" The value is: ");
            serialPrintInt(*snsr_value);
            serialPrintln(" ");

            debugFlag = 1;
        }

        else if (strstr(var_to_get_ip, "pb")) {
            int verboseFlag = strstr(var_to_get_ip, "-v") != NULL;


            struct ButtonConfig {
                int bitPosition;
                int* buttonState;
                const char* buttonName;
            } buttonConfigs[] = {
                {BIT7, &checkbuttonpb4, "pb4"},
                {BIT7, &checkbuttonpb3, "pb3"},
                {BIT7, &checkbuttonpb2, "pb2"},
                {BIT7, &checkbuttonpb1, "pb1"},
                {BIT2, NULL, "pb5"},
                {BIT3, NULL, "pb6"}
            };


            int i, j;


            for (i = 0; i < sizeof(buttonConfigs) / sizeof(buttonConfigs[0]); i++) {
                if (i < 4) {
                    P2OUT |= (BIT2);
                    if (i == 0) P2OUT |= BIT3; else P2OUT &= ~BIT3;
                    P2OUT |= BIT4;
                    P2OUT &= ~BIT4;
                    *(buttonConfigs[i].buttonState) = (P2IN & buttonConfigs[i].bitPosition) ? 1 : 0;
                }
            }


            if (checkbuttonpb1) { //PB1: Knight Rider Pattern
                if (verboseFlag) serialPrintln("PB1 is pressed: Activating Knight Rider pattern.");
                for (j = 0; j < 4; j++) {
                    var_for_d1 = (j == 0);
                    var_for_d2 = (j == 1);
                    var_for_d3 = (j == 2);
                    var_for_d4 = (j == 3);
                    Shift_register_function_activation();
                    __delay_cycles(500000);
                }
                for (j = 2; j >= 0; j--) {
                    var_for_d1 = (j == 0);
                    var_for_d2 = (j == 1);
                    var_for_d3 = (j == 2);
                    var_for_d4 = (j == 3);
                    Shift_register_function_activation();
                    __delay_cycles(500000);
                }
            } else if (checkbuttonpb2) { //PB2: Flash All LEDs
                if (verboseFlag) serialPrintln("PB2 is pressed: Flashing all LEDs.");
                for (i = 0; i < 10; i++) { // Flash 10 times
                    var_for_d1 = var_for_d2 = var_for_d3 = var_for_d4 = 1;
                    Shift_register_function_activation();
                    __delay_cycles(250000);
                    var_for_d1 = var_for_d2 = var_for_d3 = var_for_d4 = 0;
                    Shift_register_function_activation();
                    __delay_cycles(250000);
                }
            } else if (checkbuttonpb3) { //PB3: Rotate LEDs
                if (verboseFlag) serialPrintln("PB3 is pressed: Rotating LEDs.");
                for (i = 0; i < 8; i++) {
                    var_for_d1 = (i % 4 == 0);
                    var_for_d2 = (i % 4 == 1);
                    var_for_d3 = (i % 4 == 2);
                    var_for_d4 = (i % 4 == 3);
                    Shift_register_function_activation();
                    __delay_cycles(500000);
                }
            } else if (checkbuttonpb4) { //PB4: Turn on LEDs Sequentially
                if (verboseFlag) serialPrintln("PB4 is pressed: Turning on LEDs sequentially.");
                var_for_d1 = 1; Shift_register_function_activation(); __delay_cycles(500000);
                var_for_d2 = 1; Shift_register_function_activation(); __delay_cycles(500000);
                var_for_d3 = 1; Shift_register_function_activation(); __delay_cycles(500000);
                var_for_d4 = 1; Shift_register_function_activation(); __delay_cycles(500000);
            } else if (!(P3IN & BIT2)) { // PB5: All LEDs Stay On
                if (verboseFlag) serialPrintln("PB5 is pressed: Keeping all LEDs ON.");
                var_for_d1 = var_for_d2 = var_for_d3 = var_for_d4 = 1;
                Shift_register_function_activation();
            } else if (!(P3IN & BIT3)) { // PB6: Alternate LEDs
                if (verboseFlag) serialPrintln("PB6 is pressed: Alternating LEDs.");
                for (i = 0; i < 10; i++) {
                    var_for_d1 = var_for_d3 = 1; var_for_d2 = var_for_d4 = 0;
                    Shift_register_function_activation();
                    __delay_cycles(250000);
                    var_for_d1 = var_for_d3 = 0; var_for_d2 = var_for_d4 = 1;
                    Shift_register_function_activation();
                    __delay_cycles(250000);
                }
            } else { // No button pressed: Turn off all LEDs
                var_for_d1 = var_for_d2 = var_for_d3 = var_for_d4 = 0;
                Shift_register_function_activation();
                if (verboseFlag) serialPrintln("No button is pressed: Turning off all LEDs.");
            }

            debugFlag = 1;
        }




        else if (debugFlag == 0){

                serialPrintln("entered command is not valid, try again");
            }
    }


}

















