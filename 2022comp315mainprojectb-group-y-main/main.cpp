/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
 //r.update

  /*
   * File:   main.cpp
   * Author: MSCS
   *
   * Created on 12 March 2020, 8:38 AM
   *
   */
   //updated



#include "board.h"
#include "agent.h"
#include <iostream>
#include <fstream>

  //using std::cout;
  //using std::cin;


using namespace std;

int main(int argc, char** argv) {
	int i = 0;
	int input = 0;
	char symbol = '@';
	vector<string> statesV;//vectot to stor all states
	vector<string> movesV;//vector to store all moves
	string sChoice = "s";
	double speed = 0.4;//to store user prefered speed of play
	string newLine = "y";//to ask user whether board states shuld be printed on new line or cleared
	bool bi = false;//
	while (i != 3)
	{

		cout << "Welcome to the abalone board game." << endl;
		cout << "please select one of the options below: " << endl;
		cout << "press -> 1 ,to play against a bot." << endl;
		cout << "press -> 2 ,to watch a bot play against another bot." << endl;
		cout << "press -> 3 ,if you do not wish to play at all..." << endl;
		cout << "Enter input: "; //<< endl;
		cin >> input;
		
		if (input != 3)
		{
			cout << "show previous moves during the game ? [y/n]!";// << endl;
			cin >> newLine;
			if (newLine == "y" || newLine == "yes")
			{
				bi = false;
			}
			else if (newLine == "n" || newLine == "no")
			{
				bi = true;
			}
			while (input != 1 && input != 2)
			{
				cout << "Invalid input, please Enter input again ?: " << endl;
				cout << "Enter input: ";
				cin >> input;

			}
			if (input == 1)
			{
				cout << "you up against a robot, hope you ready!" << endl;
				//start
				cout << "choose your character - [@/O] ? :";// << endl;
				cin >> symbol;
				userVbot(symbol, statesV, movesV, bi);
				//end
				input = 0;
			}
			else if (input == 2)
			{

				cout << "press [s] for fast, [f] for slow and [sf] for super-fast speed of play:";//<< endl;
				cin >> sChoice;
				if (sChoice == "s") {
					speed = 0.8;

				}
				else if (sChoice == "f")
				{
					speed = 0.3;

				}
				else if (sChoice == "sf")
				{
					speed = 0.0;

				}
				//start
				botVbot(statesV, movesV, speed, bi);
				//session saving of all states
				std::ofstream output_file("savedStates.txt");
				std::ostream_iterator<std::string> output_iterator(output_file, "\n");
				std::copy(statesV.begin(), statesV.end(), output_iterator);
				//session saving of all moves
				std::ofstream output_file2("savedMoves.txt");
				std::ostream_iterator<std::string> output_iterator2(output_file2, "\n");
				std::copy(movesV.begin(), movesV.end(), output_iterator2);
				//end
				input = 0;
			}
			char rep = 'n';
			cout << "Would you like to watch a replay of your game?[y/n]";cin >> rep;
			if (rep == 'y') {
				int a = 0, b = movesV.size();
				cout << "Enter A to watch the entire game or B to replay only a portion: ";
				cin >> rep;
				if (rep == 'B') {
					cout << "Enter the first index: ";cin >> a;
					cout << "Enter the last index: ";cin >> b;
				}
				for (int i = a;i < b;i++) {
					cout << movesV[i] << endl;
					cout << statesV[i] << endl;
					SLP(0.2);
					system(CL);
				}
			}
		}
		else
		{

			cout << "Goodbye !!!" << endl;
			break;
		}


	}



}
