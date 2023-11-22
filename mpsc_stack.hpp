#ifndef mpsc_stack_hpp
#define mpsc_stack_hpp

#include <iostream>
#include <atomic>
#include <exception>


#define __stack_debug_define 0

namespace mkw
{

    class mpmc_stack_exception : public std::runtime_error
    {
    public:
        mpmc_stack_exception() : std::runtime_error("stack is empty") {}
        mpmc_stack_exception(const std::string message) : std::runtime_error(message) {}
    };

    template <typename T>
    class mpmc_stack
    {
        static_assert(std::is_trivial<T>::value, "this queue is only for trivial types");
    private:
        struct stack_node
        {
            const T data;
            std::atomic<stack_node *> next;
            
            stack_node(const T data) : data{data}, next{nullptr} { }
            stack_node() : data{T{}}, next{nullptr} { }
        };
        
        std::atomic<stack_node *> head;
        std::atomic_size_t _size;
    public:
        
        mpmc_stack() : _size{0}
        {
            head.store(nullptr, std::memory_order_relaxed);
        }
        
        ~mpmc_stack()
        {
            stack_node *head_temp = head.load(std::memory_order_acquire);
            stack_node *temp{head_temp};
            for(;head_temp;)
            {
                head_temp = (head_temp->next).load();
                if(temp) delete temp;
                temp = head_temp;
            }
        }
        
    #if 0
        void push(const T value)
        {
            
    #if __stack_debug_define
            std::cout << __FUNCTION__ << '\n';
    #endif
            stack_node *new_node = new stack_node(value);
            stack_node *old_tail = tail.load();
            bool success{};
            
            do
            {
                old_tail = tail.load();
                stack_node *null_node = nullptr;
                success = old_tail->next.compare_exchange_weak(null_node, new_node);
                if(!success)
                    tail.compare_exchange_weak(old_tail, old_tail->next);
            } while(!success);
            
            tail.compare_exchange_weak(old_tail, new_node);
            ++_size;
        }
    #endif
        
        void push(const T &value)
        {
            
    #if __stack_debug_define
            std::cout << __FUNCTION__ << '\n';
    #endif
            stack_node *new_head = new stack_node(value);
            
            stack_node *old_head = head.load(std::memory_order_acquire);
            
            
            do
            {
                (new_head->next).store(old_head);
            }   while(!head.compare_exchange_weak(old_head, new_head, std::memory_order_relaxed));

            
            ++_size;
        }
        
        
        void try_pop()
        {
            
    #if __stack_debug_define
            std::cout << __FUNCTION__ << '\n';
    #endif
            
            if(was_empty()) return;
            
            stack_node *old_head = head.load(std::memory_order_acquire);
            stack_node *new_head {nullptr};
            
            do
            {
                if(!old_head) return;
                new_head = (old_head->next).load();
            }while(!head.compare_exchange_weak(old_head, new_head, std::memory_order_relaxed));
           
            
            --_size;
            
            
            
        }
        
        bool was_empty() const
        {
            
    #if __stack_debug_define
            std::cout << __FUNCTION__ << '\n';
    #endif

            return _size.load() == 0;
        }
        
        
        
        T *was_top() const
        {
            
    #if __stack_debug_define
            std::cout << __FUNCTION__ << '\n';
    #endif
            stack_node *top = head.load(std::memory_order_acquire);
            if(top)
                return const_cast<T *>(&top->data);
            
            return nullptr;
        }
        
        int was_size() const
        {
            return _size.load(std::memory_order_relaxed);
        }
        
    };
}

#endif /* mpsc_stack_hpp */
