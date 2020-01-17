/*
 * Поиск подстроки.
 * Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки ­– n. Время O(n + p), доп. память – O(p).
 * Вариант 1. С помощью префикс-функции (алгоритм Кнута-Морриса-Пратта).
 */

#include <iostream>
#include <vector>
#include <string>

std::vector<int> prefix_function (std::string s) {
    std::vector<int> prefix;
    prefix.push_back(0);
    for (int i = 1; i < s.length(); i++) {
        int j = prefix[i-1];
        while (s[i] != s[j] && j > 0) {
            j = prefix[j-1];
        }
        if (s[i] == s[j])
            prefix.push_back(j+1);
        else
            prefix.push_back(0);
    }
    return prefix;
}

void find_substrs(std::string shablon, std::string text) {
    shablon += "#";
    auto prefix = prefix_function(shablon);

    // Считываем посимвольно текст и поддерживаем значение префикс функции в переменной j
    int i = shablon.length();
    int j = prefix[shablon.length() - 1];
    for (auto symbol: text) {
        while (symbol != shablon[j] && j > 0) {
            j = prefix[j-1];
        }
        symbol == shablon[j] ? j++ : 0;

        if (j == shablon.length() - 1)
            std::cout << i - 2*(shablon.length() - 1) << " ";
        i++;
    }
}

int main() {
    std::string shablon;
    std::string text;
    std::cin >> shablon;
    std::cin >> text;
    find_substrs(shablon, text);
    return 0;
}