all:
	g++ uci.cc cli.cc game.cc positiontree.cc position.cc bluespiral.cpp -o bluespiral
	chmod +x bluespiral
debug:
	g++ -g uci.cc cli.cc game.cc positiontree.cc position.cc bluespiral.cpp -o bluespiral
	chmod +x bluespiral
	gdb -q ./bluespiral
clean:
	rm bluespiral -f
	rm massif* -f
