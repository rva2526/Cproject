#ifndef READ_PARSE
#define READ_PARSE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#define BUFFER_SIZE 256  // Define a buffer size
#define MAX_VERTEX_NUM 100  // Define a maximum number of vertices

// Define the Prism data structure
struct Prism {
    int num_vertices;   // Number of vertices for this prism
    struct Vertex {     // The coordinates of each vertex in the prism
        double north;
        double east;
    } vertex[MAX_VERTEX_NUM];
    double z1;         // Depth to the top of the prism
    double z2;         // Depth to the base of the prism
    double mi;         // Intensity of the magnetization vector
    double minc;       // Inclination of magnetization
    double mdec;       // Declination of magnetization
};

char *read_file(const char *filename);
struct Prism* read_json(const char *prism_content, int *num_prisms);
double calculateSignedArea(struct Prism *prism);
void ensureClockwise(struct Prism *prism);

#endif // PREAD_PARSE
