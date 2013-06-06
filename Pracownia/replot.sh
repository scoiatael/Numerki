g++ Pracownia1.cpp -o gen
for var in b m s e
do
	echo $var | ./gen > "data"$var".dat"
 
done
gnuplot gnumake
./remaketex.sh
for var in b m s e 
do 
	rm "data"$var".dat"
done
rm gen
