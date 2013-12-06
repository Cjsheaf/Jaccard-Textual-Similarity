gcc -std=c99 -c "C Files\jaccard.c" -o "Object Files\jaccard.o"

gcc -std=c99 "Object Files\jaccard.o" "..\List-Library\Object Files\List.o" -o jaccard