/**
  ******************************************************************************
  * @file    usb_bsp.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file is responsible to offer board support package and is
  *          configurable by user.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usb_bsp.h"
#include "stm32f4xx_conf.h"

/** @addtogroup USB_OTG_DRIVER
* @{
*/

/** @defgroup USB_BSP
  * @brief This file is responsible to offer board support package
  * @{
  */ 

/** @defgroup USB_BSP_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_BSP_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 





/** @defgroup USB_BSP_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_BSP_Private_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBH_BSP_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_BSP_Private_Functions
  * @{
  */ 


/**
  * @brief  USB_OTG_BSP_Init
  *         Initilizes BSP configurations
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{

#ifdef USB_OTG_FS_CORE
	  GPIO_InitTypeDef GPIO_InitStructure;

	  // USB HOST : CORE_FS
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);

	  /* Configure  ID DM DP Pins */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 |    GPIO_Pin_12;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG1_FS) ;
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG1_FS) ;

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE) ;
#endif

#ifdef USB_OTG_HS_CORE
	  GPIO_InitTypeDef GPIO_InitStructure2;
	  // USB HOST : CORE_HS
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);

	  GPIO_InitStructure2.GPIO_Pin =  GPIO_Pin_14 | GPIO_Pin_15;
	  GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_Init(GPIOB, &GPIO_InitStructure2);

	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource14, GPIO_AF_OTG2_FS) ;
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource15, GPIO_AF_OTG2_FS) ;
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_OTG_HS, ENABLE) ;
#endif

}/**
  * @brief  USB_OTG_BSP_EnableInterrupt
  *         Enabele USB Global interrupt
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
	  /* Enable USB Interrupt */

	  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef USB_OTG_HS_CORE
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	  // HS
	  NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
#endif
#ifdef USB_OTG_FS_CORE
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	  // FS
	  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
#endif

}

void USB_OTG_BSP_DisableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
	  /* Disable USB Interrupt */

	  NVIC_InitTypeDef NVIC_InitStructure;

	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	  // FS
	  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	  NVIC_Init(&NVIC_InitStructure);

}



/**
  * @brief  BSP_Drive_VBUS
  *         Drives the Vbus signal through IO
  * @param  speed : Full, Low 
  * @param  state : VBUS states
  * @retval None
  */

void USB_OTG_BSP_DriveVBUS(uint32_t speed, uint8_t state)
{

}

/**
  * @brief  USB_OTG_BSP_ConfigVBUS
  *         Configures the IO for the Vbus and OverCurrent
  * @param  Speed : Full, Low 
  * @retval None
  */

void  USB_OTG_BSP_ConfigVBUS(uint32_t speed)
{

}

/**
  * @brief  USB_OTG_BSP_TimeInit
  *         Initialises delay unit Systick timer /Timer2
  * @param  None
  * @retval None
  */
void USB_OTG_BSP_TimeInit ( void )
{

}

/**
  * @brief  USB_OTG_BSP_uDelay
  *         This function provides delay time in micro sec
  * @param  usec : Value of delay required in micro sec
  * @retval None
  */
void USB_OTG_BSP_uDelay (const uint32_t usec)
{

  uint32_t count = 0;
  const uint32_t utime = (120 * usec / 7);
  do
  {
    if ( ++count > utime )
    {
      return ;
    }
  }
  while (1); 
  
}


/**
  * @brief  USB_OTG_BSP_mDelay
  *          This function provides delay time in milli sec
  * @param  msec : Value of delay required in milli sec
  * @retval None
  */
void USB_OTG_BSP_mDelay (const uint32_t msec)
{

    USB_OTG_BSP_uDelay(msec * 1000);    

}


/**
  * @brief  USB_OTG_BSP_TimerIRQ
  *         Time base IRQ
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_TimerIRQ (void)
{

} 

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
