/*
	Bluespiral, a simple open source chess engine
	Copyright (C) 2025 John Fischer

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <chrono>
#include "global.hh"
#include "position.hh"
#include "positiontree.hh"
#include "uci.hh"
#include "cli.hh"


void printWelcomeText()
{
	printf("Bluespiral Chess Engine, Version %s, 2025, %s\n",GLOBAL_BLUESPIRAL_VERSION,GLOBAL_CODE_SOURCE);
	printf("%s\n",LICENSE_STATEMENT);
	return;
}

int main()
{
	printWelcomeText();
	std::string userInput;
	CLI bluespiralCLI;
	do {
		getline(std::cin, userInput);
		bluespiralCLI.parseCommand(userInput);
	} while(userInput!="exit" && userInput!="quit");
	return 0;
}
