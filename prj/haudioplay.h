#ifndef HAUDIOPLAY_H
#define HAUDIOPLAY_H

#include <portaudio.h>
#include "ds_global.h"
#include "hringbuffer.h"
#include <QMutex>

class HAudioPlay
{
public:
    explicit HAudioPlay(int buf_size = 10);
    ~HAudioPlay();

public:
    int startPlay();
    void stopPlay();
    void pausePlay(bool bPause);
    int pushAudio(av_pcmbuff* pcm);

    static int playCallback(
            const void *input, void *output,
            unsigned long frameCount,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData );

private:
    PaStream* m_pStream;

    HRingBuffer* audio_buffer;
    int buf_size;
    QMutex audio_mutex;
    int pcmlen;
    int samplerate;
    int channels;
};

#endif // HAUDIOPLAY_H
