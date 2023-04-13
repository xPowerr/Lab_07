/* 
 * File: LIBRERIA PWM.H  
 * Author: Gabriel Carrera 21216
 */

#include <xc.h> // include processor files - each processor file is guarded.  

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PWM_H
#define	PWM_H

//definir las dos funciones necesarias
void PWM_SETUP(char channel, float periodo_ms);
void PWM_DUTY(char channel, float duty);

#endif	/* PWM_H */

