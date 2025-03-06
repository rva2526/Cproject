#include <stdio.h>
#include <stdlib.h>
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
    double px, py;
    for (int j = 0; j < num_obs; j++) {
        // For each observation, get the east and north coordinates
        py = obsmag[j].east;
        px = obsmag[j].north;

        // Loop through all prisms to calculate the total anomaly for this observation
        for (int i = 0; i < num_prisms; i++) {
            struct Prism currentPrism = prisms[i];

            // Add the calculated anomaly from this prism to the total anomaly for the observation
            obsmag[j].calc_mag += calculateVolumeIntegral(&currentPrism, px, py);

            // Calculate the residual for this observation (difference between observed and calculated anomaly)
            obsmag[j].residuals = obsmag[j].obs_mag - obsmag[j].calc_mag;  // Residual = Observed - Calculated
        }
    }

    // CALCULATE AND PRINT RMSE (ROOT MEAN SQUARED ERROR) BASED ON THE RESIDUALS
    double rmse = calculateRMSE(obsmag, num_obs);  // Calculate the RMSE for all observations based on residuals
    fprintf(stderr, "\nRMSE: %lf\n", rmse);  // Print the calculated RMSE value
    fprintf(stderr, "\n");

    // PRINT RESULTS (Observed vs Calculated Values and Residuals)
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

    return 0;  // Return 0 indicating successful execution
}
