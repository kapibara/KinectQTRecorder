#ifndef SYNCQUEUE
#define SYNCQUEUE

#include <queue>
#include <tuple>

#include <iostream>
/*this should not be here, but for now it can live..*/
class Buffer
{
public:
    Buffer(){
        data_ = nullptr;
        N_ = 0;
    }

    Buffer(unsigned char *buffer, size_t N){
        //std::cerr << "Buffer::Buffer(buffer,N)" << std::endl;
        if (N >0)
        {
            data_ = new unsigned char[N];
            memcpy(data_,buffer,N);
            N_ = N;
        }else{
            data_ = nullptr;
            N_ = 0;
        }
    }

    Buffer(Buffer &&from){
        //std::cerr << "Buffer::Buffer(Buffer &&)" << std::endl;
        data_ = from.data_;
        N_ = from.N_;
        from.data_ = nullptr;
        from.N_ = 0;
    }

    Buffer(const Buffer & from){
        //std::cerr << "Buffer::Buffer(const Buffer &)" << std::endl;
        data_ = new unsigned char[from.N_];
        memcpy(data_,from.data_,from.N_);
        N_ = from.N_;
    }

    ~Buffer(){
       if(data_ != nullptr)
            delete [] data_;
    }

    Buffer& operator=(Buffer&& from){
        //std::cerr << "Buffer::operator=(Buffer&&)" << std::endl;
        if(this != &from){
            if(data_ != nullptr)
                delete [] data_;
            data_ = from.data_;
            N_ = from.N_;
            from.data_ = nullptr;
            from.N_ = 0;
        }
        return *this;
    }

    Buffer& operator=(const Buffer& from){
        //std::cerr << "Buffer::operator=(const Buffer&)" << std::endl;
        if(this != &from){
            if(data_ != nullptr)
                delete [] data_;
            data_ = new unsigned char[from.N_];
            memcpy(data_,from.data_,from.N_);
            N_ = from.N_;
        }
        return *this;
    }

    unsigned char *getData(){
        return data_;
    }

private:
    size_t N_;
    unsigned char *data_;
};


/** MUTEX - any mutex class with two methods: lock() and unlock()**/
template <class MUTEX, class T>
class SyncQueue
{
public:
    SyncQueue(size_t rej_size = 0)
    {
        rejSize_ = rej_size;
    }

    ~SyncQueue()
    {

    }

    bool push(const T& data){
        bool success = false;
        lock_.lock();
        if(rejSize_ == 0 || q.size() < rejSize_){
            q.push(data);
            success = true;
        }
        lock_.unlock();
        return success;
    }

    bool push(T&& data){
        bool success = false;
        lock_.lock();
        if(rejSize_ == 0 || q.size() < rejSize_){
            q.push(std::move(data));
            success = true;
        }
        lock_.unlock();
        return success;
    }

    T front(){
        T data;
        lock_.lock();
        data = q.front();
        lock_.unlock();
        return std::move(data);
    }

    void pop(){
        lock_.lock();
        q.pop();
        lock_.unlock();
    }

    T front_and_pop(){
        T data;
        lock_.lock();
        data = std::move(q.front());
        q.pop();
        lock_.unlock();
        return std::move(data);
    }

    bool empty() {
        bool result;
        lock_.lock();
        result = q.empty();
        lock_.unlock();
        return result;
    }

    size_t size() {
        size_t size;
        lock_.lock();
        size = q.size();
        lock_.unlock();
        return size;
    }

private:
    MUTEX lock_;
    std::queue<T> q;
    size_t rejSize_;
};


typedef std::tuple<Buffer,Buffer,Buffer> ToFFrame;


#endif
