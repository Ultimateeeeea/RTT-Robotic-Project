//#include "task_state.h"
//
//volatile task_state_t g_task_state = TASK_SHELF2_GRAB;      //全局变量，记录目前执行的任务
//
//void task_state_update(rt_uint32_t both_cnt)
//{
//    if (both_cnt < 5U)  /* 用5U统一数据类型 */
//    {
//        g_task_state = TASK_SHELF2_GRAB;
//    }
//    else
//    {
//        g_task_state = TASK_GO_HOME;        /* 先留空，后续你可以换成别的任务 */
//    }
//}
