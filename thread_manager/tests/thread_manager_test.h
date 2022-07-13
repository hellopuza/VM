#include <gtest/gtest.h>

#include "../include/Manager.h"

struct Environment{

    std::atomic_int check_point = 0;
    std::atomic_int num_of_errors = 0;
};

//-------------------
void empty_func(int test){}

TEST(Thread_manager, initial){

    ThreadManager manager;
    manager.create_GC(empty_func, 1);
    manager.join_GC();
}
//-------------------


//-------------------
#define NUM_OF_THREADS 10

void pass_check_point(int env){
    
    //env.check_point++;
}

TEST(Thread_manager, native_threads){

    ThreadManager manager;
    std::vector<NativeThread> threads;
    Environment env;

    for (int i = 0; i < NUM_OF_THREADS; i++){
                                                                        //V----тут надо засунуть env
        NativeThread tmp = manager.create_native_thread(pass_check_point, 1); //почему-то ссылки не пролезают
        threads.push_back(tmp);             
    }

    for (int i = 0; i < NUM_OF_THREADS; i++){

        NativeThread cur_thread = threads[i];

        if (manager.join_native_thread(cur_thread) != Errors::OK)
            env.num_of_errors++;
    }

    ASSERT_EQ(env.check_point, NUM_OF_THREADS);
    ASSERT_EQ(env.num_of_errors, 0);
}
//-------------------

//надо негативные тесты прописать 
