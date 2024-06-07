#!/bin/bash

c_file="kurock.c"
race_checker="race_catch.c"
executable_file="kurock"
executable_file_check="race"

echo "================================"
echo "Building KUROCK program..."
gcc -o "$executable_file" "$c_file"
if [ $? -ne 0 ]; then
    echo "Error: Failed to build KUROCK program."
    exit 1
fi
echo "================================"

echo "KUROCK program built successfully."


echo "================================"
echo "실행할 락 모드를 선택하세요 (rwr, rww, seq) : "
read -r mod
echo "실행할 읽기 스레드 개수를 입력하세요 : "
read -r rthread
echo "실행할 읽기 frequency를 입력하세요 : "
read -r rfreq
echo "실행할 쓰기 스레드 개수를 입력하세요 : "
read -r wthread
echo "실행할 쓰기 frequency를 입력하세요 : "
read -r wfreq
echo "실행시간을 입력하세요: "
read -r exectime
echo "==============================="



echo "Running KUROCK program with arguments:"
./"$executable_file" "$mod" "$rthread" "$rfreq" "$wthread" "$wfreq" "$exectime"


echo "================================"
echo "Building Validator..."
gcc -o "$executable_file_check" "$race_checker"
if [ $? -ne 0 ]; then
    echo "Error: Failed to build Validator."
    exit 1
fi
echo "================================"
echo "Validator built successfully."

echo "================================"
echo "Validate Race Conditions..."
./"$executable_file_check"




