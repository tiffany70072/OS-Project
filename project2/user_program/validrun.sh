for FILE in `find ../data -name *.in`
do
	echo $FILE
	sudo ./master $FILE $1 &
	sudo ./slave $FILE.out $1 127.0.0.1
	dmesg | tail -n 15
done
