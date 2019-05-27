set title 'Simulated Annealing'
set ylabel 'Valor'
set xlabel 'Iteração'
set grid
set term png
set output 'bpwt_2000_3.png'
plot 'bpwt_2000_3Solution.csv' with lines notitle
