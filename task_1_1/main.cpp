/*
Реализовать очередь с динамическим зацикленным буфером.
Обрабатывать команды push back и pop front.
*/

#include <iostream>

using namespace std;

class Queue {
 public:
    Queue();
    void push_back(int value);
    int pop_front();

 private:
    int capacity;
    int *body;
    // head - индекс указывающий куда добавлять элемент
    int head;
    // tail - индекс указывающий откуда брать элемент
    int tail;
    int size;
    void body_expansion();
};

Queue::Queue() {
    capacity = 4;
    body = new int[capacity];
    head = 0;
    tail = 0;
    size = 0;
}

int Queue::pop_front() {
    if (size > 0) {
        int value = body[tail];
        // Сдвиг "указателя" вправо на единицу с учётом зацикленности
        tail = (tail + 1) % capacity;
        size--;
        return value;
    } else {
        return -1;
    }
}

void Queue::push_back(int value) {
    // Если при добавлении получающийся размер превышает ёмкость, то переносим элементы в новый больший массив
    if (size + 1 > capacity)
        body_expansion();
    body[head] = value;
    // Сдвиг "указателя" вправо на единицу с учётом зацикленности
    head = (head + 1) % capacity;
    size++;
};

// Перенос элементов в новый массив в 2 раза большего размера
void Queue::body_expansion() {
    int new_capasity = capacity*2;
    int *new_body = new int[new_capasity];

    for (int i = 0; i < capacity; i++) {
        int index_in_body = (tail + i) % capacity;
        new_body[i] = body[index_in_body];
    }

    delete []body;

    // В новом массиве заполнена левая половина и хвост очереди в начале, а голова в середине
    head = capacity;
    tail = 0;
    capacity = new_capasity;
    body = new_body;
};

int main() {
    int n;
    cin >> n;
    Queue queue;

    bool done = false;
    for (int i = 0; i < n; i++) {
        int k;
        int value;
        cin >> k >> value;
        if (k == 3) {
            queue.push_back(value);
        } else if (k == 2) {
            int value_from_queue = queue.pop_front();
            if (value_from_queue != value) {
                done = true;
                cout << "NO";
                break;
            }
        }
    }
    if (!done) {
        cout << "YES";
    }

    return 0;
}
