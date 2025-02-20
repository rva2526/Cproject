#include <stdio.h>
#include <stdlib.h>
#include "read_parse.h"

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
