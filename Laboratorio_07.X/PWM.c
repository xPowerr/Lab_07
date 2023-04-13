/*
 * File:   PWM.c
 * Author: Gabriel Carrera 21216
 *
 * Created on April 9, 2023, 10:03 PM
 */

#include "PWM.h"
#include <xc.h>


#define PRESCALER 16        // definir el prescaler
#define _XTAL_FREQ 4000000  // frecuencia de 4MHZ

// es el mismo setup para el PWM que fue realizado en el lab

void PWM_SETUP(char channel, float periodo_ms) {
    PR2 = (unsigned char)((_XTAL_FREQ/1000 * periodo_ms)/(4 * PRESCALER) - 1);
    PIR1bits.TMR2IF = 0; // Limpiar bandera del TMR2
    T2CONbits.T2CKPS = 0b11; // Preescaler 16
    T2CONbits.TMR2ON = 1; // Encender el TMR2
    while (!PIR1bits.TMR2IF); // Ciclo de espera
    
    PIR1bits.TMR2IF = 0; // Limpiar la bandera del TMR2

    if (channel == 1) {
        TRISCbits.TRISC1 = 1; // CCP1 como entrada
        CCP1CON = 0; // Apagar CCP1 inicialmente
        CCP1CONbits.P1M = 0; // Modo de single output
        CCP1CONbits.CCP1M = 0b1100; // Modo PWM para CCP1
        TRISCbits.TRISC1 = 0; // Habilitar salida en RC1
    } else if (channel == 2) {
        TRISCbits.TRISC2 = 1; // CCP2 como entrada
        CCP2CON = 0; // Apagar CCP2 inicialmente
        CCP2CONbits.CCP2M = 0b1100; // Modo PWM para CCP2
        TRISCbits.TRISC2 = 0; // Habilitar salida en RC2
    }
    return;
}

void PWM_DUTY(char channel, float duty) {
    unsigned int duty_var = (unsigned int)((duty * (PR2+1) * 4) / 100.0);

    if (channel == 1) {
        CCPR1L = duty_var >> 2; // VALOR DE LA VARIABLE DEL ANCHO DE TRABAJO
        CCP1CONbits.DC1B = duty_var & 0b11; // Establecer los bits menos significativos
    } else if (channel == 2) {
        CCPR2L = duty_var >> 2; // VALOR DE LA VARIABLE DEL ANCHO DE TRABAJO
        CCP2CONbits.DC2B0 = duty_var & 0b01; // Establecer los bits menos significativos
        CCP2CONbits.DC2B1 = duty_var & 0b10; // Establecer los bits menos significativos
    }
    return;
}