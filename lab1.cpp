#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

class Monitor {
private:
    bool eventReady;                         
    std::mutex mtx;                          
    std::condition_variable condition;

public:
    Monitor() : eventReady(false) {}

    void produceEvent() {
        std::unique_lock<std::mutex> lock(mtx); 
        while (eventReady) {
            condition.wait(lock);
        }

        // Генерация события
        std::cout << "Producer: event created." << std::endl;
        eventReady = true;
        condition.notify_one();                // Уведомляем потребителя
    }

    void consumeEvent() {
        std::unique_lock<std::mutex> lock(mtx); // Захват мьютекса
        while (!eventReady) {
            condition.wait(lock);              // Ждем, пока появится событие
        }

        // Обработка события
        std::cout << "Producer: event " << std::endl;
        eventReady = false;
        condition.notify_one();                // Уведомляем производителя
    }
};

void producer(Monitor& monitor) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Задержка в 1 секунду
        monitor.produceEvent();
    }
}

void consumer(Monitor& monitor) {
    while (true) {
        monitor.consumeEvent();
    }
}

int main() {
    Monitor monitor;

    // Создаем потоки для производителя и потребителя
    std::thread producerThread(producer, std::ref(monitor));
    std::thread consumerThread(consumer, std::ref(monitor));

    // Ожидаем завершения потоков (в данном случае бесконечно)
    producerThread.join();
    consumerThread.join();

    return 0;
}
