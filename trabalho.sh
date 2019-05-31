insts=('bpwt_500_' 'bpwt_1000_' 'bpwt_2000_')

if [ -d "Results" ]; then
  rm -rf Results
fi

if [ -f "Results.zip" ]; then
  rm Results.zip
fi

mkdir Results

for (( k=0; k<3; k++ ))
do
        i=0
	for (( j=0; j<4; j++ ))
	do
	   inst=${insts[${k}]}$j
	   mkdir Results/$inst
	   cp instancias/$inst/plotinst.sh Results/$inst/.
           cp instancias/$inst/$inst".dat" Results/$inst/.
	   ./SOLV instancias/$inst/$inst.dat &
	   pids[${i}]=$!
	   #echo $((pids[${i}]))
	   i=$((i+1))
	done

        #Wait all processes
	for pid in ${pids[*]}; do
	   #echo $pid
	   wait $pid
	done

        for (( j=0; j<4; j++ ))
	do
           inst=${insts[${k}]}$j
	   mv instancias/$inst/$inst"Solution.csv" Results/$inst/.
           mv instancias/$inst/$inst"Solution.txt" Results/$inst/.
           cd Results/$inst
           gnuplot plotinst.sh
	   rm plotinst.sh
           cd ../..
	done
done

zip -r Results.zip Results > /dev/null
rm -rf Results
( speaker-test -t sine -f 1000 > /dev/null )& pid=$! ; sleep 1.5s ; kill -9 $pid




