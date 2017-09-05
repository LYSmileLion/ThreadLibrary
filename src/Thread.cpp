#include <iostream>
#include <Thread.hpp>
namespace HPCs {

namespace CurrentThread {
__thread int t_cached_tid = 0;
__thread char t_tid_string[32];
__thread int t_tid_string_length = 6;
__thread const char *t_thread_name = "unknow";

void CacheTid() {
    if (t_cached_tid == 0) {
        t_cached_tid = static_cast<pid_t>(::syscall(SYS_gettid));
        t_tid_string_length = 
            snprintf(t_tid_string, sizeof(t_tid_string), "%5d ", t_cached_tid) 
    }
}

bool IsMainThread() {
    return (GetTid() == getpid());
}

void SleepUsec(int64_t usec) {
    struct timespec ts = {0, 0};
    ts.tv_sec = static_cast<time_t> (usec / (1000 * 1000));
    ts.tv_nsec = static_cast<long> (usec % (1000 * 1000));
    nanosleep(&ts, NULL);
}
}

class ThreadInfo {
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    std::weak_ptr<pid_t> wptid_;
    ThreadInfo(
        const ThreadFunc &func,
        const std::string name,
        const std::shared_ptr<pid_t> &ptr_tid):
        func_(func),
        name_(name),
        wptid_(ptr_tid) {}
    
    void runThread() {
        pid_t tid = tid();

        std::shared_ptr<pid_t> sptid = wptid_.lock();
        if (sptid) {
            *sptid = tid;
            sptid.reset();//better to reset this shared ptr
        }
        t_thread_name = name_.empty() ? "HPCs Thread" : name_.c_str();
        prctl(PR_SET_NAME, t_thread_name);//this API now support Thread,it
        //will set thread name for now
        func_();
        t_thread_name = "Thread : finish the task";
    }


};

void *startThread(void *obj) {
	ThreadInfo *threadinfo = static_cast<ThreadInfo*>(obj);
	threadinfo->runThread();
	delete threadinfo;
	return NULL;
}

Thread::Thread(const ThreadFunc &func,const std::string &name):
    started_(false),joined_(false),pthreadId_(0),func_(func),name_(name) {
    setDefaultName();
}

Thread::~Thread() {
    if (started_ && !joined_) {
        pthread_detach(pthreadId_);//detach status
    }
}

void Thread::SetDefaultName() {
    if (name_.empty()) {
        name_ = std::string("HPCs Thread");
    }
}

void Thread::Start() {
    if (started_) {
        std::cout << "Thread already runnnig " << std::endl;
        return ;
    }
    ThreadInfo *threadinfo = new ThreadInfo(func_, name_, tid_);
	if (pthread_create(&pthreadId_, NULL, &startThread, threadinfo)) {
		started_ = false;
		delete threadinfo;
	}
}
int Thread::join() {
	assert(started_);
	assert(!joined_);
	joined_ = true;
	return pthread_join();
}

