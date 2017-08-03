#include "haudioplay.h"

int HAudioPlay::playCallback(
    const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData ){

    HAudioPlay* pObj = (HAudioPlay*)userData;
    if (!pObj)
        return -1;

    int bytes = frameCount*2*pObj->m_pcmInfo.channels;
    if (pObj->m_pcmBuf->length() >= bytes){
        pObj->m_mutex.lock();
        pObj->m_pcmBuf->pop((unsigned char*)output, bytes);
        pObj->m_mutex.unlock();
    }else{
        //qDebug("sound delay");
    }

    return 0;
}

HAudioPlay::HAudioPlay(){
    m_pStream = NULL;
    m_pcmBuf = NULL;
}

HAudioPlay::~HAudioPlay(){
    stopPlay();
}

int HAudioPlay::startPlay(){
    qDebug("");
    PaError err = Pa_Initialize();
    if (err != paNoError){
        qCritical("Pa_Initialize error:%s", Pa_GetErrorText(err));
        goto ERROR;
    }

    if (!m_pcmBuf){
        m_pcmBuf = new cyc_buf<unsigned char, 0>;
        m_pcmBuf->size(m_pcmInfo.pcmlen << 3);  // set buf size = pcmlen*8
    }

    err = Pa_OpenDefaultStream(&m_pStream, 0, m_pcmInfo.channels, paInt16, m_pcmInfo.samplerate, 1024, playCallback, this);
    if (err != paNoError){
        qCritical("Pa_OpenDefaultStream error:%s", Pa_GetErrorText(err));
        goto ERROR;
    }

    err = Pa_StartStream(m_pStream);
    if (err != paNoError){
        qCritical("Pa_StartStream error:%s", Pa_GetErrorText(err));
        goto ERROR;
    }
    return paNoError;

ERROR:
    Pa_Terminate();
    return err;
}

int HAudioPlay::stopPlay(){
    qDebug("");
    if (m_pStream){
        Pa_StopStream(m_pStream);
        Pa_CloseStream(m_pStream);
        Pa_Terminate();
        m_pStream = NULL;
    }

    if (m_pcmBuf){
        delete m_pcmBuf;
        m_pcmBuf = NULL;
    }
}

int HAudioPlay::pushAudio(av_pcmbuff* pcm){
    if (!m_pStream || m_pcmInfo.channels != pcm->channels ||
            m_pcmInfo.samplerate != pcm->samplerate){
        m_pcmInfo.channels = pcm->channels;
        m_pcmInfo.samplerate = pcm->samplerate;
        m_pcmInfo.pcmlen = pcm->pcmlen;
        stopPlay();
        startPlay();
    }

    m_mutex.lock();
    m_pcmBuf->push(pcm->pcmbuf, pcm->pcmlen);
    m_mutex.unlock();

    return pcm->pcmlen;
}
