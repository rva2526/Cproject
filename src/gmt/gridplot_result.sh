

point_data=/home/jovyan/Cproject/results/anom_a_cone1.out
# out_txt=/home/jovyan/Cproject/data/observed_data/points_anom_a_cone_1.txt
grid_data=/home/jovyan/Cproject/results/anom_a_cone1_50m.out
grid_surface=/home/jovyan/Cproject/results/result_anom_a_cone_1.nc

color=/home/jovyan/Cproject/data/color2.cpt

image_out=/home/jovyan/Cproject/plots/calculated_plot

R=$(gmt gmtinfo ${point_data} -I-)
# echo ${R}

if [ ! -f $grid_surface ]; then
    gmt blockmedian ${point_data} ${R} -i0,1,3 -I50 -V > ${grid_data}
    gmt surface ${grid_data} -R -I25 -G${grid_surface} -V

fi

T=$(gmt grdinfo ${grid_surface} -T-)
gmt makecpt -Chaxby -i0,1,3 ${T}/1 > ${color} -V

gmt begin ${image_out} png E300

    gmt basemap -Jx1:10000 ${R} -Baf -BWSne -V
   
    gmt grdimage ${grid_surface} -C${color} -V

    # gmt grdcontour ${grid_surface} -C10 -V

    gmt psscale -C${color} -DJMR+w12c/0.5c+o0.4c/0c -Bx50af+l'magnetic field (nT)'

gmt end