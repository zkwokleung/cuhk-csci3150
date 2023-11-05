# -----------------------------------------------------------------------------
#   Author: Andrew SZE-TO
#   Last Modified: 2023-11-06
# -----------------------------------------------------------------------------
#  Description:
#   This script is used to test your code for Bonus Assignment 3.
# ? Before running your code, you should structure your directory as follows:
#   .
#   ├── Bonus-Assign3-Code
#   │   ├── Makefile
#   |   ...
#   │   └── TaskScheduler.h
#   ├── Bonus-Assign3-Testcases
#   │   ├── testcase1
#   │   │   ├── input.txt
#   │   │   └── output.log
#   │   ├── testcase2
#   │   ...
#   │   └── testcase5
#   │       ├── input.txt
#   │       └── output.log
#   └── run_test.sh
# 
# ? Then, you can run this script to test your code with the following command:
#   Linux/MacOS:
#   $ ./run_test.sh
# 
#   Windows:
#   $ bash run_test.sh
#
# * In case you cannot run this script, you can run the following commands:
#   Linux/MacOS:
#   $ sudo chmod 777 run_test.sh
# 
# * After running this script, a folder named "test" will be created.
#   You can check the output of your code in test/output.log
#   If your code is correct, the output of your code will be the same as the expected output
#   in Bonus-Assign3-Testcases/testcase1/output.log
#
#   If your code is incorrect, the output of your code will be different from the expected output.
#   You may check test/diff.log for more information.
#   The diff.log is generated with the following command:
#   $ diff ./test/output.log ./test/expected.log > ./test/diff.log

rm -rf test
mkdir test

cd Bonus-Assign3-Code
make
mv Scheduler ../test
mv *.o ../test
cd ..

for i in {1..5}
do
    src="./Bonus-Assign3-Testcases/testcase$i"
    echo ">> Running $src"
    cp -r "$src"/* test/
    mv ./test/output.log ./test/expected.log

    cd test
    ./Scheduler > /dev/null &
    cd ..

    pid=$!

    # ! If you are running on a slow computer, you may need to increase the sleep time
    sleep 0.5

    if ps -p $pid > /dev/null
    then
        kill -9 $pid
        printf "\033[31m!!! Error: Timeout while running testcase$i\033[0m\n"
        exit 1
    fi

    if [ ! -f "./test/output.log" ]; then
        printf "\033[31m!!! Error: Segmentation fault while running testcase$i\033[0m\n"
        exit 1
    fi
    diff ./test/output.log ./test/expected.log > ./test/diff.log

    if [ -s "./test/diff.log" ]; then
        printf "\033[31m!!! Error: Output mismatch while running testcase$i\033[0m\n"
        printf "\033[31m!!!        Please check test/diff.log for more info\033[0m\n"
        exit 1
    fi

    printf "\033[32m== Testcase$i passed\033[0m\n"
done