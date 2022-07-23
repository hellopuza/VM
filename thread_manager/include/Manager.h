#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <vector>
#include <atomic>

class GCThread{

    std::thread::id thread_id;

public: 

    GCThread(std::thread::id id): thread_id{id}{}

    operator std::thread::id(){ return thread_id; }
};

class NativeThread{

    std::thread::id thread_id;

public:

    NativeThread(const std::thread& thread): thread_id{thread.get_id()}{}
    NativeThread(std::thread::id id): thread_id{id}{}

    operator std::thread::id() const{ return thread_id; }
};


class ThreadManager{

    std::vector<std::thread> threads;
    std::atomic_int num_of_active_threads = 0;
    std::atomic_int num_of_threads = 0;

    std::atomic_bool stop_world;
    std::condition_variable resume_time;

    std::mutex GC_mutex;
    std::mutex native_threads;
    std::mutex cond_var;

    std::thread GC;
    bool GC_exists = false;

    void wait_activation();

public: 

    enum class Errors{

        OK,
        Native_block,
        Permision_denied,
        Invalid_native_thread,
        Invalid_GC
    };

    void save_point(); 
    Errors ZA_WARUDO();                        
    void resume_time_flow();

    template<typename Function, typename ... Args>
    GCThread create_GC(Function&& func, Args&&... args);
    Errors join_GC();

    template<typename Function, typename ... Args>
    NativeThread create_native_thread(Function&& func, Args&&... args);
    Errors join_native_thread(NativeThread thread);
};

using SharedThreadManager = std::shared_ptr<ThreadManager>;
//-------------------

/**
 * @brief Создание GC. Если GC уже существует возвращает его.
 * 
 * @tparam Function 
 * @tparam Args 
 * @param func 
 * @param args 
 * @return GCThread 
 */
template<typename Function, typename ... Args>
GCThread ThreadManager::create_GC(Function&& func, Args&&... args){

    std::lock_guard GC_lock(GC_mutex);

    if (GC_exists) 
        return GCThread{GC.get_id()};

    GC_exists = true;
    std::thread tmp(func, std::forward<Args>(args) ... );
    std::swap(tmp, GC);

    return GCThread{GC.get_id()};
}

/**
 * @brief Создает native thread с указанными параметрами.
 * 
 * @tparam Function 
 * @tparam Args 
 * @param func 
 * @param args 
 * @return NativeThread 
 */
template<typename Function, typename ... Args>
NativeThread ThreadManager::create_native_thread(Function&& func, Args&&... args){

    std::lock_guard native_lock(native_threads);

    threads.emplace_back(func, std::forward<Args>(args) ... );
    std::thread::id tmp_id = threads.back().get_id();
    num_of_threads++;
    num_of_active_threads++;

    return NativeThread(tmp_id);
}