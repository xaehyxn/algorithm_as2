#include <iostream>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstring>

using namespace std;

// tsp 파일 파싱하기 (LLM 활용) 
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