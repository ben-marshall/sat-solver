#!/bin/bash

TEST_VECTORS=./build/test_vectors
TEST_FILES=`ls $TEST_VECTORS`
OUTPUT_LOGS=./build/test_logs

BINARY=./build/sats

mkdir -p $OUTPUT_LOGS

FINAL_RESULT=0

function run_test {

    $BINARY $1 > $OUTPUT_LOGS/$TEST
    RESULT=$?

    if [ "$RESULT" = "0" ]; then
        
        echo "[PASS] $1"

         valgrind  --error-exitcode=1 \
            $BINARY $1 &> $OUTPUT_LOGS/$TEST

        VG_RESULT=$?
        
        if [ "$VG_RESULT" = "0" ]; then
            echo "[PASS] $1 memcheck"
        else
            echo "[FAIL] $1 memcheck."
            FINAL_RESULT=1
        fi

    else

        echo "[FAIL] $1"
        FINAL_RESULT=1

    fi

}


echo "Running randomly generated tests."

for TEST in $TEST_FILES 
do

    run_test $TEST_VECTORS/$TEST

done

exit $FINAL_RESULT
