#include "hdsctx.h"
#include "hrcloader.h"
#include "hmainwidget.h"

HDsContext* g_dsCtx = NULL;
HMainWidget* g_mainWdg = NULL;

#include <QDateTime>
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

    QString strLog = QString::asprintf("[%s] [%s]:%s [%s:%d - %s]\n",
                                       QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toLocal8Bit().data(),
                                       szType,
                                       msg.toUtf8().data(),
                                       ctx.file,ctx.line,ctx.function);

    //QString strLogFilePath = QCoreApplication::applicationDirPath() + "/ds.log";
    QString strLogFilePath = "/opt/anystreaming/transcoder/ds.log";

    FILE* fp = fopen(strLogFilePath.toLocal8Bit().data(), "a");
    if (fp){
        fseek(fp, 0, SEEK_END);
        if (ftell(fp) > (2 << 20)){
            fclose(fp);
            fp = fopen(strLogFilePath.toLocal8Bit().data(), "w");
        }
    }

    if (fp){
        fputs(strLog.toLocal8Bit().data(), fp);
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
#include "heffectwidget.h"
#include "hexprewidget.h"
#include "hdsconf.h"
#ifdef WIN32
void HDsContext::thread_gui(void* param) {
#else
void* HDsContext::thread_gui(void* param){
#endif
    qInfo("--------------thread_gui start------------------");

    HDsContext* pObj = (HDsContext*)param;

    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    int argc = 0;
    QApplication app(argc, NULL);

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

    splash->showMessage("Loading settings...", Qt::AlignCenter, Qt::white);
    progress->setValue(10);
    app.processEvents();
    QString str = g_dsCtx->cur_path.c_str();
#if LAYOUT_TYPE_OUTPUT_AND_MV
    QFile file("/var/www/appname.txt");
    if (file.open(QIODevice::ReadOnly)){
        QString appname = file.readAll();
        if (appname == "transcoder_sohu")
            str += "ds_sohu.conf";
        else
            str += "ds.conf";
    }else{
        str += "ds.conf";
    }
#else
    str += "ds_out.conf";
#endif
    HDsConf::instance()->load(str);

    splash->showMessage("Loading icon...", Qt::AlignCenter, Qt::white);
    progress->setValue(20);
    app.processEvents();
    HRcLoader::instance()->loadIcon();

    splash->showMessage("Creating widgets...", Qt::AlignCenter, Qt::white);
    progress->setValue(30);
    app.processEvents();
    HExpreWidget::instance();
    HEffectWidget::instance();

    splash->showMessage("Creating main UI...", Qt::AlignCenter, Qt::white);
    progress->setValue(50);
    app.processEvents();
    g_mainWdg = new HMainWidget;

    splash->showMessage("Loading completed!", Qt::AlignCenter, Qt::white);
    progress->setValue(90);
    app.processEvents();

    qInfo("-----------------enter event loop----------------");
    pObj->m_mutex.unlock();
    splash->finish(g_mainWdg);
    app.exec();

    HEffectWidget::exitInstance();
    HExpreWidget::exitInstance();
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

    m_audioPlay = new HAudioPlay;
    m_playaudio_srvid = 1;
}

HDsContext::~HDsContext(){
    if (m_audioPlay){
        delete m_audioPlay;
        m_audioPlay = NULL;
    }
}

#include <QWaitCondition>
void HDsContext::start_gui_thread(){
    qInfo("start_gui_thread<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");

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

    // beacause QApplication not create in main thread so wait until event-loop start
    m_mutex.lock();
    m_mutex.unlock();
    sleep(1);
    qInfo("start_gui_thread>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}

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
#include <QtXml/QDomComment>
int HDsContext::parse_layout_xml(const char* xml_file){
    qDebug(xml_file);

    QDomDocument dom;
    QFile file(xml_file);
    QString err;
    if (!dom.setContent(&file, &err)){
        qWarning("parse_layout_xml failed:%d", err.toLocal8Bit().data());
        return -1;
    }

    QDomElement elem_root = dom.documentElement();
    QDomElement elem_layout = elem_root.firstChildElement("layout");
    if (elem_layout.isNull())
        return -2;
    QDomElement elem_head = elem_layout.firstChildElement("head");
    if (elem_head.isNull())
        return -3;

    QDomElement elem_param = elem_head.firstChildElement("param");
    while (!elem_param.isNull()) {
        QString n = elem_param.attribute("n");
        QString v = elem_param.attribute("v");
        if (n == "debug")
            m_tInit.debug = v.toInt();
        else if (n == "drawDebugInfo")
            m_tInit.drawDebugInfo = v.toInt();
        else if (n == "mouse")
            m_tInit.mouse = v.toInt();
        else if (n == "autolayout")
            m_tInit.autolayout = v.toInt();
        else if (n == "row")
            m_tInit.row = v.toInt();
        else if (n == "col")
            m_tInit.col = v.toInt();
        else if (n == "maxnum_layout")
            m_tInit.maxnum_layout = v.toInt();
        else if (n == "output")
            m_tInit.output = v.toInt();
        else if (n == "merge"){
            QRegExp re("(\\d+)-(\\d+)");
            if (re.indexIn(v) != -1){
                m_tInit.merge[0] = re.cap(1).toInt();
                m_tInit.merge[1] = re.cap(2).toInt();
                qDebug("merge: %d-%d", m_tInit.merge[0], m_tInit.merge[1]);
            }
        }
        else if (n == "audio"){
            m_tInit.audio = v.toInt();
        }else if(n == "fps"){
            m_tInit.fps = v.toInt();
        }else if (n == "display_mode"){
            m_tInit.display_mode = v.toInt();
        }else if (n == "scale_mode"){
            m_tInit.scale_mode = v.toInt();
        }else if (n == "drawtitle"){
            m_tInit.drawtitle = v.toInt();
        }else if (n == "drawfps"){
            m_tInit.drawfps = v.toInt();
        }else if (n == "drawnum"){
            m_tInit.drawnum = v.toInt();
        }else if (n == "drawaudio"){
            m_tInit.drawaudio = v.toInt();
        }else if(n == "drawinfo")
            m_tInit.drawinfo = v.toInt();
        else if (n == "taskinfo_format")
            m_tInit.taskinfo_format = v;
        else if (n == "title_format")
            m_tInit.title_format = v;
        else if (n == "drawoutline")
            m_tInit.drawoutline = v.toInt();
        else if(n == "infcolor")
            m_tInit.infcolor = v.toUInt(NULL, 16);
        else if(n == "titcolor")
            m_tInit.titcolor = v.toUInt(NULL, 16);
        else if (n == "audiostyle")
            m_tInit.audiostyle = v.toInt();
        else if(n == "audiocolor_bg")
            m_tInit.audiocolor_bg = v.toUInt(NULL, 16);
        else if(n == "audiocolor_fg_low")
            m_tInit.audiocolor_fg_low = v.toUInt(NULL, 16);
        else if(n == "audiocolor_fg_high")
            m_tInit.audiocolor_fg_high = v.toUInt(NULL, 16);
        else if(n == "audiocolor_fg_top")
            m_tInit.audiocolor_fg_top = v.toUInt(NULL, 16);
        else if (n == "spacing")
            m_tInit.spacing = v.toInt();
        else if (n == "titlebar_height")
            m_tInit.titlebar_height = v.toInt();
        else if (n == "toolbar_height")
            m_tInit.toolbar_height = v.toInt();
        else if (n == "output_titlebar_height")
            m_tInit.output_titlebar_height = v.toInt();
        else if (n == "output_toolbar_height")
            m_tInit.output_toolbar_height = v.toInt();
        else
            qWarning("Invalid key:%s", n.toLocal8Bit().data());

        elem_param = elem_param.nextSiblingElement("param");
    }

    if (!m_tInit.autolayout){
        QDomElement elem_body = elem_layout.firstChildElement("body");
        if (elem_body.isNull())
            return -4;
        QDomElement elem_item = elem_body.firstChildElement("item");
        m_tLayout.width = elem_body.attribute("width").toInt();
        m_tLayout.height = elem_body.attribute("height").toInt();
        int cnt_item = 0;
        int x,y,w,h;
        while (!elem_item.isNull()){
            QDomElement elem_param = elem_item.firstChildElement("param");
            while (!elem_param.isNull()){
                QString n = elem_param.attribute("n");
                QString v = elem_param.attribute("v");
                if(n == "w")
                    w = v.toInt();
                else if(n == "h")
                    h = v.toInt();
                else if(n == "x")
                    x = v.toInt();
                else if(n == "y")
                    y = v.toInt();

                elem_param = elem_param.nextSiblingElement("param");
            }

            m_tLayout.items[cnt_item].setRect(x, y, w, h);

            ++cnt_item;
            if(cnt_item >= MAXNUM_LAYOUT)
                break;

            elem_item = elem_item.nextSiblingElement("item");
        }

        m_tLayout.itemCnt = cnt_item;
    }

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
    DsCombInfo ci;
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
        int x,y,w=-1,h=-1;
        int u = 0;
        int bV = 1;
        int a = 0;
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

        if (u == 0){
            int pos = src.indexOf("lmic://");
            if (pos != -1){
                QRegExp re("(\\d+)");
                if (re.indexIn(src.mid(pos+strlen("lmic://"))) != -1){
                    u = re.cap(1).toInt();
                    qDebug("lmic://%d", u);
                }
            }
        }

        if (a && !bV)
            ci.micphone = u; // record micphone

        ci.items[ci.itemCnt].id = ci.itemCnt;
        ci.items[ci.itemCnt].rc.setRect(x,y,w,h);
        ci.items[ci.itemCnt].a = a;
        ci.items[ci.itemCnt].v = bV;
        ci.items[ci.itemCnt].srvid = u;
        strncpy(ci.items[ci.itemCnt].src, src.toLocal8Bit().constData(), MAXLEN_STR);

        if ((w > ci.width - 10) && (h > ci.height - 10)){
            ci.items[ci.itemCnt].bMainScreen = true;
            ci.comb_type = DsCombInfo::PIP;
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
        HCombItem* pScreen = getScreenItem(src);
        if (pScreen){
            pScreen->a = a;
        }

        DsSrvItem* pItem = getSrvItem(src);
        if (pItem){
            pItem->bVoice = a;
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
int HDsContext::parse_taskinfo_xml(int srvid, const char* xml){
    //qDebug(xml);

    DsSrvItem* pItem = g_dsCtx->getSrvItem(srvid);
    if (!pItem)
        return -100;

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
    int inputpkgs[2] = {-1};
    std::string inputchar;
    std::string ttid;
    const ook::xml_element * e;
    ook::xml_parser::enum_childen(item, NULL);
    while(1)
    {
        e = ook::xml_parser::enum_childen(item, "param");
        if(!e)
            break;
        const std::string & n = e->get_attribute("n");
        const std::string & v = e->get_attribute("v");
        if (n == "TTID")
            ttid = v;
        else if(n == "buffer"){
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
        }else if (n == "inputpkgs"){
            separator sept(v.c_str(), "/");
            if (sept[0])
                inputpkgs[0] = atoi(sept[0]);
            if (sept[1])
                inputpkgs[1] = atoi(sept[1]);
        }else if (n == "inputchar"){
            inputchar = v.c_str();
        }
    }

    if (srvid == OUTPUT_SRVID){
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
        char cont[128] = {0};
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

        pItem->taskinfo = s_cont;
    }else{
        QString str(m_tInit.taskinfo_format);
        str.replace("%title", pItem->title.c_str());
        str.replace("%avcodec", inputchar.c_str());
        QString rate = QString::asprintf("码率:%dkps", inputpkgs[0]);
        str.replace("%rate", rate);
        pItem->taskinfo = str.toLocal8Bit().data();
    }

    QMap<QString, QString>::iterator iter = m_mapTTID2Src.find(ttid.c_str());
    if (iter != m_mapTTID2Src.end()){
        pItem->src_addr = iter.value();
        //qDebug("srvid:%d, src_addr:%s", req_srvid, pItem->src_addr.toLocal8Bit().data());
    }

    return 0;
}

#include "hffmpeg.h"
int HDsContext::push_video(int srvid, const av_picture* pic){
    //qDebug("srvid=%d, framerate=%d, stamp=%d", srvid, pic->framerate, pic->stamp);

    if (action < 1)
        return -1;

    DsSrvItem* item = getSrvItem(srvid);
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
        if (pic->framerate > 0 && pic->framerate <= 60)
            item->framerate = pic->framerate;
        else
            item->framerate = 25;
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
        m_tComb.items[0].srvid = OUTPUT_SRVID;
        m_tComb.items[0].rc.setRect(0,0,w,h);

        qInfo("m_tComb w=%d h=%d", w, h);
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

void HDsContext::onWndSizeChanged(int srvid, QRect rc){
    DsSrvItem* pItem = getSrvItem(srvid);
    if (pItem){
        pItem->mutex.lock();
        pItem->wnd_w = rc.width();
        pItem->wnd_h = rc.height();

        double ratio = 1.0;
        if (pItem->isAdjustRatio(&ratio)){
            qDebug("ratio=%lf", ratio);
            pItem->adjustRatio(ratio);
        }else{
            pItem->show_w = pItem->wnd_w;
            pItem->show_h = pItem->wnd_h;
        }
        pItem->mutex.unlock();

        //g_mainWdg->getGLWdgBysrvid(srvid)->setVertices(ratio);
        QRect vertices(rc.x() + (pItem->wnd_w - pItem->show_w) / 2, rc.y() + (pItem->wnd_h - pItem->show_h) / 2, pItem->show_w, pItem->show_h);
        g_mainWdg->getGLWdgBysrvid(srvid)->setVertices(vertices);
        pItem->bNeedReallocTexture = true;
    }
}

int HDsContext::pop_video(int srvid){
    if (action < 1)
        return -1;

    DsSrvItem* item = getSrvItem(srvid);
    if (!item)
        return -2;

    if (item->bPause)
        return -3;

    if (!item->video_buffer)
        return -4;

    if (!item->canShow()){
        emit requestShow(srvid);
        return -5;
    }

    if (!item->tex_yuv.data || item->bNeedReallocTexture){
        int w = item->pic_w;
        int h = item->pic_h;
        if (m_tInit.scale_mode == ENABLE_SCALE && item->isAdjustScale(&w,&h)){
            qDebug("scale=%d*%d", w, h);
            item->adjustScale(w,h);
        }
        if (!item->tex_yuv.alloc(w,h))
            return -6;

        item->bNeedReallocTexture = false;

        qDebug("wnd=%d*%d, pic=%d*%d, show=%d*%d, tex=%d*%d",
               item->wnd_w, item->wnd_h,
               item->pic_w, item->pic_h,
               item->show_w, item->show_h,
               item->tex_yuv.width, item->tex_yuv.height);
    }

    int retcode = -7;
    item->mutex.lock();
    char* ptr = item->video_buffer->read();
    if (!ptr){
        //qDebug("[srvid=%d]read to fast", srvid);
        if (++item->pop_video_failed_cnt > 3*m_tInit.fps)
            stop(srvid);
    }else{
        item->pop_video_failed_cnt = 0;
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

    DsSrvItem* item = getSrvItem(srvid);
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

HCombItem* HDsContext::getScreenItem(int srvid){
    HCombItem* item = NULL;
    for (int i = 0; i < m_tComb.itemCnt; ++i){
        if (INNER_SRVID(m_tComb.items[i].srvid) == srvid){
            item = & m_tComb.items[i];
            break;
        }
    }

    return item;
}

HCombItem* HDsContext::getScreenItem(QString src){
    HCombItem* item = NULL;
    for (int i = 0; i < m_tComb.itemCnt; ++i){
        if (m_tComb.items[i].src == src){
            item = & m_tComb.items[i];
            break;
        }
    }

    return item;
}

void HDsContext::pause(int srvid, bool bPause){
    DsSrvItem* item = getSrvItem(srvid);
    if (item && item->ifcb){
        qInfo("srvid=%d ifservice_callback::e_service_cb_pause", srvid);
#if LAYOUT_TYPE_ONLY_OUTPUT
        item->ifcb->onservice_callback(ifservice_callback::e_service_cb_pause, libchar(), 0, 0, bPause, NULL);
#else
        if (isOutputSrvid(srvid)){
            item->ifcb->onservice_callback(ifservice_callback::e_service_cb_chr, libchar(), OOK_FOURCC('P', 'A', 'U', 'S'), 0, 0, NULL);
        }else{
            item->ifcb->onservice_callback(ifservice_callback::e_service_cb_pause, libchar(), OOK_FOURCC('P', 'A', 'U', 'S'), 0, bPause, NULL);
            item->bPause = bPause;
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
