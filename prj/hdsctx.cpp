#include "hdsctx.h"
#include "hrcloader.h"
#include "hmainwidget.h"

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

    //qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    int argc = 0;
    QApplication app(argc, NULL);

    qInstallMessageHandler(myLogHandler);

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

    g_mainWdg = new HMainWidget(pObj);

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
    display_mode = DISPLAY_MODE_TIMER;
    frames = 25;
    scale_mode = BIG_VIDEO_SCALE;
    filter = 0;

    m_curTick = 0;
    m_lastTick = 0;

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

    if (m_pFont){
        delete m_pFont;
        m_pFont = NULL;
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
#endif

#ifdef WIN32
    unsigned int hThread_glut = _beginthread(thread_glut, 0, this);
    //WaitForSingleObject((HANDLE)hThread_glut, INFINITE);
#endif

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
int HDsContext::parse_init_xml(const char* xml){
    qDebug(xml);

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

            if(n == "audio")
                m_tInit.audio    = atoi(v.c_str());
            else if(n == "play_audio")
                m_tInit.play_audio    = atoi(v.c_str());

            else if(n == "info")
                m_tInit.info     = atoi(v.c_str());
            else if(n == "infcolor")
                m_tInit.infcolor      = (unsigned int)strtoul(v.c_str(), NULL, 16);

            else if(n == "title")
                m_tInit.title     = atoi(v.c_str());
            else if(n == "titcolor")
                m_tInit.titcolor      = (unsigned int)strtoul(v.c_str(), NULL, 16);
            else if(n == "scale_method")
                m_tInit.scale_method = (int)strtoul(v.c_str(), NULL, 16);
            else if(n == "pause_method")
                m_tInit.pause_method= atoi(v.c_str());
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

        if(n == "width")
            m_tLayout.width     = atoi(v.c_str());
        else if(n == "height")
            m_tLayout.height    = atoi(v.c_str());
        else if(n == "frames"){
            frames = atoi(v.c_str());
            if (frames < 1)
                frames = 25;
            else if (frames > 30)
                frames = 30;
            qDebug("frames=%d", frames);
        }
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
        }

        if (w < 0 || h < 0)
            continue;

        ci.items[ci.itemCnt].rc.setRect(x,y,w,h);
        ci.items[ci.itemCnt].a = a;
        ci.items[ci.itemCnt].v = bV;
        ci.items[ci.itemCnt].srvid = u;

        ++ci.itemCnt;
        if(ci.itemCnt >= MAXNUM_COMB_SCREEN)
            break;
    }

    for (int i = 0; i < MAXNUM_COMB_SCREEN; ++i){
        m_preselect[i] = ci.items[i].srvid;
    }

    if (memcmp(&m_tComb, &m_tCombUndo, sizeof(DsScreenInfo)) != 0){
        m_tCombUndo = m_tComb;
    }
    m_tComb = ci;

    emit combChanged();

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

    if (buffer[0] < 0 || queue[0] < 0)
        return -5;

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
            __snprintf(cont, 128, "发 送 %d/%d", outputpkgs[0], outputpkgs[1]);
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

void HDsContext::fullscreen(int srvid, bool bFull){
    DsSvrItem* item = getItem(srvid);
    if (item){
        item->mutex.lock();
        if (bFull){
            filter = srvid;
            scale_mode = NONE_SCALE;
        }else{
            filter = 0;
            scale_mode = BIG_VIDEO_SCALE;
        }
        item->tex_yuv.release();
        item->mutex.unlock();
    }
}

#include "hffmpeg.h"
int HDsContext::push_video(int srvid, const av_picture* pic){
    if (action < 1)
        return -1;

    DsSvrItem* item = getItem(srvid);
    if (!item || item->bPause)
        return -2;

    if (filter != 0 && filter != srvid)
        return -3;

    m_curTick = (unsigned int)chsc_gettick();

    if (!item->bUpdateVideo)
        return 0;

    if (display_mode == DISPLAY_MODE_TIMER)
        item->bUpdateVideo = false;

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
                if (scale_mode == BIG_VIDEO_SCALE && h > 720){
//                    item->tex_yuv.width = w >> 2;
//                    item->tex_yuv.height = h >> 2;
                    if (srvid == 1){
                        item->tex_yuv.width = m_tLayout.items[m_tLayout.itemCnt-1].width();
                        item->tex_yuv.height = m_tLayout.items[m_tLayout.itemCnt-1].height();
                    }else{
                        item->tex_yuv.width = m_tLayout.items[0].width();
                        item->tex_yuv.height = m_tLayout.items[0].height();
                    }
                }else{
                    item->tex_yuv.width = w;
                    item->tex_yuv.height = h;
                }
                item->tex_yuv.data = (unsigned char *)malloc(item->tex_yuv.width * item->tex_yuv.height * 3 / 2);
                item->tex_yuv.type = GL_I420;
                bFirstFrame = true;
                qDebug("malloc w=%d,h=%d", item->tex_yuv.width, item->tex_yuv.height);
            }

            int y_size = item->tex_yuv.width * item->tex_yuv.height;
            unsigned char * y = item->tex_yuv.data;
            unsigned char * u = y + y_size;
            unsigned char * v = u + (y_size >> 2);
            unsigned char * s_y = pic->data[0];
            unsigned char * s_u = pic->data[1];
            unsigned char * s_v = pic->data[2];
            if(pic->fourcc == OOK_FOURCC('Y', 'V', '1', '2'))
            {
                s_v = pic->data[1];
                s_u = pic->data[2];
            }

            if (scale_mode == BIG_VIDEO_SCALE && h > 720){
                SwsContext* pSwsCtx = sws_getContext(w,h,AV_PIX_FMT_YUV420P, item->tex_yuv.width,item->tex_yuv.height,AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
                uint8_t* src_date[3];
                src_date[0] = s_y;
                src_date[1] = s_u;
                src_date[2] = s_v;
                uint8_t* dst_data[3];
                dst_data[0] = y;
                dst_data[1] = u;
                dst_data[2] = v;
                int stride[3];
                stride[0] = item->tex_yuv.width;
                stride[1] = stride[2] = item->tex_yuv.width / 2;
                sws_scale(pSwsCtx, src_date, pic->stride, 0, h, dst_data, stride);
                sws_freeContext(pSwsCtx);
            }else{
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
        }
        break;
#if 0
    case OOK_FOURCC('Y', 'U', 'Y', '2'):
    case OOK_FOURCC('Y', 'U', 'Y', 'V'):
    case OOK_FOURCC('U', 'Y', 'V', 'Y'):
    case OOK_FOURCC('H', 'D', 'Y', 'C'):
        {
            if(!tex_yuv[srvid].data)
            {
                tex_yuv[srvid].data = (unsigned char *)malloc(w * h * 2);
                qDebug("malloc w=%d,h=%d", w, h);
            }

            unsigned char * y   = tex_yuv[srvid].data;
            unsigned char * s_y = pic->data[0];
            w <<= 1;
            for(int i = 0; i < h; i++)
            {
                memcpy(y, s_y, w);
                y   += w;
                s_y += pic->stride[0];
            }
            tex_yuv[srvid].width = pic->width;
            tex_yuv[srvid].height = pic->height;
            //tex_yuv[srvid].type = GL_I420;
        }
        break;
#endif
    default:
        return -3000;
    }

    emit videoPushed(srvid, bFirstFrame);

    return 0;
}

int HDsContext::push_audio(int srvid, const av_pcmbuff* pcm){
    if (action < 1 || m_tInit.audio < 1)
        return -1;

    DsSvrItem* item = getItem(srvid);
    if (!item || item->bPause)
        return -2;

    if (filter != 0 && filter != srvid)
        return -3;

    // just comb window play audio
    if (srvid == 1){
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
                a[c % 2] += *src;
                ++src;
                if (!src)
                    return -6;
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

void* thread_http_req(void* param){
    char* szReq = (char*)param;
    qDebug(szReq);

    http_client * clt = new http_client(g_dsCtx->m_trans);
    clt->tracegrade = 4;

    std::string strRes;
    if(clt->request(url_handle_event, szReq, 3000))
    {
        clt->query(strRes, 2000);
        qDebug(strRes.c_str());
    }

    free(szReq);
    clt->close();
    clt->release();

    pthread_exit(NULL);
}

void HDsContext::handle_event(DsEvent& event){
    qDebug("");

    // scale comb x y
    int x = event.dst_x;
    int y = event.dst_y;
    if (x != -1 && y != -1 && m_tLayout.combW && m_tLayout.combH){
        x *= (double)m_tComb.width / (double)m_tLayout.combW;
        y *= (double)m_tComb.height / (double)m_tLayout.combH;
    }

    char* szReq = (char*)malloc(512); // 在调用线程中注意释放
    if (event.type == DS_EVENT_PICK){
        __snprintf(szReq,
                   512,
                   "director.pick\r\n"
                   "pos=%d\r\n"
                   "pos=%d (%d,%d)\r\n",
                   event.src_srvid,
                   event.dst_srvid,
                   x,
                   y);
    }else if (event.type == DS_EVENT_STOP){
        __snprintf(szReq,
                   512,
                   "director.dclick\r\n"
                   "pos=%d (%d,%d)\r\n",
                   event.dst_srvid,
                   x,
                   y);
    }

    pthread_t pth;
    pthread_create(&pth, NULL, thread_http_req, szReq);
    pthread_detach(pth);
}

ScreenItem* HDsContext::getScreenItem(int srvid){
    ScreenItem* item = NULL;
    for (int i = 0; i < m_tComb.itemCnt; ++i){
        if (m_tComb.items[i].srvid == srvid){
            item = & m_tComb.items[i];
            break;
        }
    }

    return item;
}
