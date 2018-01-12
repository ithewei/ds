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

    qDebug("frameCount = %d, pcmlen=%d", frameCount, pObj->pcmlen);
    pObj->audio_mutex.lock();
    if (pObj->audio_buffer && frameCount*2*pObj->channels == pObj->pcmlen){
        char* ptr = pObj->audio_buffer->read();
        if (ptr){
            memcpy(output, ptr, pObj->pcmlen);
        }else{
            qDebug("sound delay");
        }
    }
    pObj->audio_mutex.unlock();

    return 0;
}

HAudioPlay::HAudioPlay(int buf_size){
    this->buf_size = buf_size;
    m_pStream = NULL;
    audio_buffer = NULL;
    pcmlen = 0;
    channels = 0;
    samplerate = 0;
}

HAudioPlay::~HAudioPlay(){
    stopPlay();
}

int HAudioPlay::startPlay(){
    qInfo("");
    PaError err = Pa_Initialize();
    if (err != paNoError){
        qCritical("Pa_Initialize error:%s", Pa_GetErrorText(err));
        goto ERROR;
    }

    err = Pa_OpenDefaultStream(&m_pStream, 0, channels, paInt16, samplerate, pcmlen/channels/2, playCallback, this);
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

void HAudioPlay::stopPlay(){
    qInfo("");
    if (m_pStream){
        Pa_StopStream(m_pStream);
        Pa_CloseStream(m_pStream);
        Pa_Terminate();
        m_pStream = NULL;
    }

    if (audio_buffer){
        delete audio_buffer;
        audio_buffer = NULL;
    }

    pcmlen = 0;
    channels = 0;
    samplerate = 0;
}

void HAudioPlay::pausePlay(bool bPause){
    qInfo("");
    if (!m_pStream)
        return;

    if (bPause){
        Pa_StopStream(m_pStream);
    }else{
        Pa_StartStream(m_pStream);
    }
}

int HAudioPlay::pushAudio(av_pcmbuff* pcm){
    if (pcm->pcmlen != pcmlen || pcm->channels != channels || pcm->samplerate != pcm->samplerate){
        stopPlay();
        pcmlen = pcm->pcmlen;
        channels = pcm->channels;
        samplerate = pcm->samplerate;
        //startPlay();
    }

    audio_mutex.lock();
    if (!audio_buffer || pcm->pcmlen != audio_buffer->size()){
        audio_buffer = new HRingBuffer(pcmlen, buf_size);
    }

    if (audio_buffer){
        char* ptr = audio_buffer->write();
        if (ptr){
            memcpy(ptr, pcm->pcmbuf, pcmlen);
        }
    }
    audio_mutex.unlock();

    if (!m_pStream)
        return 1; // need startPlay

    return 0;
}
