/*
 * «MST». Дан неориентированный связный граф. Требуется найти вес минимального остовного дерева в этом графе.
 * С помощью алгоритма Крускала.
 */

#include <iostream>
#include <vector>
#include <algorithm>

class DSU {
public:
    explicit DSU(size_t size);

    int find(int element);
    bool merge(int left, int right);

private:
    std::vector<int> parent;
    std::vector<int> rank;
};

DSU::DSU(size_t size) :
        parent(size, -1),
        rank(size, 1) {}

int DSU::find(int element) {
    if (parent[element] == -1) {
        return element;
    }
    return parent[element] = find(parent[element]);
}

bool DSU::merge(int left, int right) {
    // Изменён метод. Теперь возвращает true, если удалось соединить компоненты и flase - в противном случае.
    const int deputat_left = find(left);
    const int deputat_right = find(right);
    if (deputat_left != deputat_right) {
        if (rank[deputat_left] == rank[deputat_right]) {
            parent[deputat_right] = deputat_left;
            ++rank[deputat_left];
        } else if (rank[deputat_left] > rank[deputat_right]) {
            parent[deputat_right] = deputat_left;
        } else {
            parent[deputat_left] = deputat_right;
        }
        return true;
    }
    return false;
}


struct Edge {
    int from;
    int to;
    int weight;
};

bool CompareEdges(Edge &a, Edge &b) {
    return a.weight < b.weight;
}

void solution(std::vector<Edge> edges, int N) {
    // Алгоритм Крускала
    std::sort(edges.begin(), edges.end(), CompareEdges);

    int cum_weight = 0;
    DSU dsu(N);
    for (auto edge: edges) {
        if (dsu.merge(edge.from, edge.to))
            cum_weight += edge.weight;
    }

    std::cout << cum_weight;
}

int main() {
    int N, M;
    std::cin >> N;
    std::cin >> M;
    std::vector<Edge> edges;
    int from;
    int to;
    int weight;
    for (int i = 0; i < M; i++) {
        std::cin >> from;
        std::cin >> to;
        std::cin >> weight;
        edges.push_back(Edge{from - 1, to - 1, weight});
    }

    solution(edges, N);

    return 0;
}

