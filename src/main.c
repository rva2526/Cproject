#include <stdio.h>
#include <stdlib.h>
#include "read_parse.h"

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <observed_file> <prism_json_file>\n", argv[0]);
        return 1;
    }

    char *obsfile_name = argv[1];
    char *prismfile_name = argv[2];

    char *prism_content = read_file(prismfile_name);

    if (prism_content != NULL) {
        int num_prisms = 0;
        struct Prism *prisms = read_json(prism_content, &num_prisms);
        free(prism_content);
        
        if (prisms != NULL) {
            printf("\nTotal Prisms Found: %d\n", num_prisms);



            for (int i = 0; i < num_prisms; i++) {
                struct Prism *prism = &prisms[i];

                ensureClockwise(prism);

            }

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

        } else {
            printf("No prisms found.\n");
        }

    } else {
        printf("Failed to read the file\n");
    }



int read_obs(const char *filename){
    FILE *obsfile = fopen(obsfile_name, "r");
    if (obsfile == NULL) {
        fprintf(stderr, "Error opening file: %s\n", obsfile_name);
        return 1;
    }


    // Determine the number of rows (num_obs) in the observed points data file - this is the number of data points for calcs
    int num_obs = 0;
    char obsline[256];
    while (fgets(obsline, sizeof(obsline), obsfile)) {
        num_obs++;
    }
    fseek(obsfile, 0, SEEK_SET);
    
    // Allocate memory for the array of obs points 
    struct ObservedMag *obsmag = (struct ObservedMag*)malloc(num_obs * sizeof(struct ObservedMag));
    if (obsmag == NULL) {
        fprintf(stderr, "Memory allocation failed in building observed data structure - do you have no observed data?\n");
        fclose(obsfile);
        return 1;
    }

    for (int i = 0; i < num_obs; i++) {
        
        struct ObservedMag *currentObs = &obsmag[i];
        
        fscanf(obsfile, "%lf %lf %lf ", &currentObs->east, &currentObs->north,
               &currentObs->mag);
    }
    
    // Close the file
    fclose(obsfile);
    return 0;
}


    return 0;
}
