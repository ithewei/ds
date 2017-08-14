#ifndef HRCLOADER_H
#define HRCLOADER_H

#include "ds_global.h"
#include "hglwidget.h"

class HRcLoader
{
public:
    HRcLoader();
    ~HRcLoader();

public:
    static HRcLoader* instance();
    static void exitInstance();

    void loadIcon();
    void loadTexture();

public:
    static HRcLoader* s_rcLoader;

    QPixmap icon_fullscreen;
    QPixmap icon_exit_fullscreen;
    QPixmap icon_left_expand;
    QPixmap icon_right_fold;
    QPixmap icon_start;
    QPixmap icon_pause;
    QPixmap icon_stop;
    QPixmap icon_sound;
    QPixmap icon_snapshot;
    QPixmap icon_record;
    QPixmap icon_recording;

    Texture tex_video;
    Texture tex_novideo;
    Texture tex_pick;
    Texture tex_prohibit;
    Texture tex_sound;
    Texture tex_spacer;
    Texture tex_refresh;
};

#endif // HRCLOADER_H
