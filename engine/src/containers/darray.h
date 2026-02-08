#pragma once

#include <defines.h>

// TODO: NEED TO WORK ON ALL OF THIS, UNDER CONSTRUCTION
template<typename T>
class MGO_API darray {
    public:

        darray() : elements_(nullptr), capacity_(0), length_(0), stride_(sizeof(T)) noexcept{}

        darray(u64 length, u64 capacity) : capacity_(capacity), length_(length), stride_(sizeof(T)) noexcept {
            elements_ = static_cast<T*>(mg_allocate(capacity_ * stride_, MEMORY_TAG_DARRAY));
        }

        ~darray() {
            if (elements_) {
                mg_free(elements_, capacity_ * stride_, MEMORY_TAG_DARRAY);
                elements_ = nullptr;
            }
        }

        inline void push_back(const T& element) {
            if (length_ == capacity_) {
                grow();
            }
            mg_copy_memory(&elements_[length_], &element, stride_);
            length_++;
        }

        inline void emplace_back(T&& element) {
            if (length_ == capacity_) {
                grow();
            } 
            new (&elements_[length_]) T(move(element));
            length_++;
        }
        
        template<typename... Args>
        inline void emplace_back(Args&& ...) {
            if (length_ == capacity_) {
                grow();
            }
            new (&elements_[length_]) T(forward<Args>(args)...);
            length_++;
        }

        inline void insert(i32 index, const T& element) {
            if (length_ == capacity_) {
                grow();
            }

            for (int i=length_ ; i > index ; i--) {
                elements_[i] = move(elements_[i-1]);
            }

            elements_[index] = element;
            length_++;
        }

        void insert(i32 index, T&& element) {
            if (length_ == capacity_) {
                grow();
            }

            for (int i=length_ ; i > index ; i--) {
                elements_[i] = move(elements_[i-1]);
            }

            elements_[index] = move(element);
            length_++;
        }

        T pop_back() {

        }

        T get(i32 index) {

        }

        inline u64 size() const noexcept { return length_; }
        inline u64 capacity() const noexcept { return capacity_; }

        void resize(u64 new_capacity) {
            if (new_capacity <= capacity_) {
                return;
            }
            T* new_elements = static_cast<T*>(mg_allocate(new_capacity * stride_, MEMORY_TAG_DARRAY));
            if (elements_) {
                mg_copy_memory(new_elements, elements_, length_ * stride_);
                mg_free(elements_, capacity_ * stride_, MEMORY_TAG_DARRAY);
            }
            elements_ = new_elements;
            capacity_ = new_capacity;
        }

    private:
        void grow() {
            resize(capacity_ == 0 ? 4 : capacity_ * 2);
        }

        T* elements_;
        u64 capacity_;
        u64 length_;
        u64 stride_;
};