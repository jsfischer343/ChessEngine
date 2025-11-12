all:
	g++ game.cc positiontree.cc position.cc utilities.cc bluespiralv4.cpp -o bluespiralv4
	chmod +x bluespiralv4
debug:
	g++ -g game.cc positiontree.cc position.cc utilities.cc bluespiralv4.cpp -o bluespiralv4
	chmod +x bluespiralv4
	gdb -q ./bluespiralv4
clean:
	rm bluespiralv4 -f
	rm massif* -f
