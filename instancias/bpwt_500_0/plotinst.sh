set title 'Simulated Annealing'
set ylabel 'Valor'
set xlabel 'Iteração'
set grid
set term png
set output 'bpwt_500_0.png'
plot 'bpwt_500_0Solution.csv' with lines notitle