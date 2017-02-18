#!/bin/bash
Optimisation="-O2" # Оптимизация
Compiler="g++" # Компилятор
Libartes="-lboost_system -lboost_filesystem -lpthread -lmysqlclient -lmysqlpp" # Библиотеки
Includes="-I./" # Где искать include
Src="*.cpp" # Где искать include
$Compiler -std=c++11 -o $1 -Wall $Optimisation $Libartes $Includes $Src # Собственно, сам вызов
#Стандарт C++11
