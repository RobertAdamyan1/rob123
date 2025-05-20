#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

std::queue<std::string> buffer;
std::mutex mtx;
std::condition_variable cv;
std::atomic<bool> done(false);

void read_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка при открытии файла!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::lock_guard<std::mutex> lock(mtx);
        buffer.push(line);
        cv.notify_all();
    }

    done = true;
    cv.notify_all();
}

void print_from_buffer() {
    int total_characters = 0;

    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{ return !buffer.empty() || done.load(); });

        if (!buffer.empty()) {
            std::string data = buffer.front();
            buffer.pop();
            lock.unlock();
            std::cout << data << std::endl;
            total_characters += data.size();
        }

        if (done && buffer.empty()) {
            break;
        }
    }

    std::cout << "Total characters printed: " << total_characters << std::endl;
}

int main(void) {
    std::string filename = "example.txt";

    std::thread reader_thread(read_from_file, filename);
    std::thread printer_thread(print_from_buffer);

    reader_thread.join();
    printer_thread.join();

    return 0;
}
