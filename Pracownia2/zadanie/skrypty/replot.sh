#! /bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd ..  && pwd )"
cd "$DIR/prog"
g++ program.cpp -o ../gen -v
cd "$DIR"
if [ ! $? -eq 0 ]; then
  exit
fi
echo "Compilation done."
for var in "/skrypty/gplot" "/wykresy" "/wyniki"
do
  if [ ! -d "$DIR$var" ]; then
    mkdir --verbose "$DIR$var"
  fi 
done
for var in $( ls $DIR/testy/*.din)
do
  var="$( basename "${var%.din}")"
  cd "$DIR"
  if [ ! -d "$DIR/wyniki/$var" ]; then
   mkdir --verbose "$DIR/wyniki/$var"
  fi 
	echo "$var" | "$DIR/gen"
  if [ ! $? -eq 0 ]; then
    exit
  fi
  $DIR/skrypty/prepare_t $var > $DIR/skrypty/gplot/$var
  # DEBUG
  #$DIR/skrypty/prepare_tdb $var > $DIR/skrypty/gplot/d$var
  #gnuplot "$DIR/skrypty/gplot/d$var"
  # END DEBUG
	echo "$var" | "$DIR/gen"
  if [ ! $? -eq 0 ]; then
    exit
  fi
  gnuplot "$DIR/skrypty/gplot/$var"
  if [ ! $? -eq 0 ]; then
    exit
  fi
done
"$DIR/skrypty/remaketex.sh"
rm "$DIR/gen"
