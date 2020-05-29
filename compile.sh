for x in `ls programs/`
do
filename=$(basename -- "$x");
extension="${filename##*.}";
filename="${filename%.*}";
gcc -O0 programs/$x -o binaries/bin_$filename -lrt 
done;
