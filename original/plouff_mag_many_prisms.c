#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX_VERTEX_NUM 10 // the maximum number of vertices per prism, set to whatever
#define PI 3.14159

// Define the Prism data structure
struct Prism {
    int num_vertices;   //number of vertices for this prism
    struct Vertex {     // the coordinate of each vertex in the prism
        double north;
        double east;
    } vertex[MAX_VERTEX_NUM];
    double z1;         // depth to the top of the prism
    double z2;         // depth to the base of the prism
    double mi;         // intensity of vector of magnetization (Amp/m))
    double minc;       // inclination of vector of magnetization (rad)
    double mdec;       // declination of vector of magnetization (rad))
};

// Define the observed points data structure
struct ObservedMag {
    double east;      // easting of observation point
    double north;     // northing of observation point
    double mag;       // magnetic value (nT) at observation point)
};


double calculateVolumeIntegral(const struct Prism *prism, double px, double py) {
    double v1 = 0.0, v2 = 0.0, v3 = 0.0, v4 = 0.0, v5 = 0.0, v6 = 0.0;  //volume integrals
    double prop = 400.0 * PI; //conversion to nT (given magnetic permeability of free space))
    double bx, by, bz, b_total; //components of the magnetic field and total magnetic field
    
    for (int i = 0; i < prism->num_vertices; i++) {
        double x1, y1, x2, y2;
        if (i == prism->num_vertices - 1) {
            x1 = prism->vertex[i].north - px;
            y1 = prism->vertex[i].east - py;
            x2 = prism->vertex[0].north - px;
            y2 = prism->vertex[0].east - py;
        } else {
            x1 = prism->vertex[i].north - px;
            y1 = prism->vertex[i].east - py;
            x2 = prism->vertex[i+1].north - px;
            y2 = prism->vertex[i+1].east - py;
        }

        double delta_x = x2 - x1;
        double delta_y = y2 - y1;
        double delta_s = sqrt(delta_x * delta_x + delta_y * delta_y);
        double c = delta_y / delta_s;
        double s = delta_x / delta_s;
        double p = (x1 * y2 - x2 * y1) / delta_s;

        double d1 = x1 * s + y1 * c;
        double d2 = x2 * s + y2 * c;

        double r1sq = x1 * x1 + y1 * y1;
        double r2sq = x2 * x2 + y2 * y2;
        double r11 = sqrt(r1sq + prism->z1 * prism->z1);
        double r12 = sqrt(r1sq + prism->z2 * prism->z2);
        double r21 = sqrt(r2sq + prism->z1 * prism->z1);
        double r22 = sqrt(r2sq + prism->z2 * prism->z2);

        double f = log((r22 + prism->z2) / (r12 + prism->z2) * (r11 + prism->z1) / (r21 + prism->z1));
        double q = log((r22 + d2) / (r12 + d1) * (r11 + d1) / (r21 + d2));
        double w = atan2((prism->z2 * d2), (p * r22)) 
                     - atan2((prism->z2 * d1), (p * r12)) 
                     - atan2((prism->z1 * d2), (p * r21)) 
                     + atan2((prism->z1 * d1), (p * r11));

        v1 += s * c * f - c * c * w;
        v2 += s * c * w + c * c * f;
        v3 += c * q;
        v4 += -s * c * f - s * s * w;
        v5 += -s * q;
        v6 += w;

    }

    double ml = cos(prism->minc) * cos(prism->mdec);
    double mm = cos(prism->minc) * sin(prism->mdec);
    double mn = sin(prism->minc);


    double mx = prism->mi * ml;
    double my = prism->mi * mm;
    double mz = prism->mi * mn;
   
    bx = prop * (mx * v1 + my * v2 + mz * v3);
    by = prop * (mx * v2 + my * v4 + mz * v5);
    bz = prop * (mx * v3 + my * v5 + mz * v6);

   b_total = ml*bx + mm*by + mn*bz;

    return b_total;
}

// find the ranges of observed data read in from file - print to make sure the data are ok
void findMinMax(struct ObservedMag data[], int num_instances) {
    int max_east = data[0].east;
    int min_east = data[0].east;
    int max_north = data[0].north;
    int min_north = data[0].north;
    int max_mag = data[0].mag;
    int min_mag = data[0].mag;

    for (int i = 1; i < num_instances; i++) {
        if (data[i].east > max_east)
            max_east = data[i].east;
        if (data[i].east < min_east)
            min_east = data[i].east;

        if (data[i].north > max_north)
            max_north = data[i].north;
        if (data[i].north < min_north)
            min_north = data[i].north;

        if (data[i].mag > max_mag)
            max_mag = data[i].mag;
        if (data[i].mag < min_mag)
            min_mag = data[i].mag;
    }

    fprintf(stderr, "Number of observations (observed mag readings): %d\n", num_instances);
    fprintf(stderr, "Ranges of observations:\n");
    fprintf(stderr, "    East: Min=%d Max=%d\n", min_east, max_east);
    fprintf(stderr, "    North: Min=%d Max=%d\n", min_north, max_north);
    fprintf(stderr, "    Mag (nT): Min=%d Max=%d\n", min_mag, max_mag);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf( stderr, "Usage: %s <observed mag data> <prism file>\n", argv[0]);
        return 1;
    }
	
	char *obsfile_name = argv[1];
   char *prismfile_name = argv[2];
	
	/* Step 1:  read the prism file 
	   and build the prisms data structure
   */
    
    

    // Open the prism data file
    FILE *prismfile = fopen(prismfile_name, "r");
    if (prismfile == NULL) {
        fprintf(stderr, "Error opening file: %s\n", prismfile_name);
        return 1;
    }
    
    // Open the observed data file
     
    FILE *obsfile = fopen(obsfile_name, "r");
    if (obsfile == NULL) {
        fprintf(stderr, "Error opening file: %s\n", obsfile_name);
        return 1;
    }

    // Determine the number of rows (n) in the prism data file - this is the number of prisms
    int num_prisms = 0;
    char line[256];
    while (fgets(line, sizeof(line), prismfile)) {
        num_prisms++;
    }
    fseek(prismfile, 0, SEEK_SET);
    
    // Allocate memory for the array of prisms (array of prism data structures)
    struct Prism *prisms = (struct Prism*)malloc(num_prisms * sizeof(struct Prism));
    if (prisms == NULL) {
        fprintf(stderr, "Memory allocation failed in building prism structure - do you have no prisms, too many prisms, or too many vertices?\n");
        fclose(prismfile);
        return 1;
    }
    
     /* Read data for each prism and store it in the prisms array
        the data file consists of one row per prism
        each prism row has columns separated by blank space
        the first column is an integer - num_vertices - number of vertices in that prism
        the next columns are doubles - the coordinates (easting, northing) of each vertex)
        these coordinates must be in clockwise order - the code does not check for clockwise order
        The coordinates do not close (that is the first coordinate is NOT the last coordinate 
        - the code closes the polygon to the first vertex)fseek(file, 0, SEEK_SET);
        the next column is the depth to the top of the prism - z1 
        the next column is the depth to the bottom of the prism - z2
        the next column is the intensity of the vector of magnetization for that prism - mi (Amp/m)
        the next column is the inclination of the vector of magnetization for that prism - minc (degrees)
        the next column is the declination of the vector of magnetization for that prism - mdec (degrees)
        
        e.g., 3 -10.0 10.0 10.0 10.0 5.0 0.0 10.0 20.0 1.0 45.0 0.0
        
        is a three vertex polygon, the vertices are (-10.0 E, 10.0 N), (10,0 E, 10.0 N), (5.0 E, 0.0 N)
        the depth to the top and bottom are 10.0 and 20.0, respectively
        the intensity of magnetization is 1 Amp/m
        the inclination is 45 degrees (down from horizontal), declination is 0.0 degrees (north)
        Note: the code will work for any distance units, but assume meters.
     */
    for (int i = 0; i < num_prisms; i++) {
        
        struct Prism *currentPrism = &prisms[i];
        // Read the number of vertices for the current prism
        fscanf(prismfile, "%d", &currentPrism->num_vertices);

        // Read east and north coordinates of the prism vertices into the Vertex structure for that prism
        for (int j = 0; j < currentPrism->num_vertices; j++) {
            fscanf(prismfile, "%lf %lf", &currentPrism->vertex[j].east, &currentPrism->vertex[j].north);
        }

        // Read z1, z2, mi, minc, and mdec
        fscanf(prismfile, "%lf %lf %lf %lf %lf", &currentPrism->z1, &currentPrism->z2,
               &currentPrism->mi, &currentPrism->minc, &currentPrism->mdec);
        // here the declination and inclination are converted to radians
        // they are stored in degrees in the prism data file       
        currentPrism->minc = currentPrism->minc*PI/180.0;
        currentPrism->mdec = currentPrism->mdec*PI/180.0;
    }

    // Close the file
    fclose(prismfile);

    fprintf(stderr, "Number of prisms in input file): %d\n", num_prisms);
    // Print the values of variables in the ith prism 
    int i = 0; // Replace this with the desired index of the prism you want to print
    if (i >= 0 && i < num_prisms) {
        struct Prism *selectedPrism = &prisms[i];

        fprintf(stderr, "Prism %d:\n", i + 1);
        fprintf(stderr, "num_vertices: %d\n", selectedPrism->num_vertices);

        fprintf(stderr, "Points (x,y):\n");
        for (int j = 0; j < selectedPrism->num_vertices; j++) {
            fprintf(stderr, "(%lf, %lf) ", selectedPrism->vertex[j].east, selectedPrism->vertex[j].north);
        }
        fprintf(stderr,"\n");

        fprintf(stderr, "z1: %lf\n", selectedPrism->z1);
        fprintf(stderr,"z2: %lf\n", selectedPrism->z2);
        fprintf(stderr,"mi: %lf\n", selectedPrism->mi);
        fprintf(stderr,"minc: %lf\n", selectedPrism->minc);
        fprintf(stderr,"mdec: %lf\n", selectedPrism->mdec);
    } else {
        fprintf(stderr,"Invalid prism index.\n");
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
    
     /* 
     */
    for (int i = 0; i < num_obs; i++) {
        
        struct ObservedMag *currentObs = &obsmag[i];
        
        fscanf(obsfile, "%lf %lf %lf ", &currentObs->east, &currentObs->north,
               &currentObs->mag);
    }

    // Close the file
    fclose(obsfile);
    
    // find the minimum and maximum for the observed data and print the answers
    findMinMax(obsmag, num_obs);
  
    double px, py, anomaly;
    for (int j=0; j<num_obs; j++) {
    	
    	py = obsmag[j].east;
    	px = obsmag[j].north;
    	anomaly = 0;
    for(int i = 0; i<num_prisms; i++){
            struct Prism currentPrism = prisms[i];
            anomaly += calculateVolumeIntegral(&currentPrism, px, py);
         }
            printf("%lf %lf %lf\n", py, px, anomaly);
       
        }
 
 /*   
    double px, py, anomaly;

    for (int j =4051000; j < 4055700; j+=100) {
        for (int k = 543800; k < 546826; k+=100) {
            px = (float)(j);
            py = (float)(k);
            anomaly = 0.0;  // zero anomaly for each new obs point
            for(int i = 0; i<num_prisms; i++){
            struct Prism currentPrism = prisms[i];
            anomaly += calculateVolumeIntegral(&currentPrism, px, py);
         }
            printf("%lf %lf %lf\n", py, px, anomaly);
       
        }
	         printf(" \n");
    }
*/
  
    return 0;
}
