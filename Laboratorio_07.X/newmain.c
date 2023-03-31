/*
 * File:   newmain.c
 * Autor: Gabriel Carrera 21216
 *
 * Created on March 31st, 2023 at 8:45 AM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 8000000 // establecer oscilador intero a 8MHz

// --------------- Rutina de  interrupciones --------------- 
void __interrupt() isr(void) {
    }

// --------------- Prototipos --------------- 
void setup(void);
void setupPWM(void); 

// --------------- main --------------- 
void main(void) {
    
    setup ();
    setupPWM ();
    while(1){
        CCPR1L = 0b01111101; // Valor necesario para establecer servo en 90 grados, ancho de pulso 1 ms
        CCP1CONbits.DC1B = 0b00; // Establecer los bits menos significativos
    }
    //return;
}

// --------------- Setup General --------------- 
void setup(void){
    // --------------- Definir como digitales --------------- 
    ANSEL = 0;
    ANSELH = 0;

    // --------------- Configurar puertos --------------- 
    TRISC = 0; // Configura PORTC como salida

    // --------------- Limpiar puertos --------------- 
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    
    // --------------- Oscilador --------------- 
    OSCCONbits.IRCF = 0b111 ; // establecerlo en 8 MHz
    OSCCONbits.SCS = 1; // utilizar oscilador interno
    

    // --------------- Banderas e interrupciones --------------- 
    INTCONbits.GIE = 1; // habilitar interrupciones globales
    INTCONbits.PEIE = 1; // habilitar interrupciones perifericas
    //PIE1bits.TMR2IE = 1; // habilitar interrupciones de TMR2
    //PIR1bits.TMR2IF = 0; // limpiar la bandera de interrupcion del TMR2
    


}

// --------------- Setup PWM --------------- 
void setupPWM(void){
    TRISCbits.TRISC2 = 1; // Habilitar CCP1 como entrada
    PR2 = 199; // Utilizar periodo de 20 ms del TMR2
    
    CCP1CONbits.P1M = 0; // Habilitar modo single output
    CCP1CONbits.CCP1M = 0b00001100; // Habilitar modo PWM, P1A y P1C HIGH Y P1B y P1D LOW
    
    CCPR1L = 0b01111101; // Valor necesario para establecer servo en 90 grados, ancho de pulso 1 ms
    CCP1CONbits.DC1B = 0b00; // Establecer los bits menos significativos
    
    // --------------- TMR2 ---------------
    PIR1bits.TMR2IF = 0; // Reiniciar la bandera de interrupcion del TMR2
    T2CONbits.T2CKPS = 0b11; // Establecer prescaler de TMR2 en 1:16
    T2CONbits.TMR2ON = 1; // Encender el TMR2
    
    while (PIR1bits.TMR2IF == 0);
    PIR1bits.TMR2IF = 0;
    TRISCbits.TRISC2 = 0;
}