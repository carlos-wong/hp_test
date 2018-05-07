#include "stdafx.h"
#include "Protocol.h"

//接收到MCU的命令
static void RecCmd_SendDevState(BYTE *pBuf,BYTE len)
{
	MCU_DEV_INFO RdevInfo;
	BYTE *pData = &pBuf[4];
	BYTE dataLen = len - 4;
	
	RdevInfo.bECDC			= (*pData)&0x01;
	RdevInfo.bEDISC			= (*pData)&0x02;
	RdevInfo.bNoOutAutoIn	= ((*pData)&0x04) !=0;


	RdevInfo.bBack = (*(pData+1))&0x01;				//	
	RdevInfo.bLightOn = ((*(pData+1))&0x02)!=0;		//
	RdevInfo.bNoBrake = ((*(pData+1))&0x04)!=0 ;	//
	RdevInfo.bTA = (*(pData+2))&0x01;				//

	RdevInfo.bytStartSrc	=	pData[3];			//


	//外部视频源状态
	BYTE btData = pData[4];
	RdevInfo.bMpegValid = btData&0x0F;
	RdevInfo.btMpegFormat = (btData&0xF0)>>4;

	btData = pData[5];
	RdevInfo.bTVValid = btData&0x0F;
	RdevInfo.btTVFormat = (btData&0xF0)>>4;

	btData = pData[6];
	RdevInfo.bCameraValid = btData&0x0F;
	RdevInfo.btCameraFormat = (btData&0xF0)>>4;

	btData = pData[7];
	RdevInfo.bDVDCValid = btData&0x0F;
	RdevInfo.btDVDCFormat = (btData&0xF0)>>4;

	btData = pData[8];
	RdevInfo.bAUX1_Valid = btData&0x0F;
	RdevInfo.btAUX1_CFormat = (btData&0xF0)>>4;

	btData = pData[8];
	RdevInfo.bAUX2_Valid = btData&0x0F;
	RdevInfo.btAUX2_CFormat = (btData&0xF0)>>4;


}

static void RecCmd_SendChangeSource(BYTE *pBuf,BYTE len)
{
	
}

static void RecCmd_SendVolChange(BYTE *pBuf,BYTE len)
{
	
}

static void RecCmd_SendAudioEQ(BYTE *pBuf,BYTE len)
{
}

static void RecCmd_SendSpectrum(BYTE *pBuf,BYTE len)
{
}

static void RecCmd_SendKeyAncControl(BYTE *pBuf,BYTE len)
{
	static DWORD lastEjectTime = 0;
	
	KeyOrContrl		g_keyctrlRp;
	BYTE *pData = &pBuf[4];
	BYTE dataLen = len - 4;
	
	g_keyctrlRp.btState  = pData[0];
	g_keyctrlRp.btKey	 = pData[1];
	g_keyctrlRp.btParam1 = pData[2];
	g_keyctrlRp.btParam2 = pData[3];
	g_keyctrlRp.btParam3 = pData[4];
	g_keyctrlRp.btParam4 = pData[5];


}	

static void RecCmd_SendRadioChange(BYTE *pBuf,BYTE len)
{
	RADIOINFO radioInfo;
	BYTE *pData = &pBuf[4];
	BYTE dataLen = pBuf[3];
	
	radioInfo.btType = pData[0];
	if (CUR_FREQ==radioInfo.btType)
	{
		radioInfo.nMinFreq = MAKEWORD(pData[2], pData[1]);
		radioInfo.nMaxFreq = MAKEWORD(pData[4], pData[3]);
		radioInfo.btGranularity = pData[5];
		radioInfo.btSignal = pData[6];

		WORD wdRadioState = MAKEWORD(pData[7],pData[8]);
		radioInfo.bSeekUp      = wdRadioState&0x01;
		radioInfo.bSeekDown	   = (wdRadioState&(1<<1))!= 0;
		radioInfo.bPreviewScan = (wdRadioState&(1<<2))!= 0;
		radioInfo.bAutoScan	   = (wdRadioState&(1<<3))!= 0;
		radioInfo.bStereo	   = (wdRadioState&(1<<4))!= 0;

		radioInfo.bUseStereo   = (wdRadioState&(1<<5)) != 0;
		radioInfo.bManualAuto  = (wdRadioState&(1<<6)) != 0;
		radioInfo.bLoc		   = (wdRadioState&(1<<7)) != 0;
		radioInfo.bBrowse      = (wdRadioState&(1<<8)) != 0;

		BYTE btRDS = pData[9];
		radioInfo.btRDS = btRDS;
	}
	else if (PRESET_LIST==radioInfo.btType)
	{
		U16* pnFrep = &radioInfo.nFM1Freq;
		U16* pnFM1List = radioInfo.nFM1List;
		radioInfo.btCurBand = pData[1];		
		radioInfo.btCurPreset = pData[2];	
		switch(radioInfo.btCurBand){
		case BAND_FM1:
			break;
		case BAND_FM2:
			pnFrep = &radioInfo.nFM2Freq;
			pnFM1List = radioInfo.nFM2List;
			break;
		case BAND_FM3:
			pnFrep = &radioInfo.nFM3Freq;
			pnFM1List = radioInfo.nFM3List;
			break;
		case BAND_AM1:
			pnFrep = &radioInfo.nAM1Freq;
			pnFM1List = radioInfo.nAM1List;
			break;
		case BAND_AM2:
			pnFrep = &radioInfo.nAM2Freq;
			pnFM1List = radioInfo.nAM2List;
			break;
		};	

		*pnFrep = MAKEWORD(pData[4], pData[3]);
		radioInfo.nFreq = *pnFrep;

		for(int i=0;i<PRESET_SZ;i++)
			pnFM1List[i] = MAKEWORD(pData[i*2+6], pData[i*2+5]);

	}
	else if (PS_PTY==radioInfo.btType)
	{
		memcpy(radioInfo.PS,pData+1,8);
		radioInfo.btPTY = pData[9];
	}
	else if (RADIO_RT==radioInfo.btType)
	{
		radioInfo.btRTIn = pData[1];
	}
}

static void RecCmd_SendCDCChange(BYTE *pBuf,BYTE len)
{
}	

static void RecCmd_SendSysParamChange(BYTE *pBuf,BYTE len)
{
	SYSTEM_PARAM sysSet;
	BYTE *pData = &pBuf[4];
	BYTE dataLen = len - 4;

	BYTE btParam;
	sysSet.btParamType = pData[0];	
	if (PARAM_MPEG==sysSet.btParamType){
		if (dataLen<12)
			return;

		sysSet.mpeg.wdOSDLan = MAKEWORD(pData[2],pData[1]);
		sysSet.mpeg.wdVolLan = MAKEWORD(pData[4],pData[3]);
		sysSet.mpeg.wdCaptionLan = MAKEWORD(pData[6],pData[5]);
		sysSet.mpeg.wdCDMenuLan = MAKEWORD(pData[8],pData[7]);
	
		btParam = pData[9];
		sysSet.mpeg.bDRCSet = btParam&0x01;
		sysSet.mpeg.bStereo = btParam&0x02;
		sysSet.mpeg.btSpeaker = (btParam&0x1C)>>2;
		sysSet.mpeg.btPrologic = (btParam&0x70)>>4;
		sysSet.mpeg.btRegion = pData[10];	
		sysSet.mpeg.bRear = pData[11];

	}
	else if (PARAM_BT==sysSet.btParamType){
		if (dataLen<4)
			return;

		btParam = pData[1];
		char chParBCD[8] = {0};
		sysSet.bt.bBTOn = (btParam&0x01);
		sysSet.bt.bAutoConnect = (btParam&0x02);
		sysSet.bt.bAutoAns = (btParam&0x04);
		WORD wdBCD = (((WORD)pData[2])<<8) + pData[3];
		sprintf(chParBCD,"%04x",wdBCD);
		memcpy(sysSet.bt.chParingCode,chParBCD,4);	
	}
	else if (PARAM_TIME==sysSet.btParamType){
		if (dataLen<8)
			return;

		BcdToTime(pData+1,sysSet.TimeSet.time);
		BYTE btTimeSet = pData[8];
		sysSet.TimeSet.b12Hour = btTimeSet&0x01;
		sysSet.TimeSet.bShowTime = (btTimeSet&0x02)!=0;
	}
	else if (PARAM_USEFUL==sysSet.btParamType){
		btParam = pData[1];
		sysSet.useful.bBeep = btParam&0x01;
		sysSet.useful.bRed = btParam&0x02;
		sysSet.useful.btBlackOut = (btParam&0xC)>>2;
		sysSet.useful.bFbdVdoInNav = (btParam&(1<<4))!=0;
		sysSet.useful.bNavMixVol = (btParam &(1<<5))!=0;
		sysSet.useful.bNavWithVol = (btParam &(1<<6))!=0;

		btParam = pData[2];
		sysSet.useful.bNoAnyKeyPowerUp = (btParam&(1<<4))!=0;
	}
	else if (PARAM_LCD==sysSet.btParamType)
	{
		sysSet.LcdUserSet.btBright	 = pData[1];
		sysSet.LcdUserSet.btContrast = pData[2];
		sysSet.LcdUserSet.btHue = pData[3];
		sysSet.LcdUserSet.btSaturation = pData[4];
		sysSet.LcdUserSet.btBacklight  = pData[5];
		sysSet.LcdUserSet.btCameraMirror = min(3, pData[6]);

	}
	else if (PARAM_RADIO==sysSet.btParamType){
		btParam = pData[1];
		sysSet.radio.btCurArea = btParam&0x0F;
		sysSet.radio.btDefArea = (btParam&0xF0)>>4;
		sysSet.radio.btFMLocal = pData[2];
		sysSet.radio.btFMDistant = pData[3];
		sysSet.radio.btAMLocal = pData[4];
		sysSet.radio.btAMDistant = pData[5];
		sysSet.radio.bRemote = pData[6];
	}
	else if (PARAM_RDS==sysSet.btParamType){
		btParam = pData[1];
		sysSet.rds.btRDSType = btParam&0x3;
		sysSet.rds.bCTOn = btParam&0x04;
		sysSet.rds.bAFOn = btParam&0x08;
		sysSet.rds.bTAOn = btParam&0x10;
		sysSet.rds.bEONOn = btParam&0x20;
		sysSet.rds.bRegionOn = btParam&0x40;
		sysSet.rds.btTAVol = pData[2];
		sysSet.rds.btRDSAFLev = pData[3];
	}
	else if (PARAM_MEDIA==sysSet.btParamType){
		btParam = pData[1];
		sysSet.media.btVideoInFormat = btParam&0x3;
		sysSet.media.bAspectNor = btParam&0x04;
		sysSet.media.bParingON = btParam&0x08;
		sysSet.media.bBrakeOve = btParam&0x10;
		sysSet.media.bCameralNor = btParam&0x20;
		btParam = pData[2];
		sysSet.media.bDBB = btParam&0x01;
		sysSet.media.bSubSwitchOn = btParam&0x02;
		sysSet.media.bSubPhase0 = btParam&0x04;
		sysSet.media.bSubWFilter = (btParam&0x38)>>3;
	}
	else if (PARAM_BACKVIDEO==sysSet.btParamType)
	{
		sysSet.BackCarSet.btBright	 = pData[1];
		sysSet.BackCarSet.btContrast = pData[2];
		sysSet.BackCarSet.btHue 	 = pData[3];
		sysSet.BackCarSet.btColor	 = pData[4];

		sysSet.BackCarSet.bUseVideo = pData[5]&0X01;
		sysSet.BackCarSet.bUseRadar = (pData[5]&0X02)!=0;
	}
}

static void RecCmd_SendSWCTable(BYTE *pBuf,BYTE len)
{
	SWC_TABLE keytab;
	BYTE *pData = &pBuf[4];
	BYTE dataLen = len - 4;
	for ( int i=0;i<(dataLen)/2 && i<MAX_SWC_KEY;i++ )
	{
		keytab.KEY_FLAG[i] = *(pData+i*2);
		keytab.KEY_IR[i] = *(pData+i*2+1);
	}
}

static void RecCmd_SendWheelStudyResult(BYTE *pBuf,BYTE len)
{
}

static void RecCmd_GetSourceState(BYTE *pBuf,BYTE len)
{
}

static void RecCmd_SendACCChange(BYTE *pBuf,BYTE len)
{
	BYTE *pData = &pBuf[4];
	BYTE dataLen = len - 4;
	BOOL bAccState = *(pData);


}

static void RecCmd_SendExCustonInfo(BYTE *pBuf,BYTE len)
{
}

static void RecCmd_SendEx2CustonState(BYTE *pBuf,BYTE len)
{
}

static void RecCmd_SendBtCmd(BYTE *pBuf,BYTE len)
{
	BYTE *pData = &pBuf[4];
	BYTE dataLen = len - 5;
	OnRecBtCmd(pData,dataLen);
}

//接收到MCU的应答
static void RecAns_SendHandShake(BYTE *pBuf,BYTE len)
{
}

static void RecAns_SetBaudRate(BYTE *pBuf,BYTE len)
{
}

static void RecAns_GetMcuInitInfo(BYTE *pBuf,BYTE len)
{
	MCU_INIT_INFO info;
	BYTE *pData = &pBuf[5];
	BYTE dataLen = len - 5;
	
	info.nMaxPacklen = 	MAKEWORD( pData[1], pData[0] );

	WORD wdhi  = MAKEWORD( pData[3], pData[2] );
	WORD wdLow = MAKEWORD( pData[5], pData[4] );			 
	info.dwMaxBaudRate = ( (((DWORD)wdhi)<<16) ) | wdLow;    //&0xFFFF0000

	//协议版本号
	info.wVersion =  MAKEWORD( pData[7], pData[6] ); 
	if (dataLen <= 7)
		return;

	info.btAcState = *(pData+8);
	
	if (dataLen < 7+8)
		return;

	BcdToTime(pData+9,info.time);
	
	//BYTE bitY1 = *(pData+9);
	//BYTE bitY2 = *(pData+10);
	//info.time.wYear = (( bitY1>>4)&0x0F)*10 + bitY1&0x0F;
	//info.time.wYear =  info.time.wYear*100+ (( bitY2>>4)&0x0F)*10 + bitY2&0x0F;
	//BYTE bittemp = *(pData+11);
	//info.time.wMonth = (( bittemp>>4)&0x0F)*10 + bittemp&0x0F;
	//bittemp = *(pData+12);
	//info.time.wDay = (( bittemp>>4)&0x0F)*10 + bittemp&0x0F;

	//bittemp = *(pData+13);
	//info.time.wHour = (( bittemp>>4)&0x0F)*10 + bittemp&0x0F;
	//bittemp = *(pData+14);
	//info.time.wMinute = (( bittemp>>4)&0x0F)*10 + bittemp&0x0F;
	//bittemp = *(pData+15);
	//info.time.wSecond = (( bittemp>>4)&0x0F)*10 + bittemp&0x0F;

	if (dataLen <= 7+8)
		return;

	info.btBackSource = *(pData+16);

	info.bytExLen = dataLen - 17;
	info.bytExLen =  min( info.bytExLen,MCU_EX_INFO_LEN );
	LPBYTE PbytEx = pData+17;
	
	memcpy( info.bytExInfo,PbytEx, info.bytExLen );
}

static void RecAns_StateConfig(BYTE *pBuf,BYTE len)
{
}

static void RecAns_GetDeviceState(BYTE *pBuf,BYTE len)
{
	BYTE *pBuffer = new BYTE[len-1];
	CopyMemory(pBuffer,pBuf,4);
	CopyMemory(pBuffer+4,pBuf+5,len-5);
	RecCmd_SendDevState(pBuffer,len-1);
	delete pBuffer;
}

static void RecAns_SendDeviceState(BYTE *pBuf,BYTE len)
{
}

static void RecAns_SendChangeSource(BYTE *pBuf,BYTE len)
{
}

static void RecAns_GetVolume(BYTE *pBuf,BYTE len)
{
}

static void RecAns_SetVolume(BYTE *pBuf,BYTE len)
{
}

static void RecAns_GetAudioEQ(BYTE *pBuf,BYTE len)
{
}

static void RecAns_SetAudioEQ(BYTE *pBuf,BYTE len)
{
}

static void RecAns_SendKey(BYTE *pBuf,BYTE len)
{
}

static void RecAns_SendTouch(BYTE *pBuf,BYTE len)
{
}

static void RecAns_GetRadioInfo(BYTE *pBuf,BYTE len)
{
	BYTE *pBuffer = new BYTE[len-1];
	CopyMemory(pBuffer,pBuf,4);
	CopyMemory(pBuffer+4,pBuf+5,len-5);
	RecCmd_SendRadioChange(pBuffer,len-1);
	delete pBuffer;
	
	RADIOINFO radioInfo;
	BYTE *pData = &pBuf[5];
	BYTE dataLen = len - 5;
	
	radioInfo.btType = pData[0];
}

static void RecAns_GetCDCInfo(BYTE *pBuf,BYTE len)
{
}

static void RecAns_GetSystemParam(BYTE *pBuf,BYTE len)
{
	BYTE *pBuffer = new BYTE[len-1];
	CopyMemory(pBuffer,pBuf,4);
	CopyMemory(pBuffer+4,pBuf+5,len-5);
	RecCmd_SendSysParamChange(pBuffer,len-1);
	delete pBuffer;
	
	SYSTEM_PARAM sysSet;
	BYTE *pData = &pBuf[5];
	BYTE dataLen = len - 5;
	BYTE btParam;
	sysSet.btParamType = pData[0];	
}

static void RecAns_SetSystemParam(BYTE *pBuf,BYTE len)
{
}

static void RecAns_GetSWCTable(BYTE *pBuf,BYTE len)
{
	BYTE *pBuffer = new BYTE[len-1];
	CopyMemory(pBuffer,pBuf,4);
	CopyMemory(pBuffer+4,pBuf+5,len-5);
	RecCmd_SendSWCTable(pBuffer,len-1);
	delete pBuffer;
}

static void RecAns_SetSWCTable(BYTE *pBuf,BYTE len)
{
}

static void RecAns_ReqWheelStudy(BYTE *pBuf,BYTE len)
{
}

static void RecAns_GetMcuVersion(BYTE *pBuf,BYTE len)
{
	BYTE *pData = &pBuf[5];
	BYTE dataLen = len - 5;

	ZeroMemory(g_pSystemInfo->mcu.mcuVersion,sizeof(g_pSystemInfo->mcu.mcuVersion));
	for(int i=0;i<dataLen-1;i++)
	{
		g_pSystemInfo->mcu.mcuVersion[i] = pData[i];
	}
}

static void RecAns_ExtCustomInfo(BYTE *pBuf,BYTE len)
{
}

static void RecAns_Ext2CustomState(BYTE *pBuf,BYTE len)
{
}

static void RecAns_SendDvpPlayState(BYTE *pBuf,BYTE len)
{
}

typedef struct
{
	BYTE cmd;
	void (*pProcCmd)(BYTE *pBuf,BYTE len);
}S_MCU_PROC_CMD;

//命令处理函数
static S_MCU_PROC_CMD mcuProcCmdTab[]=
{
	CMD_ID_GET_REPORT_STATE,RecCmd_SendDevState,
	CMD_ID_RREQUEST_SOURCE_CHANGE,RecCmd_SendChangeSource,		//无用
	CMD_ID_RREPORT_VOLCHANGE,RecCmd_SendVolChange,		//无用
	CMD_ID_RREPORT_AUDIO_EQ,RecCmd_SendAudioEQ,		//无用
	CMD_ID_RREPORT_SPECTRUM,RecCmd_SendSpectrum,		//无用
	CMD_ID_REMOTE_KEY,RecCmd_SendKeyAncControl,
	CMD_ID_RREPORT_RADIO_INFO,RecCmd_SendRadioChange,
	CMD_ID_RREPORT_CDC_INFO,RecCmd_SendCDCChange,		//无用
	CMD_ID_RREPORT_SYSTEMPARAM,RecCmd_SendSysParamChange,
	CMD_ID_RREPORT_SWC_TABLE,RecCmd_SendSWCTable,
	CMD_ID_WHEEL_RESULT,RecCmd_SendWheelStudyResult,
	CMD_ID_RQUERY_SOURCE_STATE,RecCmd_GetSourceState,
	CMD_ID_ACC_STATE_CHANGE,RecCmd_SendACCChange,
	CMD_ID_EX_RRPORTCUSTOMINFO,RecCmd_SendExCustonInfo,
	CMD_ID_EX2_RRPORTCUSTOMSTATE,RecCmd_SendEx2CustonState,
	CMD_ID_SEND_BT_CMD,RecCmd_SendBtCmd
};

//应答处理函数
static S_MCU_PROC_CMD mcuProcAnsTab[]=
{
	CMD_ID_HANDSHAKE,RecAns_SendHandShake,
	CMD_ID_BAUDRATE,RecAns_SetBaudRate,
	CMD_ID_INIT,RecAns_GetMcuInitInfo,
	CMD_ID_STATE_CFG,RecAns_StateConfig,		//无用
	CMD_ID_QUERY_STATE,RecAns_GetDeviceState,
	CMD_ID_REPORT_STATE,RecAns_SendDeviceState,
	CMD_ID_SOURCE_CHANGE,RecAns_SendChangeSource,
	CMD_ID_GETVOLUMN,RecAns_GetVolume,			//无用
	CMD_ID_SETVOLUMN,RecAns_SetVolume,
	CMD_ID_GETAUDIO_EQ,RecAns_GetAudioEQ,		//无用
	CMD_ID_SETAUDIO_EQ,RecAns_SetAudioEQ,		//无用
	CMD_ID_KEYBOARD,RecAns_SendKey,
	CMD_ID_LBTNDOWN,RecAns_SendTouch,		//无用
	CMD_ID_RADIO_INFO,RecAns_GetRadioInfo,
	CMD_ID_QUERY_CDC_INFO,RecAns_GetCDCInfo,			//无用
	CMD_ID_QUERY_SYSTEMPARAM,RecAns_GetSystemParam,
	CMD_ID_SET_SYSTEMPARAM,RecAns_SetSystemParam,
	CMD_ID_QUERY_SWC_TABLE,RecAns_GetSWCTable,
	CMD_ID_SETREMOTE_SWC_TABLE,RecAns_SetSWCTable,
	CMD_ID_WHEEL_STUDY,RecAns_ReqWheelStudy,
	CMD_ID_QUERY_MCU_VERSION,RecAns_GetMcuVersion,
	CMD_ID_EX_SETCUSTOMINFO,RecAns_ExtCustomInfo,
	CMD_ID_EX2_CUSTOMSTATE,RecAns_Ext2CustomState,
	CMD_ID_NOTIFY_MEDIA_STATE,RecAns_SendDvpPlayState
};