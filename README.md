# C++ LOCK FREE DATA STRUCTURES

In this project I'm working on some Lock Free DSs. These DSs are light weight with really simple APIs, which are almost same as the standard APIs. These DSs are almost 3x faster than their mutex based counterparts.
There are both SPSC(Single Producer and Single Consumer) and MPMC(Multiple Producer and Multiple Consumer) version of every DS.

> NOTE: At the moment the queue and other DSs are meant for only [trivial](https://en.cppreference.com/w/cpp/named_req/TrivialType) data types.

> NOTE: Above "DS" stands for data structure.

## FEATURES

- Fully thread safe DSs.
- Can be accessed from any number of threads.
- Fully Deadlock safe.
- Header only DSs. Just Drop them directly in the project.
- Written in standard c++14, compatible with almost every platform supporting std::atomic.

## But Why? Why do we need them?
Simply because standard does't have one. We can use mutex based, but they are really very slow, even these lock free versions are, upto 2x, slower than normal data structures. But the biggest problem with mutex based DSs is the deadlock. Lock Free DSs are deadlock free simply because they do not hold lock. At any given time one of the thread is guaranted to make the progress. If a thread fails to make a change it is only because some other thread has made the progress. 
### Comparison bw mutex based queue and mkw::mpmc_q
![comparison 1 graph](https://github.com/Yashsoni-1/lock_free_queue/blob/main/images/1.1.png)

## mkw::mpmc_q - MPMC QUEUE
- This is a simple and general purpose queue supporting multiple producers(push) and multiple consumers(pop) at the same time.
- This queue is based on linked list implementation.
- This queue takes the hit on memory and performance compared to its SPSC counterpart.
- It is slow in the beginning but as number of elements and threads increase it catches up on the performance.
### API
The queue class provides following member functions:
- `push` - Appends the element at the end.
- `try_pop` - Removes the element from the beginning. If the queue is empty it does nothing.
- `was_front` - Returns pointer to the first element in the queue. Basically, the element that will be removed first if try_pop() is called.
- `was_back` - Returns pointer to the last element in the queue. 
- `was_empty` - Returns true if container was empty at the time of calling the functions else false.
- `was_size` - Returns size of the container at the time of calling the functions.

## mkw::spsc_q - SPSC QUEUE 
- This is a simple SPSC Lock Free Queue based on circular buffer based implementation.
- Should be used only with one thread producing(pushing) and other one consuming(popping).
- Maximum queue capcity should be set at compile time.
- This is much more performant compared to its MPMC counterpart.
### Comparison with all the queues above
![comparison 2 graph](https://github.com/Yashsoni-1/lock_free_queue/blob/main/images/2.png)
### API
The queue class provides following member functions:
- `try_push` - Appends the element at the end. Returns true if successfull else false.
- `try_pop` - Removes the element from the beginning. Returns true if successfull else false.


## mkw::mpmc_stack - MPMC STACK
- This is a simple and general purpose stack supporting multiple producers(push) and multiple consumers(pop) at the same time.
- Other properties are similar to MPMC QUEUE above.
### API
All the functions are similar to MPMC Queue above.

## mkw::spsc_stack - SPSC STACK 
- This is a simple SPSC Lock Stack, based on bounded buffer based implementation.
- Should be used only with one thread producing(pushing) and other one consuming(popping).
- This is much more performant compared to its MPMC counterpart.
- ### API
All the functions are similar to MPMC Queue above.
> Note: It is crashing right now.
