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

#define _XTAL_FREQ 4000000 // establecer oscilador intero a 4 MHz

// ----------- Valores para potenciometro y mappeo -------------
#define inputmin 0 //valor minimo del potenciometro 
#define inputmax 255 //valor maxiomo del potenciometro
#define outmin 100 //valor minimo el pwm 0.4 ms para
#define outmax 600
// variables para controlar pulse width
unsigned int CCPR1VAR = 0; //variable para el CCPR1
unsigned int CCPR2VAR = 0; //variable para el CCPR2

// ------------- funcion para mappear valores ------------
unsigned int map(uint8_t INPUT, uint8_t IN_MIN, uint8_t IN_MAX,
        unsigned int OUT_MIN, unsigned int OUT_MAX);

// --------------- Rutina de  interrupciones --------------- 
void __interrupt() isr(void) {
    if (PIR1bits.ADIF) { // si se activa la bandera de interrupcion del ADC
        if (ADCON0bits.CHS == 0b0000){ // si está en ADC AN0
            CCPR1VAR = map(ADRESH, inputmin, inputmax, outmin, outmax); // Mappear el valor del potenciometro a los valores necesarios para el CCPR1L
            CCPR1L = (uint8_t)(CCPR1VAR>>2); // Elimina los dos bits menos significativos y lo guarda en CCPR1L
            CCP1CONbits.DC1B = CCPR1VAR & 0b11; // Guarda los dos bits menos significativos en DC1B
           // ADCON0bits.CHS = 0b0100;
        }
        else if (ADCON0bits.CHS == 0b0100){ // si está en ADC AN5
            CCPR2VAR = map(ADRESH, inputmin, inputmax, outmin, outmax); // Mappear el valor del potenciometro a los valores necesarios para el CCPR2L
            CCPR2L = (uint8_t)(CCPR2VAR>>2); // Elimina los dos bits menos significativos y lo guarda en CCPR2L
            CCP2CONbits.DC2B0 = CCPR2VAR & 0b01; // Guardar el primer bit menos significativo
            CCP2CONbits.DC2B1 = CCPR2VAR & 0b10; // Guardar el segundo bit menos significativo
            //ADCON0bits.CHS = 0b0000; // cambiar a ADC AN0
        }
        PIR1bits.ADIF = 0; // limpiar la bandera de la interrupcion
    }
}

// --------------- Prototipos --------------- 
void setup(void);
void setupPWM(void); 
void setupADC(void);
//void setupOSC(void);

// --------------- main --------------- 
void main(void) {
    
    setup ();
    setupPWM ();
    setupADC ();
    while(1){
        __delay_ms(10);
        if (ADCON0bits.GO == 0) { // si la lectura del ADC se desactiva
            if (ADCON0bits.CHS == 0b0000){ // si está en ADC AN0
            ADCON0bits.CHS = 0b0100;
            }
        else if (ADCON0bits.CHS == 0b0100){ // si está en ADC AN5
            ADCON0bits.CHS = 0b0000; // cambiar a ADC AN0
            }
            ADCON0bits.GO = 1;
            __delay_us(20);
        }
        
        
        
    }
    return;
}

// --------------- Setup General --------------- 
void setup(void){
    // --------------- Definir como digitales --------------- 
    ANSELbits.ANS0 = 1;
    ANSELbits.ANS4 = 1;
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
    OSCCONbits.IRCF = 0b110 ; // establecerlo en 4 MHz
    OSCCONbits.SCS = 1; // utilizar oscilador interno
    

    // --------------- Banderas e interrupciones --------------- 
    INTCONbits.GIE = 1; // habilitar interrupciones globales
    INTCONbits.PEIE = 1; // habilitar interrupciones perifericas
    //PIE1bits.TMR2IE = 1; // habilitar interrupciones de TMR2
    //PIR1bits.TMR2IF = 0; // limpiar la bandera de interrupcion del TMR2
    PIE1bits.ADIE = 1; // habilitar interrupciones de ADC
    PIR1bits.ADIF = 0; // limpiar la bandera de interrupcion del ADC


}

// --------------- Setup PWM --------------- 
void setupPWM(void){
    // --------------- CCP1 ---------------
    TRISCbits.TRISC1 = 1; // Habilitar CCP2 como entrada
    TRISCbits.TRISC2 = 1; // Habilitar CCP1 como entrada
    PR2 = 249; // Utilizar periodo de 4 ms del TMR2
    
    CCP1CON = 0;
    CCP2CON = 0;
    
    CCP1CONbits.P1M = 0; // Habilitar modo single output
    CCP1CONbits.CCP1M = 0b00001100; // Habilitar modo PWM, P1A y P1C HIGH Y P1B y P1D LOW
    CCP2CONbits.CCP2M = 0b00001100; // Habilitar modo PWM, P1A y P1C HIGH Y P1B y P1D LOW
    
    CCPR1L = 250 >> 2; // Valor necesario para establecer servo en 90 grados, ancho de pulso 1 ms
    CCP1CONbits.DC1B = 250 & 0b11; // Establecer los bits menos significativos
    
    CCPR2L = 250 >> 2; // Valor necesario para establecer servo en 90 grados, ancho de pulso 1 ms
    CCP2CONbits.DC2B0 = 250 & 0b01;
    CCP2CONbits.DC2B1 = 250 & 0b10;
    
    
    // --------------- TMR2 ---------------
    PIR1bits.TMR2IF = 0; // Reiniciar la bandera de interrupcion del TMR2
    T2CONbits.T2CKPS = 0b11; // Establecer prescaler de TMR2 en 1:16
    T2CONbits.TMR2ON = 1; // Encender el TMR2
    
    while (PIR1bits.TMR2IF == 0);
    PIR1bits.TMR2IF = 0;
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC1 = 0;
}

// --------------- Setup del ADC --------------- 
void setupADC(void){
    // --------------- Configura el canal --------------- 
    ADCON0bits.CHS = 0b0000; // seleccionar AN0
    
            
    // --------------- Seleccion voltaje referencia --------------- 
    ADCON1bits.VCFG1 = 0; // Voltaje de referencia de 0V
    ADCON1bits.VCFG0 = 0; // Voltaje de referencia de 5V
            
    // --------------- Seleccion de reloj ---------------
    ADCON0bits.ADCS = 0b10; // Fosc/32
            
    // --------------- Habilitar interrupciones del ADC ---------------
    
            
    // --------------- Asignar 8 bits, justificado izquierda ---------------
    ADCON1bits.ADFM = 0;        
            
    //--------------- Iniciar el ADC ---------------
    ADCON0bits.ADON = 1;  
    __delay_ms(1);
}

//void setupOSC(void) {
//    mappot_1 = map(pot_1, 0, 255, 0, 1000); // Mappear el valor del potenciometro a los valores necesarios para el CCPR1L
//    CCPR1L = mappot_1 >> 2;
//    CCP1CONbits.DC1B = mappot_1 & 0b11;
//    mappot_2 = map(pot_2, 0, 255, 0, 1000); // Mappear el valor del potenciometro a los valores necesarios para el CCPR1L
//    CCPR2L = mappot_2 >> 2;
//    CCP2CONbits.DC2B0 = mappot_2 & 0b01;
//    CCP2CONbits.DC2B1 = mappot_2 & 0b10;
//}

// Ejecucion de funcion de mappeo
unsigned int map(uint8_t INPUT, uint8_t IN_MIN, uint8_t IN_MAX,
        unsigned int OUT_MIN, unsigned int OUT_MAX){
    return (unsigned int)(OUT_MIN+((float)(OUT_MAX-OUT_MIN)/(IN_MAX-IN_MIN))
            *(INPUT-IN_MIN));
};