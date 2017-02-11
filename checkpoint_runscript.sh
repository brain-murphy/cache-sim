echo ""
echo "****************************"
echo "***        V = 0         ***"
echo "****************************"

for i in `ls ./traces/`; do
    echo ""
    echo "--- $i ---"
    ./cachesim -V 0 -i ./traces/$i
done;

