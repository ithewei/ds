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
    explicit HAudioPlay();
    ~HAudioPlay();

public:
    int startPlay(int dev = PCH_ANALOG);
    void stopPlay();
    void pausePlay(bool bPause);

    static int playCallback(
            const void *input, void *output,
            unsigned long frameCount,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData );

public:
    int srvid;
    int pcmlen;
    int samplerate;
    int channels;
    bool pause;
private:
    static bool s_bInit;

    PaStream* m_pStream;
    int dev;
};

#endif // HAUDIOPLAY_H
