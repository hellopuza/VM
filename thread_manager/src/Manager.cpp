#include "../include/Manager.h"

/**
 * @brief Оператор сравнения для использования в std::find()
 * 
 * @param lhs 
 * @param rhs 
 * @return true 
 * @return false 
 */
bool operator ==(const NativeThread lhs, const NativeThread rhs){

    return static_cast<std::thread::id>(lhs) == static_cast<std::thread::id>(rhs);
}

/**
 * @brief Ожидание окончания нативного треда.
 *        В это время нельзя вызвать GC и create_native_thread().
 * 
 * @param thread 
 */
ThreadManager::Errors ThreadManager::join_native_thread(NativeThread thread){

    std::lock_guard native_lock(native_threads);

    auto&& thread_it = std::find(threads.begin(), threads.end(), thread);

    if (thread_it == threads.end())
        return ThreadManager::Errors::Invalid_native_thread;

    thread_it->join();

    auto&& last_elem = std::prev(threads.end());
    std::swap(*thread_it, *last_elem);
    threads.pop_back();
    num_of_threads--;
    num_of_active_threads--;

    return ThreadManager::Errors::OK;
} 

/**
 * @brief Проверка необходимости остановки.
 * 
 */
void ThreadManager::save_point(){
    
    if (stop_world){
        
        num_of_active_threads--;

        std::unique_lock cond_var_guard(cond_var);
        resume_time.wait(cond_var_guard, [&]{ return !stop_world; });

        num_of_active_threads++;
    }    
}

/**
 * @brief Остановка всех потоков. 
 *        Возвращает ошибку, если производятся действия с native threads (join/create).
 *        Использование native_threads mutex обусловлено тем, что при введении дополнительной
 *        переменной для учета действий join/create native threads приведет к дедлоку из-за 
 *        невозможности одновременного изменения ее и mutex.
 */
ThreadManager::Errors ThreadManager::ZA_WARUDO(){

    std::thread::id cur_id = std::this_thread::get_id();

    if (cur_id != GC.get_id())
        return ThreadManager::Errors::Permision_denied;

    stop_world = true;

    while (num_of_active_threads != 0){
        
        if (!native_threads.try_lock()){

            resume_time_flow();
            return ThreadManager::Errors::Native_block;
        }

        native_threads.unlock();
    }

    return ThreadManager::Errors::OK;
}

/**
 * @brief Посылание сигналов, пока native threads не дойдут до resume_time 
 *        и не получат там сигнал продолжения.
 * 
 */
void ThreadManager::wait_activation(){

    while (num_of_active_threads != num_of_threads){

        //micro_sleep();
        resume_time.notify_all();
    }
}

/**
 * @brief Активация потоков.
 *        GC ожидант активации всех потоков.
 */
void ThreadManager::resume_time_flow(){

    stop_world = false;
    resume_time.notify_all();
    wait_activation();
}

/**
 * @brief Ожидание работы GC
 * 
 * @return Errors 
 */
ThreadManager::Errors ThreadManager::join_GC(){

    std::lock_guard GC_lock(GC_mutex);

    if (!GC_exists)
        return ThreadManager::Errors::Invalid_GC;

    GC_exists = false;
    GC.join();

    return ThreadManager::Errors::OK;
}