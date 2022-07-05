#include "../include/Manager.h"

NativeThread ThreadManager::create_native_thread(){

    std::lock_guard native_lock(native_threads);

    threads.emplace_back();
    std::thread::id tmp_id = threads.back().get_id();

    return NativeThread(tmp_id);
}

void ThreadManager::join_native_thread(NativeThread thread){

    auto&& thread_it =  std::find(threads.begin(), threads.end(), thread);
    thread_it->join();
} 

void ThreadManager::save_point(){
    
    if (stop_world){ //вроде на атомарность пофиг
        
        deactivate_thread();

        std::unique_lock cond_var_guard(cond_var);
        resume_time.wait(cond_var_guard);           //надо вставить лямбду для проверки!!!

        activate_thread(); //возможна проблема с двумя близкими вызовами остановки времени тк треды могут не успеть выйти 
    }    
}

/*
Общий принцип:

    при остановке времени меняется состояние stop_world
    треды, которые проходят поинты, проверяют это состояние и при необходимости останавливаются на resume_time
    до этого они атомарно(через мьютекс) уменьшают количество активных тредов. Если количество активных тредов равно 0,
    то ОДИН тред посылает сообщение на everything_stoped (тут кстати вилы тк тред может не дойти до cond_var_guard а ГК уже пошлет сигнал)

    После завершения работы GC посылается сигнал на resume_time и все треды восстанавливают количество активных тредов

*/