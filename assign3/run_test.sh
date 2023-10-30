rm -rf test
mkdir test

cd Bonus-Assign3-Code
make
cp Scheduler ../test
cp *.o ../test
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

    sleep 1

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
        exit 1
    fi

    printf "\033[32m== Testcase$i passed\033[0m\n"
done