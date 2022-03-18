// This file is part of the course TPV2@UCM - Samir Genaim

#include <iostream>

#include "game/Game.h"

int main(int ac, char **av) {

	try {
		//Se inicializa el juego, donde se controla todo
		Game* g = new Game();
		g->start();
		g->update();
	} catch (const std::string &e) { // catch exceptions thrown as strings
		std::cerr << e << std::endl;
	} catch (const char *e) { // catch exceptions thrown as char*
		std::cerr << e << std::endl;
	} catch (const std::exception &e) { // catch exceptions thrown as a sub-type of std::exception
		std::cerr << e.what();
	} catch (...) {
		std::cerr << "Caught and exception of unknown type ...";
	}

	return 0;
}

