#include <iostream>
#include <thread>

int main() {
    unsigned int num_threads = std::thread::hardware_concurrency();
    
    // Fallback in case it returns 0
    if (num_threads == 0) {
        num_threads = 2; // Assume at least 2 threads are supported
    }
    
    std::cout << "This machine supports " << num_threads << " concurrent threads.\n";
    return 0;
}