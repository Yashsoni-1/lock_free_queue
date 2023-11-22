#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <list>
#include "mpmc_q.hpp"
#include <atomic>
#include <utility>
#include "Timer.h"
#include "c_queue.hpp"
#include "spsc_q.hpp"
#include "mpsc_stack.hpp"
#include "spsc_stack.hpp"

using namespace std::chrono_literals;

#define mpsc_stack_test 0
#define performance_test 1


#if performance_test
int main(int argc, const char * argv[]) {
    
    constexpr size_t N {100'000'000};
    c_queue<size_t> c_q;
    mkw::mpmc_q<size_t> m_q;
    mkw::spsc_q<size_t, N*4> s_q;
    std::atomic<size_t> n;
    
    [&]() {
        for(size_t i{}; i<N; ++i)
        {
            c_q.push(i);
            m_q.push(i);
            s_q.push(i);
        }
    }();
    
    
    auto consume_c_queue = [&]() {
        while(!c_q.is_empty())
        {
            c_q.pop();
        }
    };
    
    auto consume_m_q = [&]() {
        while(!m_q.was_empty())
        {
            m_q.try_pop();
        }
            
    };
    
    auto consume_s_q = [&]() {
        for(size_t i=0; i<N; ++i)
            s_q.pop(i);
            
    };
    
    
    {
        Timer T;
        std::vector<std::thread> vec;
        uint32_t number_of_threads{std::thread::hardware_concurrency()};
        
        for(uint32_t i{}; i<number_of_threads; ++i)
        {
            vec.emplace_back(consume_c_queue);
        }
        
        
        for(uint32_t i{}; i<number_of_threads; ++i)
        {
            vec.at(i).join();
        }
    }
    
    {
        Timer T;
        std::vector<std::thread> vec;
        uint32_t number_of_threads{std::thread::hardware_concurrency()};
        
        for(uint32_t i{}; i<number_of_threads; ++i)
        {
            vec.emplace_back(consume_m_q);
        }
        
        for(uint32_t i{}; i<number_of_threads; ++i)
        {
            vec.at(i).join();
        }
    }
    
    {
        Timer T;
        
        std::thread thr(consume_s_q);
        
        thr.join();
    }
    
    std::cout << n.load() << '\n';
    
    
    return 0;
}

#endif




#if mpsc_stack_test
int main(int argc, const char * argv[]) {
    
    constexpr size_t N{200};
    mkw::mpmc_stack<int> stack;
    mkw::mpmc_q<int> q;
    
    auto produce = [&] () {
        std::this_thread::sleep_for(1ms);
        for(int i=0; i<N; ++i)
        {
            q.push(i);
        }
    };
    
    auto consume = [&] ()
    {
        std::this_thread::sleep_for(1ms);
        for(int i=0; i<N; ++i)
        {
            auto j = q.was_front();
            
            std::cout << (j ? *j : -111) << '\n';
            
            q.try_pop();
            
        }
    };
    
    
    std::vector<std::thread> vec;
    uint32_t number_of_threads{std::thread::hardware_concurrency()};
    
    for(uint32_t i{}; i<number_of_threads; ++i)
    {
        vec.emplace_back(produce);
    }
    
    std::vector<std::thread> vec2;
    
    for(uint32_t i{}; i<number_of_threads; ++i)
    {
        vec2.emplace_back(consume);
    }
    
    for(uint32_t i{}; i<number_of_threads; ++i)
    {
        vec.at(i).join();
    }
    
    for(uint32_t i{}; i<number_of_threads; ++i)
    {
        vec2.at(i).join();
    }
    
    return 0;
}

#endif
