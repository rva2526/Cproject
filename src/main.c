#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "read_parse.h"
#include "forward_inversion.h"

int main(int argc, char *argv[]) {
    // Ensure the correct number of command line arguments are provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <observed_file> <prism_json_file>\n", argv[0]);
        return 1;
    }

    // Assign the input filenames from the command line arguments
    char *obsfile_name = argv[1];
    char *prismfile_name = argv[2];

    // READ AND PARSE THE OBSERVED DATA
    int num_obs = 0;  // Initialize the number of observed data points to 0
    struct ObservedMag *obsmag = read_observed_data(obsfile_name, &num_obs);

    // If observed data couldn't be read, print an error and exit
    if (obsmag == NULL) {
        fprintf(stderr, "Error reading observed data from file: %s\n", obsfile_name);
        return 1;
    }

    // READ AND PARSE THE PRISM DATA
    char *prism_content = read_file(prismfile_name);

    // If prism data couldn't be read, print an error and free the observed data memory
    if (prism_content == NULL) {
        fprintf(stderr, "Failed to read the prism file: %s\n", prismfile_name);
        free(obsmag);  // Free previously allocated memory for observed data
        return 1;
    }

    // Parse the prism data and get the number of prisms
    int num_prisms = 0;
    struct Prism *prisms = read_json(prism_content, &num_prisms);
    free(prism_content);  // No longer need the raw prism content after parsing

    // If no prisms were found in the file, print an error and free the observed data memory
    if (prisms == NULL) {
        fprintf(stderr, "No prisms found in the file: %s\n", prismfile_name);
        free(obsmag);  // Free the previously allocated observed data memory
        return 1;
    }

    // Print the total number of prisms found in the file
    fprintf(stderr, "\nTotal Prisms Found: %d\n", num_prisms);

    // ENSURE PRISMS HAVE CLOCKWISE ORIENTATION
    for (int i = 0; i < num_prisms; i++) {
        struct Prism *prism = &prisms[i];
        ensureClockwise(prism);  // Ensure the vertices of each prism are in clockwise order
    }

    // PRINT DETAILS OF EACH PRISM
    for (int i = 0; i < num_prisms; i++) {
        struct Prism *prism = &prisms[i];
        fprintf(stderr, "\nPrism %d:\n", i + 1);
        fprintf(stderr, "Magnetic Inclination: %.2f\n", prism->minc);
        fprintf(stderr, "Magnetic Declination: %.2f\n", prism->mdec);
        fprintf(stderr, "Magnetic Intensity: %.2f\n", prism->mi);
        fprintf(stderr, "Top Depth: %.2f\n", prism->z1);
        fprintf(stderr, "Bottom Depth: %.2f\n", prism->z2);

        fprintf(stderr, "Vertices:\n");
        for (int j = 0; j < prism->num_vertices; j++) {
            fprintf(stderr, "Vertex %d: North = %.2f, East = %.2f\n", j + 1, prism->vertex[j].north, prism->vertex[j].east);
        }
    }

    // CALCULATE ANOMALY FOR EACH OBSERVATION

    // Step 1: Calc intensity of magnetization
    double tol = 1e-6;
    double best_mi = golden_search_magnetization_multi(obsmag, prisms, num_prisms, num_obs, tol);
    fprintf(stderr, "\nBest-fit shared magnetization intensity: %lf A/m\n", best_mi);

    // Step 2: Calculate total anomaly from all prisms
    for (int j = 0; j < num_obs; j++) {
        obsmag[j].calc_mag = 0.0;
        for (int i = 0; i < num_prisms; i++) {
            obsmag[j].calc_mag += calculateVolumeIntegral(&prisms[i], obsmag[j].north, obsmag[j].east);
        }
        obsmag[j].residuals = obsmag[j].obs_mag - obsmag[j].calc_mag;
    }

    // Step 3: Summarize residuals
    double min_residual = obsmag[0].residuals;
    double max_residual = obsmag[0].residuals;
    double sum_residuals = 0.0;

    for (int j = 0; j < num_obs; j++) {
        double r = obsmag[j].residuals;
        if (r < min_residual) min_residual = r;
        if (r > max_residual) max_residual = r;
        sum_residuals += r;
    }

    double mean_residual = sum_residuals / num_obs;
    fprintf(stderr, "Residuals - Min: %f, Max: %f, Mean: %f\n", min_residual, max_residual, mean_residual);

    // PRINT RESULTS
    for (int j = 0; j < num_obs; j++) {
        printf("%lf %lf %lf %lf %lf\n",  // Print east, north, observed magnitude, calculated magnitude, and residual
               obsmag[j].east,
               obsmag[j].north,
               obsmag[j].obs_mag,
               obsmag[j].calc_mag,
               obsmag[j].residuals);
    }

    // FREE ALLOCATED MEMORY FOR OBSERVED DATA AND PRISM DATA
    free(obsmag);  // Free memory allocated for observed data
    obsmag = NULL;  // Prevent access to freed memory

    free(prisms);  // Free memory allocated for prism data
    prisms = NULL;  // Prevent access to freed memory

    fprintf(stderr,"\nOutfile Columns: ['X','Y','Obsereved_Mag','Calculated_Mag','Residuals']\n");
    return 0;  // Return 0 indicating successful execution
}
