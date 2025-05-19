#include "gyro/Inc/sensor_port.h"

#define GYRO_UART_NAME  "uart2"
volatile rt_device_t gyro_uart = RT_NULL;

const uint32_t c_uiBaud[10] = {0, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
volatile char s_cDataUpdate = 0;     // 不要 static
volatile char s_cCmd = 0xff;         // 同理

volatile float g_yaw_angle = 0.0f;  //Z轴角度

static void Delayms(uint16_t ucMs)//延时程序
{
    rt_thread_delay(ucMs);
}


static void ShowHelp(void)//上电提示并且打印下面内容
{
    rt_kprintf("\r\n************************     WIT_SDK_DEMO   ************************");
    rt_kprintf("\r\n************************          HELP           ************************\r\n");
    rt_kprintf("UART SEND:K\\r\\n   Z-axis zero setting.\r\n");  //只对101Z轴置零起作用
    //101获取零偏过程中，不要动设备，然后等待20秒左右，再发送退出校准指令P这个
    rt_kprintf("UART SEND:A\\r\\n   AUTO setting.\r\n");  //只对101自动获取零偏起作用
    rt_kprintf("UART SEND:P\\r\\n   AUTO setting.\r\n");  //只对101结束自动获取零偏起作用

    rt_kprintf("UART SEND:h\\r\\n   help.\r\n");
    rt_kprintf("******************************************************************************\r\n");
}

/* 打开陀螺仪串口uart2 */
rt_err_t gyro_uart_init(const char *uart_name)
{
    gyro_uart = rt_device_find(uart_name);
    if (!gyro_uart)
    {
        rt_kprintf("Error: cannot find %s\n", uart_name);
        return -RT_ERROR;
    }
    rt_err_t err = rt_device_open(gyro_uart,
                        RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_DMA_RX);
    if (err != RT_EOK)
    {
        rt_kprintf("Error: open %s failed: %d\n", uart_name, err);
        return err;
    }
    rt_kprintf("%s opened\n", uart_name);
    return RT_EOK;
}

/* 手动归零指令 */
rt_err_t gyro_cmd_zero(void)
{
    if (WitStartIYAWCali() != WIT_HAL_OK)
    {
        rt_kprintf("⚠️ Yaw-zero command failed\n");
        return -RT_ERROR;
    }
    rt_kprintf("✅ Yaw reset requested\n");
    return RT_EOK;
}

/* 读取陀螺仪的Yaw值这里的接收方式是非阻塞 */
void uart2_recv_thread(void *parameter)
{
    char buf[64];
    rt_size_t recv_len;

    RT_ASSERT(gyro_uart);

    while (1)
    {
        recv_len = rt_device_read(gyro_uart, 0, buf, sizeof(buf));  //这里的rt_device_read是怎么读取到的？DMA？

        if (recv_len > 0)
        {
            for (int i = 0; i < recv_len; i++)
                WitSerialDataIn((uint8_t)buf[i]);
            // 不再把 buf 当作字符串打印
        }

        rt_thread_mdelay(10);

    }
}

static void Uart2Send(const uint8_t *buf, size_t len)
{

    // 检查 gyro_uart 是否有效
    if (gyro_uart == RT_NULL)
    {
        rt_kprintf("gyro_uart device is NULL! can't send!\n");
        return;
    }

    rt_device_write(gyro_uart, 0, buf, len);
    rt_thread_mdelay(2);    /* 等 2ms发送数据 */
}

static void SensorUartSend(uint8_t *p_data, uint32_t uiSize)//传感器串口发送
{
    Uart2Send(p_data, uiSize);//串口2发送
}

static void AutoScanSensor(void)//串口波特率检测
{
    int i, iRetry;
    for(i = 1; i < 10; i++)//for循环待i不符合条件会跳出循环
    {
        iRetry = 2;
        do//do-while()循环语句先执行在判断
        {
            s_cDataUpdate = 0;
            WitReadReg(AX, 3);//给函数进行导入形参
            rt_thread_delay(100);//延时100ms
            if(s_cDataUpdate != 0)//判断s_cDataUpdate不等于0,如果是不等于0就执行括号里面的内容
            {
                rt_kprintf("%d baud find sensor\r\n\r\n", c_uiBaud[i]);//打印找到传感器和打印对应的波特率
                ShowHelp();//执行函数打印内容
                return ;
            }
            iRetry--;//变量自减
        }while(iRetry);//while不为0就一直执行循环的内容
    }
    rt_kprintf("can not find sensor\r\n");//如果上面没有找到传感器就会执行下面这两句，如果找到就不会执行这两句
    rt_kprintf("please check your connection\r\n");//
}

static void SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum)//传感器数据升级
{
    // 每收到一段寄存器数据，uiReg 会告诉你是哪种数据：
    //   AZ→加速度  GZ→陀螺角速  Yaw→角度  HZ→磁力
    int i;
    for(i = 0; i < uiRegNum; i++)
    {
        switch(uiReg)//判断uiReg的数据是什么来进行选择对应的操作
        {
//            case AX:
//            case AY:
            case AZ:
                s_cDataUpdate |= ACC_UPDATE;//s_cDataUpdate变量和ACC_UPDATE变量或运算后的结果再把结果赋值给s_cDataUpdate，如s_cDataUpdate=s_cDataUpdate|和ACC_UPDATE
            break;
//            case GX:
//            case GY:
            case GZ:
                s_cDataUpdate |= GYRO_UPDATE;
            break;
//            case HX:
//            case HY:
            case HZ:
                s_cDataUpdate |= MAG_UPDATE;
            break;
//            case Roll:
//            case Pitch:
            case Yaw:
                s_cDataUpdate |= ANGLE_UPDATE;
            break;
            default:
                s_cDataUpdate |= READ_UPDATE;
            break;
        }
        uiReg++;
    }
}

int sensor_port_init(void)
{
    if (gyro_uart_init(GYRO_UART_NAME) != RT_EOK) {
           rt_kprintf("Fatal: gyro UART init failed, halting sensor thread\n");
           return -RT_ERROR;
       }

    WitInit(WIT_PROTOCOL_NORMAL, 0x50);//初始化标准协议，设置设备地址
    WitSerialWriteRegister(SensorUartSend);//注册写回调函数    串口1接收数据调用 SensorUartSend函数
    WitRegisterCallBack(SensorDataUpdata);//注册获取传感器数据回调函数   串口2接收数据调用SensorDataUpdata函数
    WitDelayMsRegister(Delayms);//注册延时回调函数
    AutoScanSensor();//自动搜索传感器，如果线没有插对或者使用的串口工具不对会搜索不到传感器

    return 0;
}

int uart2_recv_init(void)
{
    gyro_uart_init(GYRO_UART_NAME);
    rt_thread_t tid = rt_thread_create("uart2_rd",
                                       uart2_recv_thread,
                                       RT_NULL,
                                       1024,
                                       15,
                                       10);
    if (tid) rt_thread_startup(tid);
    return tid ? RT_EOK : -RT_ERROR;
}

INIT_APP_EXPORT(uart2_recv_init);

