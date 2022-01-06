#include "ev3api.h"
#include "app.h"
#include <stdlib.h>
#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

//port settings
#define M_M_PORT EV3_PORT_A
#define R_M_PORT EV3_PORT_B
#define L_M_PORT EV3_PORT_C
#define G_S_PORT EV3_PORT_4
#define C_S_PORT EV3_PORT_1

//parameter settings
#define M_M_D_DEGREE 20
#define M_M_D_POWER 20
#define M_M_U_POWER 20
#define M_M_U_DEGREE 20
#define STRAIGHT_POWER 20
#define TURN_POWER 20
#define WAIT_TIME_MS 100
#define STRAIGHT_ONESTEP 10
#define STRAIGHT_ONELINE 10
#define BLACK 6
#define WHITE 91
#define STRAIGHT_WITH_GYRO 1 //not 0

//I dont like typing long function name
#define EMC(a, b) ev3_motor_config(a, b)
#define ECSGR(a) ev3_color_sensor_get_reflect(a)
#define EMGC(a) ev3_motor_get_counts(a)
#define EGSR(a) ev3_gyro_sensor_reset(a)
#define EMSTOP(a, b) ev3_motor_stop(a, b)
#define EMR(a, b, c, d) ev3_motor_rotate(a, b, c, d)
#define EMSP(a, b) ev3_motor_set_power(a, b)
#define ESC(a, b) ev3_sensor_config(a, b)
#define EGSGR(a) ev3_gyro_sensor_get_rate(a)

//function
#define STRAIGHT(a) ev3_motor_steer(L_M_PORT, R_M_PORT, a, 0)
#define TURN(a) EMSP(M_M_PORT, -a), EMSP(R_M_PORT, a)
#define KILL_MOTOR (EMSTOP(L_M_PORT, true), EMSTOP(R_M_PORT, true))
#define CCSV(a, b) ((ECSGR(a) >= b) ? ((ECSGR(a) > b) ? 1 : 0) : -1)
#define ONESTEPPED (EMGC(L_M_PORT) - startval_l > STRAIGHT_ONESTEP || EMGC(R_M_PORT) - startval_r > STRAIGHT_ONESTEP)
#define ONELINE(diff) (EMGC(L_M_PORT) - startval_l > STRAIGHT_ONELINE - diff || EMGC(R_M_PORT) - startval_r > STRAIGHT_ONELINE - diff)

inline void
GoStraight(void);
inline void MiddleMotorDown(void);
inline void MiddleMotorUp(void);

void run_task(intptr_t unused)
{
	uint8_t sequence = 0, diff = 0, turnval = 140;
	EGSR(G_S_PORT);

	while (1)
	{

		switch (sequence)
		{
		case 0:
			/* move lines */
			{
				MiddleMotorDown();
#ifdef STRAIGHT_WITH_GYRO
				EGSR(G_S_PORT);
#endif
				int32_t startval_l = EMGC(L_M_PORT), startval_r = EMGC(R_M_PORT);

				while (1)
				{
					GoStraight();
#ifdef STRAIGHT_WITH_GYRO
					while (EGSGR(G_S_PORT) > 10)
						EMSP(R_M_PORT, 20), EMSP(L_M_PORT, -20);
					while (EGSGR(G_S_PORT) < -10)
						EMSP(R_M_PORT, -20), EMSP(L_M_PORT, 20);
#endif
					while (ECSGR(G_S_PORT) > BLACK)
						EMSP(R_M_PORT, 20), EMSP(L_M_PORT, -20);
				}
				if (ONELINE(diff))
				{
					diff++, sequence = 1;
				}
			}
		case 1:
			/* turn */
			{
				KILL_MOTOR;
				int32_t startval = EGSGR(G_S_PORT);
				while (abs(EGSGR(G_S_PORT) - startval) > turnval)
					TURN(TURN_POWER);
				if (turnval == 140)
					turnval = 170;
				if (turnval == 170)
					turnval = 90;
				if (turnval == 90)
					turnval = 140;
			}
			break;
		}
	}
}

void main_task(intptr_t unused)
{
	EMC(L_M_PORT, LARGE_MOTOR), EMC(R_M_PORT, LARGE_MOTOR), EMC(M_M_PORT, MEDIUM_MOTOR);
	ESC(G_S_PORT, GYRO_SENSOR), ESC(C_S_PORT, COLOR_SENSOR);
	act_tsk(RUN_TASK);
}

inline void GoStraight(void)
{
	KILL_MOTOR;
	int32_t startval_l = EMGC(L_M_PORT), startval_r = EMGC(R_M_PORT);
	STRAIGHT(STRAIGHT_POWER);
	if (ONESTEPPED)
		EMSTOP(L_M_PORT, true), EMSTOP(R_M_PORT, true);
}
inline void MiddleMotorDown(void)
{
	EMSTOP(M_M_PORT, true);
	int32_t startval_m = EMGC(M_M_PORT);
	KILL_MOTOR;
	EMR(M_M_PORT, M_M_D_DEGREE, M_M_D_POWER, true);
}
inline void MiddleMotorUp(void)
{
	EMSTOP(M_M_PORT, true);
	KILL_MOTOR;
	EMR(M_M_PORT, M_M_U_DEGREE, M_M_U_POWER, true);
}
