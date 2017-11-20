#include "hdsctx.h"
#include "hrcloader.h"
#include "hmainwidget.h"
#include "ds.h"

HDsContext* g_dsCtx = NULL;
HMainWidget* g_mainWdg = NULL;
const char* url_handle_event = "http://127.0.0.1/transcoder/index.php?controller=channels&action=srcchange";

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
    char szLog[2048];

#ifndef QT_NO_DEBUG
    snprintf(szLog, 2047, "%s %s [%s:%u, %s]\n", szType, msg.toLocal8Bit().constData(), ctx.file, ctx.line, ctx.function);
#else
    if (msg.length() > 0){
        snprintf(szLog, 2047, "%s %s\n", szType, msg.toLocal8Bit().constData());
    }
#endif

    QString strLogFilePath = QCoreApplication::applicationDirPath() + "/ds.log";

    FILE* fp = fopen(strLogFilePath.toLocal8Bit().data(), "a");
    if (fp){
        fseek(fp, 0, SEEK_END);
        if (ftell(fp) > (2 << 20)){
            fclose(fp);
            fp = fopen(strLogFilePath.toLocal8Bit().data(), "w");
        }
    }

    if (fp){
        fwrite(szLog, 1, strlen(szLog), fp);
        fclose(fp);
    }
}

#include <QApplication>
#include <QDesktopWidget>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QSplashScreen>
#include <QProgressBar>
#ifdef WIN32
void HDsContext::thread_gui(void* param) {
#else
void* HDsContext::thread_gui(void* param){
#endif
    qDebug("thread_gui start");

    HDsContext* pObj = (HDsContext*)param;

    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    int argc = 0;
    QApplication app(argc, NULL);

    qInstallMessageHandler(myLogHandler);
    qDebug("log print start");

#ifndef QT_NO_TRANSLATION
    QString translatorFileName = QLatin1String("qt_");
    translatorFileName += QLocale::system().name();
    QTranslator *translator = new QTranslator(&app);
    if (translator->load(translatorFileName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(translator);
#endif

    QFont font = QApplication::font();
    font.setPointSize(18);
    QApplication::setFont(font);

    int sw = QApplication::desktop()->width();
    int sh = QApplication::desktop()->height();

    QSplashScreen* splash = new QSplashScreen;
    splash->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    splash->setGeometry(0,0,sw,sh);
    splash->setStyleSheet("background-color: black");

    QProgressBar* progress = new QProgressBar(splash);
    progress->setGeometry(sw/2 - 150, sh/2 + 50, 300, 30);
    progress->setStyleSheet("background-color: white; border:2px solid gray; border-radius: 5px");
    progress->setRange(0,100);
    progress->setValue(0);
    splash->showFullScreen();
    app.processEvents();

    splash->showMessage("Loading icon...", Qt::AlignCenter, Qt::white);
    progress->setValue(10);
    app.processEvents();
    HRcLoader::instance()->loadIcon();
    HNetwork::instance();

    splash->showMessage("Creating main UI...", Qt::AlignCenter, Qt::white);
    progress->setValue(50);
    app.processEvents();

    g_mainWdg = new HMainWidget;

    splash->showMessage("Loading completed!", Qt::AlignCenter, Qt::white);
    progress->setValue(100);
    app.processEvents();

    qDebug("mainwdg create succeed");
    pObj->m_mutex.unlock();

    splash->finish(g_mainWdg);
    delete splash;

    app.exec();

    HRcLoader::exitInstance();
    HNetwork::exitInstance();

#ifdef linux
    pthread_exit(NULL);
#endif
}

HDsContext::HDsContext()
{
    ref     = 1;
    init    = 0;
    action  = -1;

    m_curTick = 0;
    m_lastTick = 0;

    m_audioPlay = new HAudioPlay;
    m_playaudio_srvid = 1;
}

HDsContext::~HDsContext(){
    if (m_audioPlay){
        delete m_audioPlay;
        m_audioPlay = NULL;
    }

    if (m_pFont){
        delete m_pFont;
        m_pFont = NULL;
    }
}

#include <QWaitCondition>
void HDsContext::start_gui_thread(){
    qDebug("start_gui_thread<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");

    m_mutex.lock();

#ifdef linux
    pthread_t pth;
    pthread_create(&pth, NULL, thread_gui, this);
    //void* pRet;
    //pthread_join(pth, &pRet);
    pthread_detach(pth);
#endif

#ifdef WIN32
    unsigned int hThread_glut = _beginthread(thread_glut, 0, this);
    //WaitForSingleObject((HANDLE)hThread_glut, INFINITE);
#endif

    // wait until gui create succeed
    m_mutex.lock();
    m_mutex.unlock();
    qDebug("start_gui_thread>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}

/*
    <complex type="director_service">
        <render oper="display" >
            <param n="width"  v="640" />
            <param n="height" v="480" />
        </render>
    </complex>
 */
//int HDsContext::parse_init_xml(const char* xml){
//    qDebug(xml);

//    ook::xml_element root;
//    if(root.parse(xml, strlen(xml)) < 0)
//        return -1001;
//    if(root.tag_name() != "complex")
//        return -1002;
//    if(root.get_attribute("type") != "director_service")
//        return -1003;

//    ook::xml_parser::enum_childen(&root, NULL);
//    const ook::xml_element * render = NULL;
//    while(1)
//    {
//        render = ook::xml_parser::enum_childen(&root, "render");
//        if(!render)
//            break;
//        const std::string & oper = render->get_attribute("oper");

//        ook::xml_parser::enum_childen(render, NULL);
//        const ook::xml_element * e = NULL;
//        while(1)
//        {
//            e = ook::xml_parser::enum_childen(render, "param");
//            if(!e)
//                break;
//            const std::string & n = e->get_attribute("n");
//            const std::string & v = e->get_attribute("v");

//            if(n == "audio")
//                m_tInit.audio    = atoi(v.c_str());
//            else if(n == "drawinfo")
//                m_tInit.drawinfo     = atoi(v.c_str());
//            else if(n == "infcolor")
//                m_tInit.infcolor      = (unsigned int)strtoul(v.c_str(), NULL, 16);
//            else if(n == "titcolor")
//                m_tInit.titcolor      = (unsigned int)strtoul(v.c_str(), NULL, 16);
//        }
//    }

//    return 0;
//}

/*
<?xml version="1.0" encoding="UTF-8" ?>
<director_service>
    <layout>
        <head>
            <param n="width"  v="1280" />
            <param n="height" v="800"  />
            <param n="fps" v="15"   />
        </head>
        <body>
            <item>
                <param n="u"   v="1" />
                <param n="w"   v="424" />
                <param n="h"   v="264" />
                <param n="x"   v="4"   />
                <param n="y"   v="4"   />
            </item>
            <item>
                <param n="u"   v="2"   />
                <param n="w"   v="424" />
                <param n="h"   v="264" />
                <param n="x"   v="4"   />
                <param n="y"   v="268" />
            </item>
            <item>
                <param n="u"   v="3"   />
                <param n="w"   v="424" />
                <param n="h"   v="264" />
                <param n="x"   v="4"   />
                <param n="y"   v="532" />
            </item>
            <item>
                <param n="u"   v="4"   />
                <param n="w"   v="424" />
                <param n="h"   v="264" />
                <param n="x"   v="428" />
                <param n="y"   v="532" />
            </item>
            <item>
                <param n="u"   v="5"   />
                <param n="w"   v="424" />
                <param n="h"   v="264" />
                <param n="x"   v="852" />
                <param n="y"   v="532" />
            </item>
            <item>
                <param n="u"   v="6"   />
                <param n="w"   v="848" />
                <param n="h"   v="528" />
                <param n="x"   v="428" />
                <param n="y"   v="4"   />
            </item>
        </body>
    </layout>
</director_service>
*/
int HDsContext::parse_layout_xml(const char* xml_file){
    qDebug(xml_file);

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

        if (n == "autolayout"){
            m_tInit.autolayout = atoi(v.c_str());
        }else if (n == "row"){
            m_tInit.row = atoi(v.c_str());
        }else if (n == "col"){
            m_tInit.col = atoi(v.c_str());
        }else if(n == "width"){
            m_tLayout.width     = atoi(v.c_str());
        }else if(n == "height"){
            m_tLayout.height    = atoi(v.c_str());
        }else if (n == "audio"){
            m_tInit.audio = atoi(v.c_str());
        }else if(n == "fps"){
            m_tInit.fps = atoi(v.c_str());
        }else if (n == "display_mode"){
            m_tInit.display_mode = atoi(v.c_str());
        }else if (n == "scale_mode"){
            m_tInit.scale_mode = atoi(v.c_str());
        }else if (n == "drawtitle"){
            m_tInit.drawtitle = atoi(v.c_str());
        }else if (n == "drawfps"){
            m_tInit.drawfps = atoi(v.c_str());
        }else if (n == "drawnum"){
            m_tInit.drawnum = atoi(v.c_str());
        }else if (n == "drawaudio"){
            m_tInit.drawaudio = atoi(v.c_str());
        }else if(n == "drawinfo")
            m_tInit.drawinfo     = atoi(v.c_str());
        else if(n == "infcolor")
            m_tInit.infcolor      = (unsigned int)strtoul(v.c_str(), NULL, 16);
        else if(n == "titcolor")
            m_tInit.titcolor      = (unsigned int)strtoul(v.c_str(), NULL, 16);
    }

    int i = 0;
    ook::xml_parser::enum_childen(body, NULL);
    const ook::xml_element * item;
    while(1)
    {
        item = ook::xml_parser::enum_childen(body, "item");
        if(!item)
            break;
        ook::xml_parser::enum_childen(item, NULL);
        const ook::xml_element * e;
        int x,y,w,h;
        while(1)
        {
            e = ook::xml_parser::enum_childen(item, "param");
            if(!e)
                break;
            const std::string & n = e->get_attribute("n");
            const std::string & v = e->get_attribute("v");
            if(n == "w")
                w = atoi(v.c_str());
            else if(n == "h")
                h = atoi(v.c_str());
            else if(n == "x")
                x = atoi(v.c_str());
            else if(n == "y")
                y = atoi(v.c_str());
        }
        w = w >> 2 << 2; // let w is 4的整数倍
        m_tLayout.items[i].setRect(x, y, w, h);

        ++i;
        if(i >= MAXNUM_LAYOUT)
            break;
    }
    m_tLayout.itemCnt = i;

    // last is comb window
    m_tLayout.combW = m_tLayout.items[m_tLayout.itemCnt-1].width();
    m_tLayout.combH = m_tLayout.items[m_tLayout.itemCnt-1].height();

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
int HDsContext::parse_comb_xml(const char* xml){
    qDebug(xml);

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

    ook::xml_parser::enum_childen(head, NULL);
    const ook::xml_element * e;
    DsScreenInfo ci;
    while(1)
    {
        e = ook::xml_parser::enum_childen(head, "param");
        if(!e)
            break;
        const std::string & n = e->get_attribute("n");
        const std::string & v = e->get_attribute("v");

        if(n == "width")
            ci.width = atoi(v.c_str());
        else if(n == "height")
            ci.height = atoi(v.c_str());
    }

    if(ci.width < 1 || ci.height < 1)
        return -10;

    ook::xml_parser::enum_childen(body, NULL);
    const ook::xml_element * item;
    while(1)
    {
        item = ook::xml_parser::enum_childen(body, "item");
        if(!item)
            break;

        ook::xml_parser::enum_childen(item, NULL);
        const ook::xml_element * e;
        int x,y,w=-1,h=-1,a;
        int u = 0;
        int bV = 1;
        QString src;
        while(1)
        {
            e = ook::xml_parser::enum_childen(item, "param");
            if(!e)
                break;
            const std::string & n = e->get_attribute("n");
            const std::string & v = e->get_attribute("v");
            if(n == "w")
                w = atoi(v.c_str());
            else if(n == "h")
                h = atoi(v.c_str());
            else if(n == "x")
                x = atoi(v.c_str());
            else if(n == "y")
                y = atoi(v.c_str());
            else if(n == "a")
                a = atoi(v.c_str());
            else if(n == "u")
                u = atoi(v.c_str());
            else if (n == "v")
                bV = atoi(v.c_str());
            else if (n == "src")
                src = v.c_str();
        }

        if (w < 0 || h < 0)
            continue;

        if (a && !bV)
            ci.micphone = u; // record micphone

        ci.items[ci.itemCnt].id = ci.itemCnt;
        ci.items[ci.itemCnt].rc.setRect(x,y,w,h);
        ci.items[ci.itemCnt].a = u && a;
        ci.items[ci.itemCnt].v = u && bV;
        ci.items[ci.itemCnt].srvid = u;
        strncpy(ci.items[ci.itemCnt].src, src.toLocal8Bit().constData(), MAXLEN_STR);

        if ((w > ci.width - 10) && (h > ci.height - 10)){
            ci.items[ci.itemCnt].bMainScreen = true;
            ci.comb_type = DsScreenInfo::PIP;
        }else{
            ci.items[ci.itemCnt].bMainScreen = false;
        }

        ++ci.itemCnt;
        if(ci.itemCnt >= MAXNUM_COMB_SCREEN)
            break;
    }

    for (int i = 0; i < MAXNUM_COMB_SCREEN; ++i){
        m_preselect[i] = ci.items[i].srvid;
    }

    m_tComb = ci;

    emit combChanged();

    return 0;
}

/*
<sound_mixer>
    <body>
        <item>
            <param n="src" v="l=http&amp;i=http://192.168.1.232:8000/live/ZJWS"></param>
            <param n="a" v="0"></param>
            <param n="v" v="22"></param>
        </item>
        <item>
            <param n="src" v="OUTPUT"></param>
            <param n="a" v="1"></param>
            <param n="v" v="-50"></param>
        </item>
        <item>
            <param n="src" v="l=http&amp;i=http://192.168.1.232:8000/live/YXFY"></param>
            <param n="a" v="1"></param>
            <param n="v" v="26"></param>
        </item>
        <item>
            <param n="src" v="OUTPUT"></param>
            <param n="a" v="1"></param>
            <param n="v" v="-50"></param>
        </item>
    </body>
</sound_mixer>
*/
int HDsContext::parse_audio_xml(const char* xml){
    qDebug(xml);
    ook::xml_element root;
    if(!root.parse(xml, strlen(xml)))
        return -1;

    const ook::xml_element * cocktail = ook::xml_parser::get_element(&root, "<sound_mixer>");
    if(!cocktail)
        return -2;

    const ook::xml_element * body = ook::xml_parser::get_element(cocktail, "<body>");
    if(!body)
        return -3;

    ook::xml_parser::enum_childen(body, NULL);
    const ook::xml_element * item;
    while(1)
    {
        item = ook::xml_parser::enum_childen(body, "item");
        if(!item)
            break;

        ook::xml_parser::enum_childen(item, NULL);
        const ook::xml_element * e;
        QString src;
        int a;
        int vol;
        while (1){
            e = ook::xml_parser::enum_childen(item, "param");
            if(!e)
                break;
            const std::string & n = e->get_attribute("n");
            const std::string & v = e->get_attribute("v");
            if(n == "src")
                src = v.c_str();
            else if (n == "a")
                a = atoi(v.c_str());
            else if (n == "v")
                vol = atoi(v.c_str());
        }

        for (int i = 0; i < m_tComb.itemCnt; ++i){
            if (m_tComb.items[i].src == src){
                m_tComb.items[i].a = a;
            }
        }
    }

    return 0;
}

/*
    <?xml version="1.0" encoding="UTF-8" ?>
    <rsp version="1.1">
    <list>
    <item>
    <param n="TTID" v="uAtCXCR9W0i1v5gazCp9" />
    <param n="taskname" v="ook" />
    <param n="age" v="24" />
    <param n="wheel" v="1" />
    <param n="inputpkgs" v="0/714/0" />
    <param n="sync" v="30343963|0/30343475|0/30343963|30343643" />
    <param n="audiopkgs" v="110/989(MPEG1|48000|1)/1109/900/0" />
    <param n="videopkgs" v="603/589(MPEG2|720X576|25|De:2)/538/483/0" />
    <param n="buffer" v="7/0/0" />
    <param n="queue" v="xxx/xxx" />
    <param n="overflow" v="0/0/0" />
    <param n="outputpkgs" v="2046" />
    <param n="outputspeed" v="2046" />
    </item>
    </list>
    </rsp>
 */
int HDsContext::parse_taskinfo_xml(const char* xml){
    //qDebug(xml);

    ook::xml_element root;
    if(!root.parse(xml, strlen(xml)))
        return -1;

    const ook::xml_element * rsp = ook::xml_parser::get_element(&root, "<rsp>");
    if(!rsp)
        return -2;

    const ook::xml_element * list = ook::xml_parser::get_element(rsp, "<list>");
    if(!list)
        return -3;

    const ook::xml_element * item = ook::xml_parser::get_element(list, "<item>");
    if(!item)
        return -4;

    int buffer[3] = {-1};
    int queue [2] = {-1};
    int outputpkgs[2] = {-1};
    int outputspeed = -1;
    const ook::xml_element * e;
    ook::xml_parser::enum_childen(item, NULL);
    while(1)
    {
        e = ook::xml_parser::enum_childen(item, "param");
        if(!e)
            break;
        const std::string & n = e->get_attribute("n");
        const std::string & v = e->get_attribute("v");
        if(n == "buffer"){
            separator sept(v.c_str(), "/");
            if(sept[0])
                buffer[0] = atoi(sept[0]);
            if(sept[1])
                buffer[1] = atoi(sept[1]);
            if(sept[2])
                buffer[2] = atoi(sept[2]);
        }else if(n == "queue"){
            separator sept(v.c_str(), "/");
            if(sept[0])
                queue[0] = atoi(sept[0]);
            if(sept[1])
                queue[1] = atoi(sept[1]);
        }else if(n == "outputpkgs"){
            separator sept(v.c_str(), "/");
            if (sept[0])
                outputpkgs[0] = atoi(sept[0]);
            if (sept[1])
                outputpkgs[1] = atoi(sept[1]);
        }else if(n == "outputspeed"){
            outputspeed = atoi(v.c_str());
        }
    }

    const ook::xml_element * elem_outputpkgs  = ook::xml_parser::get_element(item, "<outputpkgs>");
    const ook::xml_element * udptransfer = NULL;
    udptrf_s us;
    init_udptrf_s(&us);
    if(elem_outputpkgs)
        udptransfer = ook::xml_parser::get_element(elem_outputpkgs, "<udp-transfer>");
    if (udptransfer){
        /*
            <outputpkgs>
            <udp-transfer>
            <server>192.168.1.21:3066</server>
            <dest>127.0.0.1:1234</dest>

            <received>578</received>
            <dispatch>578</dispatch>
            <resend>0</resend>
            <loss>0</loss>
            <buffer>0</buffer>
            <overflow>0</overflow>

            <intf>
            <name>eth0</name>
            <state>on</state>
            <disp>578</disp>
            <loss>0</loss>
            <speeds>230</speeds>
            <speedl>235</speedl>
            </intf>
            </udp-transfer>
            </outputpkgs>
         */
        const ook::xml_element * received = ook::xml_parser::get_element(udptransfer, "<received>");
        if(received)
            us.received = (unsigned int)strtoul(received->text().c_str(), NULL, 10);

        const ook::xml_element * dispatch = ook::xml_parser::get_element(udptransfer, "<dispatch>");
        if(dispatch)
            us.dispatch = (unsigned int)strtoul(dispatch->text().c_str(), NULL, 10);

        const ook::xml_element * resend   = ook::xml_parser::get_element(udptransfer, "<resend>");
        if(resend)
            us.resend   = (unsigned int)strtoul(resend->text().c_str(), NULL, 10);

        const ook::xml_element * loss     = ook::xml_parser::get_element(udptransfer, "<loss>");
        if(loss)
            us.loss     = (unsigned int)strtoul(loss->text().c_str(), NULL, 10);

        const ook::xml_element * buffer   = ook::xml_parser::get_element(udptransfer, "<buffer>");
        if(buffer)
            us.buffer   = (unsigned int)strtoul(buffer->text().c_str(), NULL, 10);

        const ook::xml_element * overflow = ook::xml_parser::get_element(udptransfer, "<overflow>");
        if(overflow)
            us.overflow = (unsigned int)strtoul(overflow->text().c_str(), NULL, 10);

        int index = 0;
        ook::xml_parser::enum_childen(udptransfer, NULL);
        const ook::xml_element * e_intf = NULL;
        while(1)
        {
            e_intf = ook::xml_parser::enum_childen(udptransfer, "intf");
            if(!e_intf)
                break;

            const ook::xml_element * e_name  = ook::xml_parser::get_element(e_intf, "<name>");
            if(e_name)
                us.intf[index].name  = e_name->text();

            const ook::xml_element * e_stat  = ook::xml_parser::get_element(e_intf, "<state>");
            if(e_stat)
            {
                if(e_stat->text() != "on")
                {
                    us.intf[index].name = "";
                    continue;
                }
            }

            const ook::xml_element * e_loss  = ook::xml_parser::get_element(e_intf, "<loss>");
            if(e_loss)
                us.intf[index].loss  = (unsigned int)strtoul(e_loss->text().c_str(), NULL, 10);

            const ook::xml_element * e_speed = ook::xml_parser::get_element(e_intf, "<speeds>");
            if(e_speed)
                us.intf[index].speed = (unsigned int)strtoul(e_speed->text().c_str(), NULL, 10);
            index++;
        }
    }

    std::string s_cont;
    char cont[128];
    __snprintf(cont, 128, "缓 存 %d/%d/%d ", buffer[0], buffer[1], buffer[2]);
    s_cont = cont;

    if(udptransfer)
    {
        __snprintf(cont, 128, "接 收 %d", us.received);
        s_cont += "\r\n";
        s_cont += cont;
        __snprintf(cont, 128, "发 送 %d", us.dispatch);
        s_cont += "\r\n";
        s_cont += cont;
        __snprintf(cont, 128, "重 传 %d", us.resend);
        s_cont += "\r\n";
        s_cont += cont;
        __snprintf(cont, 128, "丢 失 %d", us.loss);
        s_cont += "\r\n";
        s_cont += cont;
        __snprintf(cont, 128, "待 发 %d", us.buffer);
        s_cont += "\r\n";
        s_cont += cont;
        __snprintf(cont, 128, "溢 出 %d", us.overflow);
        s_cont += "\r\n";
        s_cont += cont;

        int i = 0;
        int total_speed = 0;
        for(; i < 8; i++)
        {
            if(us.intf[i].name.length() == 0)
                break;
            total_speed += us.intf[i].speed;
            __snprintf(cont, 128, "%-5s: %5ukps (%u) ", us.intf[i].name.c_str(), us.intf[i].speed, us.intf[i].loss);
            s_cont += "\r\n";
            s_cont += cont;
        }
        if(i > 1 && total_speed > 0)
        {
            s_cont += "\r\n";
            s_cont += "------------";
            __snprintf(cont, 128, "%-4s: %5ukps ", "TOT", total_speed);
            s_cont += "\r\n";
            s_cont += cont;
        }
    }
    else
    {
        if(queue[0] > -1)
        {
            __snprintf(cont, 128, "排 队 %d/%d", queue[0], queue[1]);
            s_cont += "\r\n";
            s_cont += cont;
        }
        if(outputspeed > -1)
            __snprintf(cont, 128, "发 送 %d/%d (%dkps)", outputpkgs[0], outputpkgs[1], outputspeed);
        else
            __snprintf(cont, 128, "发 送 %d (%dkps)", outputpkgs[0], outputpkgs[1]);
        s_cont += "\r\n";
        s_cont += cont;
    }

    m_strTaskInfo = s_cont;
    //qDebug(m_strTaskInfo.c_str());

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

void HDsContext::resizeForScale(int srvid, int w, int h){
    qDebug("rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr %d %d", w, h);
    DsSvrItem* item = getItem(srvid);
    if (item){
        item->mutex.lock();
        item->show_w = w;
        item->show_h = h;
        if (m_tInit.scale_mode == BIG_VIDEO_SCALE){
            item->tex_yuv.release();

            if (item->pSwsCtx){
                sws_freeContext(item->pSwsCtx);
                item->pSwsCtx = NULL;
            }
        }
        item->mutex.unlock();
    }
}

#include "hffmpeg.h"
int HDsContext::push_video(int srvid, const av_picture* pic){
    if (action < 1)
        return -1;

    DsSvrItem* item = getItem(srvid);
    if (!item)
        return -2;

    int w = pic->width;
    int h = pic->height;
    if (pic->fourcc != OOK_FOURCC('I', '4', '2', '0') && pic->fourcc != OOK_FOURCC('Y', 'V', '1', '2'))
        return -4;


    item->mutex.lock();
    bool bFirst = false;
    if (w != item->pic_w || h != item->pic_h || !item->video_buffer){
        item->pic_w = w;
        item->pic_h = h;
        if (item->video_buffer){
            delete item->video_buffer;
            item->video_buffer = NULL;
        }
        item->video_buffer = new HRingBuffer(w*h*3/2, 10);
        bFirst = true;

#if LAYOUT_TYPE_ONLY_OUTPUT
        m_tComb.width = w;
        m_tComb.height = h;
        m_tComb.itemCnt = 1;
        m_tComb.items[0].srvid = 1;
        m_tComb.items[0].rc.setRect(0,0,w,h);
#endif
    }

    char* ptr = item->video_buffer->write();
    if (ptr){
        int y_size = w*h;
        char * y = ptr;
        char * u = y + y_size;
        char * v = u + (y_size >> 2);
        char * s_y = (char*)pic->data[0];
        char * s_u = (char*)pic->data[1];
        char * s_v = (char*)pic->data[2];
        if(pic->fourcc == OOK_FOURCC('Y', 'V', '1', '2'))
        {
            char* tmp = s_u;
            s_u = s_v;
            s_v = tmp;
        }
        for(int i = 0; i < h; ++i)
        {
            memcpy(y, s_y, w);
            y   += w;
            s_y += pic->stride[0];
        }
        h >>= 1;
        w >>= 1;
        for(int i = 0; i < h; ++i)
        {
            memcpy(u, s_u, w);
            memcpy(v, s_v, w);
            u   += w;
            v   += w;
            s_u += pic->stride[1];
            s_v += pic->stride[2];
        }
    }
    item->mutex.unlock();

    emit videoPushed(srvid, bFirst);
}

int HDsContext::pop_video(int srvid){
    if (action < 1)
        return -1;

    DsSvrItem* item = getItem(srvid);
    if (!item)
        return -2;

    if (!item->canShow()){
        return -3;
    }

    if (!item->video_buffer)
        return -4;

    int retcode = -5;
    item->mutex.lock();
    char* ptr = item->video_buffer->read();
    if (!ptr){
        qDebug("read to fast");
    }
    if (ptr){
        if (!item->tex_yuv.data){
            if (m_tInit.scale_mode == BIG_VIDEO_SCALE && item->pic_w > item->show_w && item->pic_h > item->show_h){
                item->tex_yuv.width = item->show_w >> 2 << 2;
                item->tex_yuv.height = item->show_h;

                item->pSwsCtx = sws_getContext(item->pic_w,item->pic_h,AV_PIX_FMT_YUV420P,
                                                     item->tex_yuv.width,item->tex_yuv.height,AV_PIX_FMT_YUV420P,
                                                     SWS_POINT, NULL, NULL, NULL);
            }else{
                item->tex_yuv.width = item->pic_w;
                item->tex_yuv.height = item->pic_h;
            }

            item->tex_yuv.data = (unsigned char *)malloc(item->tex_yuv.width * item->tex_yuv.height * 3 / 2);
            item->tex_yuv.type = GL_I420;
            qDebug("malloc %d*%d, pic=%d*%d, show=%d*%d", item->tex_yuv.width, item->tex_yuv.height,
                   item->pic_w, item->pic_h, item->show_w, item->show_h);
        }

        if (item->pic_w == item->tex_yuv.width && item->pic_h == item->tex_yuv.height){
            memcpy(item->tex_yuv.data, ptr, item->pic_w*item->pic_h*3/2);
        }else{
            uint8_t* src_date[3];
            src_date[0] = (unsigned char*)ptr;
            int src_ysize = item->pic_w * item->pic_h;
            src_date[1] = src_date[0] + src_ysize;
            src_date[2] = src_date[1] + (src_ysize >> 2);
            int src_stride[3];
            src_stride[0] = item->pic_w;
            src_stride[1] = src_stride[2] = item->pic_w >> 1;
            uint8_t* dst_data[3];
            int dst_ysize = item->tex_yuv.width * item->tex_yuv.height;
            dst_data[0] = item->tex_yuv.data;
            dst_data[1] = dst_data[0] + dst_ysize;
            dst_data[2] = dst_data[1] + (dst_ysize >> 2);
            int dst_stride[3];
            dst_stride[0] = item->tex_yuv.width;
            dst_stride[1] = dst_stride[2] = item->tex_yuv.width >> 1;
            sws_scale(item->pSwsCtx, src_date, src_stride, 0, item->pic_h, dst_data, dst_stride);
        }

        retcode = 0;
    }
    item->mutex.unlock();

    return retcode;
}

int HDsContext::push_audio(int srvid, const av_pcmbuff* pcm){
    if (action < 1 || m_tInit.audio < 1)
        return -1;

    DsSvrItem* item = getItem(srvid);
    if (!item)
        return -2;

    if (!item->canShow()){
        return -3;
    }

    if (srvid == m_playaudio_srvid){
        m_audioPlay->pushAudio((av_pcmbuff*)pcm);
    }

    if (item->bUpdateAverage){
        int channels = pcm->channels;
        unsigned short * src = (unsigned short *)pcm->pcmbuf;
        int samples = pcm->pcmlen / 2 / channels; // /2 beacause default bpp=16

        unsigned long long a[2];
        a[0] = 0;
        a[1] = 0;
        for (int s = 0; s < samples; ++s){
            for (int c = 0; c < channels; ++c){
                a[c % 2] += *src;
                ++src;
            }
        }
        item->a_average[0] = a[0] / ((channels + 1) / 2) / samples;
        if (channels > 1){
            item->a_average[1] = a[1] / (channels / 2) / samples;
            item->a_channels = 2;
        }else{
            item->a_average[1] = 0;
            item->a_channels = 1;
        }

        item->bUpdateAverage = false;
    }

    emit audioPushed(srvid);

    return 0;
}

HScreenItem* HDsContext::getHScreenItem(int srvid){
    HScreenItem* item = NULL;
    for (int i = 0; i < m_tComb.itemCnt; ++i){
        if (m_tComb.items[i].srvid == srvid){
            item = & m_tComb.items[i];
            break;
        }
    }

    return item;
}

void HDsContext::pause(int srvid, bool bPause){
    DsSvrItem* item = getItem(srvid);
    if (item && item->ifcb){
        qDebug("srvid=%d ifservice_callback::e_service_cb_pause", srvid);
        //item->bPause = true;
#if LAYOUT_TYPE_ONLY_OUTPUT
        item->ifcb->onservice_callback(ifservice_callback::e_service_cb_pause, libchar(), 0, 0, bPause, NULL);
#else
        if (srvid == 1){
            item->ifcb->onservice_callback(ifservice_callback::e_service_cb_chr, libchar(), OOK_FOURCC('P', 'A', 'U', 'S'), 0, 0, NULL);
        }else{
            item->ifcb->onservice_callback(ifservice_callback::e_service_cb_pause, libchar(), OOK_FOURCC('P', 'A', 'U', 'S'), 0, bPause, NULL);
        }
#endif
    }
}

void HDsContext::setPlayaudioSrvid(int id){
    if (m_playaudio_srvid != id){
        m_audioPlay->stopPlay();
        m_playaudio_srvid = id;
    }
}
