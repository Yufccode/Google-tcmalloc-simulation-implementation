

#ifndef __YUFC_THREAD_CACHE_HPP__
#define __YUFC_THREAD_CACHE_HPP__

#include "./common.hpp"

class thread_cache {
private:
    free_list __free_lists[BUCKETS_NUM]; // 哈希表

public:
    void* allocate(size_t size);
    void deallocate(void* ptr, size_t size);

public:
    // 向centralCache获取内存
    void* fetch_from_central_cache(size_t index, size_t size);
    // 释放对象，链表过长的时候，回收内存到centralCache
    void list_too_long(free_list& list, size_t size);
};

static __thread thread_cache* p_tls_thread_cache = nullptr;

#endif