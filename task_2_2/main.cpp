#include <iostream>
#include <vector>

using namespace std;

vector<int>::iterator median (vector<int>::iterator left, vector<int>::iterator middle, vector<int>::iterator right) {
    // Определение какой из итераторов указывает на медиану из трёх элементов

    if ((*left <= *middle && *left >= *right) || (*left <= *right && *left >= *middle))
        return left;
    else if ((*middle <= *left && *middle >= *right) || (*middle <= *right && *middle >= *left))
        return middle;
    else
        return right;
}

vector<int>::iterator partition (vector<int>::iterator start, vector<int>::iterator end) {
    /*
     * Операция partition релазиованная с помощью прохода с конца в начало и с использованием медианы трёх
     * как опорного элемента.
     */

    // Итераторы для прохождения с конца в начало
    auto j = end - 1;
    auto i = end - 1;

    // Итераторы для функции определения меиданы трёх
    int size = end - start;
    auto left = start;
    auto right = end - 1;
    auto middle = start + size/2;

    // Определение опорного элемента и его перемещение в начало рассматриваемого участка вектора
    auto pivot_it = median(left, middle, right);
    int pivot = *pivot_it;
    swap(*left, *pivot_it);

    /*
     * Проход с конца в начало. В процессе прохода осуществляются свопы таким образом, чтобы после элемента, на который
     * указывает итератор i, все элементы были не меньше опорного. Все элементы меньше опорного будут в отрезке
     * от start и до i
     */
    while (j != start) {
        if (*j >= pivot) {
            swap(*j, *i);
            j--;
            i--;
        } else {
            j--;
        }
    }
    swap(*start, *i);
    return i;
}

int solution(vector<int> &arr, int k) {
    auto start = arr.begin();
    auto end = arr.end();
    vector<int>::iterator pivot_it;
    int pivot_index;
    // В цикле делаем операцию partition до тех пор, пока индекс пивота после завершения операции не станет равным K
    do {
        pivot_it = partition(start, end);
        pivot_index = pivot_it - arr.begin();
        // Выбор правой или левой половины для следующего partition
        if (pivot_index > k) {
            end = pivot_it;
        } else if (pivot_index < k) {
            start = pivot_it + 1;
        }
    } while (pivot_index != k);
    return *pivot_it;
}

int main() {
    int n, k;
    cin >> n >> k;
    vector<int> arr;
    int h;
    for (int i = 0; i < n; i++) {
        cin >> h;
        arr.push_back(h);
    }
    int answer = solution(arr, k);
    cout << answer;
}