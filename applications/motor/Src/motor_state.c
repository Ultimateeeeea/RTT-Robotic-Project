/* motor_state.c */
#include "motor/Inc/motor_state.h"
#include "gyro/Inc/sensor_port.h"
#include <PID/Inc/pid.h>
#include <math.h>

/* 右旋转90度用 */
static PID_Inc_t pivot_yaw_pid;   // 掉头用的航向 PID
static float  pivot_target_yaw = 0.0f;
static const float PIVOT_TOLERANCE = 2.0f;    // ±3° 容差
static const uint16_t PIVOT_RPM  = 50;  // 旋转速度
#define PIVOT_DELAY_MS (int)(CTRL_PERIOD_S * 1000)


/* 50ms 20Hz*/
#define CTRL_PERIOD_S 0.05f    // 50 ms

/* 假设四个轮地址固定：LF=0x01, RF=0x02, LB=0x03, RB=0x04 */
#define LF 0x01
#define RF 0x02
#define LB 0x03
#define RB 0x04

/* 57步进电机 */
#define Elevator 0x05

/* CW为顺时针 CCW为逆时针 */
#define CW 0x00
#define CCW 0x01

/* Relative为相对位置 Absolute为绝对位置*/
#define Relative 0x00
#define Absolute 0x01

/* 运动状态 */
static motor_state_t cur_state = ST_IDLE;

/* 速度差 PID，用来把左右侧速度拉平 */

/* 新增：航向 PID 实例，用于计算目标速度差 V_sp */
static PID_Inc_t straight_pid;
/* 速度差 PID，用来把左右侧速度拉平 */
static PID_Inc_t speed_pid;

static float target_yaw = 0.0f;  // 前进时的目标航向

volatile int16_t Curent_Speed[4] = {0};

/* 角度处理函数 */
static float normalize_angle(float angle)
{
    // 先把角度变到 0 ~ 360
    while (angle < 0.0f)   angle += 360.0f;
    while (angle >= 360.0f) angle -= 360.0f;
    // 再转到 -180 ~ +180
    if (angle > 180.0f)
        angle -= 360.0f;
    return angle;
}

/* 在进入前进状态时，记录当前航向，并重置 PID */
void motor_state_set(motor_state_t st)
{
    if ((st == ST_FORWARD
      || st == ST_BACKWARD
      || st == ST_LEFT
      || st == ST_RIGHT)
     && cur_state != st)
    {
        target_yaw = g_yaw_angle;
        PID_Inc_Init(&straight_pid, 2.0f, 200.0f, 0.5f, target_yaw);    /* 角度外环 */
        PID_Inc_Init(&speed_pid,    1.0f,  10.0f, 0.1f,  0.0f);         /* 速度内环 */
    }

    if (st == ST_PIVOT_RIGHT90 && cur_state != ST_PIVOT_RIGHT90)
       {
           // 1) 先计算目标航向：向右（顺时针）转 90°
           pivot_target_yaw = normalize_angle(g_yaw_angle - 90.0f);

           // 2) 用目标航向初始化 PID（只清历史一次）
           PID_Inc_Init(&pivot_yaw_pid,
                        /*kp=*/2.0f,
                        /*ki=*/0.0f,
                        /*kd=*/0.1f,
                        pivot_target_yaw);
       }

    else if (st == ST_PIVOT_RIGHT180 && cur_state != ST_PIVOT_RIGHT180)
    {
        pivot_target_yaw = normalize_angle(g_yaw_angle - 180.0f);
        PID_Inc_Init(&pivot_yaw_pid, 2.0f, 0.0f, 0.1f, pivot_target_yaw);
    }

    cur_state = st;
}



/* 当前状态对应的运动状态 */
void motor_state_poll(void)
{
    switch (cur_state)
    {
    case ST_IDLE:                motion_stop();            break;
    case ST_FORWARD:             motion_forward(50);       break;
    case ST_BACKWARD:            motion_backward(50);     break;
    case ST_LEFT:                motion_strafe_left(50);   break;
    case ST_RIGHT:               motion_strafe_right(50); break;
    case ST_PIVOT_RIGHT90:       motion_pivot_right_90();  break;
    case ST_PIVOT_RIGHT180:      motion_pivot_right_180(); break;
    case ST_UP:                  elevator_up(100);         break;
    case ST_DOWN:                elevator_down(100);       break;
    case ST_Elevator_Stop:       elevator_stop();          break;
    default:                                               break;
    }
}
/***************************** 42电机 ****************************/
/* 前进指令 —— 四轮驱动，双环 PID */
rt_err_t motion_forward(uint16_t rpm_base)
{
    /* 1) 航向 PID 输出期望差速 V_sp */
    float V_sp = PID_Inc_Compute(&straight_pid, g_yaw_angle, CTRL_PERIOD_S);  // current=g_yaw_angle, target=pid.target :contentReference[oaicite:2]{index=2}:contentReference[oaicite:3]{index=3}
    V_sp = fmaxf(fminf(V_sp,  50.0f), -50.0f);

    /* 2) 读取左右实际速度（单位同 rpm） */
    float left_speed  = ((float)Curent_Speed[0] + Curent_Speed[2]) * 0.5f;
    float right_speed = ((float)Curent_Speed[1] + Curent_Speed[3]) * 0.5f;

    /* 3) 速度差误差 = (实际差速) - V_sp */
    float speed_error = (left_speed - right_speed) - V_sp;

    /* 4) 速度差 PID 输出修正 corr */
    float delta_u = PID_Inc_Compute(&speed_pid, speed_error, CTRL_PERIOD_S);
    delta_u = fmaxf(fminf(delta_u, 20.0f), -20.0f);
    int16_t corr = (int16_t)delta_u;
    uint16_t delta = (corr >= 0) ? corr : -corr;

    /* 5) 计算左右电机转速 */
    uint16_t rpm_left  = (corr >= 0)
                         ? ((rpm_base > delta) ? rpm_base - delta : 0)
                         : (rpm_base + delta);

    uint16_t rpm_right = (corr >= 0)
                         ? (rpm_base + delta)
                         : ((rpm_base > delta) ? rpm_base - delta : 0);

    /* 6) 下发四轮命令并同步启动 */
    motor_cmd_speed(LF, CW,  rpm_left,  0, RT_TRUE);
    motor_cmd_speed(LB, CW,  rpm_left,  0, RT_TRUE);
    motor_cmd_speed(RF, CCW, rpm_right, 0, RT_TRUE);
    motor_cmd_speed(RB, CCW, rpm_right, 0, RT_TRUE);
    motor_cmd_sync(0x00);

    /* 7) 打印调试信息 —— 整数 + 两位小数格式 */
    {
        static int _print_cnt = 0;
        const int PRINT_PERIOD = 5; // 每5次打印一次
        if (++_print_cnt >= PRINT_PERIOD)  // 5 × 20ms = 100ms 打印一次
        {
            _print_cnt = 0;
            float yaw_error = normalize_angle(target_yaw - g_yaw_angle);
            int32_t yi = (int32_t)(yaw_error * 100);
            int32_t vi = (int32_t)(V_sp      * 100);
            int32_t y_frac = yi % 100; if (y_frac < 0) y_frac = -y_frac;
            int32_t v_frac = vi % 100; if (v_frac < 0) v_frac = -v_frac;
            rt_kprintf("yaw_err=%d.%02d Vsp=%d.%02d corr=%d L=%u R=%u\r\n",
                       yi/100, y_frac,
                       vi/100, v_frac,
                       corr, rpm_left, rpm_right);
        }
    }
    return RT_EOK;
}

/* 直线后退 —— 四轮驱动，双环 PID */
rt_err_t motion_backward(uint16_t rpm_base)
{
    /* 1) 外环：航向 PID 输出期望差速 V_sp */
    float V_sp = PID_Inc_Compute(&straight_pid, g_yaw_angle, CTRL_PERIOD_S);
    V_sp = fmaxf(fminf(V_sp,  50.0f), -50.0f);

    /* 2) 读左右实际速度 */
    float left_speed  = ((float)Curent_Speed[0] + Curent_Speed[2]) * 0.5f;
    float right_speed = ((float)Curent_Speed[1] + Curent_Speed[3]) * 0.5f;

    /* 3) 内环：速度差误差 = (实际差速) - V_sp */
    float speed_error = (left_speed - right_speed) - V_sp;

    /* 4) 速度差 PID 输出修正 corr */
    float delta_u = PID_Inc_Compute(&speed_pid, speed_error, CTRL_PERIOD_S);
    delta_u = fmaxf(fminf(delta_u, 20.0f), -20.0f);
    int16_t corr = (int16_t)delta_u;
    uint16_t delta = corr >= 0 ? corr : -corr;

    /* 5) 计算左右转速（后退方向下要符号翻转） */
    uint16_t rpm_left  = (corr >= 0)
                         ? (rpm_base + delta)               // corr>0：左侧加速
                         : ((rpm_base > delta) ? rpm_base - delta : 0);
    uint16_t rpm_right = (corr >= 0)
                         ? ((rpm_base > delta) ? rpm_base - delta : 0)
                         : (rpm_base + delta);              // corr<0：右侧加速

    /* 6) 下发四轮命令并同步启动（后退方向 CW/CCW 对调） */
    motor_cmd_speed(LF, CCW, rpm_left,  0, RT_TRUE);
    motor_cmd_speed(LB, CCW, rpm_left,  0, RT_TRUE);
    motor_cmd_speed(RF, CW,  rpm_right, 0, RT_TRUE);
    motor_cmd_speed(RB, CW,  rpm_right, 0, RT_TRUE);
    motor_cmd_sync(0x00);


    /* 7) 调试打印 */
    {
        static int _print_cnt = 0;
        const int PRINT_PERIOD = 5; // 每5次打印一次
        if (++_print_cnt >= PRINT_PERIOD)  // 5×20ms=100ms 打印一次
           {
                _print_cnt = 0;
               float yaw_error = normalize_angle(target_yaw - g_yaw_angle);
               int32_t yi = (int32_t)(yaw_error * 100);
               int32_t vi = (int32_t)(V_sp      * 100);
               int32_t yf = yi % 100; if (yf < 0) yf = -yf;
               int32_t vf = vi % 100; if (vf < 0) vf = -vf;

               rt_kprintf("BWD yaw_err=%d.%02d Vsp=%d.%02d corr=%d L=%u R=%u\r\n",
                          yi/100, yf,
                          vi/100, vf,
                          corr, rpm_left, rpm_right);
           }
    }
    return RT_EOK;
}

/* 右平移指令 —— 四轮驱动，双环 PID */
rt_err_t motion_strafe_right(uint16_t rpm_base)
{
    /* 1) 外环：航向 PID 输出期望对角差速 V_sp */
    // 让小车沿着 target_yaw 平移时不偏航
    float V_sp = PID_Inc_Compute(&straight_pid, g_yaw_angle, CTRL_PERIOD_S);
    V_sp = fmaxf(fminf(V_sp,  50.0f), -50.0f);

    /* 2) 读取实际对角速度（单位同 rpm） */
    // 前左(LF) & 后右(RB) 组成一对，前右(RF) & 后左(LB) 组成一对
    float left_diag  = ((float)Curent_Speed[0] + Curent_Speed[3]) * 0.5f;
    float right_diag = ((float)Curent_Speed[1] + Curent_Speed[2]) * 0.5f;

    /* 3) 内环：速度差误差 = (实际对角差速) - V_sp */
    float speed_error = (left_diag - right_diag) - V_sp;

    /* 4) 速度差 PID 输出修正 corr */
    float du = PID_Inc_Compute(&speed_pid, speed_error, CTRL_PERIOD_S);
    du = fmaxf(fminf(du,  20.0f), -20.0f);
    int16_t corr = (int16_t)du;
    uint16_t delta = (corr >= 0) ? (uint16_t)corr : (uint16_t)(-corr);

    /* 5) 计算两对对角线最终转速 */
    // corr > 0：左对角线减速、右对角线加速
    // corr < 0：左对角线加速、右对角线减速
    uint16_t rpm_ld = (corr >= 0)
                      ? ((rpm_base > delta) ? rpm_base - delta : 0)
                      : (rpm_base + delta);
    uint16_t rpm_rd = (corr >= 0)
                      ? (rpm_base + delta)
                      : ((rpm_base > delta) ? rpm_base - delta : 0);

    /* 6) 下发四轮平移命令并同步启动 */
    // 前左 & 后右 用 rpm_ld，方向 CW；前右 & 后左 用 rpm_rd，方向 CCW
    motor_cmd_speed(LF, CW,  rpm_ld, 0, RT_TRUE);
    motor_cmd_speed(RB, CW,  rpm_ld, 0, RT_TRUE);
    motor_cmd_speed(RF, CCW, rpm_rd, 0, RT_TRUE);
    motor_cmd_speed(LB, CCW, rpm_rd, 0, RT_TRUE);
    motor_cmd_sync(0x00);

    /* 7) 调试打印 —— 控制频率 */
       {
           static int _print_cnt = 0;
           const int PRINT_PERIOD = 5; // 每5次打印一次
           if (++_print_cnt >= PRINT_PERIOD)
           {
               _print_cnt = 0;
               float yaw_error = normalize_angle(target_yaw - g_yaw_angle);
               int32_t yi     = (int32_t)(yaw_error * 100);
               int32_t y_frac = yi % 100; if (y_frac < 0) y_frac = -y_frac;
               int32_t vi     = (int32_t)(V_sp      * 100);
               int32_t v_frac = vi % 100; if (v_frac < 0) v_frac = -v_frac;
               rt_kprintf("STRAFE R | yaw_err=%d.%02d Vsp=%d.%02d corr=%d LD=%u RD=%u\r\n",
                          yi/100, y_frac,
                          vi/100, v_frac,
                          corr, rpm_ld, rpm_rd);
           }
       }
    return RT_EOK;
}

/* 左平移指令 —— 四轮驱动，双环 PID */
rt_err_t motion_strafe_left(uint16_t rpm_base)
{
    // 1) 外环：航向 PID 输出期望对角差速 V_sp
    float V_sp = PID_Inc_Compute(&straight_pid, g_yaw_angle, CTRL_PERIOD_S);
    V_sp = fmaxf(fminf(V_sp,  50.0f), -50.0f);

    // 2) 读取实际对角速度（LF+RB vs RF+LB）
    float left_diag  = ((float)Curent_Speed[0] + Curent_Speed[3]) * 0.5f;
    float right_diag = ((float)Curent_Speed[1] + Curent_Speed[2]) * 0.5f;

    // 3) 计算速度差误差
    float speed_error = (left_diag - right_diag) - V_sp;

    // 4) 内环：速度差 PID 求修正 corr
    float du = PID_Inc_Compute(&speed_pid, speed_error, CTRL_PERIOD_S);
    du = fmaxf(fminf(du, 20.0f), -20.0f);
    int16_t corr    = (int16_t)du;
    uint16_t delta  = corr >= 0 ? (uint16_t)corr : (uint16_t)(-corr);

    // 5) 计算两条对角线的最终转速
    uint16_t rpm_ld = (corr >= 0)
                      ? ((rpm_base > delta) ? rpm_base - delta : 0)
                      : (rpm_base + delta);
    uint16_t rpm_rd = (corr >= 0)
                      ? (rpm_base + delta)
                      : ((rpm_base > delta) ? rpm_base - delta : 0);

    // 6) 下发四轮平移命令并同步
    // 左平移：前左+后右（LF,RB）向后; 前右+后左（RF,LB）向前
    motor_cmd_speed(LF, CCW, rpm_ld, 0, RT_TRUE);
    motor_cmd_speed(RB, CCW, rpm_ld, 0, RT_TRUE);
    motor_cmd_speed(RF, CW,  rpm_rd, 0, RT_TRUE);
    motor_cmd_speed(LB, CW,  rpm_rd, 0, RT_TRUE);
    motor_cmd_sync(0x00);

    /* 7) 调试打印 —— 控制频率 */
       {
           static int _print_cnt = 0;
           const int PRINT_PERIOD = 5; // 每5次打印一次
           if (++_print_cnt >= PRINT_PERIOD)
           {
               _print_cnt = 0;
               float yaw_error = normalize_angle(target_yaw - g_yaw_angle);
               int32_t yi     = (int32_t)(yaw_error * 100);
               int32_t y_frac = yi % 100; if (y_frac < 0) y_frac = -y_frac;
               int32_t vi     = (int32_t)(V_sp      * 100);
               int32_t v_frac = vi % 100; if (v_frac < 0) v_frac = -v_frac;
               rt_kprintf("STRAFE L | yaw_err=%d.%02d Vsp=%d.%02d corr=%d LD=%u RD=%u\r\n",
                          yi/100, y_frac,
                          vi/100, v_frac,
                          corr, rpm_ld, rpm_rd);
           }
       }

    return RT_EOK;
}

/* 原地以右侧为轴顺时针旋转 90°，完成后置零陀螺仪 */
void motion_pivot_right_90(void)
{
    // 1) 计算角度误差 (target - current)，并归一到 ±180°
    float err = normalize_angle(pivot_target_yaw - g_yaw_angle);

    // 2) 如果误差足够小，就停、置零并回空闲
    if (fabsf(err) <= PIVOT_TOLERANCE)
    {
        motion_stop();
        gyro_cmd_zero();
        cur_state = ST_IDLE;
        return;
    }

    // 3) PID 输出：给出“需要的转速” u，正负对应方向
    float u = PID_Inc_Compute(&pivot_yaw_pid, g_yaw_angle, CTRL_PERIOD_S);
    // 限幅到 ±PIVOT_RPM
    if (u >  PIVOT_RPM) u =  PIVOT_RPM;
    if (u < -PIVOT_RPM) u = -PIVOT_RPM;
    uint16_t speed = (uint16_t)fabsf(u);

    // 4) 根据 u 的符号，驱动左侧轮
    //    - u<0 说明 current > target，需顺时针转（左侧轮 CW）
    //    - u>0 说明 current < target，需逆时针转（左侧轮 CCW）
    if (u < 0)
    {
        motor_cmd_speed(LF, CW,  speed, 0, RT_TRUE);
        motor_cmd_speed(LB, CW,  speed, 0, RT_TRUE);
    }
    else
    {
        motor_cmd_speed(LF, CCW, speed, 0, RT_TRUE);
        motor_cmd_speed(LB, CCW, speed, 0, RT_TRUE);
    }

    // 右侧两轮保持静止
    motor_cmd_stop(RF, RT_TRUE);
    motor_cmd_stop(RB, RT_TRUE);

    // 同步启动
    motor_cmd_sync(0x00);

    // 5) 等待下一个控制周期
    rt_thread_mdelay(CTRL_PERIOD_S);
}

/* 右转180度 */
void motion_pivot_right_180(void)
{
    // 1) 计算误差 target - current，并环绕到 ±180°
    float err = normalize_angle(pivot_target_yaw - g_yaw_angle);

    // 2) 误差在 ±3°内则完成
    if (fabsf(err) <= PIVOT_TOLERANCE)
    {
        motion_stop();
        gyro_cmd_zero();
        cur_state = ST_IDLE;
        return;
    }

    // 3) PID 输出速度 u
    float u = PID_Inc_Compute(&pivot_yaw_pid, g_yaw_angle, CTRL_PERIOD_S);
    if (u >  PIVOT_RPM) u =  PIVOT_RPM;
    if (u < -PIVOT_RPM) u = -PIVOT_RPM;
    uint16_t speed = (uint16_t)fabsf(u);

    // 4) 根据 u 符号驱动左侧两轮
    if (u < 0)
    {
        // 顺时针转（左轮正转）
        motor_cmd_speed(LF, CW,  speed, 0, RT_TRUE);
        motor_cmd_speed(LB, CW,  speed, 0, RT_TRUE);
    }
    else
    {
        // 逆时针微调（左轮反转）
        motor_cmd_speed(LF, CCW, speed, 0, RT_TRUE);
        motor_cmd_speed(LB, CCW, speed, 0, RT_TRUE);
    }
    // 右侧不动
    motor_cmd_stop(RF, RT_TRUE);
    motor_cmd_stop(RB, RT_TRUE);

    // 5) 同步启动并延时一个控制周期
    motor_cmd_sync(0x00);
    rt_thread_mdelay(PIVOT_DELAY_MS);
}

//停止指令
rt_err_t motion_stop(void)
{
    motor_cmd_stop(LF, RT_TRUE);
    motor_cmd_stop(RF, RT_TRUE);
    motor_cmd_stop(LB, RT_TRUE);
    motor_cmd_stop(RB, RT_TRUE);
    motor_cmd_sync(0x00);  // 广播 使得电机同步运转
    return RT_EOK;
}

/***************************** 42电机 ****************************/

/***************************** 57电机 ****************************/
/* 57电机 CCW是下 */

//57电机上升一层
rt_err_t elevator_up(uint16_t rpm)
{
    motor_cmd_position(Elevator,
                        CW,
                        rpm,
                        0,
                        5000,
                        Absolute,
                        RT_NULL);
    return RT_EOK;
}

//57电机下降一层
rt_err_t elevator_down(uint16_t rpm)
{
    motor_cmd_position(Elevator,
                        CCW,
                        rpm,
                        0,
                        5000,
                        Absolute,
                        RT_NULL);
    return RT_EOK;
}

//57电机停止指令
rt_err_t elevator_stop(void)
{
    motor_cmd_stop(Elevator, RT_NULL);
    return RT_EOK;
}
/***************************** 57电机 ****************************/
