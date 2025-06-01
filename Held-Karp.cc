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
        if (fscanf(fp, "%d %lf %lf", &idx, &coords[i][0], &coords[i][1]) != 3) {
            fprintf(stderr, "tsp 파일의 포맷이 잘못되었습니다 (좌표 파싱 실패)\n");
            exit(1);
        }
    }

    fclose(fp);
    *coords_ptr = coords;
    return n;
}

// Held-Karp 알고리즘 구현하기
double held_karp(int n, double** coords) {
    int total = 1 << n; // total -> 2^n 즉, 가능한 방문 조합 수
    double** dp = new double*[total]; 
    // dp[방문 조합][마지막 방문 도시]의 형태, 저장 되는 값은 해당 조합을 모두 방문하면서 마지막 방문 도시에 도착했을 때의 최소 비용 저장
    for (int i = 0; i < total; ++i) {
        dp[i] = new double[n];
        for (int j = 0; j < n; ++j)
            dp[i][j] = 1e9;  // 처음에 모든 비용을 무한대(큰 값)로 저장해두고, 이후 비교를 통해 줄여나갈 예정
    }



    
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