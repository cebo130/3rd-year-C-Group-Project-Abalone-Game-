#pragma once
#pragma once
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

 /*
  * File:   board.h
  * Author: MSCS
  *
  * Created on 12 March 2020, 8:55 AM
  */

#ifndef BOARD_H
#define BOARD_H

#include "cell.h"
#include <map>
#include <sstream>

using namespace std;

class board {
    vector<cell*> rows;
    map<string, cell*> cells;
    int woc;//white marbles off the board count.
    int boc;//black marbles off the board count.
    void mapCells(cell* row, const char& r, const int& z);
public:
    board();//create 61 cells on the heap, and connect them.
    board(const board&);//copy constructor
    ~board();//recycle 61 cells.
    map<string, cell*> getCells() const; // returns the map that contains locations as keys and cells as values
    bool inPlay() const;//checks if the number of marbles off the board are enough to terminate the game
    operator std::string() const;//cast this object into a string.
    string traverseHorizontal() const;
    string traverseDiagonal() const;
    string traverseAny(const string& corner, const int& pd, const int& sd, const int& td) const;
    bool validateMove(const char& m, const string& l, const int& n, const int& fd, const int& md, int& mtype, bool& scoreMove) const;
    bool executeMove(const char& m, const string& l, const int& n, const int& fd, const int& md);
    void refreshOffboardCounts(string& winner_);//checks how many white and black marbles are still in the board and updates woc and boc accordingly
};



#endif /* BOARD_H */
