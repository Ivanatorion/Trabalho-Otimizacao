set title 'Simulated Annealing'
set ylabel 'Valor'
set xlabel 'Iteração'
set grid
set term png
set output 'bpwt_1000_1.png'
plot 'bpwt_1000_1Solution.csv' with lines notitle
