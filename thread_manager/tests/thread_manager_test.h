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

void pass_check_point(Environment& env){
    
    env.check_point++;
}

TEST(Thread_manager, native_threads){

    ThreadManager manager;
    std::vector<NativeThread> threads;
    Environment env;

    for (int i = 0; i < NUM_OF_THREADS; i++){
                                
        NativeThread tmp = manager.create_native_thread(pass_check_point, std::ref(env)); 
        threads.push_back(tmp);             
    }

    for (int i = 0; i < NUM_OF_THREADS; i++){

        NativeThread cur_thread = threads[i];

        if (manager.join_native_thread(cur_thread) != ThreadManager::Errors::OK)
            env.num_of_errors++;
    }

    ASSERT_EQ(env.check_point, NUM_OF_THREADS);
    ASSERT_EQ(env.num_of_errors, 0);
}
//-------------------


//-------------------
void GC_body(std::atomic_bool& terminate_native, SharedThreadManager manager){

    manager->ZA_WARUDO();
    terminate_native = true;
    manager->resume_time_flow();
}

void native_body(std::atomic_bool& terminate_native){

    while (!terminate_native){}
}

void test(SharedThreadManager var){}

TEST(Thread_manager, save_points){

    int num_of_errors = 0;
    SharedThreadManager manager_ptr = std::make_shared<ThreadManager>();
    std::vector<NativeThread> threads;
    std::atomic_bool terminate_native = false;

    for (int i = 0; i < NUM_OF_THREADS; i++){
                     
        NativeThread tmp = manager_ptr->create_native_thread(native_body, std::ref(terminate_native)); 
        threads.push_back(tmp);             
    }

    manager_ptr->create_GC(GC_body, std::ref(terminate_native), manager_ptr);

    for (int i = 0; i < NUM_OF_THREADS; i++){

        NativeThread cur_thread = threads[i];

        if (manager_ptr->join_native_thread(cur_thread) != ThreadManager::Errors::OK)
            num_of_errors++;
    }

    ASSERT_EQ(num_of_errors, 0);
}
