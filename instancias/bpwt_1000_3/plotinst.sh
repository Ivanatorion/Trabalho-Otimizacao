set title 'Simulated Annealing'
set ylabel 'Valor'
set xlabel 'Iteração'
set grid
set term png
set output 'bpwt_1000_3.png'
plot 'bpwt_1000_3Solution.csv' with lines notitle
