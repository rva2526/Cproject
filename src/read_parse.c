#include "read_parse.h"

// Function to read a file's contents into a string
char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open the file: %s.\n", filename);
        return NULL;
    }

    fprintf(stderr, "Reading file...%s\n", filename);

    char buffer[BUFFER_SIZE];
    char *content = malloc(1);
    if (content == NULL) {
        fprintf(stderr, "Memory allocation failed for file content.\n");
        fclose(file);
        return NULL;
    }
    content[0] = '\0';
    size_t current_length = 0;

    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        size_t buffer_length = strlen(buffer);
        char *new_content = realloc(content, current_length + buffer_length + 1);
        if (new_content == NULL) {
            fprintf(stderr, "Memory allocation failed during file read.\n");
            free(content);  // Free previously allocated memory
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
            fprintf(stderr, "Error parsing JSON: %s\n", error_ptr);
        }
        return NULL;
    }

    cJSON *shapes = cJSON_GetObjectItemCaseSensitive(json, "shapes");
    if (!cJSON_IsArray(shapes)) {
        fprintf(stderr, "Error: No shapes array found in the JSON.\n");
        cJSON_Delete(json);
        return NULL;
    }

    int shape_size = cJSON_GetArraySize(shapes);
    struct Prism *prisms = malloc(shape_size * sizeof(struct Prism));  
    if (prisms == NULL) {
        fprintf(stderr, "Memory allocation for prisms failed.\n");
        cJSON_Delete(json);
        return NULL;
    }

    *num_prisms = shape_size;
    double tmp_inc = 0.0, tmp_dec = 0.0;

    for (int i = 0; i < shape_size; i++) {
        cJSON *shape = cJSON_GetArrayItem(shapes, i);
        struct Prism *current_prism = &prisms[i];  

        cJSON *inclination = cJSON_GetObjectItemCaseSensitive(shape, "magnetic_inclination");
        if (cJSON_IsNumber(inclination)) {
            tmp_inc = inclination->valuedouble;
            current_prism->minc = tmp_inc * (PI / 180);
        }

        cJSON *declination = cJSON_GetObjectItemCaseSensitive(shape, "magnetic_declination");
        if (cJSON_IsNumber(declination)) {
            tmp_dec = declination->valuedouble;
            current_prism->mdec = tmp_dec * (PI / 180);
        }

        cJSON *intensity = cJSON_GetObjectItemCaseSensitive(shape, "magnetic_intensity");
        if (cJSON_IsNumber(intensity)) {
            current_prism->mi = intensity->valuedouble;
        }

        cJSON *top = cJSON_GetObjectItemCaseSensitive(shape, "top");
        if (cJSON_IsNumber(top)) {
            current_prism->z1 = top->valuedouble;
        }

        cJSON *bottom = cJSON_GetObjectItemCaseSensitive(shape, "bottom");
        if (cJSON_IsNumber(bottom)) {
            current_prism->z2 = bottom->valuedouble;
        }

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

// Function to calculate the signed area of a prism (for ensuring clockwise order)
double calculateSignedArea(struct Prism *prism) {
    double area = 0.0;
    for (int i = 0; i < prism->num_vertices; i++) {
        int next = (i + 1) % prism->num_vertices; 
        area += (prism->vertex[next].east - prism->vertex[i].east) * 
                (prism->vertex[next].north + prism->vertex[i].north);
    }
    return area / 2.0; 
}

// Function to ensure the vertices are in clockwise order
void ensureClockwise(struct Prism *prism) {
    if (calculateSignedArea(prism) > 0) { 
        for (int i = 0; i < prism->num_vertices / 2; i++) {
            struct Vertex temp =  prism->vertex[i]; 
            prism->vertex[i] = prism->vertex[prism->num_vertices - 1 - i]; 
            prism->vertex[prism->num_vertices - 1 - i] = temp; 
        }
    }
}

// Function to read observed data from a file
struct ObservedMag *read_observed_data(const char *filename, int *num_obs) {
    FILE *obsfile = fopen(filename, "r");
    if (obsfile == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return NULL;
    }

    fprintf(stderr, "\nReading file...%s\n", filename);

    // First pass to count the number of observations
    *num_obs = 0;
    char obsline[256];
    
    // Print the first and last lines to verify the data was read in
    if (fgets(obsline, sizeof(obsline), obsfile)) {
        printf("\nFirst line: %s", obsline);  // Print the first line
        (*num_obs)++;  // Increment num_obs for the first line

        // Keep reading until the last line
        char last_line[256];  // We only need to store the last line
        while (fgets(obsline, sizeof(obsline), obsfile)) {
            strncpy(last_line, obsline, sizeof(last_line));  // Update the last line on each iteration
            (*num_obs)++;  // Increment num_obs for each additional line
        }

        printf("Last line: %s\n", last_line);  // Print the last line
    }

    fseek(obsfile, 0, SEEK_SET);  // Reset file pointer

    // Allocate memory based on num_obs
    struct ObservedMag *obsmag = malloc(*num_obs * sizeof(struct ObservedMag));
    if (obsmag == NULL) {
        fprintf(stderr, "Memory allocation failed in building observed data structure.\n");
        fclose(obsfile);
        return NULL;
    }

    // Second pass to fill the observed data
    int obs_index = 0;
    while (fgets(obsline, sizeof(obsline), obsfile)) {
        double east, north, obs_mag;
        if (sscanf(obsline, "%lf %lf %lf", &east, &north, &obs_mag) == 3) {
            obsmag[obs_index].east = east;
            obsmag[obs_index].north = north;
            obsmag[obs_index].obs_mag = obs_mag;
            obsmag[obs_index].calc_mag = 0;
            obsmag[obs_index].residuals = 0;
            obs_index++;
        }
    }

    fprintf(stderr, "\nReading success!\n");
    fclose(obsfile);
    return obsmag;
}
