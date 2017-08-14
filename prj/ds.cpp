#include "ds.h"
#include "hdsctx.h"

#define VERSION 6
#define RELEASEINFO "6.0 @ 20170712"

void myLogHandler(QtMsgType type, const QMessageLogContext & ctx, const QString & msg){
    char szType[16];
    switch (type) {
    case QtDebugMsg:
        strcpy(szType, "Debug");
        break;
    case QtInfoMsg:
        strcpy(szType, "Info");
        break;
    case QtWarningMsg:
        strcpy(szType, "Warning");
        break;
    case QtCriticalMsg:
        strcpy(szType, "Critical");
        break;
    case QtFatalMsg:
        strcpy(szType, "Fatal");
    }
    char szLog[1024];

    sprintf(szLog, "%s %s [%s:%u, %s]\n", szType, msg.toLocal8Bit().constData(), ctx.file, ctx.line, ctx.function);
    fprintf(stderr, szLog);

    QString strLogFilePath = QCoreApplication::applicationDirPath() + "/ds.log";

    FILE* fp = fopen(strLogFilePath.toLocal8Bit().data(), "a");
    if (fp){
        fseek(fp, 0, SEEK_END);
        if (ftell(fp) > (2 << 20)){
            fclose(fp);
            fopen(strLogFilePath.toLocal8Bit().data(), "w");
        }
    }

    if (fp){
        fwrite(szLog, 1, strlen(szLog), fp);
        fclose(fp);
    }
}

DSSHARED_EXPORT int libversion()    { return VERSION; }
DSSHARED_EXPORT int libchar()       { return OOK_FOURCC('D', 'I', 'R', 'C'); }
DSSHARED_EXPORT int libtrace(int t) {  qInstallMessageHandler(myLogHandler); return t; }

DSSHARED_EXPORT int libinit(const char* xml, void* task, void** ctx){
    qInstallMessageHandler(myLogHandler);
    qDebug("libinit version=%d,%s", VERSION, RELEASEINFO);

    if(!xml || !task || !ctx)
        return -1;

    if(g_dsCtx)
    {
        *ctx = g_dsCtx;
        g_dsCtx->ref++;
        int mask  = SERVICE_POSITION_VIDEO_AFDEC;
        if(g_dsCtx->audio)
            mask |= SERVICE_POSITION_AUDIO_AFDEC;
        return mask;
    }

    int err = 0;
    do {
        g_dsCtx = new HDsContext;

        if (g_dsCtx->parse_init_xml(xml) != 0){
            qWarning("parse_init_xml failed");
            err = -1003;
            break;
        }

        task_info_s        * ti = (task_info_s        *)task;
        task_info_detail_s * tid = (task_info_detail_s *)ti->extra;
        std::string strXmlPath = tid->cur_path;
        APPENDSEPARTOR(strXmlPath)
        strXmlPath += "director_service.xml";
        if(job_check_path(strXmlPath.c_str()) != 0)
        {
            qWarning("not found director_service.xml");
            err = -1004;
            break;
        }
        if (g_dsCtx->parse_layout_xml(strXmlPath.c_str()) != 0){
            qWarning("parse_layout_xml failed");
            err = -1005;
            break;
        }

        std::string img_path = tid->cur_path;
        APPENDSEPARTOR(img_path)
        img_path += "..";
        APPENDSEPARTOR(img_path)
        img_path += "img";
        APPENDSEPARTOR(img_path)
        img_path += "director_service";
        APPENDSEPARTOR(img_path)
        g_dsCtx->img_path = img_path;
        if(job_check_path(img_path.c_str()) == 0)
            g_dsCtx->initImg(img_path);

        std::string ttf_path = tid->cur_path;
        APPENDSEPARTOR(ttf_path)
        ttf_path += "..";
        APPENDSEPARTOR(ttf_path)
        ttf_path += "fonts";
        APPENDSEPARTOR(ttf_path)
        ttf_path += "default.ttf";
        if(job_check_path(ttf_path.c_str()) == 0)
            g_dsCtx->initFont(ttf_path, 24);

        int mask  = SERVICE_POSITION_VIDEO_AFDEC;
        if(g_dsCtx->audio)
            mask |= SERVICE_POSITION_AUDIO_AFDEC;

        *ctx = g_dsCtx;
        qDebug("libinit ok");
        return mask;
    }while(0);

    if (g_dsCtx){
        delete g_dsCtx;
        g_dsCtx = NULL;
    }

    return err;
}

DSSHARED_EXPORT int libstop(void* ctx){
    qDebug("");
    if (!ctx)
        return -1;

    if (g_dsCtx){
        if (--g_dsCtx->ref == 0){
            qDebug("quit");
            g_dsCtx->quit();
//            delete g_dsCtx;
//            g_dsCtx = NULL;
        }
    }

    return 0;
}

DSSHARED_EXPORT int liboper(int media_type, int data_type, int opt, void* param, void * ctx){
    //qDebug("media_type=%d,data_type=%d,opt=%d", media_type, data_type, opt);

    if (!ctx)
        return -1;

    switch(media_type){
    case MediaTypeUnknown:
    {
        if (data_type == SERVICE_DATATYPE_DEP){
            switch(opt)
            {
            case SERVICE_OPT_DISPLAY:
                if(g_dsCtx->init == 0)
                {
                    g_dsCtx->init = 1;
                    g_dsCtx->start_gui_thread();
                }
                g_dsCtx->setAction(*(int*)param);
                break;
            case SERVICE_OPT_TASKSTATUS:
                if (param){
                    std::string * str = (std::string *)param;
                    g_dsCtx->setInfo(str->c_str());
                }
                break;
            case SERVICE_OPT_TASKSTATUSREQ:
                break;
            case SERVICE_OPT_PREVSTOP:
                if(param)
                {
                    int svrid = *(int *)param;
                    if (svrid < 1 || svrid > DIRECTOR_MAX_SERVS)
                        return -2;
                    g_dsCtx->stop(svrid);
                }
                break;
            default:
                if(opt > 0x1000)
                {
                    int svrid = opt - 0x1000;
                    if (svrid < 1 || svrid > DIRECTOR_MAX_SERVS)
                        return -2;

                    if (param){
                        const char* title = (const char*)param;
                        if (is_ascii_string(title)){
                            qDebug("svrid=%d ascii=%s strlen=%d", svrid, title, strlen(title));
                            g_dsCtx->setTitle(svrid, title);
                        }else{
                            ANSICODE2UTF8 a2u(title);
                            if (a2u.c_str() == NULL || strlen(a2u.c_str()) == 0){
                                qCritical("title format error!");
                                return -111;
                            }
                            qDebug("svrid=%d utf8=%s strlen=%d", svrid, a2u.c_str(), strlen(a2u.c_str()));
                            g_dsCtx->setTitle(svrid, a2u.c_str());
                        }
                    }
                }
                break;
            }
        }else if (data_type == SERVICE_DATATYPE_CHR){
            if (opt != libchar())
                return -2;

            const director_service_cont * dsc = (const director_service_cont *)param;
            int svrid = dsc->servid;
            if(svrid < 1 || svrid > DIRECTOR_MAX_SERVS)
                return -2;

            DsItemInfo* item = g_dsCtx->getItem(svrid);

            if(dsc->action == OOK_FOURCC('S', 'V', 'C', 'B')){
                qDebug("OOK_FOURCC('S', 'V', 'C', 'B')");
                if (item){
                    item->ifcb = (ifservice_callback *)dsc->ptr;
                }
            }else if(dsc->action == OOK_FOURCC('L', 'O', 'U', 'T')){
                qDebug("OOK_FOURCC('L', 'O', 'U', 'T')");
                g_dsCtx->parse_cock_xml((const char *)dsc->ptr);
            }else if (dsc->action == OOK_FOURCC('S', 'R', 'C', 'L')){
                qDebug("OOK_FOURCC('S', 'R', 'C', 'L')");
                if (strcmp((const char*)dsc->ptr, "file") == 0){
                    item->src_type = SRC_TYPE_FILE;
                }
            }else if (dsc->action == OOK_FOURCC('P', 'L', 'Y', 'R')){
                int progress = *(int*)dsc->ptr;
                //qDebug("OOK_FOURCC('P', 'L', 'Y', 'R') progress=%d", progress);
                emit g_dsCtx->sigProgressNty(svrid, progress);
            }

        }
    }
        break;
    case MediaTypeVideo:
    {
        if(data_type != SERVICE_DATATYPE_PIC)
            return -4;

        int svrid = opt;
        if (svrid < 1 || svrid > DIRECTOR_MAX_SERVS)
            return -5;

        const av_picture * pic = (const av_picture *)param;
        if (!pic)
            return -6;

        if (g_dsCtx->getItem(svrid)->v_input < 1){
            char c[5] = {0};
            memcpy(c, &pic->fourcc, 4);
            qDebug("pic[%d] type=%s w=%d h=%d", svrid, c, pic->width, pic->height);
        }

        ++g_dsCtx->getItem(svrid)->v_input;
        g_dsCtx->push_video(svrid, pic);
    }
        break;
    case MediaTypeAudio:
    {
        if(data_type != SERVICE_DATATYPE_PCM)
            return -2;

        int svrid = opt;
        if (svrid < 1 || svrid > DIRECTOR_MAX_SERVS)
            return -5;

        const av_pcmbuff * pcm = (const av_pcmbuff *)param;
        if (!pcm)
            return -6;

        if (g_dsCtx->getItem(svrid)->a_input < 1){
            qDebug("pcm[%d] channel=%d, sample=%d len=%d", svrid, pcm->channels, pcm->samplerate, pcm->pcmlen);
        }

        ++g_dsCtx->getItem(svrid)->a_input;
        g_dsCtx->push_audio(svrid, pcm);
    }
        break;
    default:
        return -9;
    }

    return 0;
}
