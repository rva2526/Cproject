#ifndef READ_PARSE
#define READ_PARSE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#define BUFFER_SIZE 256  // Define a buffer size
#define MAX_VERTEX_NUM 100  // Define a maximum number of vertices
#define PI 3.14159

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


// Define the observed points data structure
struct ObservedMag {
    double east;      // easting of observation point
    double north;     // northing of observation point
    double obs_mag;   // magnetic value (nT) at observation point)
    double calc_mag;
};

char *read_file(const char *filename);
struct Prism *read_json(const char *prism_content, int *num_prisms);
double calculateSignedArea(struct Prism *prism);
void ensureClockwise(struct Prism *prism);
struct ObservedMag *read_observed_data(const char *filename, int *num_obs);

#endif // PREAD_PARSE
