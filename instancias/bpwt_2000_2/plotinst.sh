set title 'Simulated Annealing'
set ylabel 'Valor'
set xlabel 'Iteração'
set grid
set term png
set output 'bpwt_2000_2.png'
plot 'bpwt_2000_2Solution.csv' with lines notitle
