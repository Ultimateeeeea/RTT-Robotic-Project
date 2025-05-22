/* motor_state.c */
#include "motor/Inc/motor_state.h"
#include "gyro/Inc/sensor_port.h"
#include <PID/Inc/pid.h>
#include <math.h>

/* 右旋转90度用 */
static PID_Inc_t pivot_yaw_pid;   // 掉头用的航向 PID
static float  pivot_target_yaw = 0.0f;
static const float PIVOT_TOLERANCE = 2.0f;    // ±2° 容差
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

/* 在文件顶部或合适的作用域定义可调参数 */
volatile float g_pivot_kp = 2.0f;
volatile float g_pivot_ki = 0.05f;
volatile float g_pivot_kd = 0.0f;

volatile float g_straight_kp = -2.0f;
volatile float g_straight_ki = 0.0f;
volatile float g_straight_kd = -0.2f;

volatile float g_strafe_kp = -1.5f;
volatile float g_strafe_ki = -0.05f;
volatile float g_strafe_kd = -0.2f;

volatile float g_speed_kp = -1.0f;
volatile float g_speed_ki = 0.0f;
volatile float g_speed_kd = 0.0f;

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
    || st == ST_BACKWARD)
   && cur_state != st)
  {
      target_yaw = g_yaw_angle;
      PID_Inc_Init(&straight_pid, g_straight_kp, g_straight_ki, g_straight_kd, target_yaw);    /* 角度外环 for straight movement */
      PID_Inc_Init(&speed_pid,    g_speed_kp,    g_speed_ki,    g_speed_kd,    0.0f);         /* 速度内环 */
  }
  else if ((st == ST_LEFT
           || st == ST_RIGHT)
          && cur_state != st)
  {
      target_yaw = g_yaw_angle;
      PID_Inc_Init(&straight_pid, g_strafe_kp, g_strafe_ki, g_strafe_kd, target_yaw);    /* 角度外环 for strafing */
      PID_Inc_Init(&speed_pid,    g_speed_kp,    g_speed_ki,    g_speed_kd,    0.0f);         /* 速度内环 (can also be made strafe-specific if needed) */
  }

  if (st == ST_PIVOT_RIGHT90 && cur_state != ST_PIVOT_RIGHT90)
     {
         // 1) 先计算目标航向：向右（顺时针）转 90°
         pivot_target_yaw = normalize_angle(g_yaw_angle - 90.0f);

         // 2) 用目标航向初始化 PID（只清历史一次）
         PID_Inc_Init(&pivot_yaw_pid,
                      g_pivot_kp, // 使用可调参数
                      g_pivot_ki, // 使用可调参数
                      g_pivot_kd, // 使用可调参数
                      pivot_target_yaw);
     }

  else if (st == ST_PIVOT_RIGHT180 && cur_state != ST_PIVOT_RIGHT180)
  {
      pivot_target_yaw = normalize_angle(g_yaw_angle + 180.0f);
      PID_Inc_Init(&pivot_yaw_pid,
                   g_pivot_kp, // 使用可调参数
                   g_pivot_ki, // 使用可调参数
                   g_pivot_kd, // 使用可调参数
                  pivot_target_yaw);
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

motor_state_t motor_get_current_state(void)
      {
          return cur_state; // cur_state 是电机状态机中的当前状态变量
      }

/***************************** 42电机 ****************************/
/* 前进指令 —— 四轮驱动，双环 PID */
rt_err_t motion_forward(uint16_t rpm_base)
{
  /* 1) 航向 PID 输出期望差速 V_sp */
  float V_sp = PID_Inc_Compute(&straight_pid, g_yaw_angle, CTRL_PERIOD_S); 
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

/* 左平移指令 —— 四轮驱动，双环 PID */
rt_err_t motion_strafe_left(uint16_t rpm_base)
{
  /* 1) 外环：航向 PID 输出期望对角差速 V_sp */
  float V_sp = PID_Inc_Compute(&straight_pid, g_yaw_angle, CTRL_PERIOD_S);
  V_sp = fmaxf(fminf(V_sp,  50.0f), -50.0f);

  /* 2) 读取实际对角速度（LF+RB vs RF+LB） */
  float left_diag  = ((float)Curent_Speed[0] + Curent_Speed[3]) * 0.5f; // LF + RB
  float right_diag = ((float)Curent_Speed[1] + Curent_Speed[2]) * 0.5f; // RF + LB

  /* 3) 计算速度差误差 */
  float speed_error = (left_diag - right_diag) - V_sp;

  /* 4) 内环：速度差 PID 求修正 corr */
  float du = PID_Inc_Compute(&speed_pid, speed_error, CTRL_PERIOD_S);
  du = fmaxf(fminf(du, 30.0f), -30.0f);
  int16_t corr = (int16_t)du;
  uint16_t delta = corr >= 0 ? (uint16_t)corr : (uint16_t)(-corr);

  /* 5) 计算两条对角线的最终转速 */
  // corr > 0: left_diag 减速, right_diag 加速
  // corr < 0: left_diag 加速, right_diag 减速
  uint16_t rpm_ld = (corr >= 0) // Target for LF, RB
                    ? ((rpm_base > delta) ? rpm_base - delta : 0)
                    : (rpm_base + delta);
  uint16_t rpm_rd = (corr >= 0) // Target for RF, LB
                    ? (rpm_base + delta)
                    : ((rpm_base > delta) ? rpm_base - delta : 0);

  /* 6) 下发四轮平移命令并同步 */
  // 左平移期望: LF(向后), RF(向前), LB(向前), RB(向后)
  // 根据用户定义: 左侧CW向前,右侧CW向后
  // LF (左侧) 向后 -> CCW
  // RF (右侧) 向前 -> CCW
  // LB (左侧) 向前 -> CW
  // RB (右侧) 向后 -> CW
  motor_cmd_speed(LF, CCW, rpm_ld, 0, RT_TRUE);
  motor_cmd_speed(RB, CW,  rpm_ld, 0, RT_TRUE);
  motor_cmd_speed(RF, CCW, rpm_rd, 0, RT_TRUE);
  motor_cmd_speed(LB, CW,  rpm_rd, 0, RT_TRUE);

  motor_cmd_sync(0x00);

  /* 7) 调试打印 —— 控制频率 */
     {
         static int _print_cnt = 0;
         const int PRINT_PERIOD = 5; // 每5次打印一次 (之前是25, 过于稀疏)
         if (++_print_cnt >= PRINT_PERIOD)
         {
             _print_cnt = 0;
             float yaw_error = normalize_angle(target_yaw - g_yaw_angle);
             int32_t yi     = (int32_t)(yaw_error * 100);
             int32_t y_frac = yi % 100; if (y_frac < 0) y_frac = -y_frac;
             int32_t vi     = (int32_t)(V_sp      * 100);
             int32_t v_frac = vi % 100; if (v_frac < 0) v_frac = -v_frac;
             rt_kprintf("STRAFE L | yaw_err=%d.%02d Vsp=%d.%02d corr=%d LD_rpm=%u RD_rpm=%u\r\n",
                        yi/100, y_frac,
                        vi/100, v_frac,
                        corr, rpm_ld, rpm_rd);
         }
     }

  return RT_EOK;
}

/* 右平移指令 —— 四轮驱动，双环 PID */
rt_err_t motion_strafe_right(uint16_t rpm_base)
{
  /* 1) 外环：航向 PID 输出期望对角差速 V_sp */
  float V_sp = PID_Inc_Compute(&straight_pid, g_yaw_angle, CTRL_PERIOD_S);
  V_sp = fmaxf(fminf(V_sp,  50.0f), -50.0f);

  /* 2) 读取实际对角速度（单位同 rpm） */
  float left_diag  = ((float)Curent_Speed[0] + Curent_Speed[3]) * 0.5f; // LF + RB
  float right_diag = ((float)Curent_Speed[1] + Curent_Speed[2]) * 0.5f; // RF + LB

  /* 3) 内环：速度差误差 */
  // 对于右平移，V_sp > 0 意味着希望车头向右偏 (顺时针)
  // (right_diag - left_diag) 是实际的右偏趋势速度
  float speed_error = (right_diag - left_diag) - V_sp;

  /* 4) 速度差 PID 输出修正 corr */
  float du = PID_Inc_Compute(&speed_pid, speed_error, CTRL_PERIOD_S);
  du = fmaxf(fminf(du,  30.0f), -30.0f);
  int16_t corr = (int16_t)du;
  uint16_t delta = (corr >= 0) ? (uint16_t)corr : (uint16_t)(-corr);

  /* 5) 计算两对对角线最终转速 */
  // corr > 0: right_diag (RF+LB) 加速, left_diag (LF+RB) 减速
  // corr < 0: right_diag (RF+LB) 减速, left_diag (LF+RB) 加速
  uint16_t rpm_ld = (corr >= 0) // Target for LF, RB (left_diag)
                    ? ((rpm_base > delta) ? rpm_base - delta : 0)
                    : (rpm_base + delta);
  uint16_t rpm_rd = (corr >= 0) // Target for RF, LB (right_diag)
                    ? (rpm_base + delta)
                    : ((rpm_base > delta) ? rpm_base - delta : 0);

  /* 6) 下发四轮平移命令并同步启动 */
  // 右平移期望: LF(向前), RF(向后), LB(向后), RB(向前)
  // 根据用户定义: 左侧CW向前,右侧CW向后
  // LF (左侧) 向前 -> CW
  // RF (右侧) 向后 -> CW
  // LB (左侧) 向后 -> CCW
  // RB (右侧) 向前 -> CCW
  motor_cmd_speed(LF, CW,  rpm_ld, 0, RT_TRUE); // LF,RB 使用 rpm_ld
  motor_cmd_speed(RB, CCW, rpm_ld, 0, RT_TRUE); 
  motor_cmd_speed(RF, CW,  rpm_rd, 0, RT_TRUE); // RF,LB 使用 rpm_rd
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
             rt_kprintf("STRAFE R | yaw_err=%d.%02d Vsp=%d.%02d corr=%d LD_rpm=%u RD_rpm=%u\r\n",
                        yi/100, y_frac,
                        vi/100, v_frac,
                        corr, rpm_ld, rpm_rd);
         }
     }
  return RT_EOK;
}

/* 停止电机转动 */
rt_err_t motion_stop(void)
{
  /* 停止四个电机 */
  motor_cmd_stop(LF, RT_TRUE);
  motor_cmd_stop(RF, RT_TRUE);
  motor_cmd_stop(LB, RT_TRUE);
  motor_cmd_stop(RB, RT_TRUE);
  /* 同步发命令 */
  motor_cmd_sync(0x00);
  return RT_EOK;
}



/* 原地以右侧为轴顺时针旋转 90°，完成后置零陀螺仪 */
/* 右转 90° —— 利用麦克纳姆轮四轮驱动 */
void motion_pivot_right_90(void)
{
  /* 1) 误差环绕到 ±180° */
  float err = normalize_angle(pivot_target_yaw - g_yaw_angle);

  /* 2) 收敛后停止 */
  if (fabsf(err) <= PIVOT_TOLERANCE)
  {
      motion_stop();
      cur_state = ST_IDLE;
      return;
  }

  /* 3) PID 增量输出 */
  float u = PID_Inc_Compute(&pivot_yaw_pid, g_yaw_angle, CTRL_PERIOD_S);
  /* 4) 限幅 */
  if (u >  PIVOT_RPM) u =  PIVOT_RPM;
  if (u < -PIVOT_RPM) u = -PIVOT_RPM;
  uint16_t speed = (uint16_t)fabsf(u);

  /* 5) 根据符号给出全四轮纯旋转指令 */
  if (u < 0)
  {
      /* u<0：需要逆时针（机器人右转） */
      /* 左侧轮子 CCW，右侧轮子 CW */
      motor_cmd_speed(LF, CCW, speed, 0, RT_TRUE);
      motor_cmd_speed(LB, CCW, speed, 0, RT_TRUE);
      motor_cmd_speed(RF, CCW,  speed, 0, RT_TRUE);
      motor_cmd_speed(RB, CCW,  speed, 0, RT_TRUE);
  }
  else
  {
      /* u>0：需要顺时针（机器人左转） */
      motor_cmd_speed(LF, CW,  speed, 0, RT_TRUE);
      motor_cmd_speed(LB, CW,  speed, 0, RT_TRUE);
      motor_cmd_speed(RF, CW, speed, 0, RT_TRUE);
      motor_cmd_speed(RB, CW, speed, 0, RT_TRUE);
  }

  /* 6) 同步并延时 */
  motor_cmd_sync(0x00);
  rt_thread_mdelay(PIVOT_DELAY_MS);
}

/* 右转180度 */
/**
* 在原地顺时针（右转向）旋转 180°：
*   利用麦克纳姆轮全四轮驱动：
*   - 顺时针旋转：左侧轮 CW，右侧轮 CCW
*   - 逆时针旋转：左侧轮 CCW，右侧轮 CW
*/
/* 在进入前进状态时，记录当前航向，并重置 PID */
void motion_pivot_right_180(void)
{
  /* 1) 误差环绕 */
  float err = normalize_angle(pivot_target_yaw - g_yaw_angle);

  /* 2) 收敛后停止 */
  if (fabsf(err) <= PIVOT_TOLERANCE)
  {
      motion_stop();
      cur_state = ST_IDLE;
      return;
  }

  /* 3) PID 计算 */
  float u = PID_Inc_Compute(&pivot_yaw_pid, g_yaw_angle, CTRL_PERIOD_S);
  u = fmaxf(fminf(u,  PIVOT_RPM), -PIVOT_RPM);
  uint16_t speed = (uint16_t)fabsf(u);

  /* 4) 全轮 CW → 物理顺时针 */
  motor_cmd_speed(LF, CW,  speed, 0, RT_TRUE);
  motor_cmd_speed(LB, CW,  speed, 0, RT_TRUE);
  motor_cmd_speed(RF, CW,  speed, 0, RT_TRUE);
  motor_cmd_speed(RB, CW,  speed, 0, RT_TRUE);

  /* 5) 同步并延时 */
  motor_cmd_sync(0x00);
  rt_thread_mdelay(PIVOT_DELAY_MS);
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
                      145000,   /* 绝对位置,养乐多可能够不到 */
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
                      0,
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
