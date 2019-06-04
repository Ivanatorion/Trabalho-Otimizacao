set title 'Simulated Annealing'
set ylabel 'Valor'
set xlabel 'Iteração'
set grid
set term png
set output 'bpwt_500_1.png'
plot 'result.csv' with lines notitle
