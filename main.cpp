#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <list>
#include "lock_free_queue.hpp"

using namespace std::chrono_literals;

std::queue<int> que;
std::mutex mt;

lock_free_queue<int> l_q;


void foo1()
{
    for(int i=0; i<100; ++i)
        l_q.push(i);
}

void foo2()
{
    for(int i=100; i<200; ++i)
        l_q.push(i);
}

void foo3()
{
    
}


int main(int argc, const char * argv[]) {
    
    
    std::vector<int> vec;
    
    std::thread thr1(foo1);
    std::thread thr2(foo2);
    
    thr1.join();
    thr2.join();
    
    int j{};
    for(int i=0; i<200; ++i)
    {
        j = l_q.pop();
        vec.push_back(j);
        std::cout << j << ' ';
    }
    
    std::cout << '\n';
    
    std::cout << vec.size() << '\n';
    
    
    
    return 0;
}
