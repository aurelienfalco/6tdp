set yrange [0:]
set xlabel xname
set ylabel "Speedup"
set terminal png
set output output
plot data1 u 1:1:xtic(1) title "y=x" with linespoints lt rgb "#000000", \
	 data1 u 1:2:xtic(1) title name1 with linespoints lt rgb "#0099cc", \
 	 data2 u 1:2:xtic(1) title name2 with linespoints lt rgb "#FF0000", \
 	 data3 u 1:2:xtic(1) title name3 with linespoints lt rgb "#339900", 