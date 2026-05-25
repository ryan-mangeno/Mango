#pragma once

#include <utility/type_traits.h>
#include <core/assert.h>
#include <defines.h>
#include <core/mgmemory.h>


template<typename T>
class MGO_API darray {
    public:
        darray() noexcept : m_elements(nullptr), m_capacity(0), m_length(0) {
        }

        darray(u64 length, u64 capacity) noexcept : m_capacity(capacity), m_length(length) {
            m_elements = static_cast<T*>(mg_allocate(m_capacity * sizeof(T), MEMORY_TAG_DARRAY));
            for (u64 i=0 ; i<m_length ; ++i) {
                mg_placement_construct(&m_elements[i], MEMORY_TAG_DARRAY);
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
                grow();
            } 
            mg_placement_construct(&m_elements[m_length], MEMORY_TAG_DARRAY, forward<Args>(args)...);
            m_length++;
        }

        template <typename U, typename = enable_if_t<is_same<T, decay_t<U>>::value>>
        inline void push_back(U&& element) {
            if (m_length == m_capacity) {
                grow();
            }
            if constexpr (is_trivial_v<T>) {
                mg_copy_memory(&m_elements[m_length], &element, sizeof(T));
            } else {
                mg_placement_construct(&m_elements[m_length], MEMORY_TAG_DARRAY, forward<U>(element));
            }
            m_length++;
        }

        template <typename U>
        void insert(i32 index, U&& element) {
            if (m_length == m_capacity) {
                grow();
            }
            
            for (i32 i=m_length; i>index; --i) {
                if constexpr (is_trivial_v<T>) {
                    m_elements[i] = m_elements[i - 1];
                } else {
                    // destroy before we construct else we get memory leaks
                    if (i < m_length) {
                        m_elements[i].~T();
                    }
                    mg_placement_construct(&m_elements[i], MEMORY_TAG_DARRAY, move(m_elements[i - 1]));
                }
            }
            
            // destroy obj at index if non trivial
            if constexpr (!is_trivial_v<T>) {
                if (index < m_length) {
                    m_elements[index].~T();
                }
            }
            
            // construct new element
            if constexpr (is_trivial_v<T>) {
                mg_copy_memory(&m_elements[index], &element, sizeof(T));
            } else {
                mg_placement_construct(&m_elements[index], MEMORY_TAG_DARRAY, forward<U>(element));
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

        inline void resize(u64 new_capacity) {
            if (new_capacity <= m_capacity) return;

            T* new_elements = static_cast<T*>(mg_allocate(new_capacity * sizeof(T), MEMORY_TAG_DARRAY));
            
            if (m_elements) {
                if constexpr (is_trivial_v<T>) {
                    mg_copy_memory(new_elements, m_elements, m_length * sizeof(T));
                } else {
                    for (u64 i=0; i<m_length; ++i) {
                        mg_placement_construct(&new_elements[i], MEMORY_TAG_DARRAY, move(m_elements[i]));
                        m_elements[i].~T();
                    }
                }
                mg_free(m_elements, m_capacity * sizeof(T), MEMORY_TAG_DARRAY);
            }
            
            m_elements = new_elements;
            m_capacity = new_capacity;
        }

        inline const T& get(i32 index) {
            MGO_ASSERT(index >= 0 && index < m_length);
            return m_elements[index];
        }

        inline const T& operator [](i32 index) noexcept {
            return m_elements[index];
        }

        inline u64 size() const noexcept { return m_length; }
        inline u64 capacity() const noexcept { return m_capacity; }

    private:
        inline void grow() {
            resize(m_capacity == 0 ? 4 : m_capacity * 2);
        }

        T* m_elements;
        u64 m_capacity;
        u64 m_length;
};