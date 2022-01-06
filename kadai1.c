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

#define gyro_sensor EV3_PORT_2

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

const uint32_t WAIT_TIME_MS = 100;

void run_task(intptr_t unused)
{
    int lpower, rpower, langle, rangle;
    ev3_gyro_sensor_reset(gyro_sensor);
    lpower = 50;
    rpower = 20;
    langle = 20;
    rangle = 10;
	char str[20];
    //M down
    int rotate_times = 2;
	MiddleMotorDown();
    while (rotate_times > 0)
    {
        while (1)
        {
        	sprintf(str,"111%4d",ev3_gyro_sensor_get_angle(gyro_sensor));
        	ev3_lcd_draw_string(str,0,0);
            ev3_motor_rotate(L_motor, langle, lpower, false);
            ev3_motor_rotate(R_motor, rangle, lpower, false);
            if (rotate_times == 2 && ev3_gyro_sensor_get_angle(gyro_sensor) >= 370)
            {
            	MiddleMotorUp();
                //M up
                ev3_motor_stop(L_motor, true);
                ev3_motor_stop(R_motor, true);
                break;
            } else if (rotate_times == 1 && ev3_gyro_sensor_get_angle(gyro_sensor) >= 170){
            	MiddleMotorUp();
            	ev3_motor_stop(L_motor, true);
                ev3_motor_stop(R_motor, true);
                break;
            }
            tslp_tsk(WAIT_TIME_MS);
        }
        tslp_tsk(WAIT_TIME_MS);
    	if (rotate_times ==1) break;
        ev3_motor_rotate(L_motor, 100, 60, false); //rotate 90 angle
        ev3_motor_rotate(R_motor, -100, 60, true); //rotate 90 angle
    	ev3_motor_rotate(L_motor, 40, 60, false); //rotate 90 angle
    	ev3_motor_rotate(R_motor, 40, 60, true); //rotate 90 angle

        lpower += 2;
        rpower *= 2;
    	langle+=30;
        ev3_motor_rotate(R_motor, 70, 60, true); //rotate 90 angle
        tslp_tsk(WAIT_TIME_MS);
        ev3_gyro_sensor_reset(gyro_sensor);
        //M down
    	MiddleMotorDown();

        rotate_times--;
    }
}

void main_task(intptr_t unused)
{
	ev3_sensor_config(gyro_sensor,GYRO_SENSOR);
    ev3_motor_config(L_motor, LARGE_MOTOR);
    ev3_motor_config(R_motor, LARGE_MOTOR);
    ev3_motor_config(M_motor, MEDIUM_MOTOR);

        act_tsk(RUN_TASK);
}

inline void MiddleMotorDown(void)
{
	ev3_motor_stop(M_M_PORT, true);
	int32_t startval_m = ev3_motor_get_counts(M_M_PORT);
	KILL_MOTOR();
	ev3_motor_rotate(M_M_PORT, M_D_DEGREE, M_D_POWER, true);
}
inline void MiddleMotorUp(void)
{
	ev3_motor_stop(M_M_PORT, true);
	int32_t startval_m = ev3_motor_get_counts(M_M_PORT);
	KILL_MOTOR();
	ev3_motor_rotate(M_M_PORT, M_U_DEGREE, M_U_POWER, true);
}
