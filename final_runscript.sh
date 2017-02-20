echo ""
echo "****************************"
echo "***        V = 4         ***"
echo "****************************"

for i in `ls ./traces/`; do
    echo ""
    echo "--- $i ---"
    ./cachesim -i ./traces/$i
done;

