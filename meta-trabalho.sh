base="ResultadosOtimizacao"

seeds=('4' '8' '15' '16' '23')

tempInicis=('20000' '10000' '5000' '1000' '200') 
esfriamentos=('0.95' '0.9' '0.85' '0.8' '0.75')
numVizinis=('1000' '500' '100' '50' '10')

stdTemp='10000'
stdEsfriamento='0.9'
stdVizinhos='100'

echo "This script will run a lot of simulations, this will take a while..."
echo "We are running 4 simulations at the same time,"
echo "so the script is best used on a 4-core processor"$'\n'

if [ -d $base ]; then
  rm -rf $base
fi

if [ -f $base.zip ]; then
 rm $base.zip
fi

mkdir $base

echo "Created directory for the results: "$base

./compile.sh

echo "Compiled source to binary: SOLV"

start_time="$(date -u +%s)"

for (( k=0; k<5; k++ ))
do
  seeda=${seeds[${k}]}

  echo "Running Simulations with Seed: "$seeda" ("$((k+1))"/5)"

  mkdir $base/Seed$seeda
  mkdir $base/Seed$seeda/Esfriamento/

  echo "Running with various decay values..."
  #Esfriamento
  for (( e=0; e<5; e++ ))
  do
    ./trabalho.sh -q $seeda $stdTemp ${esfriamentos[${e}]} $stdVizinhos
    mv Results.zip $base/Seed$seeda/Esfriamento/Esfriamento$((e+1)).zip
  done

  echo "Running with various initial temperatures..."
  #Temperatura inicial
  for (( e=0; e<5; e++ ))
  do
    ./trabalho.sh -q $seeda ${tempInicis[${e}]} $stdEsfriamento $stdVizinhos
    mv Results.zip $base/Seed$seeda/Esfriamento/Esfriamento$((e+1)).zip
  done

  echo "Running with various neighboors per temperature..."
  #Num Vizinhos
  for (( e=0; e<5; e++ ))
  do
    ./trabalho.sh -q $seeda $stdTemp $stdEsfriamento ${numVizinis[${e}]}
    mv Results.zip $base/Seed$seeda/Esfriamento/Esfriamento$((e+1)).zip
  done

  end_time="$(date -u +%s)"
  elapsed="$(($end_time - $start_time))"

  echo $'\n'"We are doing great! I guess less than $((((($elapsed * 5) / (k+1)) - $elapsed)/3600 + 1)) hour(s) remaining!"$'\n'

done

echo "Cleaning up.."

rm SOLV

echo "Removed SOLV"

#Zips
zip -r $base.zip $base > /dev/null
rm -rf $base

echo "Zipped results"

end_time="$(date -u +%s)"
elapsed="$(($end_time - $start_time))"

echo "Its finally over! Only took $(($elapsed / 3600)) hours!"
