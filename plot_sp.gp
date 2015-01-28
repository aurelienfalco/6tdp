set yrange [0:]
set xlabel xname
set ylabel "Speedup"
set terminal png
set output output
plot data1 u 1:2 title name1 with linespoints lt rgb "#0099cc", \
 data2 u 1:2 title name2 with linespoints lt rgb "#0099cc", \
 data3 u 1:2 title name3 with linespoints lt rgb "#0099cc", \
 data4 u 1:2 title name4 with linespoints lt rgb "#0099cc", \
 data5 u 1:2 title name5 with linespoints lt rgb "#0099cc"
