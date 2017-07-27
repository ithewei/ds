#ifndef HGUITHREAD_H
#define HGUITHREAD_H

#ifdef WIN32
void thread_gui(void*);
#else
void* thread_gui(void*);
#endif

//class HGuiThread : public QThread
//{
//public:
//    HGuiThread();
//    ~HGuiThread();

//protected:
//    virtual void run();
//};

#endif // HGUITHREAD_H
