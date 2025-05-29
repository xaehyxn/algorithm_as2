#include <iostream>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstring>

using namespace std;

// 각 도시(x, y)간의 거리 구하기
double distance(int x1, int y1, int x2, int y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

// tsp 파일 파싱하기 (LLM 활용) -> dimension(정점 개수) 반환
int read_tsp(const char* filename, double*** coords_ptr) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "파일 열기 실패: %s\n", filename);
        exit(1);
    }

    char line[256];
    int n = 0;

    // DIMENSION과 NODE_COORD_SECTION까지 읽기
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

//MinHeap 구현 -> prim에 활용
struct HeapNode {
    int vertex; // 정점 번호
    double key; // 해당 정점까지의 최소 거리 값 (우선순위)
};

class MinHeap {
public:
    int capacity; // 최대 크기 (정점 수 == demension)
    int size; // (현재까지 힙에 있는 노드 수)
    HeapNode* array; // 힙 배열
    int* pos; // 정점 번호 -> 힙 내의 인덱스 매핑 -> 즉, pos[j] = i의 경우 j번째 정점이 heap의 i번째에 위치한다는 것.

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

    // 두 힙 노드 교환 -> array갱신, pos 갱신
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

    // 정점 번호 i이며 key 값(위치)이 j인 노드를 힙에 넣을때
    void insert(int i, double j) {
        array[size] = {i, j};
        pos[i] = size;
        heapify_up(size);
        size++;
    }

    // 힙이 비어있는지 확인
    bool is_empty() {
        return size == 0;
    }

    // 최소값 정점 추출 (맨위 추출 -> 맨 뒤 정점으로 맨 위 대체, 위에서부터 heapify_down 진행)
    HeapNode out_min() {
        HeapNode min = array[0];
        size--;
        array[0] = array[size];
        pos[array[0].vertex] = 0;
        heapify_down(0);
        pos[min.vertex] = -1;
        return min;
    }

    // 새로운 키 값을 설정해 줄 때
    void change_key(int i, double new_key) {
        int j = pos[i]; // i번째 정점의 heap에서의 인덱스 == j
        double old_key = array[j].key;
        array[j].key = new_key;
        if (old_key < new_key) {
            heapify_down(i);
        }
        else {
            heapify_up(i);
        }
    }

    // pos 배열 안에 i(tsp파일 내에서의 정점(도시 등)의 인덱스)가 있는지 확인
    bool in_heap(int i) {

    }
};
