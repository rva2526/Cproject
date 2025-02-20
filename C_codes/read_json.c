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

char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) { 
        printf("Error: Unable to open the file: %s.\n", filename);
        return NULL;
    }

    printf("Reading file...\n");

    char buffer[BUFFER_SIZE];  
    char *content = malloc(1);  
    content[0] = '\0';
    size_t current_length = 0;

    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        size_t buffer_length = strlen(buffer);
        char *new_content = realloc(content, current_length + buffer_length + 1);
        if (new_content == NULL) {
            printf("Memory allocation failed\n");
            free(content);
            fclose(file);
            return NULL;
        }
        content = new_content;
        strcpy(content + current_length, buffer);
        current_length += buffer_length;
    }

    fclose(file);
    return content;
}

// Function to read and parse the JSON
struct Prism* read_json(const char *prism_content, int *num_prisms) {
    cJSON *json = cJSON_Parse(prism_content);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error parsing JSON: %s\n", error_ptr);
        }
        return NULL;
    }

    cJSON *shapes = cJSON_GetObjectItemCaseSensitive(json, "shapes");
    if (!cJSON_IsArray(shapes)) {
        cJSON_Delete(json);
        return NULL;
    }

    int shape_size = cJSON_GetArraySize(shapes);
    struct Prism *prisms = malloc(shape_size * sizeof(struct Prism));  // Dynamically allocate memory for prisms
    if (prisms == NULL) {
        printf("Memory allocation for prisms failed\n");
        cJSON_Delete(json);
        return NULL;
    }

    *num_prisms = shape_size;

    for (int i = 0; i < shape_size; i++) {
        cJSON *shape = cJSON_GetArrayItem(shapes, i);
        struct Prism *current_prism = &prisms[i];  // Access the current Prism struct

        // Extract magnetic properties
        cJSON *inclination = cJSON_GetObjectItemCaseSensitive(shape, "magnetic_inclination");
        if (cJSON_IsNumber(inclination)) {
            current_prism->minc = inclination->valuedouble;
        }

        cJSON *declination = cJSON_GetObjectItemCaseSensitive(shape, "magnetic_declination");
        if (cJSON_IsNumber(declination)) {
            current_prism->mdec = declination->valuedouble;
        }

        cJSON *intensity = cJSON_GetObjectItemCaseSensitive(shape, "magnetic_intensity");
        if (cJSON_IsNumber(intensity)) {
            current_prism->mi = intensity->valuedouble;
        }

        // Extract depth (z1 and z2)
        cJSON *top = cJSON_GetObjectItemCaseSensitive(shape, "top");
        if (cJSON_IsNumber(top)) {
            current_prism->z1 = top->valuedouble;
        }

        cJSON *bottom = cJSON_GetObjectItemCaseSensitive(shape, "bottom");
        if (cJSON_IsNumber(bottom)) {
            current_prism->z2 = bottom->valuedouble;
        }

        // Extract coordinates
        cJSON *x_array = cJSON_GetObjectItemCaseSensitive(shape, "x");
        if (cJSON_IsArray(x_array)) {
            current_prism->num_vertices = cJSON_GetArraySize(x_array);
            for (int j = 0; j < current_prism->num_vertices; j++) {
                cJSON *x_value = cJSON_GetArrayItem(x_array, j);
                if (cJSON_IsNumber(x_value)) {
                    current_prism->vertex[j].east = x_value->valuedouble;
                }
            }
        }

        cJSON *y_array = cJSON_GetObjectItemCaseSensitive(shape, "y");
        if (cJSON_IsArray(y_array)) {
            for (int j = 0; j < current_prism->num_vertices; j++) {
                cJSON *y_value = cJSON_GetArrayItem(y_array, j);
                if (cJSON_IsNumber(y_value)) {
                    current_prism->vertex[j].north = y_value->valuedouble;
                }
            }
        }
    }

    cJSON_Delete(json);
    return prisms;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <prism_json_file>\n", argv[0]);
        return 1;
    }

    char *prismfile_name = argv[1];

    char *prism_content = read_file(prismfile_name);
    if (prism_content != NULL) {
        int num_prisms = 0;
        struct Prism *prisms = read_json(prism_content, &num_prisms);
        
        if (prisms != NULL) {
            printf("Total Prisms Found: %d\n", num_prisms);

            // Access the prisms and print their data
            for (int i = 0; i < num_prisms; i++) {
                struct Prism *prism = &prisms[i];
                printf("\nPrism %d:\n", i + 1);
                printf("Magnetic Inclination: %.2f\n", prism->minc);
                printf("Magnetic Declination: %.2f\n", prism->mdec);
                printf("Magnetic Intensity: %.2f\n", prism->mi);
                printf("Top Depth: %.2f\n", prism->z1);
                printf("Bottom Depth: %.2f\n", prism->z2);
                
                printf("Vertices:\n");
                for (int j = 0; j < prism->num_vertices; j++) {
                    printf("Vertex %d: North = %.2f, East = %.2f\n", j + 1, prism->vertex[j].north, prism->vertex[j].east);
                }
            }

            // Free allocated memory for prisms
            free(prisms);
        } else {
            printf("No prisms found.\n");
        }

        free(prism_content);
    } else {
        printf("Failed to read the file\n");
    }

    return 0;
}