/*
Скользящий максимум
Дан массив целых чисел A[0..n), n не превосходит 100 000. Так же задан размер некотрого окна
(последовательно расположенных элементов массива) в этом массиве k, k<=n. Требуется для каждого положения окна
(от 0 и до n-k) вывести значение максимума в окне. Скорость работы O(n log n), память O(n).
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class PriorityQueue {
 public:
    bool empty() const;
    int top() const;
    int pop();
    void push(int value);

 private:
    vector<int> tree;
};

bool PriorityQueue::empty() const {
    return tree.empty();
}

int PriorityQueue::top() const {
    return tree[0];
}

int PriorityQueue::pop() {
    if (tree.size() > 1) {
        // Перстановка последнего элемента на место первого (максимального) и просеивание вниз
        int max = tree[0];
        int last_elem = tree[tree.size() - 1];
        tree[0] = last_elem;
        tree.pop_back();
        int last_elem_index = 0;

        // Просеивание пока элемент меньше хотя бы одного из детей или пока у него есть 2 ребёнка
        while ((2 * last_elem_index + 2 < tree.size())) {
            int left = tree[2 * last_elem_index + 1];
            int right = tree[2 * last_elem_index + 2];

            if (left >= right && last_elem < left) {
                swap(tree[last_elem_index], tree[2*last_elem_index + 1]);
                last_elem_index = 2*last_elem_index + 1;
            } else if (right > left && last_elem < right) {
                swap(tree[last_elem_index], tree[2*last_elem_index + 2]);
                last_elem_index = 2*last_elem_index + 2;
            } else {
                break;
            }
        }

        // Финальноое просеивание, когда у элемента есть только один (левый) ребёнок
        if (2*last_elem_index + 2 == tree.size()) {
            int left = tree[2*last_elem_index + 1];
            if (last_elem < left) {
                swap(tree[last_elem_index], tree[2*last_elem_index + 1]);
            }
        }

        return max;

    } else {
        int max = tree[0];
        tree.pop_back();
        return max;
    }
}

void PriorityQueue::push(int value) {
    // Добавление элемента в конец и просеивание вверх
    tree.push_back(value);
    int new_elem_index = tree.size() - 1;
    int parent = tree[(new_elem_index-1)/2];
    while (parent < value && new_elem_index > 0) {
        swap(tree[new_elem_index], tree[(new_elem_index - 1)/2]);
        new_elem_index = (new_elem_index - 1)/2;
        parent = tree[(new_elem_index-1)/2];
    }
}

void solution (vector<int> &input_data, int k, int n) {
    /*
    Для решения будем использовать 2 кучи.
    В первую кучу добавляем новые элементы входящие в окно и смотрим максимум.
    Вторая куча нужна для того, чтобы правильно организовать удаление элементов из первой кучи, т.к. по мере
    движения окна элементы из неё должны удаляться. В неё мы будем записывать элементы, выходящие из окна.

    Удалять из первой кучи нужно лишь тогда, когда текущий максимум в окне выходит из окна и лишь те элементы,
    которые не меньше нового максимума в окне.
    */
    PriorityQueue queue;
    PriorityQueue max_to_delete;
    // Считывание элементов для первого окна
    for (int i = 0; i < k; i++) {
        queue.push(input_data[i]);
    }
    // Проход окна далее, i - индекс указывающий на последний элемент окна.
    for (int i = k - 1; i < n; i ++) {
        // Вывод максимума в окне
        int new_max = queue.top();
        cout << new_max << " ";
        // Проверка, что окно может сдвинуться ещё раз
        if (i + 1 < n) {
            // Запись выходящего элемента в кучу на удаление
            int elem_to_delete = input_data[i-k+1];
            max_to_delete.push(elem_to_delete);
            // В случае если текущий максимум вышел из окна, удаляем из первой кучи часть вышедших элементов
            while (new_max <= elem_to_delete && !max_to_delete.empty()) {
                queue.pop();
                max_to_delete.pop();
                elem_to_delete = max_to_delete.top();
                new_max = queue.top();
            }
            queue.push(input_data[i+1]);
        }
    }
}

int main() {
    int n;
    cin >> n;
    vector<int> input_data;
    for (int i = 0; i < n; i++) {
        int h;
        cin >> h;
        input_data.push_back(h);
    }
    int k;
    cin >> k;
    solution(input_data, k, n);
    return 0;
}