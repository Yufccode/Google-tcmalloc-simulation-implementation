

#include "tcmalloc.hpp"
#include <thread>

void alloc1() {
    for (size_t i = 0; i < 5; i++) {
        void* ptr = tcmalloc(6);
        std::cout << "thread id: " << std::this_thread::get_id() << std::endl;
    }
}
void tls_test() {
    std::thread t1(alloc1);
    std::thread t2(alloc1);
    t1.join();
    t2.join();
}

int main() {
    tls_test();
    return 0;
}