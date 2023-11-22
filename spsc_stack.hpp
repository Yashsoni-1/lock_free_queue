#ifndef spsc_stack_hpp
#define spsc_stack_hpp


#include <iostream>
#include <type_traits>
#include <atomic>

namespace mkw{

    template <typename T, size_t _capacity>
    class spsc_stack
    {
        static_assert(std::is_trivial<T>::value, "this stack is only for trivial types");
        static_assert(_capacity > 2, "size must be greater than 2");
        
        
    private:
        std::unique_ptr<T[]> _buffer;
        std::atomic_size_t __r_w;
    public:
        
        spsc_stack() : __r_w{0}
        { _buffer = std::make_unique<T[]>(_capacity);}
        
        ~spsc_stack() { };
        
        
        bool push(const T &value)
        {
            const size_t w = __r_w.fetch_add(1);
            
            if(w == _capacity)
                return false;
            
            _buffer[w] = value;
            
            return true;
            
        }
        
        bool pop(T &value)
        {
            if(__r_w.load() == 0) return false;
            const size_t r = __r_w.fetch_sub(1);
            
            if(r == 0)
                return false;
            
            value = _buffer[r];
            
            return true;
        }
        
        

    };

}


#endif /* spsc_stack_hpp */
