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
    strImg += "start.png";
    icon_start.load(strImg.c_str());

    strImg = g_dsCtx->img_path;
    strImg += "pause.png";
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

    strImg = g_dsCtx->img_path;
    strImg += "123.png";
    icon_num.load(strImg.c_str());

    char num[16];
    for (int i = 0; i < MAX_NUM_ICON; ++i){
        strImg = g_dsCtx->img_path;
        sprintf(num, "b%d_64.png", i+1);
        strImg += num;
        icon_numb[i].load(strImg.c_str());
    }

    for (int i = 0; i < MAX_NUM_ICON; ++i){
        strImg = g_dsCtx->img_path;
        sprintf(num, "r%d_64.png", i+1);
        strImg += num;
        icon_numr[i].load(strImg.c_str());
    }

    strImg = g_dsCtx->img_path;
    strImg += "trash.png";
    icon_trash.load(strImg.c_str());

    strImg = g_dsCtx->img_path;
    strImg += "undo.png";
    icon_undo.load(strImg.c_str());
}

void HRcLoader::loadTexture(){
    std::string strImg;
    QImage img;

    strImg = g_dsCtx->img_path;
    strImg += "sound.png";
    img.load(strImg.c_str());
    bindTexture(&tex_sound, &img);

    char num[16];
    for (int i = 0; i < MAX_NUM_ICON; ++i){
        strImg = g_dsCtx->img_path;
        sprintf(num, "r%d.png", i+1);
        strImg += num;
        img.load(strImg.c_str());
        bindTexture(&tex_numr[i], &img);
    }
}

void HRcLoader::bindTexture(Texture* tex, QImage* img){
    if (img->format() != QImage::Format_ARGB32)
        return;

    glGenTextures(1, &tex->texID);
    glBindTexture(GL_TEXTURE_2D, tex->texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    tex->width = img->width();
    tex->height = img->height();
    tex->type = GL_BGRA;
    tex->bpp = img->bitPlaneCount();
    gluBuild2DMipmaps(GL_TEXTURE_2D, tex->bpp/8, tex->width, tex->height, tex->type, GL_UNSIGNED_BYTE, img->bits());
    //glTexImage2D(GL_TEXTURE_2D, 0, tex->bpp/8, tex->width, tex->height, 0, tex->type, GL_UNSIGNED_BYTE, img->bits());
}
