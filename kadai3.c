#include "ev3api.h"
#include "app.h"

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

#define R_motor EV3_PORT_B
#define L_motor EV3_PORT_C
#define M_motor EV3_PORT_A
#define color_sensor PORT_3

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

#define gyro_sensor EV3_PORT_4

#define black 6
#define white 91

#define M_M_PORT EV3_PORT_A
#define R_M_PORT EV3_PORT_B
#define L_M_PORT EV3_PORT_C
#define G_S_PORT EV3_PORT_4
#define STRAIGHT_POWER 20
#define M_D_DEGREE 20
#define M_D_POWER 20
#define M_U_POWER 20
#define M_U_DEGREE -20

#define ECSGR(x) ev3_color_sensor_get_reflect(x)
#define STRAIGHT(x) ev3_motor_steer(L_M_PORT, R_M_PORT, x, 0)
#define KILL_MOTOR() (ev3_motor_stop(L_M_PORT, true), ev3_motor_stop(R_M_PORT, true))
#define OneSteppd (ev3_motor_get_counts(L_M_PORT) - startval_l > STRAIGHT_ONESTEP || ev3_motor_get_counts(R_M_PORT) - startval_r > STRAIGHT_ONESTEP)

const unit32_t WAIT_TIME_MS = 100;

void run_task(intptr_t unused)
{
    int lpower, rpower, langle, rangle;
    lpower = 30;
    rpower = 30;
    langle = 30;
    rangle = 30;
    const int threshold = (black + white) / 2;
    //M down
    MiddleMotorDown();
    while (1)
    {
        while (1)
        {
            ev3_motor_rotate(L_motor, langle, lpower, false);
            ev3_motor_rotate(R_motor, rangle, lpower, false);
            if (ev3_color_sensor_get_reflect(color_sensor) < threshold)
            {
                ev3_motor_stop(L_motor, true);
                ev3_motor_stop(R_motor, true);
                ev3_motor_rotate(L_motor, 270, lpower, false);
                ev3_motor_rotate(R_motor, -270, lpower, true);
                break;
            }
            tslp_tsk(WAIT_TIME_MS);
        }
        ev3_motor_rotate(R_motor, 540, 20, true); //rotate 90 angle
        ev3_motor_rotate(L_motor, 540, lpower, false);
        ev3_motor_rotate(R_motor, 540, lpower, true);

        lpower /= 2;
        rpower /= 2;
        ev3_motor_rotate(R_motor, 540, 20, true); //rotate 90 angle
        tslp_tsk(WAIT_TIME_MS);
        //M down
    }
}

void main_task(intptr_t unused)
{
    ev3_motor_config(L_motor, LARGE_MOTOR);
    ev3_motor_config(R_motor, LARGE_MOTOR);
    ev3_motor_config(M_motor, MEDIUM_MOTOR);

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
