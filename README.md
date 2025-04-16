# MSP430-Microcontroller-projects




[Board Schematic.pdf](Board%20Schematic.pdf) Provides the hardware layout and connections to help visualize and understand how the code interacts with the microcontroller board.


1. [Digital IO and interrupts.pdf](Digital%20IO%20and%20interrupts%20.pdf) 

   Worked on configuring GPIO pins to control LEDs (D5–D9) using button inputs (PB5, PB6). Used PxDIR, PxSEL, and PxREN registers to set directions and enable pull-up/down resistors. Developed logic to monitor button states with polling and software debouncing. Later extended the implementation by using an interrupt for PB5 while keeping PB6 polling active. Modularized the code with functions and used #define switches to toggle between modes.

Focus: Gained a strong foundation in digital IO handling, event response, and modular code design.


2. [Interfacing external ICs.pdf](Interfacing%20external%20ICs.pdf)
   
Implemented serial communication with LEDs and buttons via shift registers, emulating an SPI-like protocol. Controlled LEDs D1–D4 using a "running light" system that responded to buttons PB1–PB4, simulating playback, pause, rewind, and fast-forward states. Ensured responsiveness regardless of LED state and added a bonus feature with LEDs D5–D7 for a rotating effect. Configured special pins using P2SEL and P2SEL2 for general IO use.

Focus: Explored peripheral interfacing and real-time control logic using shift registers.


3. [Pulse Width Modulation.pdf](Pulse%20Width%20Modulation.pdf)
   
Used PWM with a constant 50% duty cycle and variable frequency to create audio output through a piezo buzzer. Composed and played two melodies stored in arrays. Introduced interrupt-driven control using PB5 to switch between melodies and added knock detection via the piezo sensor for the same functionality. Added pause/resume control through PB6, ensuring full interactivity across states.

Focus: Applied timer-based PWM for sound generation, handled analog input from piezo, and implemented interrupt-driven multi-modal control.


4. [Dynamic Circuit Configuration.pdf](Dynamic%20Circuit%20Configuration.pdf) 
Redesigned the melody player from the previous task by routing the piezo buzzer through a relay, allowing it to switch between a PWM output and an analog front-end for vibration detection. The buzzer was connected through a relay-controlled circuit, and interrupts were used for real-time knock sensing. Removed button-based interaction and fully relied on signal-based triggering. Documented the signal path and analyzed the advantages of separating analog sensing from digital actuation.
Focus: Optimized power usage and signal clarity by decoupling sensing and actuation using a relay, analog front-end, and interrupt logic.


   
5.  [Watchdog Timer and Timer modules.pdf](Watchdog%20Timer%20and%20Timer%20modules.pdf)

Simulated a fault condition (infinite loop) where the system entered a deadlock upon button press. Configured the Watchdog Timer+ with ACLK and VLOCLK to reset the microcontroller after timeout. Built a temperature-sensing system using an NTC thermistor and ADC input. Displayed temperature levels with LEDs and refreshed readings every two seconds using Timer A interrupts. Added a thermal control system to prevent overheating, and introduced dynamic control logic to simulate and recover from thermal failure scenarios.

Focus: Developed robust systems using watchdog timers, ADCs, timers, and dynamic feedback mechanisms for safety-critical embedded applications.

6. [A Command Line Tool for MSP430 Configuration.pdf](A%20Command%20Line%20Tool%20for%20MSP430%20Configuration.pdf)
   
Developed a command-line interface over UART that allows flexible, text-based control of the MSP430 board. The tool supports commands to enable or disable individual LEDs, read button states (PB1–PB6), and fetch analog values from the potentiometer (U_POT), light-dependent resistor (LDR), and NTC thermistor. Also implemented functionality to control the heater and trigger the buzzer with user-defined frequency and duration.

Created a serial command parser with support for on, off, and get instructions, including a -v (verbose) mode. This mode provides real-time textual feedback such as "Turning on D1 led..." or "PB2 is pressed: Flashing all LEDs". Invalid inputs are gracefully handled with a default message: "this command is not valid, try again".

Each button press is associated with a unique LED pattern:

PB1 triggers a Knight Rider effect on LEDs D1–D4.

PB2 flashes all LEDs ten times.

PB3 rotates LEDs D1–D4 once.

PB4 turns on the LEDs in a sequential cascade.

PB5 keeps all LEDs on.

PB6 alternates LEDs seven times.

For analog readouts, get_ldr-v, get_u_pot-v, and get_ntc-v return live sensor values. The buzzer command supports dynamic input:
on_buzzer_<frequency>_<duration>-v
and by default plays a short melody (Twinkle).

LED and heater control is tightly integrated — activating the heater also illuminates all LEDs D1–D4, while turning it off disables them.

Focus: Built a robust and user-friendly configuration shell over UART, combining GPIO control, analog sensing, visual feedback, and system response handling into a scalable, extendable framework.


 

 






