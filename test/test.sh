#!/usr/bin/bash

# Signals init loop
echo TEST STARTING...

BASEDIR=$(dirname "$0")

# Target folders declaration
IN="in"
OUT_CLIENT="out-client"
OUT_SERVER="out-server"

# Path to folders
TESTS_DIR=""$BASEDIR/$IN"/*.txt"
OUT_CLIENT_DIR="$BASEDIR/$OUT_CLIENT"
OUT_SERVER_DIR="$BASEDIR/$OUT_SERVER"

# Shows target folders
echo RUNNING TESTS IN /"$IN" FOLDER...
echo DEPLOYING CLIENT OUT IN /"$OUT_CLIENT" FOLDER...
echo DEPLOYING SERVER OUT IN /"$OUT_SERVER" FOLDER...

# Loops through every file in in folder
i=1
for TEST in $TESTS_DIR
  do
    echo RUNNING "${i}"º TEST: "$TEST"
    ./server/bin/main > "$OUT_SERVER_DIR"/out_srv_${i}.txt & P1=$!
    ./client/bin/main < "$TEST" > "$OUT_CLIENT_DIR"/out_clt_${i}.txt & P2=$!
    sleep 1s
    kill $P1 & kill $P2 && fg
    (( i++ ))
done
