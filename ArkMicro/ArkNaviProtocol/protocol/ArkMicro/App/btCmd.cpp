static void RecCmd_BtState(BYTE *pBuf,BYTE len)
{
	static BOOL bInComming = FALSE;
	static BOOL bTalking = FALSE;
	int type;
	int btState = g_pSystemInfo->bt.btState;

	if(pBuf[0] == 'I')
	{
		switch(pBuf[1])
		{
			case 'S':	//上电初始化成功
				btState = CBtCtrl::BT_STATE_IDLE;
				break;
			case 'A':	//蓝牙已断开
				btState = CBtCtrl::BT_STATE_IDLE;
				break;
			case 'V':	//连接中
				btState = CBtCtrl::BT_STATE_CONNECTING;
				break;
			case 'B':	//蓝牙已连接
				btState = CBtCtrl::BT_STATE_CONNECTED;
				g_pTimer->SetTimer(TIMER_ID_SYNC_PHONEBOOK,5000);	//下载电话本
				break;
			case 'C':	//去电
				btState = CBtCtrl::BT_STATE_OUTGOING;
				break;
			case 'D':	//来电
				if(len > 4)
				{
					for(int i=2;i<len-2;i++)
					{
						g_pSystemInfo->bt.callNumber[i-2] = pBuf[i];
					}
					GetContractName(g_pSystemInfo->bt.callNumber,g_pSystemInfo->bt.callName);
				}
				btState = CBtCtrl::BT_STATE_INCOMING;
				break;
			case 'R':	//打出电话或通话中电话号码.
				if(len > 4)
				{
					for(int i=2;i<len-2;i++)
					{
						g_pSystemInfo->bt.callNumber[i-2] = pBuf[i];
					}
					GetContractName(g_pSystemInfo->bt.callNumber,g_pSystemInfo->bt.callName);
				}
				break;
			case 'G':	//通话中
				btState = CBtCtrl::BT_STATE_TALKING;
				break;
			case 'F':	//挂机
				btState = CBtCtrl::BT_STATE_CONNECTED;
				break;
			case 'I':
				break;
			default:
				break;
		}
	}
	return;
}

static void RecCmd_RemoteAddr(BYTE *pBuf,BYTE len)
{
	if(len > 4)
	{
		ZeroMemory(g_pSystemInfo->bt.remoteAddr,sizeof(g_pSystemInfo->bt.remoteAddr));
		for(int i=2;i<len-2;i++)
		{
			g_pSystemInfo->bt.remoteAddr[i-2] = pBuf[i];
		}
	}
	return;
}

static void RecCmd_RemoteName(BYTE *pBuf,BYTE len)
{
	if(len > 4)
	{
		char remoteName[MAX_PATH];
		ZeroMemory(remoteName,sizeof(remoteName));
		ZeroMemory(g_pSystemInfo->bt.remoteName,sizeof(g_pSystemInfo->bt.remoteName));
		for(int i=2;i<len-2;i++)
		{
			remoteName[i-2] = pBuf[i];
		}
		UTF8ToUnicode(remoteName,g_pSystemInfo->bt.remoteName);
	}
	return;
}

static void RecCmd_LocalName(BYTE *pBuf,BYTE len)
{
	if(len > 4)
	{
		ZeroMemory(g_pSystemInfo->bt.localName,sizeof(g_pSystemInfo->bt.localName));
		for(int i=2;i<len-2;i++)
		{
			g_pSystemInfo->bt.localName[i-2] = pBuf[i];
		}
	}
	return;
}

static void RecCmd_PinCode(BYTE *pBuf,BYTE len)
{
	if(len > 4)
	{
		ZeroMemory(g_pSystemInfo->bt.pinCode,sizeof(g_pSystemInfo->bt.pinCode));
		for(int i=2;i<len-2;i++)
		{
			g_pSystemInfo->bt.pinCode[i-2] = pBuf[i];
		}
	}
	return;
}

static void RecCmd_LocalAddr(BYTE *pBuf,BYTE len)
{
	if(len > 4)
	{
		ZeroMemory(g_pSystemInfo->bt.localAddr,sizeof(g_pSystemInfo->bt.localAddr));
		for(int i=2;i<len-2;i++)
		{
			g_pSystemInfo->bt.localAddr[i-2] = pBuf[i];
		}
	}
}

static void RecCmd_BtMusicState(BYTE *pBuf,BYTE len)
{
	switch(pBuf[1])
	{
		case 'A' :
			g_pSystemInfo->bt.btmState = CBtCtrl::BTM_STATE_CONNECTED;
			break;
		case 'B' :
			g_pSystemInfo->bt.btmState = CBtCtrl::BTM_STATE_PLAY;
			break;
		case 'C' :
			g_pSystemInfo->bt.btmState = CBtCtrl::BTM_STATE_CONNECTED;
			break;
		case 'D' :
			g_pSystemInfo->bt.btmState = CBtCtrl::BTM_STATE_IDLE;
			break;
		case 'Y' :
			g_pSystemInfo->bt.btmState = CBtCtrl::BTM_STATE_IDLE;
			break;
	}
	return;
}

static void RecCmd_PhoneBook(BYTE *pBuf,BYTE len)
{
	lastRecDataTime = timeGetTime();
	if(len < 6)
	{
		return;
	}
	char szName[100],szNumber[100];
	TCHAR strName[100],strNumber[100];
	ZeroMemory(szName,sizeof(szName));
	ZeroMemory(szNumber,sizeof(szNumber));
	ZeroMemory(strName,sizeof(strName));
	ZeroMemory(strNumber,sizeof(strNumber));

	for(int i=2;i<len-2;i++)
	{
		if(pBuf[i] == 0xFF)
		{
			for(int j=i+1;j<len-2;j++)
			{
				szNumber[j-i-1] = pBuf[j];
			}
			UTF8ToUnicode(szName,strName);
			UTF8ToUnicode(szNumber,strNumber);
			g_pBtBook->AddBook(strName,strNumber);
			break;
		}
		szName[i-2] = pBuf[i];
	}
	return;
}

static void RecCmd_PhoneBookEnd(BYTE *pBuf,BYTE len)
{
	return;
}

static void RecCmd_PhoneRecord(BYTE *pBuf,BYTE len)
{
	lastRecDataTime = timeGetTime();
	if(len < 6)
	{
		return;
	}
	char szName[100],szNumber[100];
	TCHAR strName[100],strNumber[100];
	
	ZeroMemory(szName,sizeof(szName));
	ZeroMemory(szNumber,sizeof(szNumber));
	ZeroMemory(strName,sizeof(strName));
	ZeroMemory(strNumber,sizeof(strNumber));

	for(int i=2;i<len-2;i++)
	{
		if(pBuf[i] == 0xFF)
		{
			for(int j=i+1;j<len-2;j++)
			{
				szNumber[j-i-1] = pBuf[j];
			}
			UTF8ToUnicode(szName,strName);
			UTF8ToUnicode(szNumber,strNumber);
			int type;
			SYSTEMTIME startTime = {0};
			switch(syncType)
			{
				case SYNC_RECORD_DIALED:
					type = CBtCtrl::CALL_RECORD_DIALED;
					break;
				case SYNC_RECORD_RECEIVE:
					type = CBtCtrl::CALL_RECORD_RECEIVE;
					break;
				case SYNC_RECORD_MISS:
					type = CBtCtrl::CALL_RECORD_MISS;
					break;
				default:
					CLog::E(GetTagName(),_T("Unknow record type %d\r\n"),syncType);
					break;
			}
			g_pBtBook->AddRecord(type,strName,strNumber,startTime,0);
			break;
		}
		szName[i-2] = pBuf[i];
	}
	return;
}

typedef struct
{
	char *strCmd;
	void (*pProcCmd)(BYTE *pBuf,BYTE len);
}S_BT_PROC_CMD;

static S_BT_PROC_CMD btProcCmdTab[]=
{
	"IS",RecCmd_BtState,
	"IA",RecCmd_BtState,
	"IV",RecCmd_BtState,
	"IB",RecCmd_BtState,
	"IC",RecCmd_BtState,
	"ID",RecCmd_BtState,
	"IR",RecCmd_BtState,
	"IG",RecCmd_BtState,
	"IF",RecCmd_BtState,
	"II",RecCmd_BtState,
	//"MG",RecCmd_BtState,
	
	"AD",RecCmd_RemoteAddr,
	"SA",RecCmd_RemoteName,
	"MM",RecCmd_LocalName,
	"MN",RecCmd_PinCode,
	"MZ",RecCmd_LocalAddr,
	
	"MA",RecCmd_BtMusicState,
	"MB",RecCmd_BtMusicState,
	"MC",RecCmd_BtMusicState,
	"MD",RecCmd_BtMusicState,
	"MY",RecCmd_BtMusicState,

	"PB",RecCmd_PhoneBook,
	"PC",RecCmd_PhoneBookEnd,
	"PD",RecCmd_PhoneRecord
};