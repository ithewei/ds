#include "haudioplay.h"
#include "hdsctx.h"

bool HAudioPlay::s_bInit = false;

int HAudioPlay::playCallback(
    const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData ){

    HAudioPlay* pObj = (HAudioPlay*)userData;
    if (!pObj)
        return -1;

    qDebug("dev=%d frameCount=%ld, pcmlen=%d",pObj->dev, frameCount, pObj->pcmlen);
    DsSrvItem* item = g_dsCtx->getSrvItem(pObj->srvid);
    if (item){
        item->audio_mutex.lock();
        if (item->audio_buffer && frameCount*2*pObj->channels == pObj->pcmlen){
            char* ptr = item->audio_buffer->read();
            if (ptr){
                memcpy(output, ptr, pObj->pcmlen);
            }else{
                memset(output, 0, pObj->pcmlen);
                qInfo("audio_buffer is empty, memzero");
            }
        }
        item->audio_mutex.unlock();
    }

    return 0;
}

HAudioPlay::HAudioPlay(){
    srvid = 0;
    m_pStream = NULL;
    pcmlen = 0;
    channels = 0;
    samplerate = 0;
    dev = 0;
    pause = true;

    if (!s_bInit){
        s_bInit = true;
        PaError err = Pa_Initialize();
        if (err == paNoError){
            qInfo("Pa_Initialize succeed!");
        }else{
            s_bInit = false;
            qCritical("Pa_Initialize error:%s", Pa_GetErrorText(err));
        }
    }
}

HAudioPlay::~HAudioPlay(){
    stopPlay();

    //qInfo("Pa_Terminate");
    //Pa_Terminate();
}

int HAudioPlay::startPlay(int dev){
    qInfo("ASOUND_DEVICE = %d", dev);

    const PaDeviceInfo* devinfo = Pa_GetDeviceInfo(dev);
    if (!devinfo){
        qCritical("sound device not find, dev=%d", dev);
        return -1;
    }
    this->dev = dev;

    PaDeviceIndex cnt = Pa_GetDeviceCount();
    PaDeviceIndex din = Pa_GetDefaultInputDevice();
    PaDeviceIndex dout = Pa_GetDefaultOutputDevice();
    qInfo("sound device count=%d default_in=%d default_out=%d", cnt, din, dout);
    if (cnt <= 0 || cnt > 32){
        // @bug: don't know why sometimes Pa_GetDeviceCount return a very large num.
        qWarning("Pa_GetDeviceCount err=%d", cnt);
        cnt = din;
    }

    for (int i = 0; i < cnt; ++i){
        const PaDeviceInfo* dev = Pa_GetDeviceInfo(i);
        if (dev){
            qInfo("device:%d name=%s "
                  "maxInputChannels=%d maxOutputChannels=%d "
                  "defaultLowInputLatency=%f defaultLowOutputLatency=%f "
                  "defaultHighInputLatency=%f defaultHighOutputLatency=%f "
                  "defaultSampleRate=%f",
                  i, dev->name,
                  dev->maxInputChannels, dev->maxOutputChannels,
                  dev->defaultLowInputLatency, dev->defaultLowOutputLatency,
                  dev->defaultHighInputLatency, dev->defaultHighOutputLatency,
                  dev->defaultSampleRate);
        }
    }

    PaError err;
    if (devinfo->maxOutputChannels < channels){
        // @bug: don't konw how to deal this problem
        qCritical("channel is preempted, maybe need to reboot to fix the problem!");
        this->dev = dout;
        err = Pa_OpenDefaultStream(&m_pStream, 0, channels, paInt16, samplerate, pcmlen/channels/2, playCallback, this);
    }else{
        PaStreamParameters op;
        memset(&op, 0, sizeof op);
        op.device = (int)dev;
        op.channelCount = channels;
        op.sampleFormat = paInt16;
        op.suggestedLatency = devinfo->defaultHighOutputLatency;
        err = Pa_OpenStream(&m_pStream, NULL, &op, samplerate, pcmlen/channels/2, paDitherOff, playCallback, this);
    }

    if (err != paNoError){
        qCritical("Pa_OpenStream error:%s", Pa_GetErrorText(err));
        qCritical("pcmlen=%d, channels=%d, samplerate=%d", pcmlen, channels, samplerate);
        return err;
    }

//    err = Pa_StartStream(m_pStream);
//    if (err != paNoError){
//        qCritical("Pa_StartStream error:%s", Pa_GetErrorText(err));
//        return err;
//    }
//    pause = false;
    pause = true;

    return paNoError;
}

void HAudioPlay::stopPlay(){
    qInfo("");
    if (m_pStream){
        Pa_StopStream(m_pStream);
        Pa_CloseStream(m_pStream);
        m_pStream = NULL;
    }
    pcmlen = 0;
    channels = 0;
    samplerate = 0;
}

void HAudioPlay::pausePlay(bool bPause){
    qInfo("pause=%d", bPause);
    if (!m_pStream)
        return;

    if (bPause){
        if (!pause)
            Pa_StopStream(m_pStream);
    }else{
        if (pause)
            Pa_StartStream(m_pStream);
    }

    pause = bPause;
}
