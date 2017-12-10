#include "hrcloader.h"
#include "hdsctx.h"

IMPL_SINGLETON(HRcLoader)

void HRcLoader::loadIcon(){
#define LOAD_ICON(rcid, filename) \
    m_mapIcon[rcid] = QPixmap(QString(g_dsCtx->img_path.c_str()) + filename); \

    FOR_EACH_ICON(LOAD_ICON)
    FOR_EACH_ICON_MV(LOAD_ICON)

#undef LOAD_ICON
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
