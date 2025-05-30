#include <iostream>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstring>

using namespace std;

// �� ����(x, y)���� �Ÿ� ���ϱ�
double distance(int x1, int y1, int x2, int y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

// tsp ���� �Ľ��ϱ� (LLM Ȱ��) -> dimension(���� ����) ��ȯ
int read_tsp(const char* filename, double*** coords_ptr) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "���� ���� ����: %s\n", filename);
        exit(1);
    }

    char line[256];
    int n = 0;

    // DIMENSION�� NODE_COORD_SECTION���� �б�
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "DIMENSION : %d", &n) == 1) continue;
        if (strstr(line, "NODE_COORD_SECTION")) break;
    }

    double** coords = new double*[n];
    for (int i = 0; i < n; ++i) {
        coords[i] = new double[2];
        int idx;
        fscanf(fp, "%d %lf %lf", &idx, &coords[i][0], &coords[i][1]);
    }

    fclose(fp);
    *coords_ptr = coords;
    return n;
}

//MinHeap ���� -> prim�� Ȱ��
struct HeapNode {
    int vertex; // ���� ��ȣ
    double key; // �ش� ���������� �ּ� �Ÿ� �� (�켱����)
};

class MinHeap {
public:
    int capacity; // �ִ� ũ�� (���� �� == demension)
    int size; // (������� ���� �ִ� ��� ��)
    HeapNode* array; // �� �迭
    int* pos; // ���� ��ȣ -> �� ���� �ε��� ���� -> ��, pos[j] = i�� ��� j��° ������ heap�� i��°�� ��ġ�Ѵٴ� ��.

    MinHeap(int dimension) {
        capacity = dimension;
        size = 0;
        array = new HeapNode[capacity];
        pos = new int[capacity];
        for (int i = 0; i < capacity; i++) {
            pos[i] = -1;
        }
    }

    ~MinHeap() {
        delete[] array;
        delete[] pos;
    }

    // �� �� ��� ��ȯ -> array����, pos ����
    void swap(int i, int j) {
        HeapNode temp = array[i];
        array[i] = array[j];
        array[j] = temp;
        pos[array[i].vertex] = i;
        pos[array[j].vertex] = j;
    }

    void heapify_down(int index) {
        int now = index;
        int left = index * 2 + 1;
        int right = index * 2 + 2;

        if (left < size && array[left].key < array[now].key) {
            now = left;
        }
        if (right < size && array[right].key < array[now].key) {
            now = right;
        }

        if (now != index) {
            swap(now, index);
            heapify_down(now);
        }
    }

    void heapify_up(int index) {
        while(index > 0 && array[index].key < array[(index - 1)/2].key) {
            swap(index, (index-1)/2);
            index = (index-1)/2;
        }
    }

    // ���� ��ȣ i�̸� key ��(��ġ)�� j�� ��带 ���� ������
    void insert(int i, double j) {
        array[size] = {i, j};
        pos[i] = size;
        heapify_up(size);
        size++;
    }

    // ���� ����ִ��� Ȯ��
    bool is_empty() {
        return size == 0;
    }

    // �ּҰ� ���� ���� (���� ���� -> �� �� �������� �� �� ��ü, ���������� heapify_down ����)
    HeapNode out_min() {
        HeapNode min = array[0];
        size--;
        array[0] = array[size];
        pos[array[0].vertex] = 0;
        heapify_down(0);
        pos[min.vertex] = -1;
        return min;
    }

    // ���ο� Ű ���� ������ �� ��
    void change_key(int i, double new_key) {
        int j = pos[i]; // i��° ������ heap������ �ε��� == j
        double old_key = array[j].key;
        array[j].key = new_key;
        if (old_key < new_key) {
            heapify_down(i);
        }
        else {
            heapify_up(i);
        }
    }

    // pos �迭 �ȿ� i(tsp���� �������� ����(���� ��)�� �ε���)�� �ִ��� Ȯ��
    bool in_heap(int i) {
        return pos[i] != -1;
    }
};

// heap��� prim�˰��� -> MST �����ϱ�
#define DOUBLE_MAX 1.7e+308 // LLMȰ�� -> �뷫���� doubleŸ�� �ִ밪 ������

void prim(int n, double** graph, int* parent) {
    double* key = new double[n];
    bool* in_MST = new bool[n];

    for (int i = 0; i < n; i++) {
        key[i] = DOUBLE_MAX;
        in_MST[i] = false;
        parent[i] = -1;
    }

    key[0] = 0; // ù��°(0��°) 
    MinHeap heap(n); // ũ�� n¥�� �� ����� (n == demension)
    
    for (int i = 0; i < n; i++) { // heap�� ��Ʈ�� ������ ��� ����� key���� ���� ��Ƽ�ΰ����� ����
        heap.insert(i, key[i]);
    }

    while (!heap.is_empty()) {
        HeapNode min_node = heap.out_min(); // ��Ʈ ���(Ű ���� ���� ���� ���)�� �����ͼ� MST�� ���Խ�Ű��
        int min_index = min_node.vertex;
        in_MST[min_index] = true;

        for (int i = 0; i < n; i++) {
            if (!in_MST[i] && graph[min_index][i] < key[i]) {
                key[i] = graph[min_index][i];
                parent[i] = min_index; 
                heap.change_key(i, key[i]);
            }
        }
    }

    delete[] key;
    delete[] in_MST;
}

// heap��� prim -> MST ���� -> DFS�� �̿��� ���� ��ȸ -> tour(��ȸ ���) �ϼ�
void DFS(int now_node_index, bool* visited, int* parent, int n, int* tour, int& index) {
    visited[now_node_index] = true; // ���� ��� �湮
    tour[index] = now_node_index;
    index++;

    for (int i = 0; i < n; i++) {
        if (parent[i] == now_node_index && !visited[i]) {
            DFS(i, visited, parent, n, tour, index);
        }
    }
}

// TSP �����( �ڽ�Ʈ) ����ϱ�
double tour_cost(int* tour, double** coords, int n) {
    double cost = 0;
    for (int i = 0; i < n; i++) {
        int a = tour[i];
        int b = tour[(i+1) % n];
        cost += distance(coords[a][0], coords[a][1], coords[b][0], coords[b][1]);
    }
    return cost;
}

// mst ��� 2�ٻ� �˰��� �����ϱ� (�Ÿ��׷��� ���� -> MST ����(prim Ȱ��) -> DFS�� ������ȸ�ϸ� TSP ��� ���� -> �����)
double mst_based_2_approximation(int n, double** coords) {
    // �Ÿ� �׷��� �����ϱ�
    double** graph = new double*[n];
    for (int i = 0; i < n; i++) {
        graph[i] = new double[n];
        for (int j = 0; j < n; j++) {
            graph[i][j] = distance(coords[i][0], coords[i][1], coords[j][0], coords[j][1]);
        }
    }

    // MST �����ϱ�
    int* parent = new int[n];
    prim(n, graph, parent);

    // DFS ������ȸ -> TSP ��� ����
    bool* visited = new bool[n];
    for (int i = 0; i < n; i++) {
        visited[i] = false;
    }
    int* tour = new int[n];
    int index = 0;
    DFS(0, visited, parent, n, tour, index);

    // TSP ��� cost ����ϱ�
    double cost = tour_cost(tour, coords, n);

    // �޸� �����ϱ�
    for (int i = 0; i < n; i++) {
        delete[] graph[i];
    }
    delete[] graph;

    delete[] parent;
    delete[] visited;
    delete[] tour;

    // TSP ��� cost ��ȯ�ϱ�
    return cost;
}