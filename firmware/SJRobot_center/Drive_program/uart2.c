#include "main.h"


static uint8_t uart2_rx_con=0;       //���ռ�����
static uint8_t uart2_rx_checksum;    //֡ͷ����У���
static uint8_t uart2_rx_buf[40];     //���ջ��壬��������С�ڵ���32Byte
static uint8_t uart2_tx_buf[40];     //���ͻ���
static uint8_t uart2_ping_match=0;

uint8_t ros_servo_data=0;

static void UART2_SendText(const char *text)
{
	while(*text)
	{
		USART_SendData(USART2, (uint8_t)(*text++));
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);
	}
}



/**
  * @��  ��  UART   ���ڳ�ʼ��
  * @��  ��  baud�� ����������
  * @����ֵ	 ��
  */
	
//�û��ӿڣ�Ҳ��ֱ�ӽ���ݮ�ɵ�GPIO
void UART2_Init(uint32_t baud)
{

	GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;


	/* ����USART���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//USART��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); 

	//USART �˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOD,&GPIO_InitStructure); 

	//USART��������
	USART_InitStructure.USART_BaudRate = baud;    //������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	//USARTʹ��
	USART_Cmd(USART2, ENABLE); 
	
	//�������ڽ����ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�

    //USART2 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���	
	
}

/**
  * @��  ��  UART �����жϷ�����
  * @��  ��  �� 
  * @����ֵ  ��
  */
void USART2_IRQHandler(void)
{
	uint8_t Res;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{
		Res =USART_ReceiveData(USART2);	
		TaoV2_OnByte(Res);
		
		if(Res == (uint8_t)"PING\n"[uart2_ping_match])
		{
			uart2_ping_match++;
			if(uart2_ping_match >= 5)
			{
				uart2_ping_match = 0;
				uart2_rx_con = 0;
				UART2_SendText("PONG\n");
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
				return;
			}
		}
		else
		{
			uart2_ping_match = (Res == 'P') ? 1 : 0;
		}
		
		if(uart2_rx_con < 3)    //==����֡ͷ + ����
		{
			if(uart2_rx_con == 0)  //����֡ͷ1 0xAA
			{
				if(Res == 0xAA)
				{
					uart2_rx_buf[0] = Res;
					uart2_rx_con = 1;					
				}
				else
				{
					
				}
			}else if(uart2_rx_con == 1) //����֡ͷ2 0x55
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
			else  //�������ݳ���
			{
				uart2_rx_buf[2] = Res;
				uart2_rx_con = 3;
				uart2_rx_checksum = (0xAA+0x55) + Res;	//����У���
			}
		}
		else    //==��������
		{
			if(uart2_rx_con < (uart2_rx_buf[2]-1) )
			{
				uart2_rx_buf[uart2_rx_con] = Res;
				uart2_rx_con++;
				uart2_rx_checksum = uart2_rx_checksum + Res;					
			}
			else    //�ж����1λ
			{
				//������ɣ��ָ���ʼ״̬
				uart2_rx_con = 0;	
				//����У��
				if( Res == uart2_rx_checksum )  //У����ȷ
				{	
					printf("uart2 frame id=0x%02X len=%d\r\n", uart2_rx_buf[3], uart2_rx_buf[2]);
					//�ٶȿ���֡
					if(uart2_rx_buf[3] == ID_ROS2STM_VEL)
					{
						if(TaoV2_IsRosAutoActive())
						{
							Vel.TG_IX = (int16_t)((uart2_rx_buf[4]<<8) | uart2_rx_buf[5]);
							Vel.TG_IY = (int16_t)((uart2_rx_buf[6]<<8) | uart2_rx_buf[7]);
							Vel.TG_IW = (int16_t)((uart2_rx_buf[8]<<8) | uart2_rx_buf[9]);
						}
					}
					else
					{
						//IMU������У׼
						if(uart2_rx_buf[3] == ID_ROS2STM_IMU)
						{
							imu_calibrate_flag = uart2_rx_buf[4];
						}	

						//��е�ۿ���֡
						else if(uart2_rx_buf[3] == ID_ROS2STM_HAND)
						{
							ros_servo_data=1;
							arm_angle[5] = (int16_t)((uart2_rx_buf[4]<<8) | uart2_rx_buf[5]);
							ros_servo.pwm[5] = 1500-arm_angle[5]/2.356;
							ros_servo.time[5] = 50;
							//printf("A0%d A1%d A2%d A3%d A4%d A5%d \r\n ",ros_servo.angle[0],ros_servo.angle[1], ros_servo.angle[2],ros_servo.angle[3],ros_servo.angle[4],ros_servo.angle[5]  );

						}
												

						//��е�ۿ���֡
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
						//���̿��ƻ�е�ۿ���֡
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
						//��е�����˶�ѧ����֡
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
						//��е�ۺ͵��̸�λ
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
  * @��  ��  UART �������ݣ�X-ProtocolЭ�飩
  * @��  ��  *pbuf����������ָ��
  *          len���������ݳ��ȸ�������27 (32-5)
  *          num��֡�ţ�֡����
  * @����ֵ	 ��
  */
void UART2_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num)
{
	uint8_t i,cnt;	
    uint8_t tx_checksum = 0;//����У���
	
	if(len <= 39)
	{
		/******��ȡ����******/
		uart2_tx_buf[0] = 0xAA;    //֡ͷ
		uart2_tx_buf[1] = 0x55;    //
		for(i=0; i<len; i++)
		{
			uart2_tx_buf[2+i] = *(pbuf+i);
		}
		
		/******����У���******/	
		cnt = 2+len;
		for(i=0; i<cnt; i++)
		{
			tx_checksum = tx_checksum + uart2_tx_buf[i];
		}
		uart2_tx_buf[i] = tx_checksum;
		uart2_tx_buf[35] = 0x7D;    //֡?
		
		/******��������******/	
		cnt = 4+len;
		
		//��ѯ���䷽ʽ
		for(i=0; i<cnt; i++)
		{
			USART_SendData(USART2, uart2_tx_buf[i]);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);
		}
	}
}
