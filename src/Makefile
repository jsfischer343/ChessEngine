all:
	g++ uci.cc cli.cc positiontree.cc position.cc main.cc -o bluespiral
	chmod +x bluespiral
static:
	g++ uci.cc cli.cc positiontree.cc position.cc main.cc -o bluespiral -static
	chmod +x bluespiral
windows32:
	i686-w64-mingw32-g++ uci.cc cli.cc positiontree.cc position.cc main.cc -o bluespiral_w32.exe  -static
windows64:
	x86_64-w64-mingw32-g++ uci.cc cli.cc positiontree.cc position.cc main.cc -o bluespiral_w64.exe -static
debug:
	g++ -g uci.cc cli.cc positiontree.cc position.cc main.cc -o bluespiral
	chmod +x bluespiral
clean:
	rm bluespiral -f
	rm bluespiral_w32.exe -f
	rm bluespiral_w64.exe -f
	rm massif* -f
