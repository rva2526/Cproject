#include <stdio.h>
#include <math.h>

#define PI 3.14159

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    double x[4];
    double y[4];
    int sides;
    double z1;
    double z2;
    double mi;
    double mdec;
    double minc;
} Prism;

typedef struct {
    double bx;
    double by;
    double bz;
} MagneticField;

Prism createPrism(double x[], double y[], int sides, double z1, double z2, double mi, double mdec, double minc) {
    Prism prism;
    for (int i = 0; i < sides; i++) {
        prism.x[i] = x[i];
        prism.y[i] = y[i];
    }
    prism.sides = sides;
    prism.z1 = z1;
    prism.z2 = z2;
    prism.mi = mi;
    prism.mdec = mdec * PI / 180.0;
	 prism.minc = minc * PI / 180.0;
   
    return prism;
}

double calculateVolumeIntegral(const Prism *prism, double px, double py, MagneticField *magneticField) {
    double v1 = 0.0, v2 = 0.0, v3 = 0.0, v4 = 0.0, v5 = 0.0, v6 = 0.0;
    double prop = 400.0 * PI;
    for (int i = 0; i < prism->sides; i++) {
        double x1, y1, x2, y2;
        if (i == prism->sides - 1) {
            x1 = prism->x[i] - px;
            y1 = prism->y[i] - py;
            x2 = prism->x[0] - px;
            y2 = prism->y[0] - py;
        } else {
            x1 = prism->x[i] - px;
            y1 = prism->y[i] - py;
            x2 = prism->x[i + 1] - px;
            y2 = prism->y[i + 1] - py;
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
        double w = atan2((prism->z2 * d2), (p * r22)) - atan2((prism->z2 * d1), (p * r12)) - atan2((prism->z1 * d2), (p * r21)) + atan2((prism->z1 * d1), (p * r11));

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

    magneticField->bx = prop * (mx * v1 + my * v2 + mz * v3);
    magneticField->by = prop * (mx * v2 + my * v4 + mz * v5);
    magneticField->bz = prop * (mx * v3 + my * v5 + mz * v6);

   double b_total = ml*magneticField->bx + mm*magneticField->by + mn*magneticField->bz;
    return b_total;
}

int main() {
    double x[] = {-20, -18, 20, 18};
    double y[] = {20, 20, -21, -21};
    int sides = 4;
    double z1 = 10.0;
    double z2 = 20.0;
    double mi = 9.0;
    double mdec = -45;
    double minc = 45.0;
	
    Prism prism = createPrism(x, y, sides, z1, z2, mi, mdec, minc);
    //printf("%lf \n", prism.prop);
    MagneticField magneticField;
    
    double px, py, anomaly;

    for (int j = 0; j < 101; j++) {
        for (int k = 0; k < 101; k++) {
            px = (float)(j - 50.0);
            py = (float)(k - 50.0);
            anomaly = calculateVolumeIntegral(&prism, px, py, &magneticField);
            printf("%lf %lf %lf\n", py, px, anomaly);
        }
	        printf("\n");
    }

  
    return 0;
}
