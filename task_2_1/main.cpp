/*
 * Дана очень длинная последовательность целых чисел длины n. Требуется вывести в отсортированном виде её
 * наименьшие k элементов. Последовательность может не помещаться в память.
 * Время работы O(n * log(k)). Доп. память O(k). Использовать слияние.
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void merge_first_k_elems_with_next_k_elems(vector<int> &first_k_elems, int &elements_passed, int n, int k) {
    /*
     * В процессе считывания новых k элементов последовательности инкрементируем переменную elements_passed
     */
    vector<int> next_k_elems;
    int next;
    while (elements_passed < n && next_k_elems.size() < k) {
        cin >> next;
        next_k_elems.push_back(next);
        elements_passed++;
    }
    sort(next_k_elems.begin(), next_k_elems.end());

    // Копируем старые первые k элементов для последующего заполнения first_k_elems актуальными значениями
    vector<int> old_k_elems = first_k_elems;

    // Выполнянием мердж до заполнения first_k_elems
    auto first_k_elems_it = first_k_elems.begin();
    auto next_k_elems_it = next_k_elems.begin();
    auto old_k_elems_it = old_k_elems.begin();
    int elements_merged = 0;
    while (elements_merged < k) {
        /*
         * Первый if для бработки ситуации, когда при последнем считывании число новых элементов меньше k
         * и возможная ситуация, при которой новые элементы закончатся до конца цикла
        */
        if (next_k_elems_it == next_k_elems.end()) {
            while (elements_merged < k) {
                *first_k_elems_it = *old_k_elems_it;
                first_k_elems_it++;
                old_k_elems_it++;
                elements_merged++;
            }
            break;
        } else if (*old_k_elems_it <= *next_k_elems_it) {
            *first_k_elems_it = *old_k_elems_it;
            first_k_elems_it++;
            old_k_elems_it++;
            elements_merged++;
        } else if (*next_k_elems_it < *old_k_elems_it) {
            *first_k_elems_it = *next_k_elems_it;
            first_k_elems_it++;
            next_k_elems_it++;
            elements_merged++;
        }
    }
}

void solution(int n, int k) {
    // Считываем и сортируем первые k элементов из последовательности
    vector<int> first_k_elems;
    for (int i = 0; i < k; i++) {
        int next;
        cin >> next;
        first_k_elems.push_back(next);
    }
    sort(first_k_elems.begin(), first_k_elems.end());
    // Число уже пройденных элементов
    int elements_passed = k;
    /*
     * Выполняем процедуру слияния текущих первых k элементов со следующими k элементами
     * для получения новых первых k элементов, пока не будут пройдены все элементы последовательности
     */
    while (elements_passed < n)
        merge_first_k_elems_with_next_k_elems(first_k_elems, elements_passed, n, k);

    for (auto elem: first_k_elems) {
        cout << elem << " ";
    }
}

int main() {
    int n, k;
    cin >> n >> k;
    solution(n, k);
    return 0;
}