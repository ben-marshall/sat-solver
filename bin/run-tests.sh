#!/bin/bash

TEST_VECTORS=./tests
TEST_FILES=`ls $TEST_VECTORS`
OUTPUT_LOGS=./build/test_logs

BINARY=./build/sats

mkdir -p $OUTPUT_LOGS

function run_test {

    $BINARY $1 > $OUTPUT_LOGS/$TEST

    RESULT=$?

    if [ "$RESULT" = "0" ]; then
        
        echo "[PASS] $1"

    else

        echo "[FAIL] $1"

    fi

}



for TEST in $TEST_FILES 
do

    run_test $TEST_VECTORS/$TEST

done
