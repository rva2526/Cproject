# README
## Directory Structure:
* **data**: *holds observed data points and prism shapes* products
    - **observed_data:** *holds observed magnetic data points and intermediate files*
    - **shape_data:** *holds observed prism data points and intermediate files* 
        - **grid_shapes:** *holds a bunch of coordinates for each prism in surface_anom_a_cone_1.nc - wont need until inversion complete*

* **plots:** *figures of plotted data*

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
