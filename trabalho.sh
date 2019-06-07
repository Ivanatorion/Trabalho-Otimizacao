insts=('bpwt_500_' 'bpwt_1000_' 'bpwt_2000_')
nProg='SOLV'
paramsUsed=($2 $3 $4 $5)

if [ ! -f $nProg ]; then
  echo $nProg" nao encontrado!"
  echo "Tente rodar o compile.sh, ou edite esse script e altere o nome do programa"
else

if [ -d "Results" ]; then
  rm -rf Results
fi

if [ -f "Results.zip" ]; then
  rm Results.zip
fi

mkdir Results

echo "Seed: "${paramsUsed[0]}$'\n'"Temperatura Inicial: "${paramsUsed[1]}$'\n'"Esfriamento: "${paramsUsed[2]}$'\n'"Vizinhos por Temperatura: "${paramsUsed[3]}$'\n' > "Parametros.txt"
mv Parametros.txt Results/.

for (( k=0; k<3; k++ ))
do
  i=0
  echo "Running instances "$((k + 1))"/3..."
	for (( j=0; j<4; j++ ))
	do
	   inst=${insts[${k}]}$j
	   mkdir Results/$inst
	   cp instancias/$inst/plotinst.sh Results/$inst/.
     cp instancias/$inst/$inst".dat" Results/$inst/.
     if [ $1 == "-q" ]; then
	      cat instancias/$inst/$inst.dat | ./$nProg instancias/$inst/result.txt ${paramsUsed[0]} ${paramsUsed[1]} ${paramsUsed[2]} ${paramsUsed[3]} > /dev/null &
     else
        cat instancias/$inst/$inst.dat | ./$nProg instancias/$inst/result.txt ${paramsUsed[0]} ${paramsUsed[1]} ${paramsUsed[2]} ${paramsUsed[3]} &
     fi
	   pids[${i}]=$!
	   i=$((i+1))
	done

        #Wait all processes
	for pid in ${pids[*]}; do
	   wait $pid
	done

        for (( j=0; j<4; j++ ))
	do
           inst=${insts[${k}]}$j
	   mv instancias/$inst/result.csv Results/$inst/.
           mv instancias/$inst/result.txt Results/$inst/.
           cd Results/$inst
           gnuplot plotinst.sh
	   rm plotinst.sh
           cd ../..
	done
done

#Add Results to ZIP
zip -r Results.zip Results > /dev/null
rm -rf Results

echo "Done!"

if [ $1 != "-q" ]; then
  #Beep
  ( speaker-test -t sine -f 1000 > /dev/null )& pid=$! ; sleep 1.5s ; kill -9 $pid
fi

fi
