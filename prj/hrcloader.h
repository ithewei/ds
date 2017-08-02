#ifndef HRCLOADER_H
#define HRCLOADER_H

#include "ds_global.h"

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

    Texture tex_video;
    Texture tex_novideo;
    Texture tex_pick;
    Texture tex_prohibit;
    Texture tex_sound;
    Texture tex_spacer;
    Texture tex_refresh;
};

#endif // HRCLOADER_H
