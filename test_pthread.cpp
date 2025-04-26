#include <iostream>
#include <pthread.h>

void* say_hello(void* arg) {
    std::cout << "Hello from thread!" << std::endl;
    return nullptr;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, nullptr, say_hello, nullptr);
    pthread_join(thread, nullptr);

    std::cout << "Main thread done." << std::endl;
    return 0;
}
