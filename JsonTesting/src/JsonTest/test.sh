

pushd ../..
make

if [ $? -eq 0 ]; then
   
   echo
   echo "=== Running Project ==="
   ./bin/json-test A Johnathan_A_l1.csv

fi


