#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <vector>
#include <atomic>

class GCThread{

    std::thread::id thread_id;
};

class NativeThread{

    std::thread::id thread_id;

public:

    NativeThread(std::thread::id id): thread_id{id}{}

    operator std::thread::id(){ return thread_id; }
};


enum class Errors{

    OK,
    Native_block,
    Permision_denied,
    Invalid_native_thread
};


class ThreadManager{

    std::vector<std::thread> threads;
    std::atomic_int num_of_active_threads = 0;
    std::atomic_int num_of_threads = 0;

    std::atomic_bool stop_world;
    std::condition_variable resume_time;

    std::mutex native_threads;
    std::mutex cond_var;

    std::thread GC;

    void wait_activation();

public: 
    
    Errors join_native_thread(NativeThread thread);

    void save_point(); 
    Errors ZA_WARUDO();                        
    void resume_time_flow();

    template<typename Function, typename ... Args>
    GCThread create_GC(Function&& func, Args&&... args);

    template<typename Function, typename ... Args>
    NativeThread create_native_thread(Function&& func, Args&&... args);
};