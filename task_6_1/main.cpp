/*
 * 11_1. Цикл минимальной длины.
 * Дан невзвешенный неориентированный граф. Найдите цикл минимальной длины.
 */

#include <iostream>
#include <unordered_set>
#include <vector>
#include <queue>

int BFS_min_cycle (std::vector<std::vector<int>> &graph, int v_start) {
    // BFS для графа из точки v_start, возвращающей длину первого найденного цикла, либо INT_32MAX, если циклов нет

    // Очередь для обработки
    std::queue<int> queue;
    queue.push(v_start);

    // Вектор с меткой об обработке
    std::vector<bool> used(graph.size());

    used[v_start] = true;
    // Вектор предков (-1, если нет)
    std::vector<int> predecessor(graph.size());
    predecessor[v_start] = -1;

    // Вектор расстояний до начальной точки
    std::vector<int> dist(graph.size());
    dist[v_start] = 0;

    // Номера вершин, на которых замкнулся цикл, если он был найден
    int v_cycle_1 = -1;
    int v_cycle_2 = -1;

    bool cycle_found = false;

    // BFS до нахождения первого цикла
    while (!queue.empty() && !cycle_found) {
        int v = queue.front();
        queue.pop();
        for (auto it = graph[v].begin(); it != graph[v].end(); it++) {
            int u = *it;
            if (!used[u]) {
                queue.push(u);
                used[u] = true;
                predecessor[u] = v;
                dist[u] = dist[v] + 1;
            } else if (u != predecessor[v]) {
                v_cycle_1 = v;
                v_cycle_2 = u;
                cycle_found = true;
            }
        }
    }

    // Определение длины цикла, если он есть
    if (v_cycle_1 != -1 && v_cycle_2 != -1) {
        // Путь от первой вершины, на которой замкнулся цикл до начала
        std::vector<int> path_v_1;
        int start_v_cycle_1 = v_cycle_1;
        int start_v_cycle_2 = v_cycle_2;
        while (v_cycle_1 != -1) {
            path_v_1.push_back(v_cycle_1);
            v_cycle_1 = predecessor[v_cycle_1];
        }

        // Путь от второй вершины, на которой замкнулся цикл до начала
        std::vector<int> path_v_2;
        while (v_cycle_2 != -1) {
            path_v_2.push_back(v_cycle_2);
            v_cycle_2 = predecessor[v_cycle_2];
        }

        auto it_1 = path_v_1.begin();
        auto it_2 = path_v_2.begin();
        int cycle_length = 0;

        // Выравнивание путей, если один из них больше (может быть больше на 1 вершину)
        if (dist[start_v_cycle_1] > dist[start_v_cycle_2]) {
            it_1++;
            cycle_length++;
        }
        if (dist[start_v_cycle_2] > dist[start_v_cycle_1]) {
            it_2++;
            cycle_length++;
        }

        // Прохождение по путям до встречи первой общей вершины
        while (*it_1 != *it_2) {
            it_1++;
            it_2++;
            cycle_length += 2;
        }
        // Учёт ребра между замыкающими вершинами
        cycle_length += 1;
        return cycle_length;
    } else {
        return INT32_MAX;
    }
}

void solution (std::vector<std::vector<int>> &graph) {
    // Функция, решающая задачу для данного графа

    /*
     * Для каждой вершины запускаем BFS до нахожденяи первого цикла, если он есть (BFS_min_cycle)
     * и поддерживаем минимум из них
    */
    int min_cycle_length = INT32_MAX;
    for (int i = 0; i < graph.size(); i++) {
        int cycle_legth = BFS_min_cycle(graph, i);
        if (cycle_legth < min_cycle_length) {
            min_cycle_length = cycle_legth;
        }
    }
    if (min_cycle_length != INT32_MAX)
        std::cout << min_cycle_length;
    else
        std::cout << -1;
}

int main() {
    // Считывание вызовв решающей функции
    int N, M;
    std::cin >> N;
    std::cin >> M;
    std::vector<std::vector<int>> graph(N);
    int s;
    int w;
    for (int i = 0; i < M; i++) {
        std::cin >> s;
        std::cin >> w;
        graph[s].push_back(w);
        graph[w].push_back(s);
    }
    solution(graph);
    return 0;
}