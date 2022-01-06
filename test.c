#include "ev3api.h"
#include "app.h"

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

#define R_moter EV3_PORT_B
#define L_moter EV3_PORT_C

#define gyro_sensor EV3_PORT_4

const unit32_t WAIT_TIME_MS = 100;

void run_task(intptr_t unused) {
    int lpower, rpower;
    ev3_gyro_sensor_reset(gyro_sensor);
    lpower = 30;
    rpower = 20;
    langle = 30;
    rangle = 10;
    while(1){
        ev3_moter_rotate(L_moter, langle, lpower, false);
        ev3_moter_rotate(R_moter, rangle, lpower, true);
        if (ev3_gyro_sensor_get_angle(gyro_sensor) > 360) {
            ev3_motor_stop(L_moter,true);
            ev3_motor_stop(R_moter,true);
            break;
        }
        tslp_tsk(WAIT_TIME_MS);
    }
    ev3_motor_set_power(L_moter,90);
    ev3_moter_rotate(L_moter, 540, lpower, false);
    ev3_moter_rotate(R_moter, 540, lpower, true);

    lpower /= 2:
    rpower /= 2;
    ev3_motor_set_power(R_moter,90);
    tslp_tsk(WAIT_TIME_MS);
    while(1){
        ev3_moter_rotate(L_moter, langle, lpower, false);
        ev3_moter_rotate(R_moter, rangle, lpower, true);
        if (ev3_gyro_sensor_get_angle(gyro_sensor) > 360){
            ev3_motor_stop(L_moter,true);
            ev3_motor_stop(R_moter,true);
            break;
        }
        tslp_tsk(WAIT_TIME_MS);
    }
}

void main_task(intptr_t unused) {
    ev3_moter_config(L_moter, LARGE_MOTER);
    ev3_moter_config(R_moter, LARGE_MOTER);

    act_tsk(RUN_TASK);
}