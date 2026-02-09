#pragma once

#include <utility/type_traits.h>
#include <core/assert.h>
#include <defines.h>
#include <core/mgmemory.h>

template<typename T>
class MGO_API darray {
    public:
        darray() noexcept : elements_(nullptr), capacity_(0), length_(0), stride_(sizeof(T)) {}

        darray(u64 length, u64 capacity) noexcept : capacity_(capacity), length_(length), stride_(sizeof(T)) {
            elements_ = static_cast<T*>(mg_allocate(capacity_ * stride_, MEMORY_TAG_DARRAY));
        }

        ~darray() {
            if (elements_) {
                if constexpr (!is_trivial_v<T>) {
                    for (u64 i = 0; i < length_; ++i) {
                        elements_[i].~T(); // we only destroy the object, the block is freed below
                    }
                }
                mg_free(elements_, capacity_ * stride_, MEMORY_TAG_DARRAY);
                elements_ = nullptr;
            }
        }
        
        template<typename... Args>
        inline void emplace_back(Args&&... args) {
            if (length_ == capacity_) {
                grow();
            } 
            mg_placement_construct<T>(&elements_[length_], MEMORY_TAG_DARRAY, forward<Args>(args)...);
            length_++;
        }

        template <typename U, typename = enable_if_t<is_same<T, decay_t<U>>::value>>
        inline void push_back(U&& element) {
            if (length_ == capacity_) {
                grow();
            }
            if constexpr (is_trivial_v<T>) {
                mg_copy_memory(&elements_[length_], &element, stride_);
            } else {
                mg_placement_construct<T>(&elements_[length_], MEMORY_TAG_DARRAY, forward<U>(element));
            }
            length_++;
        }

        template <typename U>
        void insert(i32 index, U&& element) {
            if (length_ == capacity_) {
                grow();
            }
            
            for (i32 i = length_; i > index; i--) {
                if constexpr (is_trivial_v<T>) {
                    elements_[i] = elements_[i - 1];
                } else {
                    // destroy before we construct else we get memory leaks
                    if (i < length_) {
                        elements_[i].~T();
                    }
                    mg_placement_construct<T>(&elements_[i], MEMORY_TAG_DARRAY, move(elements_[i - 1]));
                }
            }
            
            // destroy obj at index if non trivial
            if constexpr (!is_trivial_v<T>) {
                if (index < length_) {
                    elements_[index].~T();
                }
            }
            
            // construct new element
            if constexpr (is_trivial_v<T>) {
                mg_copy_memory(&elements_[index], &element, sizeof(T));
            } else {
                mg_placement_construct<T>(&elements_[index], MEMORY_TAG_DARRAY, forward<U>(element));
            }
            
            length_++;
        }

        inline void pop_back() {
            if (length_ > 0) {
                if constexpr (!is_trivial_v<T>) {
                    elements_[length_ - 1].~T(); 
                }
                length_--;
            }
        }

        inline void resize(u64 new_capacity) {
            if (new_capacity <= capacity_) return;

            T* new_elements = static_cast<T*>(mg_allocate(new_capacity * stride_, MEMORY_TAG_DARRAY));
            
            if (elements_) {
                if constexpr (is_trivial_v<T>) {
                    mg_copy_memory(new_elements, elements_, length_ * stride_);
                } else {
                    for (u64 i = 0; i < length_; ++i) {
                        mg_placement_construct<T>(&new_elements[i], MEMORY_TAG_DARRAY, move(elements_[i]));
                        elements_[i].~T();
                    }
                }
                mg_free(elements_, capacity_ * stride_, MEMORY_TAG_DARRAY);
            }
            
            elements_ = new_elements;
            capacity_ = new_capacity;
        }

        inline const T& get(i32 index) {
            MGO_ASSERT(index >= 0 && index < length_);
            return elements_[index];
        }

        inline const T& operator [](i32 index) noexcept {
            return elements_[index];
        }

        inline u64 size() const noexcept { return length_; }
        inline u64 capacity() const noexcept { return capacity_; }

    private:
        inline void grow() {
            resize(capacity_ == 0 ? 4 : capacity_ * 2);
        }

        T* elements_;
        u64 capacity_;
        u64 length_;
        u64 stride_;
};