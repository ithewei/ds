#ifndef HAUDIOPLAY_H
#define HAUDIOPLAY_H

#include <portaudio.h>
#include "ds_global.h"

class HAudioPlay
{
public:
    explicit HAudioPlay();
    ~HAudioPlay();

public:
    int startPlay();
    int stopPlay();
    int pushAudio(av_pcmbuff* pcm);

    static int playCallback(
            const void *input, void *output,
            unsigned long frameCount,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData );

private:
    PaStream* m_pStream;

    av_pcmbuff m_pcmInfo;
    cyc_buf<unsigned char, 0>* m_pcmBuf;
    tmc_mutex_type m_mutex;
};

#endif // HAUDIOPLAY_H
