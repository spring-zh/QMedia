#include <vector>
#include <atomic>
#include <mutex>

namespace LockFree {

template <typename T>
class CircleBuffer
{
public:
    CircleBuffer(int capacity): capacity_(capacity), buffer_(capacity){
        read_pos_.store(0);
    }
    ~CircleBuffer() {
    }

    int Capacity() const { return capacity_; }
    int Unused() const { return capacity_ - Size(); }
    int Size() const { 
        return (write_pos_ - read_pos_.load() + capacity_) % capacity_;
    }

    int Write(const T * data, int data_size) {
        if (Unused() > data_size) {
            T * w_ptr = buffer_.data();
            if (write_pos_ + data_size > capacity_)
            {
                int len1 = capacity_ - write_pos_;
                int len2 = write_pos_ + data_size - capacity_;
                memcpy(w_ptr + write_pos_, data, len1);
                memcpy(w_ptr, data + len1, len2);

                write_pos_ = len2;
            }else {
                memcpy(w_ptr + write_pos_, data, data_size);
                write_pos_ = (write_pos_ + data_size) % capacity_;
            }
            
            return data_size;
        }
        
        return -1;
    }

    int Read(T * data, int max_size) {
        int next_pos;
        int read_size;
        int read_pos = read_pos_.load();
        if (write_pos_ == read_pos) {
            return 0;
        }
        
        do
        {
            T * r_ptr = buffer_.data();
            int size_can_read = (write_pos_ - read_pos + capacity_) % capacity_;
            read_size = max_size > size_can_read ? size_can_read : max_size;
            if (read_pos + read_size > capacity_) {
                int len1 = capacity_ - read_pos;
                int len2 = read_pos + read_size - capacity_;
                memcpy(data, r_ptr + read_pos, len1);
                memcpy(data + len1, r_ptr, len2);

                next_pos = len2;
            }else {
                memcpy(data, r_ptr + read_pos, read_size);
                next_pos = (read_pos + read_size) % capacity_;
            }
        } while (!read_pos_.compare_exchange_weak(read_pos, next_pos));

        return read_size;
    }

    const T* GetReadPtr() const { return buffer_.data() + read_pos_; }

    void Clear() {
        write_pos_ = 0;
        read_pos_.store(0);
    }

private:
    std::vector<T> buffer_;

    const int capacity_;
    int write_pos_ = 0;
    std::atomic<int> read_pos_;
};

}