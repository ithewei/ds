#ifndef HAUDIOPLAY_H
#define HAUDIOPLAY_H

#include <portaudio.h>
#include "ds_global.h"
#include "hringbuffer.h"
#include <QMutex>

enum ASOUND_DEVICE{
    HDMI0 = 0,
    HDMI1 = 1,
    HDMI2 = 2,

    PCH_ANALOG = 3,
    PCH_DIGITAL = 4
};

class HAudioPlay
{
public:
    explicit HAudioPlay(int buf_size = 10);
    ~HAudioPlay();

public:
    int startPlay(ASOUND_DEVICE dev = PCH_ANALOG);
    void stopPlay();
    void pausePlay(bool bPause);
    int pushAudio(av_pcmbuff* pcm);

    static int playCallback(
            const void *input, void *output,
            unsigned long frameCount,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData );

public:
    int buf_size;
private:
    PaStream* m_pStream;

    HRingBuffer* audio_buffer;
    QMutex audio_mutex;
    int pcmlen;
    int samplerate;
    int channels;

    int dev;
};

#endif // HAUDIOPLAY_H
