#ifndef lock_free_queue_hpp
#define lock_free_queue_hpp

#include <iostream>
#include <atomic>


template <typename T>
class lock_free_queue
{
private:
    struct queue_node
    {
        T data;
        std::atomic<queue_node *> next;
        
        queue_node(T data) : data{data}, next{nullptr} { }
        queue_node() : data{T{}}, next{nullptr} { }
    };
    
    std::atomic<queue_node *> head;
    std::atomic<queue_node *> tail;
    std::atomic<int> _size;
public:
    
    lock_free_queue()
    {
        queue_node *new_node = new queue_node();
        head = new_node;
        tail = new_node;
    }
    
    void push(T value)
    {
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
        }while(!success);
        
        tail.compare_exchange_weak(old_tail, new_node);
       
    }
    
    T pop()
    {
        queue_node *second_head = head.load();
        if(second_head->next == nullptr) exit(EXIT_FAILURE);
        
        while(!head.compare_exchange_weak(second_head, second_head->next)) { }
        
        queue_node *new_head = head.load();
        
        return new_head->data;
    }
    
    
    
    T top()
    {
        if(!head->next) return T{};
        return head->next->data;
    }
    
    int size()
    {
        return _size;
    }
    
};

#endif /* lock_free_queue_hpp */
