#include <stdio.h>
#include <stdlib.h>
#include "read_parse.h"
#include "forward_inversion.h"

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <observed_file> <prism_json_file>\n", argv[0]);
        return 1;
    }

    char *obsfile_name = argv[1];
    char *prismfile_name = argv[2];

    //DOING OBS STUFF
    int num_obs;
    struct ObservedMag *obsmag = read_observed_data(obsfile_name, &num_obs);

    if (obsmag == NULL) {
        return 1;
    }


    //DOING PRISM STUFF
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


        double px, py, anomaly;
        for (int j=0; j<num_obs; j++) {
            
            py = obsmag[j].east;
            px = obsmag[j].north;
            anomaly = 0;
        for(int i = 0; i<num_prisms; i++){
                struct Prism currentPrism = prisms[i];
                anomaly += calculateVolumeIntegral(&currentPrism, px, py);
            }
                obsmag[j].calc_mag=anomaly;
                // printf("%lf %lf %lf\n", py, px, anomaly);
        
            }

        } else {
            printf("No prisms found.\n");
        }

    } else {
        printf("Failed to read the file\n");
    }

    for (int j=0; j<num_obs; j++) {
        printf("%lf %lf %lf %lf\n", 
        obsmag[j].east, 
        obsmag[j].north, 
        obsmag[j].obs_mag,
        obsmag[j].calc_mag);
    }
 

    return 0;
}
