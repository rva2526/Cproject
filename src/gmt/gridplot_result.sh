

point_data=/home/jovyan/Cproject/results/southwest_anomaly_2prims_result.out
grid_data=/home/jovyan/Cproject/results/southwest_anomaly_2prims_result.grd
grid_surface=/home/jovyan/Cproject/results/southwest_anomaly_2prims_result.nc

color=/home/jovyan/Cproject/src/gmt/color.cpt

image_out=/home/jovyan/Cproject/plots/calculated_plot_southwest_anomaly_2prims_result
R=-R543787.5/545112.5/4051037.5/4052187.5

gmt blockmedian ${point_data} ${R} -i0,1,3 -I50 -V > ${grid_data}

gmt surface ${grid_data} -R -I25 -G${grid_surface} -V

gmt makecpt -Chaxby -i0,1,3 -T-75/30/1 > ${color} -V

gmt begin ${image_out} png E300

    gmt basemap -Jx1:10000 ${R} -Baf -BWSne -V
   
    gmt grdimage ${grid_surface} -C${color} -V

    gmt grdcontour ${grid_surface} -C10 -V

    gmt psscale -C${color} -DJMR+w12c/0.5c+o0.4c/0c -Bx50a10f5+l'Magnetic Anomaly (nT)'

gmt end