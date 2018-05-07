#ifndef ARK169_H
#define ARK169_H

#include "../ArkNaviProtocol.h"

/*******************************************************/
// APP UART CMD
/*******************************************************/
#define ARM_HEADCMD 					0x02

#define ARM_SENDDATA_CMD 			0x80
#define MCU_ACKDATA_CMD 			0x80

#define MCU_SENDDATA_CMD 			0x81
#define ARM_ACKDATA_CMD 			0x81

//**********ARM Send CMD To MCU*************//
#define Arm_Send_HandShakeCmd		0xA0
#define Arm_Send_SetBaudRateCmd		0xA1
#define Arm_Req_InitMcuCmd			0xA2
#define Arm_Send_SpectrumCmd			0xA3
#define Arm_Send_McuDeviceStateCmd      0xA4
#define Arm_Req_McuDeviceStateCmd		0xA4
#define Arm_Send_ArmStateCmd			0xA5
#define Arm_Send_ChangeSourceCmd		0xA7
#define Arm_Req_McuVolumeCmd		0xA9
#define Arm_Send_SetMcuVolumeCmd		0xAA
#define Arm_Req_McuEqCmd				0xAB
#define Arm_Send_SetMcuEqCmd			0xAC
#define Arm_Send_KeyCmd				0xB0
#define Arm_Send_TouchCmd			0xB1
#define Arm_Req_RadioInfoCmd			0xB3
#define Arm_Req_CdcInfoCmd			0xB5
#define Arm_Req_SysInfoCmd			0xB7
#define Arm_Send_SysInfoCmd			0xB8
#define Arm_Req_SwcTabCmd			0xBA
#define Arm_Send_SwcStudyCmd			0xBD
#define Arm_Send_SwcKeyIdCmd			0xBE
#define Arm_Send_RadioFreqCmd			0xC2
#define Arm_Req_McuVerCmd			0xC3
#define Arm_Send_McuUpdateCmd		0xC5
#define Arm_Send_SetRadioRissCmd		0xC7
#define Arm_Send_PlayerInfoCmd		0xC9
#define Arm_Req_CarUsbIpodInfoCmd		0xCA	// Ô­³µUSB ipod ÐÅÏ¢

#define Arm_Req_AcCmd				0xD0
#define Arm_Req_RadarCmd				0xD2
#define Arm_Req_CarDoorCmd			0xD6
#define Arm_Req_CurrentTripCmd		0xD8
#define Arm_Req_HistoryTripCmd			0xDA
#define Arm_Req_KMRSysInfoCmd		0xDC
#define Arm_Send_KMRSysInfoCmd		0xDE

#define Arm_Send_CarAudioBalFadInfo	0xE1	// Ô­³µÒôÁ¿Æ½ºâÐÅÏ¢
#define Arm_Send_CarAudioEQInfo		0xE2	// Ô­³µEQ ÐÅÏ¢
#define Arm_Req_CarCompeterCmd		0xE3	// ´óÖÚÐÐ³µµçÄÔÐÅÏ¢

#define Arm_Send_HeartBeatCmd       0xF5

//**********MCU Send CMD To ARM*************//
#define Mcu_Send_HandShakeCmd           0xA0
#define Mcu_Req_McuDeviceStateCmd       0xA4
#define Mcu_Send_McuDevieStateCmd		0xA6
#define Mcu_Send_ChangeSourceCmd		0xA8
#define Mcu_Send_VolumeCmd			0xAD
#define Mcu_Send_EqCmd				0xAE
#define Mcu_Send_SpectrumCmd			0xAF		//no use
#define Mcu_Send_KeyCmd				0xB2
#define Mcu_Req_RadioInfoCmd        0xB3
#define Mcu_Send_RadioInfoCmd			0xB4
#define Mcu_Send_CdcInfoCmd			0xB6		//no use
#define Mcu_Send_SysInfoCmd			0xB9
#define Mcu_Req_SwcTabCmd			0xBA
#define Mcu_Send_SwcTabCmd			0xBB
#define Mcu_Send_SwcKeyCmd			0xBE
#define Mcu_Req_ArmSourceCmd			0xC0
#define Mcu_Send_ArmTftState			0xC1
#define Mcu_Send_ACC_STATUS			0xC2
#define Mcu_Send_Update_Status          0xC6
#define Mcu_Send_RadioRiss				0xC8
#define Mcu_Send_ArmAcCmd			0xD1
#define Mcu_Send_ArmRadarCmd			0xD3
#define Mcu_Send_CarUsbIpodInfoCmd	0xCB
#define Mcu_Send_ArmCarDoorCmd		0xD7
#define Mcu_Send_ArmCarCurCmd		0xD9
#define Mcu_Send_ArmHisTripCmd		0xDB
#define Mcu_Send_ArmCarBaseCmd		0xDD
#define Mcu_Send_OutSideTempInfo		0xE0	// ÊÒÍâÎÂ¶ÈÐÅÏ¢
#define Mcu_Send_CarCompeterCmd		0xE4


#define CMD_ID_SEND_BT_CMD		0xD4


//************ALL CMD NUM********************//
#define AllCmdHeadNum					0x01
#define AllCmdTypeNum					0x02
#define AllCmdAckNum					0x04

//**************Arm Source********************//

#define ARM_SYSTEM_STATE_STB		0x00
#define ARM_SYSTEM_STATE_DISC		0x01
#define ARM_SYSTEM_STATE_SD			0x02
#define ARM_SYSTEM_STATE_USB		0x03
#define ARM_SYSTEM_STATE_RADIO		0x04
#define ARM_SYSTEM_STATE_TV			0x05
#define ARM_SYSTEM_STATE_CMMB		0x06
#define ARM_SYSTEM_STATE_DVBT		0x07
#define ARM_SYSTEM_STATE_AUX		0x08
#define ARM_SYSTEM_STATE_AUX1		0x09
#define ARM_SYSTEM_STATE_CARUSBBT	0x0A	// Ô­³µUSB ,Ô­³µÀ¶ÑÀ
#define ARM_SYSTEM_STATE_BT			0x0B
#define ARM_SYSTEM_STATE_CCD		0x0C
#define ARM_SYSTEM_STATE_NAVI		0x0D
#define ARM_SYSTEM_STATE_MAIN		0x0F
#define ARM_SYSTEM_STATE_IPOD		0x10
#define ARM_SYSTEM_STATE_CMMB2		0x11
#define ARM_SYSTEM_STATE_SETMENU	0x12
#define ARM_SYSTEM_STATE_AC			0x13
#define ARM_SYSTEM_STATE_USB2		0x14	// ºóUSB
#define ARM_SYSTEM_STATE_CARDOOR	0x16
#define ARM_SYSTEM_STATE_BASE_INFO	0x17
#define ARM_SYSTEM_STATE_HIS_TRIP	0x18
#define ARM_SYSTEM_STATE_CUR_TRIP	0x19
#define ARM_SYSTEM_STATE_ARM_STB	0xFF


typedef enum
{
   ERR_SUCCESS = 0x00,						//³É¹¦
   ERR_FAIL = 0x01,	 						//Ê§°Ü
   ERR_UNSUPPORTEDBAUDRATE = 0x02,			//²»Ö§³Ö²¨ÌØÂÊ
   ERR_INVALIDTIME = 0x03, 					//·Ç·¨Ê±¼ä
   ERR_CLOCKHWERR = 0x04,					//Ê±¼äÓ²¼þ³ö´í
   ERR_UNSUPPORTEDCMD = 0xff				//²»Ö§³Ö¸ÃÃüÁî
}RETURN_ACK_STATE;


/*******************************************************/
// APP KEY
/*******************************************************/
#define C_KEY_NONE                   		0xFF
#define C_KEY_WHEEL_ANGLE      		0xF9  	 // ·½ÏòÅÌ×ª½Ç
#define C_KEY_USER_SET      			0xFA  	 //
#define C_KEY_CAN      				0xFB  	 //ARM »ñÈ¡CAN ÐÅÏ¢

#define C_KEY_BACKLIGHT_STATE         0xFC		// MCUÖ÷¶¯¿ØÖÆ: µÚÒ»×Ö½Ú²ÎÊý 0 °×ÌìÄ£Ê½ 1 ¹Ø±ÕÄ£Ê½ 2ºÚÒ¹Ä£Ê½
#define C_KEY_ARM_SAVE_OK           	0xFD	// Í¨Öª MCU,ARM ÒÑ¾­ÍêÈ«±£´æºÃµ±Ç°×´Ì¬£¬¿ÉÒÔ½øÈë¶Ïµç×´Ì¬
#define C_KEY_CUSTOM 				0xFE  	 //rx arm  key	Æß²ÊµÆ,°´¼üºó½ô¸úµÄµÚÒ»×Ö½Ú±íÊ¾ ÑÕÉ«,µÚ¶þ¸ö×Ö½Ú±íÊ¾  ¼ä¸ôÊ±¼ä
#define C_KEY_USER      				0xFF  	 //rx arm  key	ÓÃ»§¼ü

/*****************************************************/

//FUNCTION KEY
#define C_KEY_POWER        			0x01
#define C_KEY_MUTE         			0x02
#define C_KEY_EQ           				0x03
#define C_KEY_VOL_INC      			0x04
#define C_KEY_VOL_DEC      			0x05
#define C_KEY_PICK_UP      			0x06  // ¿ÉÇÐ»»BT½çÃæ,²¦´òºÍ½ÓÌýµç»°¹¦ÄÜ
#define C_KEY_HANG_UP      			0x07  // ¹Òµç»°¹¦ÄÜ
#define C_KEY_MODE      				0x08	 // ¸÷Ô´Ä£Ê½Ö®¼äÂÖÁ÷Ñ­»·ÇÐ»»
#define C_KEY_HOME      				0x09	 // ÇÐ»»µ½Ö÷½çÃæ
#define C_KEY_HOME_FRONTSRC      	0x0A	 // ÔÚ HOME ½çÃæºÍµ±Ç°Ã½ÌåÔ´Ö®¼äÇÐ»» // ×¢128KµÄAPP ´ËÃüÁîÎÞÐ§
#define C_KEY_RESTARM        			0x0B	 // Ôö¼ÓARM ¸´Î»°´¼ü

//
#define C_KEY_1S           				0x10
#define C_KEY_2S           				0x11
#define C_KEY_3S           				0x12
#define C_KEY_4S           				0x13
#define C_KEY_5S           				0x14
#define C_KEY_6S          				0x15
#define C_KEY_LOUD         			0x17
#define C_KEY_CLK         				0x19

//
#define C_KEY_OPEN         			0x1D
#define C_KEY_FORCE_OPEN   			0x1F

//GENERAL KEY
#define C_KEY_0            				0x20
#define C_KEY_1            				0x21
#define C_KEY_2            				0x22
#define C_KEY_3            				0x23
#define C_KEY_4            				0x24
#define C_KEY_5            				0x25
#define C_KEY_6            				0x26
#define C_KEY_7            				0x27
#define C_KEY_8            				0x28
#define C_KEY_9            				0x29
#define C_KEY_DOWN         			0x2A
#define C_KEY_UP           				0x2B
#define C_KEY_LEFT         				0x2C
#define C_KEY_RIGHT        			0x2D
#define C_KEY_ENTER        			0x2E
#define C_KEY_RETURN          			0x2F
#define C_KEY_STAR          			0x30		// *ºÅ¼ü
#define C_KEY_WELL        				0x31		// ££ºÅ¼ü
#define C_KEY_CLEAR         			0x32		// Clear
#define C_KEY_ADD      				0x33		// +


//BT
#define C_KEY_CONNECT				0x34
#define C_KEY_BT_BOOK_LOAD		0x35
#define C_KEY_HAND_SET				0x36
#define C_KEY_REDIAL				0x37
#define C_KEY_BT_CALL_NUM			0x38
#define C_KEY_DISCONNECT			0x39



//FAST FUNCTION KEY
#define C_KEY_MUSIC					0x40
#define C_KEY_PHOTO			0x41
#define C_KEY_VIDEO				0x42
#define C_KEY_GPS          			0x43
#define C_KEY_RADIO        		0x44		// RADIO, »òBANDÇÐ»»¼ü
#define C_KEY_DVD           		0x45
#define C_KEY_TEL				0x46	// ÇÐ»»µ½BT½çÃæ
#define C_KEY_SETUP				0x47		// System Setting
#define C_KEY_TOUCH_MENU		0x4F		//Touch calibrate menu  ´¥ÃþÐ£Õý

//RADIO KEY
#define C_KEY_BAND         		0x50
#define C_KEY_SEEK_DOWN		0x51
#define C_KEY_SEEK_UP			0x52
#define C_KEY_TUNE_DOWN          	0x53
#define C_KEY_TUNE_UP        	 	0x54

#define C_KEY_PS					0x56
#define C_KEY_AS					0x55		// 128K UI Ö»ÓÐ0x55

#define C_KEY_LOC          				0x57
#define C_KEY_TA	       			0x58
#define C_KEY_PTY      				0x59
#define C_KEY_AF       				0x5A
#define C_KEY_REG       				0x5B
#define C_KEY_CT       				0x5C
#define C_KEY_EON       				0x5D
#define C_KEY_RDS     				0x5E
#define C_KEY_RDS_TYPE     		0x5F
#define C_KEY_FM_FREQ           		0x60		// arm ÆµÂÊ»¬¶¯
#define C_KEY_AM_FREQ           		0x61
#define C_KEY_RADIO_SAVE  		0x62
#define C_KEY_RADIO_LOAD  		0x63
#define C_KEY_PTY_SEEK     			0x64
#define C_KEY_ST           			0x65
#define C_KEY_CH_DEC       			0x66
#define C_KEY_CH_INC       			0x67
#define C_KEY_FM       			0x68
#define C_KEY_AM       			0x69

//PLAYER KEY
#define C_KEY_PAUSE         		0x6a
#define C_KEY_STOP         			0x70
#define C_KEY_PLAY         			0x71		// BYTE0:  0  ²¥·ÅÔÝÍ£Ñ­,1 ²¥·Å£¬2 ÔÝÍ£
#define C_KEY_NEXT        	 		0x72		// ÏÂÒ»Çú,ÊÕÒôÊ±×Ô¶¯ËÑÌ¨
#define C_KEY_PRE          			0x73		// ÉÏÒ»Çú
#define C_KEY_FF           			0x74		// ¿ì½ø,×¢: APP ÄÇ±ßÈ¡ÏûÁË´Ë¹¦ÄÜ£¬ÊÕÒôÊ±µ¥²½×ß
#define C_KEY_FB          			0x75		// ¿ìÍË,×¢: APP ÄÇ±ßÈ¡ÏûÁË´Ë¹¦ÄÜ
#define C_KEY_REPEAT				0x76
#define C_KEY_SHUFFLE			0x77
#define C_KEY_SCAN          			0x78
#define C_KEY_A_B          			0x79
#define C_KEY_ZOOM         			0x7A
#define C_KEY_TITLE				0x7B
#define C_KEY_SUB_T   				0x7C
#define C_KEY_PBC   				0x7D
#define C_KEY_ANGLE   				0x7E		// ½Ç¶È
#define C_KEY_STEP   				0x7F
#define C_KEY_TRACK           			0x80		// ÉùµÀ
#define C_KEY_FULL_SCREEN          	0x81		// ²¥·ÅÊ±È«ÆÁÓë·ÇÈ«ÆÁÖ®¼äÇÐ»»
#define C_KEY_PLAYPAUSE         		0x82


// ELECTRIC_PANEL KEY
#define C_KEY_PANEL_ADJ         		0x83
#define C_KEY_PANEL_DEC         		0x84
#define C_KEY_PANEL_INC         		0x85
#define C_KEY_PANEL_CLOSE         	0x86
#define C_KEY_PANEL_OPEN         		0x87


//SYS FUNCTION KEY
#define C_KEY_BTAUDIO_MUTE		0xA0
#define C_KEY_BLACKOUT     		0xA1		// ARMÖ÷¶¯¿ØÖÆ °×Ìì£¬ºÚÒ¹£¬¹Ø±Õ3 ÖÖ±³¹âÄ£Ê½Ö®¼äµÄÇÐ»»
#define C_KEY_GPS_MUTE			0xA2		// Èç¹ûµ¼º½ÔÚÇ°Ì¨£¬µ¼º½»ìÒôºÍµ¼º½¼æÌýÖ®¼äÇÐ»»
#define C_KEY_BEEP				0XA3		// °´¼üÒô,ARM ½çÃæÉÏ²Ù×÷°´¼üÊ±£¬»á·¢ËÍ¸Ã°´¼ü¸øMCU
#define C_KEY_SELECT				0xA4
#define C_KEY_SETTING_RESET		0xA5		// »Ö¸´³ö³§ÉèÖÃÊ±»á·¢ËÍ¸Ã°´¼ü¸ø MCU
#define C_KEY_WHEEL_STUDY 		0xA6		// ²ÎÊýBYTE0£¬´ú±í µ±Ç°Ñ§Ï°µÄ°´¼ü
#define C_KEY_WHEEL_STUDY_OK	0xA7		// ·½ÏòÅÌÑ§Ï°Íê³É±£´æ
#define C_KEY_WHEEL_STUDY_REST	0xA8		// ·½ÏòÅÌÑ§Ï°¸´Î»
#define C_KEY_SEL          			0xA9
#define C_KEY_TV          			0xAA

#define C_KEY_GOTO          			0xB1		// Ò£¿Ø°´¼üÉÏµÄ GOTO°´¼ü
#define C_KEY_DISP     				0xB2
#define C_KEY_REARVIEW    			0xB3		// Ò£¿ØÆ÷ºóÊÓ°´¼ü

#define C_KEY_REST_RADIORISS    		0xB4		// »Ö¸´Í£Ì¨ÁéÃô¶È
#define C_KEY_ARMREQ_RADIORISS	0xB5		// ARM Ñ¯ÎÊRADIORISS ²ÎÊý
#define C_KEY_ARMSET_RADIORISS	0xB6		// ARM ÉèÖÃRADIORISS ²ÎÊý
//#define C_KEY_MCUTX_RADIORISS		0xB7		// MCU ·¢ËÍRADIORISS ²ÎÊý

#define C_KEY_DISPLAY_ONOFF	 	0XB7		// LOW BYTE  0x01  ´ò¿ª£¬  0x02 ¹Ø±Õ

#define C_KEY_BACKLIGHT_ON         0xB8		// ¿ª±³¹â
#define C_KEY_BACKLIGHT_OFF        0xB9		// ¹Ø±³¹â


// µç»°±¾
#define C_KEY_SYNC_PA     		 0xBA	 // Í¬²½µç»°±¾
#define C_KEY_SYNC_PH     		 0xBB	// Í¬²½ÒÑ²¦Í¨»°¼ÇÂ¼
#define C_KEY_SYNC_PI     		 0xBC	// Í¬²½ÒÑ½ÓÍ¨»°¼ÇÂ¼
#define C_KEY_SYNC_PJ    		 0xBD	// Í¬²½Î´½ÓÍ¨»°¼ÇÂ¼
#define C_KEY_SYNC_PZ     		 0xBE	// È¡ÏûÍ¬²½

#define Q_BT_CONN				 0xBF
#define Q_BT_DISCONN		        0xC0


//
#define C_KEY_POWER_MUTE		 0xC1

#define MAX_CONST_KEY      		0xCF


#define RX_FRAME_NUMBER          	10
#define RX_BUFFER_SIZE          	55

enum{
        SYSTEM_ACC_OFF,
        SYSTEM_ACC_ON
};

struct bit_t {
    unsigned char   b0:1;                   /* bit0 */
    unsigned char   b1:1;                   /* bit1 */
    unsigned char   b2:1;                   /* bit2 */
    unsigned char   b3:1;                   /* bit3 */
    unsigned char   b4:1;                   /* bit4 */
    unsigned char   b5:1;                   /* bit5 */
    unsigned char   b6:1;                   /* bit6 */
    unsigned char   b7:1;                   /* bit7 */
};

typedef	union Bb8 {
        struct bit_t    Bit;                   /* bit unit */
        unsigned char   Byte;                  /* byte unit */
} Typ_Bb8;

extern struct CustomerProtocol ark169;

#endif //SHENGLONG_H

