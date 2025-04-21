# README
To run some of the code you may need to use the "notebook" conda environment. Some python libraries may need to be installed as well. 

Run:
> conda activate notebook
>

## Directory Structure:
* **data**: *holds observed data points and prism shapes* products
    - **observed_data:** *holds observed magnetic data points and intermediate files*
    - **shape_data:** *holds observed prism data points and intermediate files* 
        - **grid_shapes:** *holds a bunch of coordinates for each prism in surface_anom_a_cone_1.nc - wont need until inversion complete*

* **plots:** *figures of plotted data*

* **results:** *output from running plouf*

* **src:** *source folder to hold all C files and folders with other code languages*
    - **gmt:** *generic mapping tools plotting codes and ancillary files*
    - **python:** *various python codes for making shapes, and checking data*

* **test:** *old data used to verify C code was producing proper results*

* **File - makefile:** used for compiling C code
* **File - plouf:** executable created by makefile

## Running plouf
### using test directory

Run: 
> ./plouf ./test/example_line.txt ./test/shape_data.json > ./test/example_plouf_result.txt
>

Check data:
> check_test_dir_results.ipynb
>

### using data directory
I cut out the most northern magnetic anomaly/cone. I am calling in cone 1. Parameters in the shapefile leave large residuals at the moment.

Observed data is already plotting using:
> bash ./src/gmt/gridplot_observed.sh
>

Run:
>./plouf ./data/observed_data/points_anom_a_cone_1.txt ./data/shape_data/cone1_shape.json > anom_a_cone1.out
>

Results can be plotted similar to observed using (assuming you keep the same outfile name):
> bash ./src/gmt/gridplot_result.sh
>

observed and result plots are in the plots folder.

At anomaly A in southern Crater Flat, 62
meters of massive basalt was encountered
beneath alluvium at a depth of 148 meters.