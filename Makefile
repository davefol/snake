build:
	g++ -std=c++17 -stdlib=libc++ main.cpp -o snake -I include -L lib -l SDL2-2.0.0
