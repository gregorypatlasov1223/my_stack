FLAGS ?=  -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef \
-Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ \
-Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion \
-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 \
-Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers \
-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo \
-Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits \
-Wwrite-strings -Werror=vla -D _DEBUG -D _EJUDGE_CLIENT_SIDE

all: main.exe

main.exe: main.o stack.o arithm_oper.o calculator.o
	g++ $(FLAGS) main.o stack.o arithm_oper.o calculator.o -o main.exe

main.o: main.cpp
	g++ $(FLAGS) -c main.cpp

stack.o: stack.cpp
	g++ $(FLAGS) -c stack.cpp

arithm_oper.o: arithm_oper.cpp
	g++ $(FLAGS) -c arithm_oper.cpp

calculator.o: calculator.cpp
	g++ $(FLAGS) -c calculator.cpp

clean:
	rm -rf *.o *.exe
