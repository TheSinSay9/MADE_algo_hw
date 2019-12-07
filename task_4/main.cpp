/*
Реализуйте структуру данных типа “множество строк” на основе динамической хеш-таблицы с открытой адресацией. Хранимые
строки непустые и состоят из строчных латинских букв. Хеш-функция строки должна быть реализована с помощью вычисления
значения многочлена методом Горнера. Начальный размер таблицы должен быть равным 8-ми. Перехеширование выполняйте при
добавлении элементов в случае, когда коэффициент заполнения таблицы достигает 3/4. Структура данных должна поддерживать
операции добавления строки в множество, удаления строки из множества и проверки принадлежности данной строки множеству.
 */

#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

class HashTable {
 public:
    explicit HashTable(size_t initial_size);

    bool Has(const std::string& key) const;
    bool Add(const std::string& key);
    bool Remove(const std::string& key);
    int Hash(const std::string& key) const;
    void ReHash();

 private:
    // Стракт для хранения поля со строкой и меткой об удалении. Если key содержит "" то это поле считается пустым.
    struct HashTableNode {
        std::string key;
        bool deleted;
        HashTableNode(std::string key_, bool deleted_) : key(std::move(key_)), deleted(deleted_) {}
    };
    std::vector<HashTableNode> table;
    // Число элементов для своевременного расширения таблицы.
    int elements_count;
    // Переменная для определения находится ли таблица в состоянии перехеширования.
    bool rehashing;
};

HashTable::HashTable(size_t initial_size) : table(initial_size, {"", false}), elements_count(0), rehashing(false) {}

bool HashTable::Has(const std::string& key) const {
    // Метод для определения есть ли данный элемент в таблице
    assert(!key.empty());

    // Расчёт хеша и квадратичное пробирование до тех пор, пока не найдётся пустая строка либо неудалённая строка с key
    int hash = Hash(key);
    int mod = table.size();
    int i = 1;
    while (table[hash].key != "" && (table[hash].key != key || table[hash].deleted)) {
        hash = (hash + i)%mod;
        i++;
    }

    return table[hash].key == key;
}

bool HashTable::Add(const std::string& key) {
    // Метод для добавления новых строк в таблицу

    assert(!key.empty());

    // Проверка элемента на наличие
    if (Has(key))
        return false;

    // При заполнении 3/4 выполнение перехеширования
    if (!rehashing && elements_count >= 3*(table.size() / 4)) {
        ReHash();
    }

    // Квдаратичное пробирование до тех пор, пока не найдётся пустая строка, либо удалённая строка
    int hash = Hash(key);
    int mod = table.size();
    int i = 1;
    while (table[hash].key != "" && !table[hash].deleted) {
        hash = (hash + (i)%mod)%mod;
        i++;
    }

    // Запись новой строки в таблицу
    table[hash].deleted = false;
    table[hash].key = key;
    elements_count++;

    return  true;
}

bool HashTable::Remove(const std::string& key) {
    // Метод для удаления элемента из таблицы

    assert(!key.empty());

    // Квадратичное пробирования до тех пор, пока не найдётся пустая строка либо неудалённая строка с key
    int hash = Hash(key);
    int mod = table.size();
    int i = 1;
    while (table[hash].key != "" && (table[hash].key != key || table[hash].deleted)) {
        hash = (hash + (i)%mod)%mod;
        i++;
    }

    if (table[hash].key != "") {
        table[hash].deleted = true;
        table[hash].key = " ";
        return true;
    } else {
        return false;
    }
}

int HashTable::Hash(const std::string &key) const {
    // Вычисление хэш-функции от строки методом Горнера

    int a = 3;
    int mod = table.size();
    int hash = key[0]%mod;
    for (int i = 1; i < key.size(); i++) {
        hash = (hash*a + key[i])%mod;
    }
    return hash;
}

void HashTable::ReHash() {
    // Метод для перехеширования таблицы

    // Создание нового тела таблицы
    std::vector<HashTableNode> new_table(table.size()*2, {"", true});
    // Создание буфера для старого тела таблицы
    std::vector<HashTableNode> old_table = std::move(table);
    table = std::move(new_table);
    elements_count = 0;
    // Добавление в новое тело таблицы непустых и неудалённых строк
    rehashing = true;
    for (auto elem : old_table) {
        if (!elem.deleted && elem.key != "") {
            Add(elem.key);
        }
    }
    rehashing = false;
}

int main() {
    HashTable table(8);
    char command = ' ';
    std::string value;
    while (std::cin >> command >> value) {
        switch (command) {
            case '?':
                std::cout << (table.Has(value) ? "OK" : "FAIL") << std::endl;
                break;
            case '+':
                std::cout << (table.Add(value) ? "OK" : "FAIL") << std::endl;
                break;
            case '-':
                std::cout << (table.Remove(value) ? "OK" : "FAIL") << std::endl;
                break;
        }
    }
    return 0;
}
