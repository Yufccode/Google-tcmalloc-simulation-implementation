

#include "../include/thread_cache.hpp"
#include "../include/central_cache.hpp"
#include "../include/log.hpp"

void* thread_cache::allocate(size_t size) {
    assert(size <= MAX_BYTES);
    size_t align_size = size_class::round_up(size);
    size_t bucket_index = size_class::bucket_index(size);
    if (!__free_lists[bucket_index].empty()) {
        return __free_lists[bucket_index].pop();
    } else {
        // 这个桶下面没有内存了！找centralCache找
        LOG(DEBUG) << "thread_cache::allocate call thread_cache::fetch_from_central_cache" << std::endl;
        return fetch_from_central_cache(bucket_index, align_size);
    }
}

void thread_cache::deallocate(void* ptr, size_t size) {
    assert(ptr);
    assert(size <= MAX_BYTES);
    size_t index = size_class::bucket_index(size);
    __free_lists[index].push(ptr);
    // ... 如果太长了，还给centralCache
}

void* thread_cache::fetch_from_central_cache(size_t index, size_t size) {
    // 慢开始反馈调节算法
    size_t batch_num = std::min(__free_lists[index].max_size(), size_class::num_move_size(size));
    if (__free_lists[index].max_size() == batch_num)
        __free_lists[index].max_size() += 1; // 最多增长到512了
    // 1. 最开始一次向centralCache要太多，因为太多了可能用不完
    // 2. 如果你一直有这个桶size大小的内存，那么后面我可以给你越来越多，直到上限(size_class::num_move_size(size))
    //      这个上限是根据这个桶的内存块大小size来决定的
    // 3. size越大，一次向centralcache要的就越小，如果size越小，相反。
    // 开始获取内存了
    void* start = nullptr;
    void* end = nullptr;
    LOG(DEBUG) << "thread_cache::fetch_from_central_cache call  central_cache::get_instance()->fetch_range_obj()" <<  std::endl;
    size_t actual_n = central_cache::get_instance()->fetch_range_obj(start, end, batch_num, size);
    LOG(DEBUG) << "actual_n" << ":" <<actual_n << std::endl;
    assert(actual_n >= 1);
    if (actual_n == 1) {
        assert(start == end);
        return start;
    } else {
        __free_lists[index].push(free_list::__next_obj(start), end);
        return start;
    }

    return nullptr;
}