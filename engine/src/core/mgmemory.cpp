#include "mgmemory.h"

#include <core/logger.h>
#include <platform/platform.h>

#include <string.h> // TODO: remove , will use own string lib
#include <stdio.h> // TODO: remove 

struct MemoryStats {
    u64 total_allocated_;
    u64 tagged_allocations_[static_cast<u64>(memory_tag::MEMORY_TAG_MAX_TAGS)];
};

static const char* memory_tag_strings_[static_cast<u64>(memory_tag::MEMORY_TAG_MAX_TAGS)] = {
    "UNKNOWN    ",
    "ARRAY      ",
    "DARRAY     ",
    "DICT       ",
    "RING_QUEUE ",
    "BST        ",
    "STRING     ",
    "APPLICATION",
    "JOB        ",
    "TEXTURE    ",
    "MAT_INST   ",
    "RENDERER   ",
    "GAME       ",
    "TRANSFORM  ",
    "ENTITY     ",
    "ENTITY_NODE",
    "SCENE      ",
};

static struct MemoryStats stats_;

void mg_initialize_memory() {
    Platform::zero_memory(&stats_, sizeof(stats_));
}

void mg_shutdown_memory() {
    
}

void* mg_allocate(u64 size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        MGO_WARN("Allocating memory with unknown tag. Reclass this allocation");
    }

    stats_.total_allocated_ += size;
    stats_.tagged_allocations_[static_cast<u64>(tag)] += size;

    // TODO: memory alignment
    void* block = Platform::allocate(size, FALSE);
    Platform::zero_memory(block, size);
    return block;
}

void mg_free(void* block, u64 size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        MGO_WARN("Freeing memory with unknown tag. Reclass this allocation");
    }

    stats_.total_allocated_ -= size;
    stats_.tagged_allocations_[static_cast<u64>(tag)] -= size;

    // TODO: memory alignment
    Platform::free(block, FALSE);
}

void* mg_zero_memory(void* block, u64 size) {
    return Platform::zero_memory(block, size);
}

void* mg_copy_memory(void* dest, const void* source, u64 size) {
    return Platform::copy_memory(dest, source, size);
}

void* mg_set_memory(void* dest, i32 value, u64 size) {
    return Platform::set_memory(dest, value, size);
}

char* mg_get_memory_usage_str() {
    const u64 gib = 1024ull * 1024ull * 1024ull;
    const u64 mib = 1024ull * 1024ull;
    const u64 kib = 1024ull;

    char buffer[8000] = "System memory use (tagged):\n";
    u64 offset = strlen(buffer);
    for (u32 i = 0; i < static_cast<u32>(memory_tag::MEMORY_TAG_MAX_TAGS); i++) {
        char unit[4] = "XiB";
        f32 amount = 1.0f;
        if (stats_.tagged_allocations_[i] >= gib) {
            amount = static_cast<f32>(stats_.tagged_allocations_[i]) / static_cast<f32>(gib);
            unit[0] = 'G';
        } else if (stats_.tagged_allocations_[i] >= mib) {
            amount = static_cast<f32>(stats_.tagged_allocations_[i]) / static_cast<f32>(mib);
            unit[0] = 'M';
        } else if (stats_.tagged_allocations_[i] >= kib) {
            amount = static_cast<f32>(stats_.tagged_allocations_[i]) / static_cast<f32>(kib);
            unit[0] = 'K';
        } else {
            amount = static_cast<f32>(stats_.tagged_allocations_[i]);
            unit[0] = 'B';
            unit[1] = 0;
        }
        
        i32 length = snprintf(buffer + offset, 8000, "  %s: %.2f %s\n", memory_tag_strings_[i], amount, unit);
        offset += length;
    }

    char* out_str = (char*)Platform::allocate(offset + 1, FALSE);
    Platform::copy_memory(out_str, buffer, offset + 1);


    return out_str;
}