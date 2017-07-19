#!/bin/sh

TEST_VECTORS=./build/test_vectors
TEST_FILES=`ls $TEST_VECTORS`
OUTPUT_LOGS=./build/test_logs

BINARY=./build/sats

mkdir -p $OUTPUT_LOGS

for TEST in $TEST_FILES 
do

    $BINARY $TEST_VECTORS/$TEST | tee $OUTPUT_LOGS/$TEST

    RESULT=$?
    echo  "$TEST - $RESULT"

done
