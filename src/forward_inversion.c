#include "read_parse.h"
#include "forward_inversion.h"

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

    double minc_rad,mdec_rad;
    minc_rad = prism->minc*(PI/180);
    mdec_rad = prism->mdec*(PI/180);

    double ml = cos(minc_rad) * cos(mdec_rad);
    double mm = cos(minc_rad) * sin(mdec_rad);
    double mn = sin(minc_rad);


    double mx = prism->mi * ml;
    double my = prism->mi * mm;
    double mz = prism->mi * mn;
   
    bx = prop * (mx * v1 + my * v2 + mz * v3);
    by = prop * (mx * v2 + my * v4 + mz * v5);
    bz = prop * (mx * v3 + my * v5 + mz * v6);

   b_total = ml*bx + mm*by + mn*bz;

    return b_total;
}