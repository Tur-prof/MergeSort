#include <algorithm>
#include <cassert>
#include <iostream>
#include <future>
#include <numeric>
#include <random>
#include <vector>


/*template <typename It>
void printRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}*/

bool make_thread = false;
constexpr int max_size_to_thread = 10000;


template <typename RandomIt>
void mergeSort(RandomIt range_begin, RandomIt range_end) {
    // 1. Если диапазон содержит меньше 2 элементов, выходим из функции
    int range_length = range_end - range_begin;
    if (range_length < 2) {
        return;
    }

    // 2. Создаём вектор, содержащий все элементы текущего диапазона
    std::vector elements(range_begin, range_end);
    // Тип элементов — typename iterator_traits<RandomIt>::value_type

    // 3. Разбиваем вектор на две равные части
    auto mid = elements.begin() + range_length / 2;

    if (make_thread && range_length > max_size_to_thread) {
        auto future1 = std::async(std::launch::async, [&]() {mergeSort(elements.begin(), mid); });
        auto future2 = std::async(std::launch::async, [&]() {mergeSort(mid, elements.end()); });
        future1.get();
        future2.get();
    }
    else {
        mergeSort(elements.begin(), mid);
        mergeSort(mid, elements.end());
    }
    // 5. С помощью алгоритма merge сливаем отсортированные половины
    // в исходный диапазон
    std::merge(elements.begin(), mid, mid, elements.end(), range_begin);
}

int main() {
    std::mt19937 generator;

    std::vector<int> test_vector(50000);

    // Заполняет диапазон последовательно возрастающими значениями
    iota(test_vector.begin(), test_vector.end(), 1);

    // Перемешивает элементы в случайном порядке
    shuffle(test_vector.begin(), test_vector.end(), generator);

    // Выводим вектор до сортировки
    //printRange(test_vector.begin(), test_vector.end());

    // Сортируем вектор с помощью сортировки слиянием
    mergeSort(test_vector.begin(), test_vector.end());

    // Выводим результат
    //printRange(test_vector.begin(), test_vector.end());

    std::vector<int> copy_vector{ test_vector };

    std::cout << "Sequenced:" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    mergeSort(copy_vector.begin(), copy_vector.end());
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "The time of merge sort: " << std::chrono::duration_cast<std::chrono::milliseconds> (stop - start).count() << " ms" << std::endl;

    std::cout << "Async:" << std::endl;
    make_thread = true;
    start = std::chrono::high_resolution_clock::now();
    mergeSort(test_vector.begin(), test_vector.end());
    stop = std::chrono::high_resolution_clock::now();
    std::cout << "The time of merge sort: " << std::chrono::duration_cast<std::chrono::milliseconds> (stop - start).count() << " ms" << std::endl;

    assert(copy_vector == test_vector);
    std::cout << "Sorted!" << std::endl;

    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported." << std::endl;

    return 0;
}
