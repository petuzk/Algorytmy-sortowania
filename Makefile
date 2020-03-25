all:
	g++ -g --std=c++11 src/*.cpp src/*/*.cpp -I. -D INSTANTIATE_TEMPLATE_TYPE=int -o driver
