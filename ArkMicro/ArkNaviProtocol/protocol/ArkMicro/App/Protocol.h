#define  MCU_EX_INFO_LEN		128

//---------------------------------------------------------------------------------
//媒体源定义
enum  SOURCE { SRC_OFF=0X00,SRC_DISC=0X01,SRC_SD=0X02,SRC_USB=0X03,SRC_RADIO=0X04,SRC_TV=0X05,SRC_CMMB=0X06,
				SRC_DVB_T=0X07,SRC_AUX_1=0X08,SRC_AUX_2=0X09,SRC_CDC=0X0A,SRC_BT=0X0B,SRC_CAMERA=0X0C,
				SRC_NAVIGATION=0X0D, SRC_MAIN=0X0F,SRC_IPOD=0X10,SRC_MCUTV=0X11, SRC_ARM_UI=0x12,SRC_IPOD_USB=0x13,
				SRC_USB_REAR=0x14, SRC_ISDBT=0x15,SRC_ANALOGUE_TV=0x16,SRC_ALL ,SRC_MCU_STANDBY=0XFF, };

enum FREP_NATION { 
FQ_EUROPE = 0,
FQ_USA = 1,
FQ_OIRT = 2,
FQ_JAPAN = 3,
FQ_M_EAST = 4,
FQ_LATIN = 5,
FQ_AUSTRALIA = 6,
FQ_ASIA = 7,
FQ_SAMER1 = 8,
FQ_ARABIA = 9,
FQ_CHINA = 10,
FQ_SAMER2 = 11,
FQ_KOREA = 12,
FQ_RUSSIAN = 13,
FQ_FM_ONLY = 14,
FQ_TOTAL,
  };

//SRC_STANDBY 

#define		PACK_SIGN_START					0x02
#define		CMD_ARM_REQUEST					0x80
#define		CMD_MCU_REQUEST					0x81

//---------------------------------------------------------------------------------------
//-------------------------------Error Code ---------------------------------------------

#define		ERR_SUCCESS							0
#define		LDERR_UNSUPPORTEDBAUDRATE			20
#define		ERR_UNSUPPORTEDCMD					21

#define		ERR_FILE_ALREADY_OPEN				1
#define		ERR_OPEN_COM						2
#define		ERR_COMHASEUSED						3
#define		ERR_SET_COM_STATE					4
#define		ERR_WRITE_COM						5
#define		ERR_CREATE_REV_THREAD				6
#define		ERR_CMD_NO_DEALCONTENT				7
#define		ERR_CMD_ACK							8
#define		ERR_CMD_REV_LENGTH					9
#define		ERR_WAITCOM_MASK					10
#define		ERR_CHECKSUM						11
#define		ERR_CMD_PACKAGE						12
#define		ERR_CMM_ONSENDING					13

//读取的包的长度小于实际解析出来的长度
#define		ERR_PACKAGE_HALF_BAKED				14
#define		ERR_PARAM_INVALID					15


//-------------------------------Protocol Command ID ---------------------------------------------
#define		CMD_ID_NONE							0x00

#define		CMD_ID_CAN_PACKAGE					0x08


#define		CMD_ID_HANDSHAKE					0xA0
#define		CMD_ID_BAUDRATE						0xA1
#define		CMD_ID_INIT							0xA2
#define		CMD_ID_STATE_CFG					0xA3
#define		CMD_ID_QUERY_STATE					0xA4
#define		CMD_ID_REPORT_STATE					0xA5
#define		CMD_ID_GET_REPORT_STATE				0xA6
#define		CMD_ID_SOURCE_CHANGE				0xA7
#define		CMD_ID_RREQUEST_SOURCE_CHANGE		0xA8
#define		CMD_ID_GETVOLUMN					0xA9
#define		CMD_ID_SETVOLUMN					0xAA
#define		CMD_ID_GETAUDIO_EQ					0xAB
#define		CMD_ID_SETAUDIO_EQ					0xAC
#define		CMD_ID_RREPORT_VOLCHANGE			0xAD
#define		CMD_ID_RREPORT_AUDIO_EQ				0xAE
#define		CMD_ID_RREPORT_SPECTRUM				0xAF


#define		CMD_ID_KEYBOARD						0xB0
#define		CMD_ID_LBTNDOWN						0xB1
#define		CMD_ID_REMOTE_KEY					0xB2
#define		CMD_ID_RADIO_INFO					0xB3
#define		CMD_ID_RREPORT_RADIO_INFO			0xB4
#define		CMD_ID_QUERY_CDC_INFO				0xB5
#define		CMD_ID_RREPORT_CDC_INFO				0xB6
#define		CMD_ID_QUERY_SYSTEMPARAM			0xB7
#define		CMD_ID_SET_SYSTEMPARAM				0xB8
#define		CMD_ID_RREPORT_SYSTEMPARAM			0xB9
#define		CMD_ID_QUERY_SWC_TABLE				0xBA
#define		CMD_ID_RREPORT_SWC_TABLE			0xBB
#define		CMD_ID_SETREMOTE_SWC_TABLE			0xBC
#define		CMD_ID_WHEEL_STUDY					0xBD
#define		CMD_ID_WHEEL_RESULT					0xBE


#define		CMD_ID_RQUERY_SOURCE_STATE		    0xC0
#define		CMD_ID_APPEND_REDEV_STATE		    0xC1
#define		CMD_ID_ACC_STATE_CHANGE			    0xC2
#define		CMD_ID_QUERY_MCU_VERSION			0xC3

#define		CMD_ID_MCU_UPDATE_START_ENC			0xC4
#define		CMD_ID_MCU_UPDATE_ACK				0xC5
#define		CMD_ID_MCU_UPDATE_DATA				0xC6

#define		CMD_ID_EX_SETCUSTOMINFO				0xC7
#define		CMD_ID_EX_RRPORTCUSTOMINFO			0xC8
#define		CMD_ID_NOTIFY_MEDIA_STATE			0xC9

#define		CMD_ID_EX2_CUSTOMSTATE				0xCA
#define		CMD_ID_EX2_RRPORTCUSTOMSTATE		0xCB

#define 	CMD_ID_SET_FACTORYPARAM				0xCB
#define 	CMD_ID_QUERY_FACTORYPARAM			0xCC


#define		CMD_ID_QUERY_CAN_INFO				0xD0
#define		CMD_ID_RREPORT_CAN_INFO				0xD1
#define		CMD_ID_SET_CAN_INFO					0xD2

#define		CMD_ID_RREPORT_RADAR_INFO			0xD3
#define		CMD_ID_CAN_CAR_COMPUTER_INFO		0xE4

#define		CMD_ID_SEND_BT_CMD					0xD4

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//--------------------------  KEY Define	---------------------------------------
#define K_NONE				0x00
#define K_POWER				0x01
#define K_MUTE				0x02
#define K_EQ				0x03
#define K_VOL_UP			0x04
#define K_VOL_DN			0x05
#define K_PHONE_ON			0x06
#define K_PHONE_OFF			0x07

#define	K_SOURCE_MODE		0x08
#define K_SOURCE_HOME		0x09

#define K_HOME_FRONTSRC		0x0A	
#define K_RESET_ARM			0x0B

//#define	K_SOURCE_MENU		0x08
//#define	K_SOURCE_MODE		0x09


//#define K_PLAY			0x10
//#define K_PAUSE			0x11
//#define K_STOP			0x12
//#define K_NEXT			0x13
//#define K_PREV			0x14
//#define	K_FASTF			0x15
//#define K_FASTB			0x16
//#define K_SLOWF			0x17
//#define K_SLOWB			0x18
//#define K_STEP			0x19
//#define K_REPEAT		0x1A
//#define K_SHUFFLE		0x1B
//#define K_SCAN			0x1C
//#define K_TRACK			0x1E

#define K_1S			0x10
#define K_2S			0x11
#define K_3S			0x12
#define K_4S			0x13
#define K_5S			0x14
#define K_6S			0x15

#define K_EJECT			0x1D


#define K_0				0x20
#define K_1				0x21
#define K_2				0x22
#define K_3				0x23
#define K_4				0x24
#define K_5				0x25
#define K_6				0x26
#define K_7				0x27
#define K_8				0x28
#define K_9				0x29
#define K_UP			0x2A
#define K_DOWN			0x2B
#define K_LEFT			0x2C
#define K_RIGHT			0x2D
#define K_ENTER			0x2E
#define K_RETURN        0x2F
#define K_STAR			0x30
#define K_NUMBER        0x31
#define K_CLEAR			0x32

#define K_MUSIC			0x40
#define K_PHOTO			0x41
#define K_VIDEO			0x42
#define K_NAVI			0x43
#define K_TUNER			0x44
#define K_DVD			0x45
#define K_BLUETOOTH		0x46
#define K_SETTING		0X47
#define K_TV			0x48
#define K_USB			0x49
#define K_SD_CARD		0x4A
#define K_TOUCH_MENU	0X4F



#define T_RADIO_BAND				0x50
#define T_RADIO_TUNE_UP				0x51
#define T_RADIO_TUNING_DOWN         0x52
#define T_RADIO_SEEK_UP				0x53
#define T_RADIO_SEEK_DOWN			0x54
#define T_RADIO_AS					0x55
#define T_RADIO_PS					0x56
#define T_RADIO_LOC					0x57
#define T_RADIO_TA					0x58
#define T_RADIO_PTY					0x59
#define T_RADIO_AF					0x5A
#define T_RADIO_REG					0x5B
#define T_RADIO_CT					0x5C
#define T_RADIO_EON					0x5D
#define T_RADIO_RDS					0x5E
#define T_RADIO_RDS_TYPE			0x5F
#define T_RADIO_FM_FREQ				0x60
#define T_RADIO_AM_FREQ				0x61
#define T_RADIO_PRESET_SAVE         0x62
#define T_RADIO_PRESET_LOAD         0x63
#define T_RADIO_PTY_SEEK				0x64
#define T_RADIO_STERO_STATE_CHANGE		0x65

#define T_RADIO_PRESET_PRE				0x66		//
#define T_RADIO_PRESET_NEXT				0x67		//



//#define T_DVDC_STOP					0x70
//#define T_DVDC_PLAY					0x71
//#define T_DVDC_NEXT_CHAPTER			0x72
//#define T_DVDC_PREV_CHAPTER			0x73
//#define T_DVDC_FF					0x74
//#define T_DVDC_FB					0x75
//#define T_DVDC_GOTO_CHAPTER			0x76
//#define T_DVDC_GOTO_DISC			0x77
//#define T_DVDC_REPEAT				0x78
//#define T_DVDC_SHUFFLE				0x79
//#define T_DVDC_SCAN					0x7A
//#define T_DVDC_A_B					0x7B
//#define T_DVDC_ZOOM					0X7C
//#define T_DVDC_TITLE				0x7D
//#define T_DVDC_SUB_T				0x7E

#define T_PAUSE		0x6A
#define T_STOP		0x70
#define T_PLAY		0x71
#define T_NEXT		0x72
#define T_PREV		0x73
#define T_FASTF		0x74
#define T_FASTB		0x75
#define T_REPEAT	0x76
#define T_SHUFFLE	0x77
#define T_SCAN		0x78
#define T_A_B		0x79
#define T_ZOOM		0X7A
#define T_TITLE		0x7B
#define T_SUB_T		0x7C
#define T_PBC		0x7D
#define T_ANGER		0x7E
#define T_STEP		0x7F
#define T_TRACK		0x80
#define T_FULL_SCREEN	0x81
#define T_PLAYPAUSE		0x82
#define T_OSD		0x83
#define T_10_ADD	0x84
#define T_SLOW		0x85

#define T_ITEM_ENTER         		0x87
#define T_ITEM_LEFT         		0x88
#define T_ITEM_RIGHT         		0x89
#define T_ITEM_BACK         		0x8A


#define K_EPG			0x90
#define K_TRANSFER		0x91

#define K_MUTE_MOMENT				0XA0
#define T_BLACKOUT					0xA1
#define T_NAVI_MUTE					0xA2
#define T_BEEP						0xA3
#define T_SELECT					0xA4
#define T_SETTING_RESET				0xA5
#define T_SEND_WHEEL_STUDY			0xA6
#define	T_WHEEL_STUDY_OK			0xA7		//
#define	T_WHEEL_STUDY_RESET			0xA8		//
#define T_SEL_EQ					0xA9		//EQ	


#define T_LOUDNESS					0xAA







#define T_IR_GOTO					0xB1
#define T_KEY_DISP					0xB2
#define T_REARVIEW					0XB3		//
/*High-order word表示Y坐标，
low-order word 表示X坐标	触摸屏坐标
按顺序接收p1 p2 p3 p4
x = p1 + (p2<<8);
y = p3 + (p4<<8);
*/
#define T_CAMERA_RECORD				0XB5		

#define T_CLOSE_DOG					0XB6	
#define T_MOUSE_POS					0XB4	

#define T_AUX_BACK					0xBA

#define T_WHEEL_ANGLE				0xF9

#define K_USER						0xFF
#define K_CUSTOM					0xFE

#define USER_WPARAM_ACCENT			0x10
#define USER_WPARAM_STERE			0x20







#define K_POWER_STATE				0xFB		//

#define K_BACKLIGHT_STATE			0xFC		//
	
#define K_SAVE_ALL					0xFD		


//-----------------------------------------------------------------------------------

//语言定义
#define  LANG_ID_INVALID				('a'*256+'0')
#define  LANG_ID_CHEROKEE				('a'*256+'0')


#define  LANG_ID_ARABIC					('a'*256+'r')
#define  LANG_ID_CHINESE				('z'*256+'h')
#define  LANG_ID_ENGLISH				('e'*256+'n')
#define  LANG_ID_JAPANESE				('j'*256+'a')
#define  LANG_ID_RUSSIAN				('r'*256+'u')
#define  LANG_ID_SPANISH				('e'*256+'s')
#define  LANG_ID_GERMAN					('d'*256+'e')
#define  LANG_ID_FRENCH					('f'*256+'r')
#define  LANG_ID_PORTUGUESE				('p'*256+'t')
#define  LANG_ID_CHINESE_TRADITIONAL	('a'*256+'2')
#define  LANG_ID_ITALIAN				('i'*256+'t')
#define  LANG_ID_PORTUGUESS_BRAZIL		('b'*256+'6')

//MCU初始化状态表
struct	MCU_INIT_INFO{
	WORD			nMaxPacklen;		//
	DWORD			dwMaxBaudRate;		//
	WORD			wVersion;			//
	BYTE			btAcState;			//
	SYSTEMTIME		time;
	BYTE			btBackSource;
	BYTE			bytExInfo[MCU_EX_INFO_LEN];
	BYTE			bytExLen;
};

struct	KeyOrContrl
{
	BYTE	btKey;				//按键类型
	BYTE	btParam1;			//按键参数第一个字节
	BYTE	btParam2;			//按键参数第二个字节
	BYTE	btParam3;			//按键参数第三个字节	
	BYTE	btParam4;			//按键参数第四个字节

	BYTE    btState;			//按键状态

	//WORD	wdLParam;			
	//DWORD	wdWParam;			//按键参数
};

struct	LBtnState
{
	BYTE	btFlag;
	unsigned short		x; 
	unsigned short		y; 
};

struct Spectrum{
	BYTE	btHZ0_60;
	BYTE	btHZ0_150;
	BYTE	btHZ0_400;
	BYTE	btHZ1K;
	BYTE	bkHZ2_5;
	BYTE    bkHZ6_3;
	BYTE	bkHZ16;
};


enum VFORMAT { V_NTSC,V_PAL,V_SECAM };
struct MCU_DEV_INFO
{
	BYTE		bECDC;				
	BOOL		bEDISC;				
	BOOL		bNoOutAutoIn;		//

	BOOL		bBack;				//	
	BOOL		bLightOn;			//
	BOOL		bNoBrake;			//
	BOOL		bTA;				//

	BYTE		bytStartSrc;		// 


				//外部视频状态
	BYTE		bMpegValid;
	BYTE		btMpegFormat;

	BOOL		bTVValid;			//视频是否有效
	BYTE		btTVFormat;			//制式

	BOOL		bCameraValid;		//视频是否有效
	BYTE		btCameraFormat;		//制式

	BOOL		bDVDCValid;			//视频是否有效
	BYTE		btDVDCFormat;		//制式

	BOOL		bAUX1_Valid;		//视频是否有效
	BYTE		btAUX1_CFormat;		//制式

	BOOL		bAUX2_Valid;		//视频是否有效
	BYTE		btAUX2_CFormat;		//制式

};

struct ARM_DEV_STATE{
	BOOL		bBTExit;
	BOOL		bCMMBExit;
	BOOL		bVideoExit;
	
	BOOL		bInCalling;
	BOOL		bNaviAudio;
	BOOL		bCameraVideo;
	BOOL		bBtMusicAudio;

	BOOL		bInTCMenu;
	BOOL		bForbidSwitcSrc;
	BOOL		bBacklightOff;
};


struct	Volume_Tab
{
	BYTE	btCurVol;
	BYTE	btRearVol;
	BYTE	btLR;
	BYTE	btFR;
	BOOL	bMute;
	BYTE	btNavVolAdd;
	BYTE	btCallVol;
	BOOL    bKeyBeepOn;
};

//--------------------------------------------------------------------------------------
//enum  EQ_TYPE { EQ_USER,EQ_POP,EQ_NORMAL,EQ_ROCK,EQ_CLASSIC,EQ_JAZZ,EQ_AUTO,EQ_TOTAL  };

struct EQ_DATA{
	BYTE	btBassValue;
	BYTE	btBassFreq;
	BYTE	btMidValue;
	BYTE	btMidFreq;
	BYTE	btTrebleValue;
	BYTE	btTrebleFreq;
};

struct EQ_Table{
	BYTE		btEQType;
	EQ_DATA		UserTab;
	EQ_DATA		PopTab;
	EQ_DATA		NorTab;
	EQ_DATA		RockTab;
	EQ_DATA		ClassicTab;
	EQ_DATA		JazzTab;
	EQ_DATA		AutoTab;

};

//--------------------------------------------------------------------------------------
//-------------------------------- RADIO 信息 ------------------------------------------

enum  RADIO_INFO_TYPE{ CUR_FREQ=0x00,PRESET_LIST=0x01,PS_PTY=0x02,RADIO_RT=0x03 };

enum RDS_STATUS	{ RDS_INFO,TP_INFO,TA_INFO,EWS_INFO,TA_SEEK,PTY_SEEK,AF_SEEK,STATUS_ALL };
enum RADIO_BAND { BAND_FM1,BAND_FM2,BAND_FM3,BAND_AM1,BAND_AM2,BAND_ALL };


#define PRESET_SZ		6
struct RADIOINFO
{
	BYTE	btType;    // 

	//
	BYTE	btCurBand;			//
	BYTE	btCurPreset;		//
	U16		nFreq;				//
	U16		nMaxFreq;			//，
	U16		nMinFreq;			
	BYTE	btGranularity;		
	BYTE	btSignal;			//

	BOOL	bSeekUp;			
	BOOL	bSeekDown;
	BOOL	bPreviewScan;		//
	BOOL	bAutoScan;			//
	BOOL	bStereo;			//
	BOOL	bUseStereo;			//
	BOOL	bManualAuto;		//
	BOOL	bLoc;				//
	
	BOOL	bBrowse;			//

	BYTE	btRDS;

	//PreSet List
	U16		nFM1Freq;
	U16		nFM1List[PRESET_SZ];
	U16		nFM2Freq;
	U16		nFM2List[PRESET_SZ];
	U16		nFM3Freq;
	U16		nFM3List[PRESET_SZ];
	U16		nAM1Freq;
	U16		nAM1List[PRESET_SZ];
	U16		nAM2Freq;
	U16		nAM2List[PRESET_SZ];

	//PS PTY
	BYTE	PS[8];
	BYTE	btPTY;	
	//RT
	BYTE	btRTIn;
	BYTE	RTInfo[64];

};

//---------------------------------------------------------------------------------
struct CDC_INFO{
	BYTE	btPlayMode;
	BYTE	btCurDiscNum;
	BYTE	btTotalNum;
	BYTE	btTotalMinute;
	BYTE	btTotalSecond;
	BYTE	btMusicIndex;
	BYTE	btCurMinute;
	BYTE	btCurSecond;
	BYTE	btChangerType;
	BYTE	btCDcapab;
	BYTE	btTip;
	BYTE	btTipTime;
	BYTE	btError;
	BYTE	btDvdcMode;
	BYTE	btMagzStatus;
	U16		btDiscPos;	
};

//---------------------------------------------------------------------------------
//参数设置
struct	MPEG_SET{
	WORD	wdOSDLan;
	WORD	wdVolLan;
	WORD	wdCaptionLan;
	WORD	wdCDMenuLan;
	BOOL	bDRCSet;
	BOOL	bStereo;
	BYTE	btSpeaker;
	BYTE	btPrologic;
	BYTE	btRegion;
	BOOL	bRear;
};
struct BT_SET{
	BOOL	bBTOn;
	BOOL	bAutoConnect;
	BOOL	bAutoAns;
	char	chParingCode[4];
};
//struct	TIME_SET{	}
struct USEFUL_SET{
	BOOL	bBeep;
	BOOL	bRed;
	BYTE	btBlackOut;
	BOOL    bFbdVdoInNav;
	//BYTE    btNavVolType;
	BOOL	bNavMixVol;
	BOOL	bNavWithVol;
	BOOL	bBackMute;

	//new add,skin index
	BOOL	btSkinIndex[4];
	BOOL	bNoAnyKeyPowerUp;

};

struct	LCD_ITEM{
	BYTE	btBright;
	BYTE	btContrast;
	BYTE	btHue;
	BYTE	btSaturation;


	BYTE    btBacklight;
	BYTE	btCameraMirror;
};


enum LCD_MODE { LCD_USER,LCD_STANDARD,LCD_SOFT,LCD_BRIGHTNESS,LCD_MODE_ALL };

struct LCDPARAM_TAB{
	BYTE			btCameraMirror;
	BYTE			btBacklight;
	BYTE			btVideoType;
	LCD_ITEM		mode[LCD_MODE_ALL];
};


struct	RADIO_SET{
	BYTE	btCurArea;
	BYTE	btDefArea;
	BYTE	btFMLocal;
	BYTE	btFMDistant;
	BYTE	btAMLocal;
	BYTE	btAMDistant;
	BOOL	bRemote;
};
struct RDS_SET{
	BYTE	btRDSType;
	BOOL	bCTOn;
	BOOL	bAFOn;
	BOOL	bTAOn;
	BOOL	bEONOn;
	BOOL	bRegionOn;
	BYTE	btTAVol;
	BYTE	btRDSAFLev;
};
struct MEDIA_SET{
	BYTE	btVideoInFormat;
	BOOL	bAspectNor;
	BOOL	bParingON;
	BOOL	bBrakeOve;
	BOOL	bCameralNor;

	BOOL	bDBB;
	BOOL	bSubSwitchOn;
	BOOL	bSubPhase0;
	BYTE	bSubWFilter;
	
};

struct TIME_SET{
	SYSTEMTIME		time;
	BOOL			b12Hour;
	BOOL			bShowTime;
};

//
struct RESERVE_ITEM{
	BYTE	btBright;
	BYTE	btContrast;
	BYTE	btColor;    //staturation
	BYTE	btHue;
	BOOL	bUseRadar;
	BOOL	bUseVideo;
};

enum { PARAM_MPEG,PARAM_BT,PARAM_TIME,PARAM_USEFUL,PARAM_LCD,PARAM_RADIO,PARAM_RDS,PARAM_MEDIA,PARAM_BACKVIDEO };
struct	SYSTEM_PARAM{
	BYTE			btParamType;

	MPEG_SET		mpeg;
	BT_SET			bt;

	TIME_SET		TimeSet;	

	USEFUL_SET		useful;

	//LCDPARAM_TAB	lcdTab;
	LCD_ITEM		LcdUserSet;

	
	RADIO_SET		radio;
	RDS_SET			rds;
	MEDIA_SET		media;

	RESERVE_ITEM	BackCarSet;
	//LCD_ITEM		BackVideoSet;
};

enum {PARAM_RADIO_MODULE,PARAM_RADIO_STOP,PARAM_CAN};

struct DVP_INFO
{
	BYTE	bytState;
	BYTE	bytMode;
	WORD	wdtotaltime;
	WORD	wdDuration;
	WORD	wdTotalCount;
	WORD    wdCurSong;

};

enum  WHELL_STUDY  { WHSTUDY_ENDINPUT,WHSTUDY_STARTINPUT,WHSTUDY_SAVEOK,WHSTUDY_RESET };
#define		MAX_SWC_KEY			12
struct SWC_TABLE{
	BYTE		KEY_FLAG[MAX_SWC_KEY];
	BYTE		KEY_IR[MAX_SWC_KEY];
};
