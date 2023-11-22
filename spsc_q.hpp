#ifndef spsc_q_hpp
#define spsc_q_hpp

#include <iostream>
#include <type_traits>
#include <atomic>

namespace mkw{

    template <typename T, size_t _capacity>
    class spsc_q
    {
        static_assert(std::is_trivial<T>::value, "this stack is only for trivial types");
        static_assert(_capacity > 2, "size must be greater than 2");
        
        
    private:
        std::unique_ptr<T[]> _buffer;
        std::atomic_size_t __r;
        std::atomic_size_t __w;
    public:
        
        spsc_q() : __r{0}, __w{0}
        { _buffer = std::make_unique<T[]>(_capacity);}
        
        ~spsc_q() { };
        
        
        bool push(const T &value)
        {
            const size_t w = __w.load(std::memory_order_relaxed);
            
            size_t w_next = w + 1;
            
            if(w_next == _capacity)
                w_next = 0U;
            
            const size_t r = __r.load(std::memory_order_acquire);
            if(w_next == r)
                return false;
            
            _buffer[w] = value;
            
            __w.store(w_next, std::memory_order_release);
            
            return true;
            
        }
        
        bool pop(T &value)
        {
            const size_t w = __w.load(std::memory_order_relaxed);
            size_t r = __r.load(std::memory_order_acquire);
            
            if(w == r)
                return false;
            
            value = _buffer[r];
            
            ++r;
            
            if(r == _capacity)
                r = 0;
            
            __r.store(r, std::memory_order_release);
            
            return true;
        }
        
        size_t capacity() const
        {
            return _capacity;
        }
        

    };

}

#endif /* spsc_q_hpp */
