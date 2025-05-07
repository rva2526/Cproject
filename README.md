# README

This repository holds codes written in C, python and GMT. These codes were written with the aim of creating a fast and readable optimization/inversion of magnetic anomalies following Plouff 1976 (). The inversion code is written in C with some helper functions around creating a json file which holds parameters related to the prism used for the inversion. GMT code will visualize the output data.

### Python Code

Python libraries needed:

> numpy
> matplotlib
> json
> pandas
> shapely

Codes:
> make_parameters.py: This code allows you to interactivly make simple shapes and output a formatted json file

> shape_csv2json.py: This code will take a csv which holds the coordinates of your prims and place it into a json format where you can insert the other parameters needed

### C Code

C libraries needed:
> cJson: https://github.com/DaveGamble/cJSON

Main.c:

The primary function inside main.c is "golden_search_magnetization_multi." This function will optimize for the intensity of magnetization for one or multiple prisms. 

## Directory Structure:

* **data**: *holds observed data points and json shape files*

  - **observed_data:** *holds observed magnetic data points and intermediate files*
  - **shape_data:** *holds observed prism data points and intermediate files*

* **plots:** *figures of plotted data*
* **results:** *output from running plouf*
* **src:** *source folder to hold all C files and folders with other code languages*

  - **gmt:** *generic mapping tools plotting codes and ancillary files*
  - **python:** *various python codes for making shapes or jsons*

* **File - makefile:** used for compiling C code
* **File - plouf:** executable created by makefile

## Running plouf

Run:

> ./plouf ./data/observed_data/anomaly_a_southwest_pts_reduced.csv ./data/shape_data/southwest_anomaly_2prisms.json > ./results/southwest_anomaly_2prims_result.out

Plot output:
> bash ./src/gmt/gridplot_result.sh
