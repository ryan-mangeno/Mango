#pragma once

#include <defines.h>
#include <utility/type_traits.h>
#include <core/logger.h>

enum memory_tag {
    // For temporary use. Should be assigned one of the below or have a new tag created.
    MEMORY_TAG_UNKNOWN,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_DARRAY,
    MEMORY_TAG_DICT,
    MEMORY_TAG_RING_QUEUE,
    MEMORY_TAG_BST,
    MEMORY_TAG_STRING,
    MEMORY_TAG_APPLICATION,
    MEMORY_TAG_JOB,
    MEMORY_TAG_TEXTURE,
    MEMORY_TAG_MATERIAL_INSTANCE,
    MEMORY_TAG_RENDERER,
    MEMORY_TAG_GAME,
    MEMORY_TAG_TRANSFORM,
    MEMORY_TAG_ENTITY,
    MEMORY_TAG_ENTITY_NODE,
    MEMORY_TAG_SCENE,

    MEMORY_TAG_MAX_TAGS
};

MGO_API void mg_initialize_memory();
MGO_API void mg_shutdown_memory();

MGO_API void* mg_allocate(u64 size, memory_tag tag);
MGO_API void mg_free(void* block, u64 size, memory_tag tag);
MGO_API void* mg_zero_memory(void* block, u64 size);
MGO_API void* mg_copy_memory(void* dest, const void* source, u64 size);
MGO_API void* mg_set_memory(void* dest, i32 value, u64 size);
MGO_API char* mg_get_memory_usage_str();


// TODO: test these templates

// allocates mem for object and constructs in place
template<typename T, typename... Args>
MGO_API T* mg_placement_new(memory_tag tag, Args&&... args) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        MGO_WARN("Freeing memory with unknown tag. Reclass this allocation");
    }
    
    void* mem = mg_allocate(sizeof(T), tag);
    return new (mem) T(forward<Args>(args)...); 
}

// constructs object inplace in memory
template<typename T, typename... Args>
MGO_API T* mg_placement_construct(T* mem, memory_tag tag, Args&&... args) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        MGO_WARN("Freeing memory with unknown tag. Reclass this allocation");
    }
    
    return new (mem) T(forward<Args>(args)...); 
}

// destructor helper if objects were allocated with placement new, ~T will not be called for you other wise
template<typename T>
MGO_API void mg_placement_delete(T* ptr, memory_tag tag) noexcept {
    if (tag == MEMORY_TAG_UNKNOWN) {
        MGO_WARN("Freeing memory with unknown tag. Reclass this allocation");
    }

    ptr->~T();
    mg_free(ptr, sizeof(T), tag);
}



