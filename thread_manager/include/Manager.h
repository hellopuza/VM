#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <vector>

#define MAX_THREAD_NUM 30

class GCThread{

    std::thread::id thread_id;
};

class NativeThread{

    std::thread::id thread_id;

public:

    NativeThread(std::thread::id id): thread_id{id}{}

    operator std::thread::id(){ return thread_id; }
};

class ThreadManager{

    /*семафор с счетчиком MAX_THREAD_NUM
        При добавлении нового треда. блокируется доступ к этому счетчику и тред менеждер добавляет к счетчику 1
        Таким образом счетчик семафора равен количеству активных тредов, но не более MAX_THREAD_NUM
        (надо синхронизировать доступ к этому счетчику из команды чекпоинта и добавления треда)
        (возможен дедлок, когда один из тредов захочет джобавить новый тред между save point и в это время активируется ZA_WARUDO)
    */

    //мьютекс остановки

    //массив тредов

    std::vector<std::thread> threads;
    int num_of_threads = 0;

    bool stop_world;
    std::condition_variable resume_time;
    std::condition_variable everything_stoped;

    std::mutex native_threads;
    std::mutex cond_var;

    void deactivate_thread(); //уменьшаеется количество активных тредов и при необходимости подается сигнал
    void activate_thread();

public: 

    NativeThread create_native_thread();
    void join_native_thread(NativeThread thread);

    void save_point(); //проверка на остановку
    void ZA_WARUDO();  //остановка всех тредов                         
    void resume_time_flow(); //возобновление натив кода

    GCThread create_GC();
    /*проверка save point(){

        уменьшение семафора активных тредов
        проверка мьютекса блокировки
        увеличение семафора активных тредов
    }
    */

   /*
        Вместо семафоров будет использована другая техника:
            при проверке save point тред вызывает функцию уменьшения количества работающих тредов
            в ней атомарно уменьшается счетчик процессов и если это количество становится нулевым, этот процесс будит основной 
            тред менеджера, который ожидает на condition_variable
   */

    /*ZA_WARUDO(){
        
        (если эта функция будет вызвана из двух тредов то дедлок, но она должна вызываться только из GC)
        блокировка мьютекса блокировки
        ожидание семафора тредов

            (можно сделать выполнение интсрукций через лямбду или через функцию запуска времени)
        
        GC не может ничего сделать, пока все треды не дошли до сейв поинта, поэтому 
        native mutrex может быть спокойно использован нативными тредами
    }
    */
};