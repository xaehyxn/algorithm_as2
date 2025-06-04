#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstring>

// �Ÿ� ��� �Լ�
inline double distance(int city_a, int city_b, double** coordinates) {
    return sqrt((coordinates[city_a][0] - coordinates[city_b][0]) * (coordinates[city_a][0] - coordinates[city_b][0]) +
                (coordinates[city_a][1] - coordinates[city_b][1]) * (coordinates[city_a][1] - coordinates[city_b][1]));
}

// tsp ���� �Ľ�
int read_tsp(const char* filename, double*** coordinates_ptr) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "���� ���� ����: %s\n", filename);
        exit(1);
    }

    char line[256];
    int num_cities = 0;

    // DIMENSION, NODE_COORD_SECTION �б�
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "DIMENSION : %d", &num_cities) == 1) continue;
        if (strstr(line, "NODE_COORD_SECTION")) break;
    }

    double** coordinates = new double*[num_cities];
    for (int i = 0; i < num_cities; ++i) {
        coordinates[i] = new double[2];
        int idx;
        if (fscanf(fp, "%d %lf %lf", &idx, &coordinates[i][0], &coordinates[i][1]) != 3) {
            fprintf(stderr, "tsp ������ ������ �߸��Ǿ����ϴ� (��ǥ �Ľ� ����)\n");
            exit(1);
        }
    }
    fclose(fp);
    *coordinates_ptr = coordinates;
    return num_cities;
}

// ���� ����ü
struct Edge {
    int city_u;
    int city_v;
    double cost;
};

// Heap Sort ����(as1���� ������ ������ ����)
void MaxHeapify(Edge* array, int heapSize, int index) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    int largest = index;
    if (left < heapSize && array[left].cost > array[largest].cost) largest = left;
    if (right < heapSize && array[right].cost > array[largest].cost) largest = right;
    if (largest != index) {
        Edge temp = array[index];
        array[index] = array[largest];
        array[largest] = temp;
        MaxHeapify(array, heapSize, largest);
    }
}
void BuildMaxHeap(Edge* array, int size) {
    for (int i = size / 2 - 1; i >= 0; --i) {
        MaxHeapify(array, size, i);
    }
}
void HeapSort(Edge* array, int size) {
    BuildMaxHeap(array, size);
    for (int i = size - 1; i > 0; --i) {
        Edge temp = array[0];
        array[0] = array[i];
        array[i] = temp;
        MaxHeapify(array, i, 0);
    }
}

// ���� set ã��
int set_find(int* parent, int x) {
    if (parent[x] != x) parent[x] = set_find(parent, parent[x]);
    return parent[x];
}

// set_union : set_size Ȱ�� -> �� ���� �迭�� ū ������ �ٵ��� �ؼ� ������ ��
void set_union(int* parent, int* set_size, int x, int y) {
    x = set_find(parent, x);
    y = set_find(parent, y);
    if (x == y) {
        return; // ���� ���տ� ������ ��� -> ��ġ�� ����
    }
    // �ٸ� ���տ� ���� -> �� ū ���������� ����
    if (set_size[x] < set_size[y]) {
        parent[x] = y; 
        set_size[y] += set_size[x];
    }
    else {
        parent[y] = x; 
        set_size[x] += set_size[y];
    }
}

double kruskal_tsp_approximation(int num_cities, double** coords, int* tour) {
    int num_edges = num_cities * (num_cities - 1) / 2; // ���� ����
    Edge* edges = new Edge[num_edges];
    int edge_index = 0;
    for (int i = 0; i < num_cities; i++) {
        for (int j = i + 1; j < num_cities; j++) {
            edges[edge_index++] = {i, j, distance(i, j, coords)};
        }
    } // ���� ����� -> i�� j�� ������ �����ϴ� �� �����̰�, cost�� �� ���� ���� �Ÿ� ����

    // ���� as1���� �����ߴ� heapsort���� int* -> Edge*�� ������ ���
    HeapSort(edges, num_edges);
    // edge���� ª��������� ���� 

    int* parent = new int[num_cities];
    int* set_size = new int[num_cities];
    int* degree = new int[num_cities];
    for (int i = 0; i < num_cities; i++) {
        parent[i] = i;
        set_size[i] = 1;
        degree[i] = 0;
    }

    Edge* selected_edges = new Edge[num_cities];
    int selected_count = 0;

    // ���� ����, ���� 
    for (int i = 0; i < num_edges && selected_count < num_cities - 1; i++) {
        int u = edges[i].city_u;
        int v = edges[i].city_v;
        if (degree[u] >= 2 || degree[v] >= 2) { // ����� edge���� 0 �Ǵ� 1�϶��� �����ϵ��� ��
            continue;
        }
        if (set_find(parent, u) == set_find(parent, v)) {  // ���� ���տ� ���� ��� ��ġ�� ����
            continue;
        }
        selected_edges[selected_count++] = edges[i]; 
        degree[u]++;
        degree[v]++;
        set_union(parent, set_size, u, v);
    }

    // ������ ���� ����
    int last_city_u = -1;
    int last_city_v = -1;
    for (int i = 0; i < num_cities; i++) { // ��ο��� �� ���� ã��(���� �� �� �� ����)
        if (degree[i] == 1) {
            if (last_city_u == -1) {
                last_city_u = i;
            }
            else {
                last_city_v = i;
            }
        }
    }
    for (int i = 0; i < num_edges; i++) { 
        if ((edges[i].city_u == last_city_u && edges[i].city_v == last_city_v) ||
            (edges[i].city_u == last_city_v && edges[i].city_v == last_city_u)) {
            selected_edges[selected_count++] = edges[i];
            degree[last_city_u]++;
            degree[last_city_v]++;
            break;
        }
    }

    // �ڽ�Ʈ ����ϱ�
    double cost = 0.0;
    for (int i = 0; i < selected_count; i++) {
        cost += selected_edges[i].cost;
    }

    // ���� ����Ʈ(adjacency list)�� ��ȯ
    int** adjacency = new int*[num_cities];
    int* neighbor_count = new int[num_cities];
    for (int i = 0; i < num_cities; i++) {
        adjacency[i] = new int[2];
        neighbor_count[i] = 0;
    }
    for (int i = 0; i < num_cities; i++) {
        int u = selected_edges[i].city_u, v = selected_edges[i].city_v;
        adjacency[u][neighbor_count[u]++] = v;
        adjacency[v][neighbor_count[v]++] = u;
    }

    // ���� �����
    bool* visited = new bool[num_cities];
    for (int i = 0; i < num_cities; i++) visited[i] = false;
    int current = 0, position = 0;
    tour[position++] = current;
    visited[current] = true;
    while (position < num_cities) {
        int next_city = -1;
        for (int j = 0; j < 2; j++) {
            if (!visited[adjacency[current][j]]) {
                next_city = adjacency[current][j];
                break;
            }
        }
        tour[position++] = next_city;
        visited[next_city] = true;
        current = next_city;
    }

    // ���� ������ �ڵ� (LLM(CHAT-GPT) Ȱ��)
    bool* seen = new bool[num_cities];
    for (int i = 0; i < num_cities; i++) {
        seen[i] = false;
    }
    bool error = false;
    for (int i = 0; i < num_cities; i++) {
        if (tour[i] < 0 || tour[i] >= num_cities) {
            printf("Tour index out of range: %d at pos %d\n", tour[i], i);
            error = true;
        }
        else if (seen[tour[i]]) {
            printf("Duplicate city in tour: %d\n", tour[i]);
            error = true;
        }
        seen[tour[i]] = true;
    }
    for (int i = 0; i < num_cities; i++) {
        if (!seen[i]) {
            printf("Missing city in tour: %d\n", i);
            error = true;
        }
    }
    delete[] seen;
    if (error) {
        printf("[ERROR] Tour contains errors!\n");
    }

    delete[] edges;
    delete[] parent;
    delete[] set_size;
    delete[] degree;
    delete[] selected_edges;
    for (int i = 0; i < num_cities; ++i) {
        delete[] adjacency[i];
    }
    delete[] adjacency;
    delete[] neighbor_count;
    delete[] visited;

    return cost;   
}

int main(int argc, char* argv[]) {
    if (argc < 2) return -1;
    double** coords;
    int num_cities = read_tsp(argv[1], &coords);

    int* tour = new int[num_cities];

    double cost = 0.0000;
    double average_time = 0.0000;
    for (int i = 0; i < 5; i++) {
        clock_t start = clock();
        cost = kruskal_tsp_approximation(num_cities, coords, tour);
        clock_t end = clock();
        average_time += (double)(end - start) * 1000 / CLOCKS_PER_SEC;
    }
    average_time = (double)average_time/5.0;

    printf("Number of cities: %d\n", num_cities);
    printf("Approximate tour cost: %.2f\n", cost);
    printf("average time: %.3f ms\n", average_time);

    for (int i = 0; i < num_cities; ++i) {
        delete[] coords[i];
    }
    delete[] coords;
    delete[] tour;
    return 0;
}
