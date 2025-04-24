

point_data=/home/jovyan/Cproject/data/observed_data/anomaly_a_southwest_pts_reduced.csv
out_txt=/home/jovyan/Cproject/src/gmt/southwest_cone_1.txt
grid_data=/home/jovyan/Cproject/src/gmt/southwest_cone_1_50m.grd
grid_surface=/home/jovyan/Cproject/src/gmt/surface_southwest_cone_1.nc

color=/home/jovyan/Cproject/src/gmt/color.cpt

image_out=/home/jovyan/Cproject/plots/observed_plot

R=$(gmt gmtinfo ${point_data} -I-)
# echo ${R}

# if [ ! -f $out_txt ]; then
#     awk -v OFS=' ' '{gsub(/,/,OFS); gsub(/^\\s+|\\s+$/,""); print}' ${point_data} > ${out_txt}
# fi

gmt blockmedian ${point_data} ${R}  -I50 -V -G${grid_data}
gmt surface ${grid_data} ${R} -I25 -G${grid_surface} -V


T=$(gmt grdinfo ${grid_surface} -T-)
gmt makecpt -Chaxby ${T}/1 > ${color} -V

# gmt begin ${image_out} png E300

#     gmt basemap -Jx1:10000 ${R} -Baf -BWSne -V
   
#     gmt grdimage ${grid_surface} -C${color} -V

#     gmt grdcontour ${grid_surface} -C10 -V

#     gmt psscale -C${color} -DJMR+w12c/0.5c+o0.4c/0c -Bx50a50f+l'magnetic field (nT)'

# gmt end