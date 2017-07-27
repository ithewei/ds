#include "ds_global.h"
#include "hguithread.h"
#include "hmainwidget.h"
#include <QApplication>

#ifdef WIN32
void thread_gui(void*) {
#else
void* thread_gui(void*){
#endif
    TRACE(3, "[hw]--------------thread_gui-start----------------------------------")
    int argc = 0;
    QApplication app(argc, NULL);

    g_dsCtx->m_mainwdg = new HMainWidget;
    g_dsCtx->m_mainwdg->show();

    app.exec();
    TRACE(3, "[hw]--------------thread_gui-end----------------------------------")

#ifdef linux
    pthread_exit(NULL);
#endif
}

//HGuiThread::HGuiThread()
//{

//}

//HGuiThread::~HGuiThread(){

//}

//void HGuiThread::run(){

//}
