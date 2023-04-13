/*
 * File:   PWMmanual.c
 * Author: Gabriel Carrera 21216
 *
 * Created on April 12, 2023, 10:43 PM
 */

#include "PWMmanual.h"
#include <xc.h>


#define TMR0_VAL 249 // valor necesario para 0.015 us
#define PWM_VAR PORTCbits.RC3 // para poder cambiar el pin de salida de manera sencilla
uint8_t LED_VAR; // variable para comparar con el PWM
uint8_t LED_PWM; // variable para almacenar el valor del ADC



void PWMmanual_setup(void) {
    // Configurar el pin de salida para el PWM manual, en este caso RC3
    TRISCbits.TRISC3 = 0;
    
    // Configurar TMR0 y sus interrupciones 
    // --------------- TMR0 --------------- 
    OPTION_REGbits.T0CS = 0; // utilizar el reloj interno (fosc/4)
    OPTION_REGbits.PSA = 0; // asignar el prescaler a TMR0
    OPTION_REGbits.PS2 = 0; // prescaler de 2:1
    OPTION_REGbits.PS1 = 0;
    OPTION_REGbits.PS0 = 0;  
    TMR0 = TMR0_VAL; // VALOR INICIAL DEL TMR0, definido arriba
}

// ----------- funcion de asignacion de valor de PWM -------------
void PWMmanual_INT(void) {
        LED_VAR ++; //incrementar cintador del tmr0
        
        if (LED_VAR <= LED_PWM){//comparar el valor del contador con el valor del potenciometro
            PWM_VAR = 1; //si el valor del contador es menor o igual a 1 encide el puerto
        }
        else{
            PWM_VAR = 0; // en cualquier otra situacion apagar el puerto
            
        }
        INTCONbits.T0IF = 0; //limpia bandera del tmr0
        TMR0 = TMR0_VAL;
}