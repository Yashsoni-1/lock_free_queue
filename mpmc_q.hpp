#ifndef mpmc_q_hpp
#define mpmc_q_hpp

#include <iostream>
#include <atomic>
#include <exception>

namespace mkw
{
    #define __mpmc_q_debug_define 0

    class mpmc_q_exception : public std::runtime_error
    {
    public:
        mpmc_q_exception() : std::runtime_error("queue is empty") {}
        mpmc_q_exception(const std::string message) : std::runtime_error(message) {}
    };

    template <typename T>
    class mpmc_q
    {
        static_assert(std::is_trivial<T>::value, "this queue is only for trivial types");
    private:
        struct queue_node
        {
            const T data;
            std::atomic<queue_node *> next;
            
            queue_node(const T data) : data{data}, next{nullptr} { }
            queue_node() : data{T{}}, next{nullptr} { }
        };
        
        std::atomic<queue_node *> head;
        std::atomic<queue_node *> tail;
        std::atomic_size_t _size;
    public:
        
        mpmc_q() : _size{0}
        {
            queue_node *new_node = new queue_node();
            head.store(new_node);
            tail.store(new_node);
        }
        
        ~mpmc_q()
        {
            queue_node *head_temp = head.load();
            queue_node *temp{head_temp};
            for(; head_temp;)
            {
                head_temp = (head_temp->next).load();
                if(temp) delete temp;
                temp = head_temp;
            }
        }
        
        void push(const T value)
        {
            
    #if __mpmc_q_debug_define
            std::cout << __FUNCTION__ << '\n';
    #endif
            queue_node *new_node = new queue_node(value);
            queue_node *old_tail = tail.load();
            bool success{};
            
            do
            {
                old_tail = tail.load();
                queue_node *null_node = nullptr;
                success = old_tail->next.compare_exchange_weak(null_node, new_node);
                if(!success)
                    tail.compare_exchange_weak(old_tail, old_tail->next);
            } while(!success);
            
            tail.compare_exchange_weak(old_tail, new_node);
            ++_size;
        }
        
        void try_pop()
        {
            
    #if __mpmc_q_debug_define
            std::cout << __FUNCTION__ << '\n';
    #endif

            if(was_empty()) return;
            
            queue_node *second_head{nullptr};
            
            do
            {
                second_head = head.load();
                if(!second_head->next) return;
            }
            while(!head.compare_exchange_weak(second_head,
                                              second_head->next));
            
            --_size;
            
            if(second_head)
            {
                delete second_head;
                second_head = nullptr;
            }
            
            
        }
        
        bool was_empty() const
        {
            
    #if __mpmc_q_debug_define
            std::cout << __FUNCTION__ << '\n';
    #endif

            return _size.load() == 0;
        }
        
        
        
        T *was_front() const
        {
            
    #if __mpmc_q_debug_define
            std::cout << __FUNCTION__ << '\n';
    #endif
            
            if(was_empty())
                return nullptr;
            
            queue_node *end = ((head.load())->next).load();
            if(end)
                return const_cast<T *>(&end->data);
            
            return nullptr;
        }
        
        T *was_back() const
        {
            
    #if __mpmc_q_debug_define
            std::cout << __FUNCTION__ << '\n';
    #endif
            if(was_empty())
                return nullptr;
            queue_node *top = tail.load();
            if(top)
                return const_cast<T *>(&top->data);
            
            return nullptr;
        }
        
        size_t was_size() const
        {
            return _size.load();
        }
        
        
        
    };
}

#endif /* mpmc_q_hpp */
