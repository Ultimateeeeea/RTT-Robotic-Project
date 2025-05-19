#ifndef __PID_H_
#define __PID_H_

#include <stdint.h>
#include <stdbool.h>

/* 增量式 PID 结构体 */
typedef struct {
    float kp;                /* 比例系数 */
    float ki;                /* 积分系数 */
    float kd;                /* 微分系数 */
    float prev_error;        /* e(k-1) */
    float prev_prev_error;   /* e(k-2) */
    float prev_output;       /* u(k-1) */
    float target;            /* 目标航向（°） */
} PID_Inc_t;

/**
 * @brief  初始化增量式 PID
 * @param  pid     指向 PID_Inc_t 的指针
 * @param  kp      比例系数
 * @param  ki      积分系数
 * @param  kd      微分系数
 * @param  target  初始目标航向（°）
 */
void PID_Inc_Init(PID_Inc_t *pid, float kp, float ki, float kd, float target);

/**
 * @brief  更新目标航向
 * @param  pid     指向 PID_Inc_t 的指针
 * @param  target  新的目标航向（°）
 */
void PID_Inc_SetTarget(PID_Inc_t *pid, float target);

/**
 * @brief  计算增量式 PID（基于航向）
 * @param  pid      指向 PID_Inc_t 的指针
 * @param  current  当前航向（°）
 * @param  dt       取样周期 T，单位秒
 * @return          当前控制量 u(k)
 */
float PID_Inc_Compute(PID_Inc_t *pid, float current, float dt);

#endif /* __PID_H_ */
