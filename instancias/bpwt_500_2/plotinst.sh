set title 'Simulated Annealing'
set ylabel 'Valor'
set xlabel 'Iteração'
set grid
set term png
set output 'bpwt_500_2.png'
plot 'bpwt_500_2Solution.csv' with lines notitle
