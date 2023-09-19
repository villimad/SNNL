SET MAIN=./main.cpp
SET HEADERS=-I./headers
SET SOURCE=./headers/sublayer.cpp ./headers/neuron.cpp
CALL g++ %HEADERS% %SOURCE% %MAIN% -o main -std=c++14 