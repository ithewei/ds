#include "hdsctx.h"
#include "hrcloader.h"

HDsContext* g_dsCtx = NULL;

#ifdef WIN32
void HDsContext::thread_gui(void* param) {
#else
void* HDsContext::thread_gui(void* param){
#endif
    qDebug("thread_gui start");

    HDsContext* pObj = (HDsContext*)param;
    int argc = 0;
    QApplication app(argc, NULL);

    QFont font = QApplication::font();
    font.setPointSize(18);
    QApplication::setFont(font);

    HRcLoader::instance()->loadIcon();
    HRcLoader::instance()->loadTexture();

    HMainWidget* mainwdg = new HMainWidget(pObj);
    mainwdg->hide();

    qDebug("mainwdg create succeed");
    pObj->m_mutex.unlock();

    app.exec();

    HRcLoader::exitInstance();

#ifdef linux
    pthread_exit(NULL);
#endif

    qDebug("thread_gui end");
}

HDsContext::HDsContext()
{
    ref     = 1;
    init    = 0;
    action  = 1;
    display_mode = DISPLAY_MODE_TIMER;

    width = 0;
    height = 0;
    audio = 1;
    play_audio = 0;
    info  = 1;
    title  = 1;
    infcolor = 0x00FF00FF;
    titcolor = 0xFF5A1EFF;
    outlinecolor = 0xFFFFFFFF;
    scale_method = 0;
    pause_method = 0;

    m_cntItem = 0;
    m_iCockW = 0;
    m_iCockH = 0;
    frames = 25;

    m_cntCock = 0;
    m_iOriginCockW = 0;
    m_iOriginCockH = 0;

    m_trans = new transaction;
    m_audioPlay = new HAudioPlay;
}

HDsContext::~HDsContext(){
    if (m_trans){
        m_trans->release();
        m_trans = NULL;
    }

    if (m_audioPlay){
        delete m_audioPlay;
        m_audioPlay = NULL;
    }
}

#include <QWaitCondition>
void HDsContext::start_gui_thread(){
    qDebug("start_gui_thread<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");

    m_mutex.lock(); // unlock while gui create succeed

#ifdef linux
    pthread_t pth;
    pthread_create(&pth, NULL, thread_gui, this);
    //void* pRet;
    //pthread_join(pth, &pRet);
    pthread_detach(pth);

    m_mutex.lock();
    m_mutex.unlock();
    qDebug("start_gui_thread>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

#endif

#ifdef WIN32
    unsigned int hThread_glut = _beginthread(thread_glut, 0, this);
    //WaitForSingleObject((HANDLE)hThread_glut, INFINITE);
#endif
}

/*
    <complex type="director_service">
        <render oper="display" >
            <param n="width"  v="640" />
            <param n="height" v="480" />
        </render>
    </complex>
 */
int HDsContext::parse_init_xml(const char* xml){
    qDebug("");

    ook::xml_element root;
    if(root.parse(xml, strlen(xml)) < 0)
        return -1001;
    if(root.tag_name() != "complex")
        return -1002;
    if(root.get_attribute("type") != "director_service")
        return -1003;

    ook::xml_parser::enum_childen(&root, NULL);
    const ook::xml_element * render = NULL;
    while(1)
    {
        render = ook::xml_parser::enum_childen(&root, "render");
        if(!render)
            break;
        const std::string & oper = render->get_attribute("oper");

        ook::xml_parser::enum_childen(render, NULL);
        const ook::xml_element * e = NULL;
        while(1)
        {
            e = ook::xml_parser::enum_childen(render, "param");
            if(!e)
                break;
            const std::string & n = e->get_attribute("n");
            const std::string & v = e->get_attribute("v");

            if(n == "width")
                width = atoi(v.c_str());
            else if(n == "height")
                height = atoi(v.c_str());

            else if(n == "audio")
                audio    = atoi(v.c_str());
            else if(n == "play_audio")
                play_audio    = atoi(v.c_str());

            else if(n == "info")
                info     = atoi(v.c_str());
            else if(n == "infcolor")
                infcolor      = (unsigned int)strtoul(v.c_str(), NULL, 16);

            else if(n == "title")
                title     = atoi(v.c_str());
            else if(n == "titcolor")
                titcolor      = (unsigned int)strtoul(v.c_str(), NULL, 16);
            else if(n == "scale_method")
                scale_method = (int)strtoul(v.c_str(), NULL, 16);
            else if(n == "pause_method")
                pause_method= atoi(v.c_str());
        }
    }

    return 0;
}

/*
<?xml version="1.0" encoding="UTF-8" ?>
<director_service>
    <layout>
        <head>
            <param n="width"  v="1280" />
            <param n="height" v="800"  />
            <param n="frames" v="10"   />

            <__param n="printmask"   v="0x008" />
            <__param n="detail_numb" v="600"  />

            <param n="pick.cb" v="http://127.0.0.1/transcoder/index.php?controller=channels&action=srcchange" />
        </head>
        <body>
            <item>
                <param n="u"   v="1" />
                <param n="w"   v="426" />
                <param n="h"   v="264" />
                <param n="x"   v="0"   />
                <param n="y"   v="4"   />
            </item>
            <item>
                <param n="u"   v="2"   />
                <param n="w"   v="426" />
                <param n="h"   v="264" />
                <param n="x"   v="0"   />
                <param n="y"   v="268" />
            </item>
            <item>
                <param n="u"   v="3"   />
                <param n="w"   v="426" />
                <param n="h"   v="264" />
                <param n="x"   v="0"   />
                <param n="y"   v="532" />
            </item>
            <item>
                <param n="u"   v="4"   />
                <param n="w"   v="426" />
                <param n="h"   v="264" />
                <param n="x"   v="426" />
                <param n="y"   v="532" />
            </item>
            <item>
                <param n="u"   v="5"   />
                <param n="w"   v="426" />
                <param n="h"   v="264" />
                <param n="x"   v="852" />
                <param n="y"   v="532" />
            </item>
            <item>
                <param n="u"   v="6"   />
                <param n="w"   v="852" />
                <param n="h"   v="528" />
                <param n="x"   v="426" />
                <param n="y"   v="4"   />
            </item>
        </body>
    </layout>
</director_service>
*/
int HDsContext::parse_layout_xml(const char* xml_file){
    qDebug("");

    ook::xml_parser x;
    if(!x.parse_file(xml_file))
        return -1;

    const ook::xml_element & root = x.get_root();
    const ook::xml_element * layout = ook::xml_parser::get_element(&root, "<layout>");
    if(!layout)
        return -2;
    const ook::xml_element * head = ook::xml_parser::get_element(layout, "<head>");
    const ook::xml_element * body = ook::xml_parser::get_element(layout, "<body>");
    if(!head || !body)
        return -3;

    const ook::xml_element * e;
    ook::xml_parser::enum_childen(head, NULL);
    while(1)
    {
        e = ook::xml_parser::enum_childen(head, "param");
        if(!e)
            break;
        const std::string & n = e->get_attribute("n");
        const std::string & v = e->get_attribute("v");

        if(n == "width")
            width     = atoi(v.c_str());
        else if(n == "height")
            height    = atoi(v.c_str());
        else if(n == "frames"){
            frames = atoi(v.c_str());
            if (frames < 1)
                frames = 25;
            else if (frames > 30)
                frames = 30;
            qDebug("frames=%d", frames);
        }

        else if(n == "v_back")
            std::string v_back = v;
        else if(n == "v_mask")
            std::string v_mask = v;
        else if(n == "f_mask")
            std::string f_mask = v;

        else if(n == "printmask")
            int printmask      = (unsigned int)strtoul(v.c_str(), NULL, 16);
        else if(n == "pic_threshold")
            int pic_threshold   = (unsigned int)strtoul(v.c_str(), NULL, 10);
        else if(n == "detail_numb")
            int detail_numb     = (unsigned int)strtoul(v.c_str(), NULL, 10);

        else if(n == "pick.cb")
            m_strUrl = v;
    }

    m_cntItem = 0;
    ook::xml_parser::enum_childen(body, NULL);
    const ook::xml_element * item;
    while(1)
    {
        item = ook::xml_parser::enum_childen(body, "item");
        if(!item)
            break;
        ook::xml_parser::enum_childen(item, NULL);
        const ook::xml_element * e;
        HRect hrc;
        while(1)
        {
            e = ook::xml_parser::enum_childen(item, "param");
            if(!e)
                break;
            const std::string & n = e->get_attribute("n");
            const std::string & v = e->get_attribute("v");
            if(n == "w")
                hrc.w = atoi(v.c_str());
            else if(n == "h")
                hrc.h = atoi(v.c_str());
            else if(n == "x")
                hrc.x = atoi(v.c_str());
            else if(n == "y")
                hrc.y = atoi(v.c_str());
        }
        m_rcItems[m_cntItem].setRect(hrc.x, hrc.y, hrc.w, hrc.h);

        m_cntItem++;
        if(m_cntItem >= MAXNUM_LAYOUT)
            break;
    }

    // last is cock window
    m_iCockW = m_rcItems[m_cntItem-1].width();
    m_iCockH = m_rcItems[m_cntItem-1].height();

    return 0;
}

/*
<cocktail>
    <head>
        <param n="width" v="1280"></param>
        <param n="height" v="720"></param>
        <param n="frame_width" v="0"></param>
        <param n="frame_color" v="0x000000"></param>
    </head>
    <body>
        <item>
            <param n="src" v="l=device&amp;i=299fab9a137819bc5fe2dfb8fb232600"></param>
            <param n="u" v="1"></param>
            <param n="w" v="1280"></param>
            <param n="h" v="720"></param>
            <param n="x" v="0"></param>
            <param n="y" v="0"></param>
            <param n="a" v="1"></param>
        </item>
        <item>
            <param n="src" v="l=ts&amp;i=rtsp://admin:12345@192.168.1.63/h264/ch1/main/av_stream"></param>
            <param n="u" v="2"></param>
            <param n="w" v="320"></param>
            <param n="h" v="180"></param>
            <param n="x" v="950"></param>
            <param n="y" v="10"></param>
            <param n="a" v="0"></param>
        </item>
        <item>
            <param n="src" v="l=rtsp&amp;i=rtsp://192.168.1.62/h264/ch1/main/av_stream"></param>
            <param n="u" v="3"></param>
            <param n="w" v="320"></param>
            <param n="h" v="180"></param>
            <param n="x" v="950"></param>
            <param n="y" v="200"></param>
            <param n="a" v="0"></param>
        </item>
    </body>
</cocktail>
 */
int HDsContext::parse_cock_xml(const char* xml){
    qDebug("");

    ook::xml_element root;
    if(!root.parse(xml, strlen(xml)))
        return -1;

    const ook::xml_element * cocktail = ook::xml_parser::get_element(&root, "<cocktail>");
    if(!cocktail)
        return -2;

    const ook::xml_element * head = ook::xml_parser::get_element(cocktail, "<head>");
    const ook::xml_element * body = ook::xml_parser::get_element(cocktail, "<body>");
    if(!head || !body)
        return -3;

    const ook::xml_element * e;
    ook::xml_parser::enum_childen(head, NULL);
    while(1)
    {
        e = ook::xml_parser::enum_childen(head, "param");
        if(!e)
            break;
        const std::string & n = e->get_attribute("n");
        const std::string & v = e->get_attribute("v");

        if(n == "width")
            m_iOriginCockW = atoi(v.c_str());
        else if(n == "height")
            m_iOriginCockH = atoi(v.c_str());
    }

    if(m_iOriginCockW < 1 || m_iOriginCockH < 1)
        return -10;

    ook::xml_parser::enum_childen(body, NULL);
    const ook::xml_element * item;
    m_cntCock = 0;
    while(1)
    {
        item = ook::xml_parser::enum_childen(body, "item");
        if(!item)
            break;

        ook::xml_parser::enum_childen(item, NULL);
        const ook::xml_element * e;
        while(1)
        {
            e = ook::xml_parser::enum_childen(item, "param");
            if(!e)
                break;
            const std::string & n = e->get_attribute("n");
            const std::string & v = e->get_attribute("v");
            if(n == "w")
                m_tOriginCocks[m_cntCock].w = atoi(v.c_str());
            else if(n == "h")
                m_tOriginCocks[m_cntCock].h = atoi(v.c_str());
            else if(n == "x")
                m_tOriginCocks[m_cntCock].x = atoi(v.c_str());
            else if(n == "y")
                m_tOriginCocks[m_cntCock].y = atoi(v.c_str());
            else if(n == "a")
                m_tOriginCocks[m_cntCock].bAudio = atoi(v.c_str());
            else if(n == "v")
                m_tOriginCocks[m_cntCock].bVideo = atoi(v.c_str());;
        }

        m_cntCock++;
        if(m_cntCock >= 8)
            break;
    }

    emit cockChanged();

    return 0;
}

void HDsContext::initImg(std::string& path){
    qDebug("");
    img_path = path;
}

void HDsContext::initFont(std::string& path, int h){
    qDebug("");
    ttf_path = path;
    m_pFont = new FTGLPixmapFont(ttf_path.c_str());
    if (m_pFont){
        m_pFont->CharMap(FT_ENCODING_UNICODE);
        m_pFont->FaceSize(h);
    }
}

int HDsContext::push_video(int svrid, const av_picture* pic){
    if (action < 1)
        return -1;

    DsItemInfo* item = getItem(svrid);
    if (!item || item->bPause)
        return -2;

    int w = pic->width;
    int h = pic->height;
    bool bFirstFrame = false;

    switch(pic->fourcc)
    {
    case OOK_FOURCC('I', '4', '2', '0'):
    case OOK_FOURCC('Y', 'V', '1', '2'):
        {
            item->mutex.lock();
            if(!item->tex_yuv.data)
            {
                item->tex_yuv.data = (unsigned char *)malloc(w * h * 3 / 2);
                bFirstFrame = true;
                qDebug("malloc w=%d,h=%d", w, h);
            }

            unsigned char * y = item->tex_yuv.data;
            unsigned char * u = y + w * h;
            unsigned char * v = u + w * h / 4;
            unsigned char * s_y = pic->data[0];
            unsigned char * s_u = pic->data[1];
            unsigned char * s_v = pic->data[2];
            if(pic->fourcc == OOK_FOURCC('Y', 'V', '1', '2'))
            {
                s_v = pic->data[1];
                s_u = pic->data[2];
            }
            for(int i = 0; i < h; i++)
            {
                memcpy(y, s_y, w);
                y   += w;
                s_y += pic->stride[0];
            }
            h >>= 1;
            w >>= 1;
            for(int i = 0; i < h; i++)
            {
                memcpy(u, s_u, w);
                memcpy(v, s_v, w);
                u   += w;
                v   += w;
                s_u += pic->stride[1];
                s_v += pic->stride[2];
            }
            item->tex_yuv.width = pic->width;
            item->tex_yuv.height = pic->height;
            item->tex_yuv.type = GL_I420;
            item->mutex.unlock();
        }
        break;
#if 0
    case OOK_FOURCC('Y', 'U', 'Y', '2'):
    case OOK_FOURCC('Y', 'U', 'Y', 'V'):
    case OOK_FOURCC('U', 'Y', 'V', 'Y'):
    case OOK_FOURCC('H', 'D', 'Y', 'C'):
        {
            if(!tex_yuv[svrid].data)
            {
                tex_yuv[svrid].data = (unsigned char *)malloc(w * h * 2);
                qDebug("malloc w=%d,h=%d", w, h);
            }

            unsigned char * y   = tex_yuv[svrid].data;
            unsigned char * s_y = pic->data[0];
            w <<= 1;
            for(int i = 0; i < h; i++)
            {
                memcpy(y, s_y, w);
                y   += w;
                s_y += pic->stride[0];
            }
            tex_yuv[svrid].width = pic->width;
            tex_yuv[svrid].height = pic->height;
            //tex_yuv[svrid].type = GL_I420;
        }
        break;
#endif
    default:
        return -3000;
    }

    emit videoPushed(svrid, bFirstFrame);

    return 0;
}

int HDsContext::push_audio(int svrid, const av_pcmbuff* pcm){
    if (action < 1 || audio < 1)
        return -1;

    DsItemInfo* item = getItem(svrid);
    if (!item || item->bPause)
        return -2;

    // just cock window play audio
    if (svrid == 1){
        m_audioPlay->pushAudio((av_pcmbuff*)pcm);
    }

    if (item->bUpdateAverage){
        int channels = pcm->channels;
        unsigned short * src = (unsigned short *)pcm->pcmbuf;
        int samples = pcm->pcmlen >> 1 / channels; // >>1 beacause default bpp=16

        unsigned long long a[2];
        a[0] = 0;
        a[1] = 0;
        for (int s = 0; s < samples; ++s){
            for (int c = 0; c < channels; ++c){
                int n = c % 2;
                a[n] += *src;
                ++src;
            }
        }
        item->a_average[0] = a[0] / ((channels + 1) / 2) / samples;
        item->a_average[1] = a[1] / (channels / 2) / samples;

        item->bUpdateAverage = false;
    }

    emit audioPushed(svrid);

    return 0;
}

void* thread_http_req(void* param){
    char* szReq = (char*)param;
    qDebug(szReq);

    http_client * clt = new http_client(g_dsCtx->m_trans);
    clt->tracegrade = 4;

    std::string strRes;
    if(clt->request(g_dsCtx->m_strUrl.c_str(), szReq, 3000))
    {
        clt->query(strRes, 2000);
        qDebug(g_dsCtx->m_strUrl.c_str());
        qDebug(strRes.c_str());
    }

    free(szReq);
    clt->close();
    clt->release();

    if(strncmp(strRes.c_str(), "success", sizeof("success") - 1) == 0){
        emit g_dsCtx->sourceChanged(1, true);
    }else{
        emit g_dsCtx->sourceChanged(1, false);
    }

    pthread_exit(NULL);
}

void HDsContext::handle_event(DsEvent& event){
    qDebug("");

    // scale cock x y
    int x = event.dst_x;
    int y = event.dst_y;
    if (x != -1 && y != -1 && m_iCockW && m_iCockH){
        x *= m_iOriginCockW / (double)m_iCockW;
        y *= m_iOriginCockH / (double)m_iCockH;
    }

    char* szReq = (char*)malloc(512); // 在调用线程中注意释放
    if (event.type == DS_EVENT_PICK){
        __snprintf(szReq,
                   512,
                   "director.pick\r\n"
                   "pos=%d\r\n"
                   "pos=%d (%d,%d)\r\n",
                   event.src_svrid,
                   event.dst_svrid,
                   x,
                   y);
    }else if (event.type == DS_EVENT_STOP){
        __snprintf(szReq,
                   512,
                   "director.dclick\r\n"
                   "pos=%d (%d,%d)\r\n",
                   event.dst_svrid,
                   x,
                   y);
    }

    pthread_t pth;
    pthread_create(&pth, NULL, thread_http_req, szReq);
    pthread_detach(pth);
}
