.PHONY: exe2c

exe2c:
	g++ -std=c++20 -Wshadow -Wall -Wfatal-errors -O2 exe2c.cpp -o exe2c
