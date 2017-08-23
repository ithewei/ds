#include "hrcloader.h"
#include "tga.h"
#include "hdsctx.h"

HRcLoader* HRcLoader::s_rcLoader = NULL;

HRcLoader::HRcLoader()
{

}

HRcLoader::~HRcLoader(){

}

HRcLoader* HRcLoader::instance(){
    if (s_rcLoader == NULL){
        s_rcLoader = new HRcLoader;
    }
    return s_rcLoader;
}

void HRcLoader::exitInstance(){
    if (s_rcLoader){
        delete s_rcLoader;
        s_rcLoader = NULL;
    }
}

void HRcLoader::loadIcon(){
    std::string strImg = g_dsCtx->img_path;
    strImg += "fullscreen.png";
    icon_fullscreen.load(strImg.c_str());

    strImg = g_dsCtx->img_path;
    strImg += "nofullscreen.png";
    icon_exit_fullscreen.load(strImg.c_str());

    strImg = g_dsCtx->img_path;
    strImg += "left_expand.png";
    icon_left_expand.load(strImg.c_str());

    strImg = g_dsCtx->img_path;
    strImg += "right_fold.png";
    icon_right_fold.load(strImg.c_str());

    strImg = g_dsCtx->img_path;
    strImg += "start.tga";
    icon_start.load(strImg.c_str());

    strImg = g_dsCtx->img_path;
    strImg += "pause.tga";
    icon_pause.load(strImg.c_str());

    strImg = g_dsCtx->img_path;
    strImg += "stop.png";
    icon_stop.load(strImg.c_str());

    strImg = g_dsCtx->img_path;
    strImg += "snapshot.png";
    icon_snapshot.load(strImg.c_str());

    strImg = g_dsCtx->img_path;
    strImg += "record.png";
    icon_record.load(strImg.c_str());

    strImg = g_dsCtx->img_path;
    strImg += "recording.png";
    icon_recording.load(strImg.c_str());

    char num[16];
    for (int i = 0; i < 3; ++i){
        strImg = g_dsCtx->img_path;
        sprintf(num, "b%d.png", i+1);
        strImg += num;
        icon_numb[i].load(strImg.c_str());
    }

    for (int i = 0; i < 3; ++i){
        strImg = g_dsCtx->img_path;
        sprintf(num, "r%d.png", i+1);
        strImg += num;
        icon_numr[i].load(strImg.c_str());
    }

    strImg = g_dsCtx->img_path;
    strImg += "trash.png";
    icon_trash.load(strImg.c_str());
}

void HRcLoader::loadTexture(){
    std::string strImg;
    strImg = g_dsCtx->img_path;
    strImg += "video.tga";
    loadTGA(strImg.c_str(), &tex_video);

    strImg = g_dsCtx->img_path;
    strImg += "novideo.tga";
    loadTGA(strImg.c_str(), &tex_novideo);

    strImg = g_dsCtx->img_path;
    strImg += "pick.tga";
    loadTGA(strImg.c_str(), &tex_pick);

    strImg = g_dsCtx->img_path;
    strImg += "prohibit.tga";
    loadTGA(strImg.c_str(), &tex_prohibit);

    strImg = g_dsCtx->img_path;
    strImg += "sound.tga";
    loadTGA(strImg.c_str(), &tex_sound);

    strImg = g_dsCtx->img_path;
    strImg += "spacer.tga";
    loadTGA(strImg.c_str(), &tex_spacer);

    strImg = g_dsCtx->img_path;
    strImg += "refresh.tga";
    loadTGA(strImg.c_str(), &tex_refresh);
}
