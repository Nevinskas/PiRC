#ifndef MAIN
#define MAIN

#define BIT(x)			(1 << (x))

#define JS_AXIS_MAX		32767

#define ACCEL_PIN		RPI_V2_GPIO_P1_12
#define ACCEL_PWM_CH		0
#define ACCEL_PWM_RANGE		65535

#define TURN_PIN		RPI_V2_GPIO_P1_33
#define TURN_PWM_CH		1
#define TURN_PWM_RANGE		JS_AXIS_MAX

#define IN0			RPI_V2_GPIO_P1_11
#define IN1			RPI_V2_GPIO_P1_13
#define IN2			RPI_V2_GPIO_P1_15
#define IN3			RPI_V2_GPIO_P1_16


#endif
