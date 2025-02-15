
#this file plots a function using gnuplot. Typically to run
# type: gnuplot thin-plate-mag.gnu
# on the command line
reset

set termoption dash

set size square
set xlabel 'East'
set ylabel 'North'
set pm3d map
set xrange[-50:50]
set yrange[-50:50]
set cbrange [-500:1000] 
set cblabel 'nT'
set palette rgbformulae 22,13,10

set arrow front from -10,-10 to -10,10 nohead
set arrow front from -10,10 to 10,10 nohead
set arrow front from 10,10 to 10,-10 nohead
set arrow front from 10,-10 to -10,-10 nohead
splot 'out.txt' notitle

set term pdf enhanced dashed
set output "test.pdf" 
replot
