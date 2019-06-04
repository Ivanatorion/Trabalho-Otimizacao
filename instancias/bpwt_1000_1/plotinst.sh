set title 'Simulated Annealing'
set ylabel 'Valor'
set xlabel 'Iteração'
set grid
set term png
set output 'bpwt_1000_1.png'
plot 'result.csv' with lines notitle
