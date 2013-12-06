gcc -std=c99 -c "C Files\shingle.c" -o "Object Files\shingle.o"

gcc -std=c99 "Object Files\shingle.o" "..\List-Library\Object Files\List.o" -o shingle