#include <PID/Inc/pid.h>
#include <math.h>

/**
 * @brief  初始化增量式 PID
 */
void PID_Inc_Init(PID_Inc_t *pid, float kp, float ki, float kd, float target)
{
    pid->kp              = kp;
    pid->ki              = ki;
    pid->kd              = kd;
    pid->prev_error      = 0.0f;
    pid->prev_prev_error = 0.0f;
    pid->prev_output     = 0.0f;
    pid->target          = target;
}

/**
 * @brief  更新目标航向
 */
void PID_Inc_SetTarget(PID_Inc_t *pid, float target)
{
    pid->target = target;
    /* 也可清除历史误差/输出，视需求而定：
    pid->prev_error = pid->prev_prev_error = 0.0f;
    pid->prev_output = 0.0f;
    */
}

/**
 * @brief  计算增量式 PID（航向环绕在 ±180° 以内）
 */
float PID_Inc_Compute(PID_Inc_t *pid, float current, float dt)
{
    /* 1. 计算误差并做 ±180° 环绕 */
    float error = current - pid->target;
    if (error > 180.0f)  error -= 360.0f;
    if (error < -180.0f) error += 360.0f;

    /* 2. 计算增量项 */
    float delta_p = pid->kp * (error - pid->prev_error);
    float delta_i = pid->ki * dt * error;
    float delta_d = 0.0f;
    if (dt > 0.0f) {
        delta_d = pid->kd * (error - 2.0f * pid->prev_error + pid->prev_prev_error) / dt;
    }

    float delta_u = delta_p + delta_i + delta_d;

    /* 3. 累加输出 */
    float output = pid->prev_output + delta_u;

    /* 4. 更新历史状态 */
    pid->prev_prev_error = pid->prev_error;
    pid->prev_error      = error;
    pid->prev_output     = output;

    return output;
}
