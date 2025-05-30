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
        return pos[i] != -1;
    }
};

// heap기반 prim알고리즘 -> MST 구성하기
#define DOUBLE_MAX 1.7e+308 // LLM활용 -> 대략적인 double타입 최대값 가져옴

void prim(int n, double** graph, int* parent) {
    double* key = new double[n];
    bool* in_MST = new bool[n];

    for (int i = 0; i < n; i++) {
        key[i] = DOUBLE_MAX;
        in_MST[i] = false;
        parent[i] = -1;
    }

    key[0] = 0; // 첫번째(0번째) 
    MinHeap heap(n); // 크기 n짜리 힙 만들기 (n == demension)
    
    for (int i = 0; i < n; i++) { // heap의 루트를 제외한 모든 노드의 key값은 거의 센티널값으로 설정
        heap.insert(i, key[i]);
    }

    while (!heap.is_empty()) {
        HeapNode min_node = heap.out_min(); // 루트 노드(키 값이 가장 작은 노드)를 가져와서 MST에 포함시키기
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

// heap기반 prim -> MST 구성 -> DFS를 이용한 전위 순회 -> tour(순회 경로) 완성
void DFS(int now_node_index, bool* visited, int* parent, int n, int* tour, int& index) {
    visited[now_node_index] = true; // 현재 노드 방문
    tour[index] = now_node_index;
    index++;

    for (int i = 0; i < n; i++) {
        if (parent[i] == now_node_index && !visited[i]) {
            DFS(i, visited, parent, n, tour, index);
        }
    }
}

// TSP 결과값( 코스트) 계산하기
double tour_cost(int* tour, double** coords, int n) {
    double cost = 0;
    for (int i = 0; i < n; i++) {
        int a = tour[i];
        int b = tour[(i+1) % n];
        cost += distance(coords[a][0], coords[a][1], coords[b][0], coords[b][1]);
    }
    return cost;
}

// mst 기반 2근사 알고리즘 구현하기 (거리그래프 생성 -> MST 구성(prim 활용) -> DFS로 전위순회하며 TSP 경로 생성 -> 비용계산)
double mst_based_2_approximation(int n, double** coords) {
    // 거리 그래프 생성하기
    double** graph = new double*[n];
    for (int i = 0; i < n; i++) {
        graph[i] = new double[n];
        for (int j = 0; j < n; j++) {
            graph[i][j] = distance(coords[i][0], coords[i][1], coords[j][0], coords[j][1]);
        }
    }

    // MST 구성하기
    int* parent = new int[n];
    prim(n, graph, parent);

    // DFS 전위순회 -> TSP 경로 생성
    bool* visited = new bool[n];
    for (int i = 0; i < n; i++) {
        visited[i] = false;
    }
    int* tour = new int[n];
    int index = 0;
    DFS(0, visited, parent, n, tour, index);

    // TSP 경로 cost 계산하기
    double cost = tour_cost(tour, coords, n);

    // 메모리 해제하기
    for (int i = 0; i < n; i++) {
        delete[] graph[i];
    }
    delete[] graph;

    delete[] parent;
    delete[] visited;
    delete[] tour;

    // TSP 경로 cost 반환하기
    return cost;
}