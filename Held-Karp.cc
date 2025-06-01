#include <iostream>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstring>

using namespace std;

// �� ����(x, y)���� �Ÿ� ���ϱ� -> 100K���� killed �� -> �����ϱ�(graph��� x)
// double distance(int x1, int y1, int x2, int y2) {
//     return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
// }
inline double distance(int a, int b, double** coords) {
    return sqrt((coords[a][0] - coords[b][0]) * (coords[a][0] - coords[b][0]) + 
                (coords[a][1] - coords[b][1]) * (coords[a][1] - coords[b][1]));
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
        if (fscanf(fp, "%d %lf %lf", &idx, &coords[i][0], &coords[i][1]) != 3) {
            fprintf(stderr, "tsp ������ ������ �߸��Ǿ����ϴ� (��ǥ �Ľ� ����)\n");
            exit(1);
        }
    }

    fclose(fp);
    *coords_ptr = coords;
    return n;
}

// Held-Karp �˰��� �����ϱ�
double held_karp(int n, double** coords) {
    int total = 1 << n; // total -> 2^n ��, ������ �湮 ���� ��
    double** dp = new double*[total]; 
    // dp[�湮 ����][������ �湮 ����]�� ����, ���� �Ǵ� ���� �ش� ������ ��� �湮�ϸ鼭 ������ �湮 ���ÿ� �������� ���� �ּ� ��� ����
    for (int i = 0; i < total; ++i) {
        dp[i] = new double[n];
        for (int j = 0; j < n; ++j)
            dp[i][j] = 1e9;  // ó���� ��� ����� ���Ѵ�(ū ��)�� �����صΰ�, ���� �񱳸� ���� �ٿ����� ����
    }



    
}

// �׽�Ʈ�� ���� �Լ�
int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "����: %s ���ϸ�.tsp\n", argv[0]);
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