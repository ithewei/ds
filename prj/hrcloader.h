#ifndef HRCLOADER_H
#define HRCLOADER_H

#include "ds_global.h"
#include "qglwidgetimpl.h"

#define MAX_NUM_ICON    3

class HRcLoader
{
private:
    HRcLoader();

public:
    static HRcLoader* instance();
    static void exitInstance();

    void loadIcon();
    void loadTexture();

    static void bindTexture(Texture* tex, QImage* img);

public:
    static HRcLoader* s_rcLoader;

    QPixmap icon_fullscreen;
    QPixmap icon_exit_fullscreen;
    QPixmap icon_info;
    QPixmap icon_left_expand;
    QPixmap icon_right_fold;
    QPixmap icon_start;
    QPixmap icon_pause;
    QPixmap icon_stop;
    QPixmap icon_snapshot;
    QPixmap icon_record;
    QPixmap icon_recording;
    QPixmap icon_num;
    QPixmap icon_numb[MAX_NUM_ICON];
    QPixmap icon_numr[MAX_NUM_ICON];
    QPixmap icon_trash;
    QPixmap icon_trash_big;
    QPixmap icon_undo;
    QPixmap icon_expre;
    QPixmap icon_ok;
    QPixmap icon_close;
    QPixmap icon_text;
    QPixmap icon_time;
    QPixmap icon_mkdir;
    QPixmap icon_rmdir;
    QPixmap icon_add;
    QPixmap icon_micphone;
    QPixmap icon_micphone_gray;

    Texture tex_sound;
    Texture tex_numr[MAX_NUM_ICON];
};

#endif // HRCLOADER_H
