//// Name: Chandrika Sethumadhavan
//// Matriculation number : 5776354
//// Exercise : 2

//********************************************************************************************************************//
////bonus task implemented
////connections : P1.5 TO K3 (red led), P1.7 TO K4 (green led), P1.6 TO HEATER X10 (blue led) // for BONUS TASK
/// No other physical connection required for task 1
//********************************************************************************************************************//

#include <msp430.h>
#include <templateEMP.h>

void custom_delay();
int pb4=0;
int pb3 = 0;
int pb2=0;
int previous_state = 0; // 0 = pause, 1 = pb3 (playback), 2 = other state (if needed)


void clockPulse(void)
{
    P2OUT |= BIT4;
    P2OUT = P2OUT & ~BIT4;
}



void led_D1()  //function to turn on D1
{
    int i=0;
    for(i =0; i<3; i++){
                P2OUT &= ~BIT6;
                P2OUT |= BIT4;
                P2OUT &= ~BIT4;
            }
            P2OUT |= BIT6;

            P2OUT |= BIT4;
            P2OUT &= ~BIT4;
}
void led_D2()  //function to turn on D2
{
    int i=0;
    for(i =0; i<2; i++){
                P2OUT &= ~BIT6;
                P2OUT |= BIT4;
                P2OUT &= ~BIT4;
            }
            P2OUT |= BIT6; //d2 on
            P2OUT |= BIT4; //Clock pulse to shift data in
            P2OUT &= ~BIT4;
            P2OUT &= ~BIT6;//d1 off
            P2OUT |= BIT4; // Clock pulse to shift data in
            P2OUT &= ~BIT4;
}
void led_D3()   //function to turn on D3
{
    int i=0;
    for(i =0; i<1; i++){
                P2OUT &= ~BIT6;
                P2OUT |= BIT4;
                P2OUT &= ~BIT4;
            }
            P2OUT |= BIT6;
            P2OUT |= BIT4;
            P2OUT &= ~BIT4;
            for(i =0; i<2; i++){
                            P2OUT &= ~BIT6;
                            P2OUT |= BIT4;
                            P2OUT &= ~BIT4;
                        }
}
void led_D4()   //function to turn on D4
{
    int i=0;


    P2OUT |= BIT6;
    P2OUT |= BIT4; // Clock pulse to shift data in
    P2OUT &= ~BIT4;
    for(i =0; i<3; i++){
                    P2OUT &= ~BIT6;
                    P2OUT |= BIT4;
                    P2OUT &= ~BIT4;
                }
}

void update_rotating_leds() {                      //bonus task
    static int rotating_state = 0;


   P1OUT &= ~(BIT5 | BIT6 | BIT7);


   switch (rotating_state) {
       case 0:
           P1OUT |= BIT5; //Turn on D5
           break;
       case 1:
           P1OUT |= BIT6; //Turn on D6
           break;
       case 2:
           P1OUT |= BIT7; //Turn on D7
           break;
   }


   rotating_state++;
   if (rotating_state >= 3) {
       rotating_state = 0;
   }

}






void button_pressed_pb4() { //function to check pb4 being pressed


        P2OUT &= ~BIT5;
        P2OUT |= BIT5;
        P2OUT &= ~BIT2; // SR 1 NO CHANGE MODE
        P2OUT &= ~BIT3;
        P2OUT |= BIT0;
        P2OUT &= ~BIT1;

        P1OUT &= ~(BIT6 + BIT7);


        update_rotating_leds();



        switch (pb4) {
            case 0:
                led_D1();

                __delay_cycles(90000);

                pb4 = 1;

                break;
            case 1:
                led_D2();

                __delay_cycles(90000);

                pb4 = 2;
                break;

            case 2:
                led_D3();

                __delay_cycles(90000);

                pb4 = 3;


                break;
            case 3:
                led_D4();

                __delay_cycles(90000);

                pb4 = 0;
                break;

        }

    }



void button_pressed_pb3() { //function to check pb3 being pressed

        if (pb3==1){
            P2OUT &= ~BIT5;
            P2OUT |= BIT5;
            P2OUT &= ~BIT2; // SR 1 NO CHANGE MODE
            P2OUT &= ~BIT3;
            P2OUT |= BIT0;
            P2OUT &= ~BIT1;


            //bonus question
            P1OUT &= ~(BIT6 + BIT7);
            update_rotating_leds();

            switch (pb4) {
                case 0:
                    led_D1();
                    __delay_cycles(230000);
                    pb4 = 1;
                    break;
                case 1:
                    led_D2();
                    __delay_cycles(230000);
                    pb4 = 2;
                    break;
                case 2:
                    led_D3();
                    __delay_cycles(230000);
                    pb4 = 3;
                    break;
                case 3:
                    led_D4();
                    __delay_cycles(230000);
                    pb4 = 0;
                    break;
            }
        }
        previous_state = 3;
}


void button_pressed_pb2(){  //function to check pb2 being pressed






    if (pb2==1) {

            switch (pb4) {
                case 0:
                    led_D1();
                    P2OUT |= BIT5;  //bonus


                    break;
                case 1:
                    led_D2();
                    P2OUT |= BIT6;

                    break;
                case 2:
                    led_D3();
                    P2OUT |= BIT7;

                    break;
                case 3:
                    led_D4();
                    P2OUT |= BIT5;

                    break;
            }
            return;
        }
    previous_state = 2;




}
void button_pressed_pb1(){   ////function to check pb2 being pressed



        P2OUT &= ~BIT5;
        P2OUT |= BIT5;
        P2OUT &= ~BIT2; // SR 1 NO CHANGE MODE
        P2OUT &= ~BIT3;
        P2OUT |= BIT0;
        P2OUT &= ~BIT1;
        update_rotating_leds();  //bonus task




        switch (pb4) {
            case 0:
                led_D4();
                __delay_cycles(100000);
                pb4=3;
                break;
            case 1:
                led_D1();
                __delay_cycles(100000);
                pb4=0;
                break;
            case 2:
                led_D2();
                __delay_cycles(100000);
                pb4=1;
                break;
            case 3:
                led_D3();
                __delay_cycles(100000);
                pb4=2;
                break;
        }


    }


void main(void)
{
     initMSP();
     WDTCTL = WDTPW | WDTHOLD;

     //Set as output pins //bonus
    P1DIR |= (BIT5 | BIT6 | BIT7);  //set pins P1.5, P1.6, P1.7
    P1OUT &= ~(BIT5 | BIT6 | BIT7); //to turn off the D5, D6 and D7 LEDs


    //Set the pin direction
    P2DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6);
    P2DIR &= ~BIT7;

    //Clear the registers for bonus task
    P2SEL &= ~(BIT6 | BIT7);
    P2SEL2 &= ~(BIT6 | BIT7);

    //Clear the shift register
    P2OUT = P2OUT & ~BIT5;
    P2OUT |= BIT5;

    //Clock_Pulse signal to LOW
    P2OUT = P2OUT & ~BIT4;

    //Both SR1 and SR2 to No change state
    P2OUT = P2OUT & ~BIT2;
    P2OUT = P2OUT & ~BIT3;
    P2OUT = P2OUT & ~BIT0;
    P2OUT = P2OUT & ~BIT1;

    while(1){
        // button - SR1

           //serial reg 2 is off. turn it onn for led
            P2OUT = P2OUT & ~BIT0;
            P2OUT = P2OUT & ~BIT1;
            //Serial register 1 in parallel shifting mode
            P2OUT |= BIT2;
            P2OUT |= BIT3;
            clockPulse();
            //FOR LED
            if (P2IN & BIT7){   //pb4 is pressed

                button_pressed_pb4();

            }
            else{
                P2OUT |= BIT2;
                P2OUT = P2OUT & ~BIT3;
                clockPulse();
                if (P2IN & BIT7){              //pb3 is pressed
                    pb3 =1;
                    pb2=0;
                    previous_state = 3;



                }
                else{
                    clockPulse();
                    if (P2IN & BIT7){             //pb2 is pressed
                        //count = 0;
                        pb2 = 1;
                        pb3 =0;
                        previous_state = 2;
                        button_pressed_pb2();
                    }
                    else{
                        clockPulse();
                        if (P2IN & BIT7){                      //pb1 is pressed
                            pb3 = 0;
                            button_pressed_pb1();
                        }
                        else if (!(P2IN & BIT7)) {  // PB1 is released
                                                // Return to the previous state when PB1 is released
                                if (previous_state == 2) {  // Resume pause
                                    pb2 = 1;
                                    pb3 = 0;
                                    button_pressed_pb2();
                                } else if (previous_state == 3) {  // Resume playback
                                    pb3 = 1;
                                    button_pressed_pb3();
                                }
                            }

                    }
                }
            }
            if (pb3 == 1) {

            button_pressed_pb3();
            }

        }

    }
















































