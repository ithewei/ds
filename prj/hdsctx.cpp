#include "hdsctx.h"
#include "hrcloader.h"
#include "hmainwidget.h"

HDsContext* g_dsCtx = NULL;
int g_fontsize = 24;

#include <QDateTime>
void myLogHandler(QtMsgType type, const QMessageLogContext & ctx, const QString & msg){
    // QtDebugMsg = 0, so
    // debug=0, no all
    // debug=1, print qInfo
    // debug=4(QtInfoMsg), no qDebug
    // debug>4, print all
    if (g_dsCtx){
        int threshold = QtInfoMsg - g_dsCtx->m_tInit.debug;
        if (type <= threshold)
            return;
    }

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
    QString strLogFilePath = "/var/log/ds.log";

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
#include <QScreen>
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

    g_fontsize = QApplication::primaryScreen()->size().height()/25;
    QFont font = QApplication::font();
    font.setPixelSize(g_fontsize);
    QApplication::setFont(font);

    QSplashScreen* splash = new QSplashScreen;
    splash->setWindowFlags(Qt::Popup | Qt::WindowStaysOnTopHint);
    splash->setGeometry(QApplication::desktop()->screenGeometry(0));
    splash->setStyleSheet("background-color: black");

    QProgressBar* progress = new QProgressBar(splash);
    int w = g_fontsize * 10;
    int h = g_fontsize * 1.5;
    progress->setGeometry(splash->width()/2 - w/2, splash->height()/2 + 50, w, h);
    progress->setStyleSheet("background-color: white; border:2px solid gray; border-radius: 5px");
    progress->setRange(0,100);
    progress->setValue(0);
    splash->showFullScreen();
    app.processEvents();

    //splash->showMessage("Loading settings...", Qt::AlignCenter, Qt::white);
    progress->setValue(10);
    app.processEvents();
    QString str = g_dsCtx->cur_path.c_str();
//#if LAYOUT_TYPE_OUTPUT_AND_INPUT
//    QFile file("/var/www/appname.txt");
//    if (file.open(QIODevice::ReadOnly)){
//        QString appname = file.readAll();
//        if (appname == "transcoder_sohu")
//            str += "ds_sohu.conf";
//        else
//            str += "ds.conf";
//    }else{
//        str += "ds.conf";
//    }
//#else
//    str += "ds_out.conf";
//#endif
    str += "ds.conf";
    dsconf->load(str);

    pObj->m_mutex.unlock(); // unlock when conf load finished

    //splash->showMessage("Loading icon...", Qt::AlignCenter, Qt::white);
    progress->setValue(20);
    app.processEvents();
    rcloader->loadIcon();

    //splash->showMessage("Creating widgets...", Qt::AlignCenter, Qt::white);
    progress->setValue(30);
    app.processEvents();
    HExpreWidget::instance();
    HEffectWidget::instance();

    //splash->showMessage("Creating main UI...", Qt::AlignCenter, Qt::white);
    progress->setValue(50);
    app.processEvents();
    g_mainWdg = new HMainWidget;

    //splash->showMessage("Loading completed!", Qt::AlignCenter, Qt::white);
    progress->setValue(90);
    app.processEvents();

    g_mainWdg->onActionChanged(1);
    splash->finish(g_mainWdg);

    qInfo("-----------------enter event loop----------------");
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
    ext_screen = false;

    playaudio_srvid = OUTPUT_SRVID;
    pre_micphone_srvid = 0;
}

HDsContext::~HDsContext(){

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

    // beacause QApplication not create in main thread so wait until splash show
    m_mutex.lock();
    m_mutex.unlock();
    qInfo("start_gui_thread>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}

#include <QtXml/QDomComment>
int HDsContext::parse_layout_xml(const char* xml_file){
    qInfo(xml_file);
    layout_file = xml_file;

    QDomDocument dom;
    QFile file(xml_file);
    QString err;
    if (!dom.setContent(&file, &err)){
        qWarning("parse_layout_xml failed:%d", err.toLocal8Bit().data());
        return -1;
    }

    QDomElement elem_root = dom.documentElement();

    QDomElement elem_im = elem_root.firstChildElement("important");
    if (elem_im.isNull())
        return -2;
    m_tInit.debug = elem_im.attribute("debug", "0").toInt();
    m_tInit.save_span = elem_im.attribute("save_span", "0").toInt();
    m_tInit.audio = elem_im.attribute("audio", "1").toInt();
    m_tInit.display_mode = elem_im.attribute("display_mode", "1").toInt();
    m_tInit.scale_mode = elem_im.attribute("scale_mode", "2").toInt();
    m_tInit.fps = elem_im.attribute("fps", "25").toInt();
    m_tInit.audio_bufnum = elem_im.attribute("audio_bufnum", "10").toInt();
    m_tInit.video_bufnum = elem_im.attribute("video_bufnum", "10").toInt();

    QDomElement elem_ui = elem_root.firstChildElement("UI");
    if (elem_ui.isNull())
        return -3;
    QDomElement elem_layout = elem_ui.firstChildElement("layout");
    if (elem_layout.isNull())
        return -4;
    m_tInit.autolayout = elem_layout.attribute("auto", "1").toInt();
    if (m_tInit.autolayout){
        m_tInit.maxnum_layout = elem_layout.attribute("maxnum", "9").toInt();
        m_tInit.row = elem_layout.attribute("row", "3").toInt();
        m_tInit.col = elem_layout.attribute("col", "3").toInt();
        m_tInit.output = elem_layout.attribute("output", "0").toInt();
        if (!elem_layout.attribute("merge").isEmpty()){
            QRegExp re("(\\d+)-(\\d+)");
            if (re.indexIn(elem_layout.attribute("merge")) != -1){
                m_tInit.merge[0] = re.cap(1).toInt();
                m_tInit.merge[1] = re.cap(2).toInt();
                qDebug("merge: %d-%d", m_tInit.merge[0], m_tInit.merge[1]);
            }
        }
    }else{
        QDomElement elem_mainwindow = elem_layout.firstChildElement("mainwindow");
        if (elem_mainwindow.isNull())
            return -41;
        m_tLayout.width = elem_mainwindow.attribute("width", "0").toInt();
        m_tLayout.height = elem_mainwindow.attribute("height", "0").toInt();
        QDomElement elem_item = elem_mainwindow.firstChildElement("item");
        int cnt_item = 0;
        int x,y,w,h;
        while (!elem_item.isNull()){
            QString str = elem_item.attribute("type", "input");
            if (str == "output"){
                m_tInit.output = cnt_item+1; // wndid of output
            }
            QString pos = elem_item.attribute("pos", "0,0,0,0");
            QStringList pos_split = pos.split(',');
            x = pos_split[0].toInt();
            y = pos_split[1].toInt();
            w = pos_split[2].toInt();
            h = pos_split[3].toInt();
            m_tLayout.items[cnt_item].setRect(x,y,w,h);

            ++cnt_item;
            if(cnt_item >= MAXNUM_WND)
                break;

            elem_item = elem_item.nextSiblingElement("item");
        }
        m_tLayout.itemCnt = cnt_item;
    }

    QDomElement elem_settings = elem_ui.firstChildElement("settings");
    if (elem_settings.isNull())
        return -5;
    m_tInit.mouse = elem_settings.attribute("mouse", "0").toInt();
    m_tInit.fontsize = elem_settings.attribute("fontsize", "24").toInt();

    m_tInit.drawtitle = elem_settings.attribute("drawtitle", "0").toInt();
    m_tInit.drawfps = elem_settings.attribute("drawfps", "0").toInt();
    m_tInit.drawnum = elem_settings.attribute("drawnum", "0").toInt();
    m_tInit.drawaudio = elem_settings.attribute("drawaudio", "0").toInt();
    m_tInit.drawinfo = elem_settings.attribute("drawinfo", "0").toInt();
    m_tInit.drawoutline = elem_settings.attribute("drawoutline", "0").toInt();
    m_tInit.drawDebugInfo = elem_settings.attribute("drawdebuginfo", "0").toInt();

    m_tInit.spacing = elem_settings.attribute("spacing", "0").toInt();
    m_tInit.titlebar_height = elem_settings.attribute("titlebar_height", "0").toInt();
    m_tInit.toolbar_height = elem_settings.attribute("toolbar_height", "0").toInt();
    m_tInit.output_titlebar_height = elem_settings.attribute("output_titlebar_height", "0").toInt();
    m_tInit.output_toolbar_height = elem_settings.attribute("output_toolbar_height", "0").toInt();

    m_tInit.infcolor = elem_settings.attribute("infcolor", "0x00FF00FF").toUInt(NULL, 16);
    m_tInit.titcolor = elem_settings.attribute("titcolor", "0xFF5A1EFF").toUInt(NULL, 16);
    m_tInit.audiostyle = elem_settings.attribute("audiostyle", "1").toInt();
    m_tInit.audiocolor_bg = elem_settings.attribute("audiocolor_bg", "0x0").toUInt(NULL, 16);
    m_tInit.audiocolor_fg_low = elem_settings.attribute("audiocolor_fg_low", "0x0").toUInt(NULL, 16);
    m_tInit.audiocolor_fg_high = elem_settings.attribute("audiocolor_fg_high", "0x0").toUInt(NULL, 16);
    m_tInit.audiocolor_fg_top = elem_settings.attribute("audiocolor_fg_top", "0x0").toUInt(NULL, 16);

    m_tInit.title_format = elem_settings.attribute("title_format", "%title");
    m_tInit.taskinfo_format = elem_settings.attribute("taskinfo_format", "%rate");

    QDomElement elem_ptz = elem_root.firstChildElement("ptz");
    if (!elem_ptz.isNull()){
        QString strNum = elem_ptz.attribute("numerator", "1,1,1,1,1,1");
        QString strDen = elem_ptz.attribute("denominator", "20,20,20,20,10,10");
        QStringList strListNum = strNum.split(',');
        QStringList strDenNum = strDen.split(',');
        for (int i = 0; i < 6; ++i){
            m_tInit.ptz_num[i] = strListNum[i].toInt();
            m_tInit.ptz_den[i] = strDenNum[i].toInt();
        }
    }

    QDomElement elem_overlay = elem_root.firstChildElement("overlay");
    if (!elem_overlay.isNull()){
        m_tInit.maxnum_overlay = elem_overlay.attribute("maxnum", "3").toInt();
        m_tInit.expre_policy = elem_overlay.attribute("expre_policy", "0").toInt();
    }

    return 0;
}

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
        if(ci.itemCnt >= MAXNUM_COMB_ITEM)
            break;
    }

    for (int i = 0; i < MAXNUM_COMB_ITEM; ++i){
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
        HCombItem* pScreen = getCombItem(src);
        if (pScreen){
            pScreen->a = a;
        }

        DsSrvItem* pItem = getSrvItem(src);
        if (pItem){
            pItem->bVoice = a;
        }
    }

    emit voiceChanged();

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
    qDebug(xml);

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

                const ook::xml_element * e_speed = ook::xml_parser::get_element(e_intf, "<speedl>");
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

        pItem->taskinfo = s_cont.c_str();
    }else{
        QString str(m_tInit.taskinfo_format);
        str.replace("%title", pItem->title.c_str());
        str.replace("%avcodec", inputchar.c_str());
        QString rate = QString::asprintf("码率:%dkps", inputpkgs[0]);
        str.replace("%rate", rate);
        pItem->taskinfo = str;
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
    qDebug("srvid=%d, framerate=%d, stamp=%d", srvid, pic->framerate, pic->stamp);

    if (!isDeal(srvid))
        return -1;

    DsSrvItem* item = getSrvItem(srvid);
    if (!item)
        return -2;

    int w = pic->width;
    int h = pic->height;
    if (w != (w >> 3 << 3)){
        qWarning("width:%d % 8 != 0", w);
        return -8;
    }

    if (pic->fourcc != OOK_FOURCC('I', '4', '2', '0') && pic->fourcc != OOK_FOURCC('Y', 'V', '1', '2')){
        char c[5] = {0};
        memcpy(c, &pic->fourcc, 4);
        qWarning("Not provide video format:%s", c);
        return -3;
    }

    item->video_mutex.lock();
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
        int bufnum = m_tInit.video_bufnum > 0 ? m_tInit.video_bufnum : DEFAULT_VIDEO_BUFNUM;
        if (item->src_type == SRC_TYPE_LMIC){
            bufnum = DEFAULT_LMIC_VIDEO_BUFNUM;
        }
        item->video_buffer = new HRingBuffer(w*h*3/2, bufnum);
        qInfo("video_bufnum=%d", bufnum);
        bFirst = true;

#if LAYOUT_TYPE_ONLY_OUTPUT
        if (srvid == OUTPUT_SRVID){
            m_tComb.width = w;
            m_tComb.height = h;
            m_tComb.itemCnt = 1;
            m_tComb.items[0].srvid = OUTPUT_SRVID;
            m_tComb.items[0].rc.setRect(0,0,w,h);

            qInfo("m_tComb w=%d h=%d", w, h);
        }
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

    qDebug("srvid=%d video_cache=%d", srvid, item->video_buffer->readable());
    item->video_mutex.unlock();

    emit videoPushed(srvid, bFirst);
}

void HDsContext::onWndSizeChanged(int srvid, QRect rc){
    DsSrvItem* pItem = getSrvItem(srvid);
    if (pItem){
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

        //g_mainWdg->getGLWdgBysrvid(srvid)->setVertices(ratio);
        QRect vertices(rc.x() + (pItem->wnd_w - pItem->show_w) / 2, rc.y() + (pItem->wnd_h - pItem->show_h) / 2, pItem->show_w, pItem->show_h);
        g_mainWdg->getGLWdgBysrvid(srvid)->setVertices(vertices);
        pItem->bNeedReallocTexture = true;
    }
}

int HDsContext::pop_video(int srvid){
    if (!isDeal(srvid))
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

        qInfo("wnd=%d*%d, pic=%d*%d, show=%d*%d, tex=%d*%d",
               item->wnd_w, item->wnd_h,
               item->pic_w, item->pic_h,
               item->show_w, item->show_h,
               item->tex_yuv.width, item->tex_yuv.height);

        return -7;
    }

    int retcode = -100;
    item->video_mutex.lock();
    char* ptr = item->video_buffer->read();
    if (!ptr){
        qWarning("[srvid=%d]read to fast", srvid);
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
    item->video_mutex.unlock();

    return retcode;
}

int HDsContext::push_audio(int srvid, const av_pcmbuff* pcm){
    qDebug("srvid=%d, samplerate=%d, stamp=%d, channels=%d, len=%d", srvid, pcm->samplerate, pcm->stamp, pcm->channels, pcm->pcmlen);

    if (m_tInit.audio < 1)
        return -1;

    if (!(action > 0 || ext_screen))
        return -10;

    DsSrvItem* item = getSrvItem(srvid);
    if (!item)
        return -2;

    item->audio_mutex.lock();
    if (item->pcmlen != pcm->pcmlen){
        if (item->audio_buffer){
            delete item->audio_buffer;
            item->audio_buffer = NULL;
        }
        item->audio_buffer = new HRingBuffer(pcm->pcmlen, m_tInit.audio_bufnum > 0 ? m_tInit.audio_bufnum : DEFAULT_AUDIO_BUFNUM);
        item->a_channels = pcm->channels;
        item->pcmlen = pcm->pcmlen;
        item->samplerate = pcm->samplerate;
    }

    if (item->audio_buffer){
        char* ptr = item->audio_buffer->write();
        if (ptr){
            memcpy(ptr, pcm->pcmbuf, pcm->pcmlen);
        }

        int cache = item->audio_buffer->readable();
        qDebug("srvid=%d audio_cache=%d", srvid, cache);
        if (cache <= 1)
            item->audio_empty++;
        else if (cache >= 6)
            item->audio_empty = 0;

        if (cache >= item->audio_buffer->size())
            item->audio_full++;
        else
            item->audio_full = 0;
    }
    item->audio_mutex.unlock();

    if (srvid == playaudio_srvid || item->src_type == SRC_TYPE_LMIC){
        if (!item->audio_player){
            item->audio_player = new HAudioPlay;
            item->audio_player->srvid = srvid;
        }

        if (item->audio_player){
            if (item->audio_player->channels != pcm->channels ||
                item->audio_player->pcmlen != pcm->pcmlen ||
                item->audio_player->samplerate != pcm->samplerate){
                item->audio_player->stopPlay();
                item->audio_player->channels = pcm->channels;
                item->audio_player->pcmlen = pcm->pcmlen;
                item->audio_player->samplerate = pcm->samplerate;
                item->audio_player->startPlay(ext_screen ? HDMI1 : Pa_GetDefaultOutputDevice());
            }

            if (item->audio_empty >= AUDIO_EXCEPTION_CNT){
                if (!item->audio_player->pause){
                    qInfo("audio_empty: pauseplay some time");
                    item->audio_player->pausePlay(true);
                }
            }else{
                if (item->audio_player->pause){
                    qInfo("resumeplay");
                    item->audio_player->pausePlay(false);
                }
            }

            if (item->audio_full >= AUDIO_EXCEPTION_CNT){
                if (item->audio_buffer){
                    item->audio_mutex.lock();
                    int audio_bufnum = item->audio_buffer->size();
                    if (audio_bufnum*2 <= AUDIO_BUFFER_MAXNUM){
                        qInfo("audio_full: extend audio_buffer=%d", audio_bufnum*2);
                        HRingBuffer *new_buf = new HRingBuffer(item->pcmlen, audio_bufnum*2);
                        for (int i = 0; i < audio_bufnum; ++i){
                            char *r = item->audio_buffer->read();
                            char *w = item->audio_buffer->write();
                            if (r && w){
                                memcpy(w, r, item->pcmlen);
                            }
                        }
                        delete item->audio_buffer;
                        item->audio_buffer = new_buf;
                    }else{
                        qInfo("audio_full: discard some frames");
                        for (int i = 0; i < audio_bufnum/2; ++i){
                            item->audio_buffer->read();
                        }
                    }
                    item->audio_mutex.unlock();
                }
            }
        }
    }

    if (item->canShow() && item->bUpdateAverage){
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

HCombItem* HDsContext::getCombItem(int srvid){
    HCombItem* item = NULL;
    for (int i = 0; i < m_tComb.itemCnt; ++i){
        if (INNER_SRVID(m_tComb.items[i].srvid) == srvid){
            item = & m_tComb.items[i];
            break;
        }
    }

    return item;
}

HCombItem* HDsContext::getCombItem(QString src){
    HCombItem* item = NULL;
    for (int i = 0; i < m_tComb.itemCnt; ++i){
        if (m_tComb.items[i].src == src){
            item = & m_tComb.items[i];
            break;
        }
    }

    return item;
}

void HDsContext::ptzControl(int srvid, struct task_PTZ_ctrl_s *param){
    DsSrvItem* item = getSrvItem(srvid);
    if (item && item->ifcb){
        qInfo("ifservice_callback::e_service_cb_ctrl");
        item->ifcb->onservice_callback(ifservice_callback::e_service_cb_ctrl, libchar(), OOKFOURCC_PTZ, OOKFOURCC_ONVF, 0, (void *)param);
    }
}

void HDsContext::pause(int srvid, bool bPause){
    qInfo("srvid=%d pause=%d", srvid, bPause);
    DsSrvItem* item = getSrvItem(srvid);
    if (item && item->ifcb){
        if (isOutputSrvid(srvid)){
#if LAYOUT_TYPE_ONLY_OUTPUT
            qInfo("ifservice_callback::e_service_cb_pause");
            item->ifcb->onservice_callback(ifservice_callback::e_service_cb_pause, libchar(), 0, 0, bPause, NULL);
#else
            qInfo("ifservice_callback::e_service_cb_chr");
            item->ifcb->onservice_callback(ifservice_callback::e_service_cb_chr, libchar(), OOK_FOURCC('P', 'A', 'U', 'S'), 0, 0, NULL);
#endif
            item->spacer_activate = bPause;
        }else{
            qInfo("ifservice_callback::e_service_cb_pause");
            item->ifcb->onservice_callback(ifservice_callback::e_service_cb_pause, libchar(), OOK_FOURCC('P', 'A', 'U', 'S'), 0, bPause, NULL);
            item->bPause = bPause;
        }
    }
}

void HDsContext::setPlayProgress(int srvid, int progress){
    DsSrvItem* item = getSrvItem(srvid);
    if (item && item->ifcb){
        qInfo("srvid=%d progress=%d ifservice_callback::e_service_cb_playratio", srvid, progress);
        item->ifcb->onservice_callback(ifservice_callback::e_service_cb_playratio, libchar(), 0, 0, progress, NULL);
    }
}

void HDsContext::setPlayaudioSrvid(int id){
    if (playaudio_srvid != id){
        DsSrvItem* item = getSrvItem(playaudio_srvid);
        if (item && item->audio_player){
            item->audio_player->stopPlay();
        }
        playaudio_srvid = id;
    }
}
