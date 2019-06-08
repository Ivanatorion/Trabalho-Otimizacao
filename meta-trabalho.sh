base="ResultadosOtimizacao"

seeds=('4' '8' '15' '16' '23')

tempInicis=('50' '100' '200' '400' '800' '1600' '3200' '6400' '12800' '25600')
esfriamentos=('0.5' '0.55' '0.6' '0.65' '0.7' '0.75' '0.8' '0.85' '0.9' '0.95')
numVizinis=('10' '20' '40' '80' '160' '320' '640' '1280' '2560' '5120')

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
  for (( e=0; e<10; e++ ))
  do
    ./trabalho.sh -q $seeda $stdTemp ${esfriamentos[${e}]} $stdVizinhos
    mv Results.zip $base/Seed$seeda/Esfriamento/Esfriamento$((e+1)).zip
  done

  mkdir $base/Seed$seeda/Temperatura/
  echo "Running with various initial temperatures..."
  #Temperatura inicial
  for (( e=0; e<10; e++ ))
  do
    ./trabalho.sh -q $seeda ${tempInicis[${e}]} $stdEsfriamento $stdVizinhos
    mv Results.zip $base/Seed$seeda/Temperatura/Temperatura$((e+1)).zip
  done

  mkdir $base/Seed$seeda/nVizinhos/
  echo "Running with various neighboors per temperature..."
  #Num Vizinhos
  for (( e=0; e<10; e++ ))
  do
    ./trabalho.sh -q $seeda $stdTemp $stdEsfriamento ${numVizinis[${e}]}
    mv Results.zip $base/Seed$seeda/nVizinhos/nVizinhos$((e+1)).zip
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

printf "Its finally over! Only took %02d:%02d:%02d\n" $(($elapsed / 3600)) $(($elapsed % 60)) $(($elapsed - (($elapsed / 60) * 60)))
