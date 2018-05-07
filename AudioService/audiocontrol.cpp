#include "audiocontrol.h"

AudioControl::AudioControl()
{
}

int AudioControl::Set(int Audio_Type, int nValue)
{
#ifndef __x86_64__
    snd_ctl_elem_id_t *pid = NULL;
    snd_ctl_elem_info_t *pinfo = NULL;
    snd_ctl_elem_value_t *pcontrol = NULL;
    snd_hctl_elem_t *pelem = NULL;
    snd_ctl_elem_id_alloca(&pid);
    snd_ctl_elem_info_alloca(&pinfo);
    snd_ctl_elem_value_alloca(&pcontrol);
    int type = 0, size = 0;
    int volume = 0;
    if(getpid(Audio_Type, &pid) == 0)
    {
        if(getpinfo(pid, &pinfo) == 0)
        {
            getpcontrol(pid, pinfo, &pcontrol);
            get_type(pinfo, &type);
            get_size(pinfo, &size);
            set_value(pid, pinfo, pcontrol, nValue);
        }
    }
#endif
}

int AudioControl::Get(int Audio_Type, int *nValue)
{
#ifndef __x86_64__
    snd_ctl_elem_id_t *pid = NULL;
    snd_ctl_elem_info_t *pinfo = NULL;
    snd_ctl_elem_value_t *pcontrol = NULL;
    snd_hctl_elem_t *pelem = NULL;
    snd_ctl_elem_id_alloca(&pid);
    snd_ctl_elem_info_alloca(&pinfo);
    snd_ctl_elem_value_alloca(&pcontrol);
    int type = 0, size = 0;
    int volume = 0;
    if(getpid(Audio_Type, &pid) == 0)
    {
        if(getpinfo(pid, &pinfo) == 0)
        {
            getpcontrol(pid, pinfo, &pcontrol);
            get_type(pinfo, &type);
            get_size(pinfo, &size);
            get_value(type, size, pcontrol, &volume);
            *nValue = volume;
        }
    }
#endif
}

int AudioControl::GetRange(int Audio_Type, int *pMin ,int *pMax)
{
#ifndef __x86_64__
    snd_ctl_elem_id_t *pid = NULL;
    snd_ctl_elem_info_t *pinfo = NULL;
    snd_ctl_elem_value_t *pcontrol = NULL;
    snd_hctl_elem_t *pelem = NULL;
    snd_ctl_elem_id_alloca(&pid);
    snd_ctl_elem_info_alloca(&pinfo);
    snd_ctl_elem_value_alloca(&pcontrol);
    int type = 0, size = 0;
    Data data;
    if(getpid(Audio_Type, &pid) == 0)
    {
        if(getpinfo(pid, &pinfo) == 0)
        {
            getpelem(pid, &pelem);
            getpcontrol(pid, pinfo, &pcontrol);
            get_type(pinfo, &type);
            get_size(pinfo, &size);
            get_range(type, pelem, pinfo, &data);
            *pMin = data.min;
            *pMax = data.max;
        }
    }
#endif
}

int AudioControl::getpid(int audio_type, snd_ctl_elem_id_t **ppid)
{
#ifndef __x86_64__
    char *paudio_id = getstring(audio_type);
    if (snd_ctl_ascii_elem_id_parse(*ppid, paudio_id))
    {
        printf("-- snd_ctl_ascii_elem_id_parse failed--\n");
        return -1;
    }
#endif
    return 0;
}

int AudioControl::getpinfo(snd_ctl_elem_id_t *pid, snd_ctl_elem_info_t **ppinfo)
{
#ifndef __x86_64__
    int err;
    snd_ctl_t *handle = NULL;

    if (handle == NULL && (err = snd_ctl_open(&handle, DEFAULT, 0)) < 0)
    {
        printf("-- snd_ctl_open failed--\n");
        return -1;
    }

    snd_ctl_elem_info_set_id(*ppinfo, pid);

    if ((err = snd_ctl_elem_info(handle, *ppinfo)) < 0)
    {
        snd_ctl_close(handle);
        handle = NULL;
        printf("-- snd_ctl_elem_info failed--\r\n");
        return -1;
    }

    snd_ctl_close(handle);
    handle = NULL;
#endif
    return 0;
}

int AudioControl::getpelem(snd_ctl_elem_id_t *pid, snd_hctl_elem_t **ppelem)
{
#ifndef __x86_64__
    int err;
    snd_hctl_t *hctl;
    if ((err = snd_hctl_open(&hctl, DEFAULT, 0)) < 0)
    {
        printf("-- snd_hctl_open failed--\n");
        return -1;
    }
    if ((err = snd_hctl_load(hctl)) < 0)
    {
        printf("-- snd_hctl_load failed--\n");
        return -1;
    }

    *ppelem = snd_hctl_find_elem(hctl, pid);
    snd_hctl_close(hctl);
#endif
    return 0;
}

int AudioControl::getpcontrol(snd_ctl_elem_id_t *pid, snd_ctl_elem_info_t *pinfo ,snd_ctl_elem_value_t **ppcontrol)
{
#ifndef __x86_64__
    int err;
    snd_hctl_t *hctl;
    snd_hctl_elem_t *elem;

    if ((err = snd_hctl_open(&hctl, DEFAULT, 0)) < 0)
    {
        printf("-- snd_hctl_open failed--\n");
        return -1;
    }

    if ((err = snd_hctl_load(hctl)) < 0)
    {
        printf("-- snd_hctl_load failed--\n");
        return -1;
    }

    elem = snd_hctl_find_elem(hctl, pid);
    if ((err = snd_hctl_elem_info(elem, pinfo)) < 0)
    {
        printf("-- snd_hctl_elem_info failed --\n");
        return -1;
    }

    snd_ctl_elem_info_is_readable(pinfo);

    if ((err = snd_hctl_elem_read(elem, *ppcontrol)) < 0)
    {
        printf("-- snd_hctl_elem_read failed--\n");
        return -1;
    }

    snd_hctl_close(hctl);
#endif
    return 0;
}

int AudioControl::get_type(snd_ctl_elem_info_t *pinfo, int *ptype)
{
#ifndef __x86_64__
    int err;
    int type = -1;
    if ((type = snd_ctl_elem_info_get_type(pinfo)) < 0)
    {
        printf("-- snd_hctl_elem_read failed--\n");
        return -1;
    }
    *ptype = type;
#endif
    return 0;
}

int AudioControl::get_size(snd_ctl_elem_info_t *pinfo, int *psize)
{
#ifndef __x86_64__
    int err;
    int size = -1;
    if ((size = snd_ctl_elem_info_get_count(pinfo)) < 0)
    {
        printf("-- snd_hctl_elem_read failed--\n");
        return -1;
    }
    *psize = size;
#endif
    return 0;
}

int AudioControl::get_range(int type, snd_hctl_elem_t *pelem, snd_ctl_elem_info_t *pinfo, Data *pdata)
{
#ifndef __x86_64__
    switch (type)
    {
    case SND_CTL_ELEM_TYPE_INTEGER:
        pdata->min = snd_ctl_elem_info_get_min(pinfo);
        pdata->max = snd_ctl_elem_info_get_max(pinfo);
        pdata->step = snd_ctl_elem_info_get_step(pinfo);
        break;
    case SND_CTL_ELEM_TYPE_INTEGER64:
        pdata->min = snd_ctl_elem_info_get_min(pinfo);
        pdata->max = snd_ctl_elem_info_get_max(pinfo);
        pdata->step = snd_ctl_elem_info_get_step(pinfo);
        break;
    default:
        pdata->min = 0;
        pdata->max = 0;
        pdata->step = 0;
        break;
    }
#endif
    return 0;
}

int AudioControl::get_value(int type, int count, snd_ctl_elem_value_t *pcontrol , int *pvalue)
{
#ifndef __x86_64__
    int value = 0;
    for (int index = 0; index < count; index++) {
        switch (type) {
        case SND_CTL_ELEM_TYPE_BOOLEAN:
            value = snd_ctl_elem_value_get_boolean(pcontrol, index);
            break;
        case SND_CTL_ELEM_TYPE_INTEGER:
            value = snd_ctl_elem_value_get_integer(pcontrol, index);
            printf("integer value = %d\r\n", value);
            break;
        case SND_CTL_ELEM_TYPE_INTEGER64:
            value = snd_ctl_elem_value_get_integer64(pcontrol, index);
            break;
        case SND_CTL_ELEM_TYPE_ENUMERATED:
            value = snd_ctl_elem_value_get_enumerated(pcontrol, index);
            printf("enumerated value = %d\r\n", value);
            break;
        case SND_CTL_ELEM_TYPE_BYTES:
            value = snd_ctl_elem_value_get_byte(pcontrol, index);
            break;
        default:
            printf("?");
            break;
        }
    }
    *pvalue = value;
#endif
    return 0;
}

int AudioControl::set_value(snd_ctl_elem_id_t *pid, snd_ctl_elem_info_t *pinfo, snd_ctl_elem_value_t *pcontrol, int value)
{
#ifndef __x86_64__
    int err;
    snd_ctl_t *handle = NULL;
    if (handle == NULL && (err = snd_ctl_open(&handle, DEFAULT, 0)) < 0)
    {
        printf("-- snd_ctl_open failed--\n");
        return -1;
    }
    snd_ctl_elem_value_set_id(pcontrol, pid);

    if ((err = snd_ctl_elem_read(handle, pcontrol)) < 0)
    {
        snd_ctl_close(handle);
        handle = NULL;
        return -1;
    }
    char sz_value[20] = {0,};
    sprintf(sz_value, "%d", value);
    //  printf("set value = %d\r\n", value);
    err = snd_ctl_ascii_value_parse(handle, pcontrol, pinfo, sz_value);
    if (err < 0)
    {
        snd_ctl_close(handle);
        handle = NULL;
        return  -1;
    }
    if ((err = snd_ctl_elem_write(handle, pcontrol)) < 0)
    {
        snd_ctl_close(handle);
        handle = NULL;
        return -1;
    }
    snd_ctl_close(handle);
    handle = NULL;
#endif
}

char* AudioControl::getstring(int type)
{
    static char szID[100] = {0,};
    switch (type) {
    case LEFT_AUDIO:
        strcpy(szID, LEFT_ID);
        break;
    case RIGHT_AUDIO:
        strcpy(szID, RIGHT_ID);
        break;
    case STRAM_AUDIO:
        strcpy(szID, STRAM_ID);
        break;
    case OUTPUT_AUDIO:
        strcpy(szID, OUTPUT_ID);
        break;
    case AMP_AUDIO:
        strcpy(szID, AMP_ID);
        break;
    case FM_AUDIO:
        strcpy(szID, FM_ID);
        break;
    case IRSTATUS_AUDIO:
        strcpy(szID, IRSTATUS_ID);
        break;
    case IRCHANNEL_AUDIO:
        strcpy(szID, IRCHANNEL_ID);
        break;
    case PAMUTE_AUDIO:
        strcpy(szID, PAMUTE_ID);
        break;
    case PAINPUTSWITCH_AUDIO: {
        strcpy(szID, painputswitch_id);
        break;
    }
    case PAOUTPUTVOLUME_AUDIO: {
        strcpy(szID, paoutputvolume_id);
        break;
    }
    case PAMUTESWITCH_AUDIO: {
        strcpy(szID, pamuteswitch_id);
        break;
    }
    case PAOUTPUTFL_AUDIO: {
        strcpy(szID, paoutputfl_id);
        break;
    }
    case PAOUTPUTFR_AUDIO: {
        strcpy(szID, paoutputfr_id);
        break;
    }
    case PAOUTPUTRL_AUDIO: {
        strcpy(szID, paoutputrl_id);
        break;
    }
    case PAOUTPUTRR_AUDIO: {
        strcpy(szID, paoutputrr_id);
        break;
    }
    case PAOUTPUTBASS_AUDIO: {
        strcpy(szID, paoutputbass_id);
        break;
    }
    case PAOUTPUTMIDDLE_AUDIO: {
        strcpy(szID, paoutputmiddle_id);
        break;
    }
    case PAOUTPUTTREBLE_AUDIO: {
        strcpy(szID, paoutputtreble_id);
        break;
    }
    default: {
        break;
    }
    }
    return szID;
}
