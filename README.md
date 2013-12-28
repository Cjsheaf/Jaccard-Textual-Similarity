Jaccard-Textual-Similarity
==========================

A collection of programs designed to analyze pairs of text files and calculate how similar their contents are using the <a href="http://en.wikipedia.org/wiki/Jaccard_index">Jaccard similarity</a> formula.

Two programs are included in this repository: Shingle and Jaccard (as well as each program's source code).
 - Shingle.exe takes any corpus of text and "Shingles" (breaks up into overlapping n-gram groups of words) it, placing the results into a .csv (comma separated value) file. The .csv file is used as input for Jaccard.exe.
 - Jaccard.exe accepts any number of .csv files containing n-gram shingles and uses the jaccard similarity formula to print the similarity percentage for all combinations of input files.

How to compile (either executable):

  1) <a href="https://github.com/Cjsheaf/List-Library">List-Library</a> is a git submodule dependency, so after cloning this repository, run the following two commands: "git submodule init" and "git submodule update". The latter command will need to be run again if <a href="https://github.com/Cjsheaf/List-Library">List-Library</a> is ever updated.

  2) Compile the .c file for the chosen program into an .o (object) file.
  
  3) Link the .o file from step 2 with the "List.o" object file from the List Library obtained in step 1, into the final executable.
  
  4) Read the Readme.txt in the appropriate sub-directory for information on what arguments the executable expects.
