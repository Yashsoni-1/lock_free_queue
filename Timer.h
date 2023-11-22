#ifndef Timer_h
#define Timer_h

#include <chrono>

class Timer
{
public:
    Timer()
    {
        m_start_point = std::chrono::high_resolution_clock::now();
    }
    
    ~Timer()
    {
        stop();
    }
    
    void stop()
    {
        auto end_point = std::chrono::high_resolution_clock::now();
        auto start =
        std::chrono::time_point_cast<std::chrono::nanoseconds>(m_start_point).time_since_epoch().count();
        
        auto end =
        std::chrono::time_point_cast<std::chrono::nanoseconds>(end_point).time_since_epoch().count();
        
        auto duration = end - start;
        
        std::cout << duration << "ns ("<< duration * 0.001 <<")ms\n";
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_point;
    
};

#endif /* Timer_h */
