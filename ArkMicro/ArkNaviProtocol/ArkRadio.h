#ifndef ARKRADIO_H
#define ARKRADIO_H

enum ArkRadioType {
    ART_Undefine = -1,
    ART_QueryFreqInfo,              //查询频率信息
    ART_QueryPresetListInfo,        //查询预存台列表信息
    ART_GetMinFreq,					//获取当前波段最小频点，参数两个字节表示频点值，第一个字节为高字节，第二个字节为低字节
    ART_GetMaxFreq,					//获取当前波段最大频点，参数同上
    ART_GetGranularity,				//获取步长，参数一个字节
    ART_GetSignalIntensity,			//获取信号强度，参数一个字节
    ART_GetStatus,					//获取当前状态，参数一个字节，值为enum ArkRadioStatus
    ART_GetCurfreqMono,				//当前频点声音为单通道
    ART_GetCurfreqStereo,			//当前频点声音为立体声
    ART_GetUseMono,					//使用单通道播放
    ART_GetUseStereo,               //使用立体声播放
    ART_GetAutoScan,				//搜台模式为自动，自动必须扫到一个台才停止
    ART_GetManualScan,				//搜台模式为手动，手动只跳一步
    ART_GetLOC,						//本地搜台
    ART_GetDX,						//远程搜台
    ART_GetBand,					//获取当前波段，参数一个字节，值为０～４，分别对应FM1,FM2,FM3,AM1,AM2
    ART_GetPreset,                  //当前频点是不是在预设值，参数一个字节，值为０～６，０表示不在预设值内，１～６对应６个预设值
    ART_GetFreq,                    //获取当前频点，参数两个字节表示频点值，第一个字节为高字节，第二个字节为低字节
    ART_GetPresetList,              //获取预设值列表，参数十二个字节，每两个字节对应一个频点，第一个字节为高字节，第二个字节为低字节

    ART_KEY = 100,
    ART_Key_1,                      //播放当前波段列表索引
    ART_Key_2,                      //播放当前波段列表索引
    ART_Key_3,                      //播放当前波段列表索引
    ART_Key_4,                      //播放当前波段列表索引
    ART_Key_5,                      //播放当前波段列表索引
    ART_Key_6,                      //播放当前波段列表索引
    ART_KEY_Band,                   //FM1->FM2->FM3->AM1->AM2 band switch
    ART_KEY_SeekDown,               //从当前频点往后搜台，收到台后停止
    ART_KEY_SeekUp,                 //从当前频点往前搜台，收到台后停止
    ART_KEY_TuningDown,             //向后微调
    ART_KEY_TuningUp,               //向前微调
    ART_KEY_AutoScan,               //自动搜台
    ART_KEY_PS,                     //预存台浏览
    ART_KEY_LOC,                    //远程/本地
    ART_KEY_TA ,                    //Traffic Announcement (RDS information)
    ART_KEY_PTY,                    //Program TYpe (RDS information)
    ART_KEY_AF,                     //Automatic Follow (RDS information)
    ART_KEY_REG,
    ART_KEY_CT,
    ART_KEY_EON,                    //增强其他网络信息(RDS)
    ART_KEY_RDS,
    ART_KEY_RDSType,
    ART_KEY_FMFreq,					//设置FM频点,参数两个字节表示频点值，第一个字节为高字节，第二个字节为低字节
    ART_KEY_AMFreq,					//设置AM频点，参数同上
    ART_KEY_PresetSave,				//保存当前频点为预设值，参数一个字节，值为１～６，表示保存到哪个预设值
    ART_KEY_PresetLoad,				//加载预设值到当前频点，参数一个字节，值为１～６，表示要加载哪个预设值
    ART_KEY_PtySeek,
    ART_KEY_StereoStateChange,      //收音界面立体声状态切换
    ART_KEY_Smeter,
};

enum ArkRadioStatus {
    ARS_Undefine = -1,
    ARS_IDLE,
    ARS_INIT,
    ARS_SEEK_UP,
    ARS_SEEK_DOWN,
    ARS_AUTOSTORE,
    ARS_BROWSE_PRESTORE,
    ARS_SCAN,
};

enum ArkRadioBand {
    ARB_Undefine = -1,
    ARB_FM1,
    ARB_FM2,
    ARB_FM3,
    ARB_AM1,
    ARB_AM2,
    ARB_LAST,
};

struct ArkRadio {
    enum ArkRadioType type;
    unsigned char length;
    unsigned char* data;
};

#define RADIO_PRESET_NUM    6
struct RadioInfo {
    int min_freq;
    int max_freq;
    int granularity;
    int signal_intensity;
    enum ArkRadioStatus status;
    int curfreq_stereo;
    int use_stereo;
    int scanmode;
    int loc_dx;
    enum ArkRadioBand current_band;
    int current_preset;
    int current_freq;
    int preset_list[RADIO_PRESET_NUM];
};

#endif //ARKRADIO_H
