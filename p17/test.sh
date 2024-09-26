#!/bin/bash

set -e

DB_FILE="db.dat"
VALGRIND="valgrind --leak-check=yes --track-origins=yes"

if [ -f $DB_FILE  ]; then
	    rm $DB_FILE
fi

echo "========================"
echo " Running tests for ex17"
echo "========================"

echo "Creating database ..."
$VALGRIND ./ex17 512 100 $DB_FILE c 2>&1 | grep "ERROR SUMMARY"

echo "Inserting records..."
$VALGRIND ./ex17 512 100 $DB_FILE s 1 "zed" "zed@zedshaw.com" 2>&1 | grep "ERROR SUMMARY"
$VALGRIND ./ex17 512 100 $DB_FILE s 2 "frank" "frank@zedshaw.com" 2>&1 | grep "ERROR SUMMARY"
$VALGRIND ./ex17 512 100 $DB_FILE s 3 "joe" "joe@zedshaw.com" 2>&1 | grep "ERROR SUMMARY"

echo "Listing records..."
$VALGRIND ./ex17 512 100 $DB_FILE l 2>&1 | grep "ERROR SUMMARY"

echo "Deleting record 3..."
$VALGRIND ./ex17 512 100 $DB_FILE d 3 2>&1 | grep "ERROR SUMMARY"

echo "Listing records after deletion..."
$VALGRIND ./ex17 512 100 $DB_FILE l 2>&1 | grep "ERROR SUMMARY"

echo "Getting record 2..."
$VALGRIND ./ex17 512 100 $DB_FILE g 2 2>&1 | grep "ERROR SUMMARY"

echo "========================"
echo " All tests completed."
echo "========================"
