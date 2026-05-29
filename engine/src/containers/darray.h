#pragma once

#include "utility/type_traits.h"
#include "core/assert.h"
#include "defines.h"
#include "core/mgmemory.h"


template<typename T>
class MGO_API darray {
    public:
        darray() noexcept : m_elements(nullptr), m_capacity(0), m_length(0) {
        }

        darray(u64 length, u64 capacity) noexcept : m_capacity(capacity), m_length(length) {
            m_elements = static_cast<T*>(mg_allocate(m_capacity * sizeof(T), MEMORY_TAG_DARRAY));
            for (u64 i=0 ; i<m_length ; ++i) {
                mg_construct_at(&m_elements[i], MEMORY_TAG_DARRAY);
            }
        }

        ~darray() {
            if (m_elements) {
                if constexpr (!is_trivial_v<T>) {
                    for (u64 i=0 ; i<m_length; ++i) {
                        m_elements[i].~T(); // we only destroy the object, the block is freed below
                    }
                }
                mg_free(m_elements, m_capacity * sizeof(T), MEMORY_TAG_DARRAY);
                m_elements = nullptr;
            }
        }
        
        template<typename... Args>
        inline void emplace_back(Args&&... args) {
            if (m_length == m_capacity) {
                _grow();
            } 
            mg_construct_at(&m_elements[m_length], MEMORY_TAG_DARRAY, forward<Args>(args)...);
            m_length++;
        }

        template <typename U, typename = enable_if_t<is_same<T, decay_t<U>>::value>>
        inline void push_back(U&& element) {
            if (m_length == m_capacity) {
                _grow();
            }
            if constexpr (is_trivial_v<T>) {
                mg_copy_memory(&m_elements[m_length], &element, sizeof(T));
            } else {
                mg_construct_at(&m_elements[m_length], MEMORY_TAG_DARRAY, forward<U>(element));
            }
            m_length++;
        }

        template <typename U>
        void insert(u64 index, U&& element) {
            MGO_ASSERT(index <= m_length);

            if (m_length == m_capacity) {
                _grow();
            }
            
            if constexpr (is_trivial_v<T>) {
                // triv types don't care about constructors
                u64 elements_to_move = m_length - index;
                if (elements_to_move > 0) {
                    mg_copy_memory(&m_elements[index + 1], &m_elements[index], elements_to_move * sizeof(T));
                }
                // raw copy assignment
                m_elements[index] = forward<U>(element);
            } else {
                if (index == m_length) {
                    // Edge case: Inserting at the very end (push_back behavior)
                    mg_construct_at(&m_elements[index], MEMORY_TAG_DARRAY, forward<U>(element));
                } else {
                    // move construct the last active element into the raw unconstructed slot
                    mg_construct_at(&m_elements[m_length], MEMORY_TAG_DARRAY, move(m_elements[m_length - 1]));
                    
                    // shift the rest of the elements backward using standard move asignment
                    for (u64 i = m_length - 1; i > index; --i) {
                        m_elements[i] = move(m_elements[i - 1]);
                    }
                    
                    // move the incoming element into the target index using move assignment
                    m_elements[index] = forward<U>(element);
                }
            }
            
            m_length++;
        }

        inline void pop_back() {
            if (m_length > 0) {
                if constexpr (!is_trivial_v<T>) {
                    m_elements[m_length - 1].~T(); 
                }
                m_length--;
            }
        }

        void erase(u64 index) {
            MGO_ASSERT(index < m_length);

            if constexpr (is_trivial_v<T>) {
                u64 elements_to_move = m_length - index - 1;
                if (elements_to_move > 0) {
                    // shift mem left by 1
                    mg_copy_memory(&m_elements[index], &m_elements[index + 1], elements_to_move * sizeof(T));
                }
            } else {
                // destroy the target element first
                m_elements[index].~T();

                // shift remaining elements down via move construction
                for (u64 i = index; i < m_length - 1; ++i) {
                    mg_construct_at(&m_elements[i], MEMORY_TAG_DARRAY, move(m_elements[i + 1]));
                    m_elements[i + 1].~T(); // Clean up the old slot
                }
            }

            m_length--;
        }

        void resize(u64 new_capacity) {
            if (new_capacity <= m_capacity) return;

            T* new_elements = static_cast<T*>(mg_allocate(new_capacity * sizeof(T), MEMORY_TAG_DARRAY));
            
            if (m_elements) {
                if constexpr (is_trivial_v<T>) {
                    mg_copy_memory(new_elements, m_elements, m_length * sizeof(T));
                } else {
                    for (u64 i=0; i<m_length; ++i) {
                        mg_construct_at(&new_elements[i], MEMORY_TAG_DARRAY, move(m_elements[i]));
                        m_elements[i].~T();
                    }
                }
                mg_free(m_elements, m_capacity * sizeof(T), MEMORY_TAG_DARRAY);
            }
            
            m_elements = new_elements;
            m_capacity = new_capacity;
        }

        inline const T& get(u64 index) {
            MGO_ASSERT(index >= 0 && index < m_length);
            return m_elements[index];
        }

        inline const T& operator [](u64 index) noexcept {
            return m_elements[index];
        }

        inline u64 size() const noexcept { return m_length; }
        inline u64 capacity() const noexcept { return m_capacity; }

    private:
        inline void _grow() {
            resize(m_capacity == 0 ? 4 : m_capacity * 2);
        }

        T* m_elements;
        u64 m_capacity;
        u64 m_length;
};