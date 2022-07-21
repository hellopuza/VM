#include <gtest/gtest.h>
#include <chrono>

#include "../include/Manager.h"

struct Environment{

    std::atomic_int check_point = 0;
    std::atomic_int num_of_errors = 0;

    std::atomic_bool terminate_native = false;
    std::atomic_bool native_is_terminated = false;
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
void GC_body(Environment& env, SharedThreadManager manager){

    if (manager->ZA_WARUDO() != ThreadManager::Errors::OK)
        env.num_of_errors++;

    env.native_is_terminated = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    env.terminate_native = true;
    
    manager->resume_time_flow();
}

void native_body(Environment& env, SharedThreadManager manager){

    while (!env.terminate_native){

        manager->save_point();

        if (env.native_is_terminated != env.terminate_native)
            env.num_of_errors++;
    }
}

TEST(Thread_manager, save_points){

    SharedThreadManager manager_ptr = std::make_shared<ThreadManager>();
    std::vector<NativeThread> threads;
    Environment env;

    for (int i = 0; i < NUM_OF_THREADS; i++){
                     
        NativeThread tmp = manager_ptr->create_native_thread(native_body, std::ref(env), manager_ptr); 
        threads.push_back(tmp);             
    }

    manager_ptr->create_GC(GC_body, std::ref(env), manager_ptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    for (int i = 0; i < NUM_OF_THREADS; i++){

        NativeThread cur_thread = threads[i];

        if (manager_ptr->join_native_thread(cur_thread) != ThreadManager::Errors::OK)
            env.num_of_errors++;
    }

    manager_ptr->join_GC();

    ASSERT_EQ(env.num_of_errors, 0);
}
//-------------------


//-------------------
void GC_double_body(){

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(Thread_manager, double_GC){

    ThreadManager manager;
    Environment env;

    GCThread fir_GC = manager.create_GC(GC_double_body);
    GCThread sec_GC = manager.create_GC(GC_double_body);

    manager.join_GC();

    ASSERT_EQ(static_cast<std::thread::id>(fir_GC), static_cast<std::thread::id>(sec_GC));
}
//-------------------


//-------------------
void native_native_body(){

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void native_body_with_block(Environment& env, SharedThreadManager manager){
    
    ThreadManager::Errors ret;
    NativeThread nat_nat_thread = manager->create_native_thread(native_native_body);
    
    if ((ret = manager->join_native_thread(nat_nat_thread)) != ThreadManager::Errors::OK)
        env.num_of_errors++;
}

void negative_GC_body(Environment& env, SharedThreadManager manager){
    
    NativeThread nat_thread = manager->create_native_thread(native_body_with_block, std::ref(env), manager);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    if (manager->ZA_WARUDO() != ThreadManager::Errors::Native_block)
        env.num_of_errors++;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    if (manager->join_native_thread(nat_thread) != ThreadManager::Errors::OK)
        env.num_of_errors++;
}

TEST(Thread_manager, native_block_negative){

    SharedThreadManager manager_ptr = std::make_shared<ThreadManager>();
    Environment env;
    
    manager_ptr->create_GC(negative_GC_body, std::ref(env), manager_ptr);

    if (manager_ptr->join_GC() != ThreadManager::Errors::OK)
        env.num_of_errors++;
    
    ASSERT_EQ(env.num_of_errors, 0);
}
//-------------------


//-------------------
TEST(Thread_manager, double_ZA_WARUDO){}
//-------------------


//-------------------
TEST(Thread_manager, fast_ZA_WARUDO){}
//-------------------


//-------------------
TEST(Thread_manager, multiple_native){}
//-------------------