/* 
 * File: LIBRERIA PWMmanual.H  
 * Author: Gabriel Carrera 21216
 */

#include <xc.h> // include processor files - each processor file is guarded.  

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PWMmanual_H
#define	PWMmanual_H

#include <stdint.h>

//definir las dos funciones necesarias
void PWMmanual_setup(void);
void PWMmanual_INT(void);

#endif	/* PWMmanual_H */

