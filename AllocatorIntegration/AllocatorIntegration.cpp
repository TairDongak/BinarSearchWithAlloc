#include <iostream>
#include <vector>
#include <chrono>
#include "Allocator.h"
#include <memory>
#include <cmath> 

template <typename T>
class MyAllocator : public std::allocator<T> {
private:
    static constexpr size_t ELEMENTS_PER_BLOCK = 100;
    static constexpr size_t BLOCK_SIZE = sizeof(T) * ELEMENTS_PER_BLOCK;
    Allocator allocatorHeapPool{ BLOCK_SIZE, 10000, nullptr, "MyAllocator" };

public:
    using value_type = T;

    MyAllocator() = default;

    template <class U>
    MyAllocator(const MyAllocator<U>&) noexcept {} // Конструктор копирования теперь пустой, т.к. аллокатор общий

    T* allocate(std::size_t n) {
        if (n == 0) return nullptr;

        size_t numBlocks = static_cast<size_t>(std::ceil(static_cast<double>(n * sizeof(T)) / BLOCK_SIZE));
        void* ptr = allocatorHeapPool.Allocate(numBlocks * BLOCK_SIZE);

        if (!ptr) {
            throw std::bad_alloc();
        }

        return static_cast<T*>(ptr);
    }

    void deallocate(T* p, std::size_t n) noexcept {
        if (p == nullptr) return;
        size_t numBlocks = static_cast<size_t>(std::ceil(static_cast<double>(n * sizeof(T)) / BLOCK_SIZE));
        allocatorHeapPool.Deallocate(p, numBlocks * BLOCK_SIZE);
    }
};

int binarySearch(const std::vector<int, MyAllocator<int>>& arr, int target) {
    int left = 0;
    int right = arr.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] == target) {
            return mid;
        }
        else if (arr[mid] < target) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    return -1;
}

int main() {
    // Создаем вектор с нашим аллокатором
    std::vector<int, MyAllocator<int>> data;
    data.reserve(1000000); // Резервируем место заранее (улучшение производительности), чтобы избежать переаллокаций
    for (int i = 0; i < 1000000; ++i) { // Заполняем вектор числами от 0 до 999999
        data.push_back(i);
    }
    int target = 543210; // Искомое число

    auto start = std::chrono::high_resolution_clock::now(); // Засекаем время начала
    int index = binarySearch(data, target); // Выполняем бинарный поиск
    auto end = std::chrono::high_resolution_clock::now(); // Засекаем время конца
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); // Вычисляем разницу во времени

    if (index != -1) { // Если элемент найден
        std::cout << "Element " << target << " found at index " << index << std::endl; // Выводим индекс
    }
    else { // Если элемент не найден
        std::cout << "Element " << target << " not found" << std::endl; // Выводим сообщение, что не найден
    }
    std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl; // Выводим затраченное время

    return 0; // Успешное завершение программы
}