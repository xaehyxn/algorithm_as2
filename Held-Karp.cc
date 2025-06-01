#include <iostream>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstring>

using namespace std;

// 각 도시(x, y)간의 거리 구하기 -> 100K에서 killed 뜸 -> 개선하기(graph사용 x)
// double distance(int x1, int y1, int x2, int y2) {
//     return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
// }
inline double distance(int a, int b, double** coords) {
    return sqrt((coords[a][0] - coords[b][0]) * (coords[a][0] - coords[b][0]) + 
                (coords[a][1] - coords[b][1]) * (coords[a][1] - coords[b][1]));
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

// 테스트용 메인 함수
int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "사용법: %s 파일명.tsp\n", argv[0]);
        return 1;
    }

    double** coords;
    int n = read_tsp(argv[1], &coords);

    clock_t start = clock();
    double cost = held_karp(n, coords);
    clock_t end = clock();

    printf("Number of cities: %d\n", n);
    printf("Optimal tour cost: %.2f\n", cost);
    printf("Execution time: %.2f ms\n", (double)(end - start) * 1000 / CLOCKS_PER_SEC);

    for (int i = 0; i < n; ++i)
        delete[] coords[i];
    delete[] coords;

    return 0;
}