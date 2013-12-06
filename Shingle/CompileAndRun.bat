echo off

gcc -std=c99 -c "C Files\shingle.c" -o "Object Files\shingle.o"

gcc -std=c99 "Object Files\shingle.o" "..\List-Library\Object Files\List.o" -o shingle

shingle -i Input.txt -o Output.csv -s 3 -d
pause