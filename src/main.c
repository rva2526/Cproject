#include <stdio.h>
#include <stdlib.h>
#include "read_parse.h"
#include "forward_inversion.h"

double calculateRMSE (struct ObservedMag *obsmag, int num_obs){
    double sum_residuals = 0.0;

    for (int i = 0; i < num_obs; i++) {
        sum_residuals += obsmag[i].residuals * obsmag[i].residuals; // Square the residual and add to sum
    }
    
    return sqrt(sum_residuals / num_obs);
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <observed_file> <prism_json_file>\n", argv[0]);
        return 1;
    }

    char *obsfile_name = argv[1];
    char *prismfile_name = argv[2];

    // DOING OBS STUFF
    int num_obs = 0;  // Initialize num_obs to 0
    struct ObservedMag *obsmag = read_observed_data(obsfile_name, &num_obs);

    if (obsmag == NULL) {
        fprintf(stderr, "Error reading observed data from file: %s\n", obsfile_name);
        return 1;
    }

    // DOING PRISM STUFF
    char *prism_content = read_file(prismfile_name);

    if (prism_content == NULL) {
        fprintf(stderr, "Failed to read the prism file: %s\n", prismfile_name);
        free(obsmag);  // Don't forget to free previously allocated memory
        return 1;
    }

    int num_prisms = 0;
    struct Prism *prisms = read_json(prism_content, &num_prisms);
    free(prism_content);  // We no longer need the prism_content after parsing the JSON

    if (prisms == NULL) {
        fprintf(stderr, "No prisms found in the file: %s\n", prismfile_name);
        free(obsmag);  // Free the previously allocated observed data memory
        return 1;
    }

    printf("\nTotal Prisms Found: %d\n", num_prisms);

    // Ensure prisms have clockwise orientation
    for (int i = 0; i < num_prisms; i++) {
        struct Prism *prism = &prisms[i];
        ensureClockwise(prism);
    }

    // Print prism details
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

    // CALCULATING ANOMALY
    double px, py, anomaly;
    for (int j=0; j<num_obs; j++) {
    	
    	py = obsmag[j].east;
    	px = obsmag[j].north;
    	anomaly = 0;
    for(int i = 0; i<num_prisms; i++){
            struct Prism currentPrism = prisms[i];
            anomaly += calculateVolumeIntegral(&currentPrism, px, py);
            obsmag[j].calc_mag = anomaly;
         }

        obsmag[j].residuals = obsmag[j].obs_mag - obsmag[j].calc_mag; // Residual = Observed - Calculated
    }
 
    double rmse = calculateRMSE(obsmag, num_obs);
    printf("RMSE: %lf\n",rmse);

    // Print results
    for (int j = 0; j < num_obs; j++) {
        printf("%lf %lf %lf %lf %lf\n",
               obsmag[j].east,
               obsmag[j].north,
               obsmag[j].obs_mag,
               obsmag[j].calc_mag,
               obsmag[j].residuals);

    }

    // Free allocated memory
    // free(residuals);


    // Free allocated memory
    free(obsmag);
    free(prisms);

    return 0;
}
