#include "main.h"


static uint8_t uart2_rx_con=0;       //接收计数器
static uint8_t uart2_rx_checksum;    //帧头部分校验和
static uint8_t uart2_rx_buf[40];     //接收缓冲，数据内容小于等于32Byte
static uint8_t uart2_tx_buf[40];     //发送缓冲

uint8_t ros_servo_data=0;



/**
  * @简  述  UART   串口初始化
  * @参  数  baud： 波特率设置
  * @返回值	 无
  */
	
//用户接口，也可直接接树莓派的GPIO
void UART2_Init(uint32_t baud)
{

	GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;


	/* 串口USART配置 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//USART对应引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); 

	//USART 端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOD,&GPIO_InitStructure); 

	//USART参数配置
	USART_InitStructure.USART_BaudRate = baud;    //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	//USART使能
	USART_Cmd(USART2, ENABLE); 
	
	//开启串口接收中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断

    //USART2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器	
	
}

/**
  * @简  述  UART 串口中断服务函数
  * @参  数  无 
  * @返回值  无
  */
void USART2_IRQHandler(void)
{
	uint8_t Res;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res =USART_ReceiveData(USART2);	
		
		if(uart2_rx_con < 3)    //==接收帧头 + 长度
		{
			if(uart2_rx_con == 0)  //接收帧头1 0xAA
			{
				if(Res == 0xAA)
				{
					uart2_rx_buf[0] = Res;
					uart2_rx_con = 1;					
				}
				else
				{
					
				}
			}else if(uart2_rx_con == 1) //接收帧头2 0x55
			{
				if(Res == 0x55)
				{
					uart2_rx_buf[1] = Res;
					uart2_rx_con = 2;
				}
				else
				{
					uart2_rx_con = 0;						
				}				
			}
			else  //接收数据长度
			{
				uart2_rx_buf[2] = Res;
				uart2_rx_con = 3;
				uart2_rx_checksum = (0xAA+0x55) + Res;	//计算校验和
			}
		}
		else    //==接收数据
		{
			if(uart2_rx_con < (uart2_rx_buf[2]-1) )
			{
				uart2_rx_buf[uart2_rx_con] = Res;
				uart2_rx_con++;
				uart2_rx_checksum = uart2_rx_checksum + Res;					
			}
			else    //判断最后1位
			{
				//接收完成，恢复初始状态
				uart2_rx_con = 0;	
				printf("%s \r\n",uart2_rx_buf);
				//数据校验
				if( Res == uart2_rx_checksum )  //校验正确
				{	
					//速度控制帧
					if(uart2_rx_buf[3] == ID_ROS2STM_VEL)
					{
						Vel.TG_IX = (int16_t)((uart2_rx_buf[4]<<8) | uart2_rx_buf[5]);
						Vel.TG_IY = (int16_t)((uart2_rx_buf[6]<<8) | uart2_rx_buf[7]);
						Vel.TG_IW = (int16_t)((uart2_rx_buf[8]<<8) | uart2_rx_buf[9]);
					}
					else
					{
						//IMU陀螺仪校准
						if(uart2_rx_buf[3] == ID_ROS2STM_IMU)
						{
							imu_calibrate_flag = uart2_rx_buf[4];
						}	

						//机械臂控制帧
						else if(uart2_rx_buf[3] == ID_ROS2STM_HAND)
						{
							ros_servo_data=1;
							arm_angle[5] = (int16_t)((uart2_rx_buf[4]<<8) | uart2_rx_buf[5]);
							ros_servo.pwm[5] = 1500-arm_angle[5]/2.356;
							ros_servo.time[5] = 50;
							//printf("A0%d A1%d A2%d A3%d A4%d A5%d \r\n ",ros_servo.angle[0],ros_servo.angle[1], ros_servo.angle[2],ros_servo.angle[3],ros_servo.angle[4],ros_servo.angle[5]  );

						}
												

						//机械臂控制帧
						else if(uart2_rx_buf[3] == ID_ROS2STM_ARM)
						{

							ros_servo_data=1;
							
							arm_angle[0] =(int16_t)((uart2_rx_buf[4]<<8) | uart2_rx_buf[5]);
							arm_angle[1] =(int16_t)((uart2_rx_buf[6]<<8) | uart2_rx_buf[7]);
							arm_angle[2] = (int16_t)((uart2_rx_buf[8]<<8) | uart2_rx_buf[9]);
							arm_angle[3] = (int16_t)((uart2_rx_buf[10]<<8) | uart2_rx_buf[11]);
							arm_angle[4] = (int16_t)((uart2_rx_buf[12]<<8) | uart2_rx_buf[13]);
							
							ros_servo.pwm[0] = 1500-arm_angle[0]/2.356;
							ros_servo.pwm[1] = 1500-arm_angle[1]/2.356;
							ros_servo.pwm[2] = 1500+arm_angle[2]/2.356;
							ros_servo.pwm[3] = 1500-arm_angle[3]/2.356;
							ros_servo.pwm[4] = 1500-arm_angle[4]/2.356;
							//ros_servo.pwm[5] = (int16_t)((uart2_rx_buf[14]<<8) | uart2_rx_buf[15]);
							

							ros_servo.time[0] = 50;
							ros_servo.time[1] = 50;
							ros_servo.time[2] = 50;
							ros_servo.time[3] = 50;
							ros_servo.time[4] = 50;

							
								
						}
						//键盘控制机械臂控制帧
						else if(uart2_rx_buf[3] == ID_ROS2STM_KEY)
						{

							ros_servo_data=1;
							
							arm_angle[0] =(int16_t)((uart2_rx_buf[4]<<8) | uart2_rx_buf[5]);
							arm_angle[1] = (int16_t)((uart2_rx_buf[6]<<8) | uart2_rx_buf[7]);
							arm_angle[2] = (int16_t)((uart2_rx_buf[8]<<8) | uart2_rx_buf[9]);
							arm_angle[3] = (int16_t)((uart2_rx_buf[10]<<8) | uart2_rx_buf[11]);
							arm_angle[4] = (int16_t)((uart2_rx_buf[12]<<8) | uart2_rx_buf[13]);
							arm_angle[5] = (int16_t)((uart2_rx_buf[14]<<8) | uart2_rx_buf[15]);
							ros_servo.pwm[0] = 1500-arm_angle[0]/2.356;
							ros_servo.pwm[1] = 1500-arm_angle[1]/2.356;
							ros_servo.pwm[2] = 1500+arm_angle[2]/2.356;
							ros_servo.pwm[3] = 1500-arm_angle[3]/2.356;
							ros_servo.pwm[4] = 1500-arm_angle[4]/2.356;
							ros_servo.pwm[5] = 1500-arm_angle[5]/2.356;
							//ros_servo.pwm[5] = (int16_t)((uart2_rx_buf[14]<<8) | uart2_rx_buf[15]);
							if(ros_servo.pwm[0]==1500&&ros_servo.pwm[1]==1500&&ros_servo.pwm[2]==1500&&ros_servo.pwm[3]==1500&&ros_servo.pwm[4]==1500&&ros_servo.pwm[5]==1500)
							{
								ros_servo.time[0] = 1500;
								ros_servo.time[1] = 1500;
								ros_servo.time[2] = 1500;
								ros_servo.time[3] = 1500;
								ros_servo.time[4] = 1500;
								ros_servo.time[5] = 1500;
							}
							else
							{
								ros_servo.time[0] = 50;
								ros_servo.time[1] = 50;
								ros_servo.time[2] = 50;
								ros_servo.time[3] = 50;
								ros_servo.time[4] = 50;
								ros_servo.time[5] = 50;
							}								
						}
						//机械臂逆运动学控制帧
						else if(uart2_rx_buf[3] == ID_ROS2STM_IK)
						{

							ros_servo_data=1;
							int arm_time=0;

							arm_angle[0] = (int16_t)((uart2_rx_buf[4]<<8) | uart2_rx_buf[5]);
							arm_angle[1] = (int16_t)((uart2_rx_buf[6]<<8) | uart2_rx_buf[7]);
							arm_angle[2] = (int16_t)((uart2_rx_buf[8]<<8) | uart2_rx_buf[9]);
							arm_angle[3] = (int16_t)((uart2_rx_buf[10]<<8) | uart2_rx_buf[11]);
							arm_angle[4] = (int16_t)((uart2_rx_buf[12]<<8) | uart2_rx_buf[13]);
							arm_angle[5] = (int16_t)((uart2_rx_buf[14]<<8) | uart2_rx_buf[15]);
							arm_time = (int)((uart2_rx_buf[16]<<8) | uart2_rx_buf[17]);
							ros_servo.pwm[0] = 1500-arm_angle[0]/2.356;
							ros_servo.pwm[1] = 1500-arm_angle[1]/2.356;
							ros_servo.pwm[2] = 1500+arm_angle[2]/2.356;
							ros_servo.pwm[3] = 1500-arm_angle[3]/2.356;
							ros_servo.pwm[4] = 1500-arm_angle[4]/2.356;
							ros_servo.pwm[5] = 1500-arm_angle[5]/2.356;


							ros_servo.time[0] = arm_time;
							ros_servo.time[1] = arm_time;
							ros_servo.time[2] = arm_time;
							ros_servo.time[3] = arm_time;
							ros_servo.time[4] = arm_time;
							ros_servo.time[5] = arm_time;
															
															
						}
						else if(uart2_rx_buf[3] == ID_ROS2STM_BEEP)
						{
							times = (int16_t)((uart2_rx_buf[4]<<8) | uart2_rx_buf[5]);
							on_time = (int16_t)((uart2_rx_buf[6]<<8) | uart2_rx_buf[7]);
							off_time = (int16_t)((uart2_rx_buf[8]<<8) | uart2_rx_buf[9]);
						}	
						//机械臂和底盘复位
						else if(uart2_rx_buf[3] == ID_ROS2STM_RESET)
						{
							if(uart2_rx_buf[4] == 100)
							{
								ros_servo_data=1;								
								ros_servo.pwm[0]=1500;
								ros_servo.pwm[1]=1839;
								ros_servo.pwm[2]=2490;
								ros_servo.pwm[3]=1160;
								ros_servo.pwm[4]=1500;
								ros_servo.pwm[5]=1500;
								ros_servo.time[0] = 1500;
								ros_servo.time[1] = 1500;
								ros_servo.time[2] = 1500;
								ros_servo.time[3] = 1500;
								ros_servo.time[4] = 1500;
								ros_servo.time[5] = 1500;
								Vel.TG_IX = (int16_t)(0);
								Vel.TG_IY = (int16_t)(0);
								Vel.TG_IW = (int16_t)(0);
						  }
						}						
					}
				}
			}
		}
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	} 
}


/**
  * @简  述  UART 发送数据（X-Protocol协议）
  * @参  数  *pbuf：发送数据指针
  *          len：发送数据长度个数，≤27 (32-5)
  *          num：帧号，帧编码
  * @返回值	 无
  */
void UART2_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num)
{
	uint8_t i,cnt;	
    uint8_t tx_checksum = 0;//发送校验和
	
	if(len <= 39)
	{
		/******获取数据******/
		uart2_tx_buf[0] = 0xAA;    //帧头
		uart2_tx_buf[1] = 0x55;    //
		for(i=0; i<len; i++)
		{
			uart2_tx_buf[2+i] = *(pbuf+i);
		}
		
		/******计算校验和******/	
		cnt = 2+len;
		for(i=0; i<cnt; i++)
		{
			tx_checksum = tx_checksum + uart2_tx_buf[i];
		}
		uart2_tx_buf[i] = tx_checksum;
		uart2_tx_buf[35] = 0x7D;    //帧?
		
		/******发送数据******/	
		cnt = 4+len;
		
		//查询传输方式
		for(i=0; i<cnt; i++)
		{
			USART_SendData(USART2, uart2_tx_buf[i]);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);
		}
	}
}

