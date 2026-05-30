#pragma once

#include "utility/type_traits.h"
#include "core/assert.h"
#include "defines.h"
#include "core/mgmemory.h"


template<typename T>
class MGO_API darray {
    public:
        darray() noexcept : _elements(nullptr), _capacity(0), _length(0) {
        }

        darray(u64 length, u64 capacity) noexcept : _capacity(capacity), _length(length) {
            _elements = static_cast<T*>(mg_allocate(_capacity * sizeof(T), MEMORY_TAG_DARRAY));
            for (u64 i=0 ; i<_length ; ++i) {
                mg_construct_at(&_elements[i], MEMORY_TAG_DARRAY);
            }
        }

        ~darray() {
            if (_elements) {
                if constexpr (!is_trivial_v<T>) {
                    for (u64 i=0 ; i<_length; ++i) {
                        _elements[i].~T(); // we only destroy the object, the block is freed below
                    }
                }
                mg_free(_elements, _capacity * sizeof(T), MEMORY_TAG_DARRAY);
                _elements = nullptr;
            }
        }
        
        template<typename... Args>
        inline void emplace_back(Args&&... args) {
            if (_length == _capacity) {
                _grow();
            } 
            mg_construct_at(&_elements[_length], MEMORY_TAG_DARRAY, forward<Args>(args)...);
            _length++;
        }

        template <typename U>
        inline void push_back(U&& element) {
            if (_length == _capacity) {
                _grow();
            }
            if constexpr (is_trivial_v<T>) {
                mg_copy_memory(&_elements[_length], &element, sizeof(T));
            } else {
                mg_construct_at(&_elements[_length], MEMORY_TAG_DARRAY, forward<U>(element));
            }
            _length++;
        }

        template <typename U>
        void insert(u64 index, U&& element) {
            MGO_ASSERT(index <= _length);

            if (_length == _capacity) {
                _grow();
            }
            
            if constexpr (is_trivial_v<T>) {
                // triv types don't care about constructors
                u64 elements_to_move = _length - index;
                if (elements_to_move > 0) {
                    mg_copy_memory(&_elements[index + 1], &_elements[index], elements_to_move * sizeof(T));
                }
                // raw copy assignment
                _elements[index] = forward<U>(element);
            } else {
                if (index == _length) {
                    // Edge case: Inserting at the very end (push_back behavior)
                    mg_construct_at(&_elements[index], MEMORY_TAG_DARRAY, forward<U>(element));
                } else {
                    // move construct the last active element into the raw unconstructed slot
                    mg_construct_at(&_elements[_length], MEMORY_TAG_DARRAY, move(_elements[_length - 1]));
                    
                    // shift the rest of the elements backward using standard move asignment
                    for (u64 i = _length - 1; i > index; --i) {
                        _elements[i] = move(_elements[i - 1]);
                    }
                    
                    // move the incoming element into the target index using move assignment
                    _elements[index] = forward<U>(element);
                }
            }
            
            _length++;
        }

        inline void pop_back() {
            if (_length > 0) {
                if constexpr (!is_trivial_v<T>) {
                    _elements[_length - 1].~T(); 
                }
                _length--;
            }
        }

        void erase(u64 index) {
            MGO_ASSERT(index < _length);

            if constexpr (is_trivial_v<T>) {
                u64 elements_to_move = _length - index - 1;
                if (elements_to_move > 0) {
                    // shift mem left by 1
                    mg_copy_memory(&_elements[index], &_elements[index + 1], elements_to_move * sizeof(T));
                }
            } else {
                // destroy the target element first
                _elements[index].~T();

                // shift remaining elements down via move construction
                for (u64 i = index; i < _length - 1; ++i) {
                    mg_construct_at(&_elements[i], MEMORY_TAG_DARRAY, move(_elements[i + 1]));
                    _elements[i + 1].~T(); // Clean up the old slot
                }
            }

            _length--;
        }

        void resize(u64 new_capacity) {
            if (new_capacity <= _capacity) return;

            T* new_elements = static_cast<T*>(mg_allocate(new_capacity * sizeof(T), MEMORY_TAG_DARRAY));
            
            if (_elements) {
                if constexpr (is_trivial_v<T>) {
                    mg_copy_memory(new_elements, _elements, _length * sizeof(T));
                } else {
                    for (u64 i=0; i<_length; ++i) {
                        mg_construct_at(&new_elements[i], MEMORY_TAG_DARRAY, move(_elements[i]));
                        _elements[i].~T();
                    }
                }
                mg_free(_elements, _capacity * sizeof(T), MEMORY_TAG_DARRAY);
            }
            
            _elements = new_elements;
            _capacity = new_capacity;
        }

        inline const T& get(u64 index) {
            MGO_ASSERT(index >= 0 && index < _length);
            return _elements[index];
        }

        inline const T& operator [](u64 index) noexcept {
            return _elements[index];
        }

        inline u64 size() const noexcept { return _length; }
        inline u64 capacity() const noexcept { return _capacity; }
        inline T* data() { return _elements; }

    private:
        inline void _grow() {
            resize(_capacity == 0 ? 4 : _capacity * 2);
        }

        T* _elements;
        u64 _capacity;
        u64 _length;
};