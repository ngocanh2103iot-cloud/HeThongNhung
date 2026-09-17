#ifndef PWM_INIT_H
#define PWM_INIT_H

extern int led_on;
extern int pwm_percent;

void Pwm_Init(void);
void Led_Update(void);

#endif
