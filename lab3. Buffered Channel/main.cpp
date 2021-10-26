#include "buffered_channel.h"
#include <iostream>
#include <thread>
#include <string>
#include <chrono>


void producer(BufferedChannel<std::string>& channel) {
    std::string arr[] = {"Wow", "look", "it", "really", "works", "!", "!", "!"};
    for (int i = 0; i < 8; i++) {
        try {
            channel.Send(arr[i]);
        } catch (std::exception& e) {
            std::cout << e.what() << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void dangerous_consumer(BufferedChannel<std::string>& channel) {
    for (int i = 0; i < 3; i++) {
        auto recv = channel.Recv();
        if(recv.second) {
            std::cout << recv.first << "\n";
        }else{
            std::cout << "Channel is closed and there's no free elements in it." << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    channel.Close();
}

void consumer(BufferedChannel<std::string>& channel) {
    for(int i = 0; i < 5; ++i) {
        auto recv = channel.Recv();
        if(recv.second) {
            std::cout << recv.first << "\n";
        }else{
            std::cout << "Channel is closed and there's no free elements in it." << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    BufferedChannel<std::string> channel(5);

    std::thread prod_thread(producer, std::ref(channel));
    std::thread dang_cons_thread(dangerous_consumer, std::ref(channel));
    std::thread cons_thread(consumer, std::ref(channel));

    prod_thread.join();
    dang_cons_thread.join();
    cons_thread.join();
}
