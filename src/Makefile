all:
	g++ uci.cc cli.cc positiontree.cc position.cc main.cc -o bluespiral
	chmod +x bluespiral
debug:
	g++ -g uci.cc cli.cc positiontree.cc position.cc main.cc -o bluespiral
	chmod +x bluespiral
	gdb -q ./bluespiral
clean:
	rm bluespiral -f
	rm massif* -f
