
CSV_DIR="./test-csvs"
#TEST_AMTS=(1 2 5 all)
TEST_AMTS=(1 2)
PROGRAM="./bin/emg-upload"


pushd ..
make

if [ $? -eq 0 ]; then
   echo 
   #./bin/emg-upload A Johnathan_A_l1.csv
   
   for amt in ${TEST_AMTS[@]}; do
      echo "=== Testing $amt CSV(s) ==="
      $PROGRAM $CSV_DIR/$amt/*.csv
      echo
   done

fi


