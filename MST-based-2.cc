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

    }

    void heapify_up(int index) {

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

    }
};
