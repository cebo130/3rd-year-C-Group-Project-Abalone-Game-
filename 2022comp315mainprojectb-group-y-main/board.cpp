/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "board.h"
#include <iomanip>
#include <iostream>
string traverse(const cell* start, const int& pd, const int& sd, const int& td) {
    string ts;
    const cell* linep = start;
    while (linep != nullptr) {
        const cell* cellp = linep;
        while (cellp != nullptr) {
            ts += (cellp->getLocation() + ' ');
            cellp = cellp->getAdjacent(td);
        }
        if (linep->getAdjacent(pd) == nullptr)
            linep = linep->getAdjacent(sd);
        else
            linep = linep->getAdjacent(pd);
    }
    return ts;
}
void setMarbles(cell* row, const int& z, const char& m) {
    for (int i = 0;i < z;i++) {
        row[i].setMarble(m);
    }
    if (z == 7) {
        char pos[4] = { 0,1,5,6 };
        for (int i = 0;i < 4;i++) {
            row[pos[i]].setMarble('+');
        }
    }
}
void connectHorizontal(cell* row, const int& rz) {
    for (int i = 0;i < rz - 1;i++) {
        row[i].setAdjacent(row + i + 1, 1);
    }
}

void connectDiagonal(cell* lrow, cell* srow, const int& f, const int& sz, const bool& top) {
    for (int i = f;i < (f + sz);i++) {
        if (top) {
            if (f == 0) {//connecting from cell 0 in longer row
                lrow[i].setAdjacent(srow + i, 0);
            }
            else if (f == 1) {//connecting from cell 1 in longer row
                lrow[i].setAdjacent(srow + i - 1, 5);
            }
            else return;
        }
        else {
            if (f == 0) {
                lrow[i].setAdjacent(srow + i, 2);
            }
            else if (f == 1) {
                lrow[i].setAdjacent(srow + i - 1, 3);
            }
            else return;
        }
    }
}
void board::mapCells(cell* row, const char& r, const int& z) {
    int f = 1;
    if (r >= 'A' && r <= 'E') {
        f = 1;
    }
    else if (r > 'E' && r <= 'I') {
        f = r - 'E' + 1;
    }
    else return;
    for (int i = 0;i < z;i++) {
        stringstream ss;
        ss << r;
        ss << (i + f);
        string lc = ss.str();
        row[i].setLocation(lc);
        cells[lc] = row + i;
    }
}
board::board() :woc(0), boc(0) {
    char r = 'I';
    for (int i = 0;i < 9;i++) {
        if (i < 5) {
            rows.push_back(new cell[5 + i]);
            connectHorizontal(rows[i], 5 + i);
            mapCells(rows[i], r--, 5 + i);
            if (i < 3) {
                setMarbles(rows[i], 5 + i, 'O');
            }
            else
                setMarbles(rows[i], 5 + i, '+');
        }
        else {
            rows.push_back(new cell[5 + 8 - i]);
            connectHorizontal(rows[i], 5 + 8 - i);
            mapCells(rows[i], r--, 5 + 8 - i);
            if (i >= 6) {
                setMarbles(rows[i], 5 + 8 - i, '@');
            }
            else
                setMarbles(rows[i], 5 + 8 - i, '+');
        }
    }
    for (int i = 1;i < 9;i++) {
        if (i < 5) {
            connectDiagonal(rows[i], rows[i - 1], 0, 5 + i - 1, true);
            connectDiagonal(rows[i], rows[i - 1], 1, 5 + i - 1, true);
        }
        else {
            connectDiagonal(rows[i - 1], rows[i], 0, 5 + 8 - i, false);
            connectDiagonal(rows[i - 1], rows[i], 1, 5 + 8 - i, false);
        }
    }
    cell* linep = rows[0];//start at I5
    while (linep != nullptr) {
        cell* cellp = linep;
        while (cellp != nullptr) {
            for (int i = 0;i < 6;i++) {
                cellp->setDistanceToEdge(i);
            }
            cellp = cellp->getAdjacent(1);
        }
        if (linep->getAdjacent(3) == nullptr)
            linep = linep->getAdjacent(2);
        else
            linep = linep->getAdjacent(3);
    }
}

board::board(const board& copy) : board() {//constructor delegation - C++11
    woc = copy.woc;
    boc = copy.boc;
    map<string, cell*>::const_iterator it = copy.cells.begin();
    while (it != copy.cells.end()) {
        (cells[it->first])->setMarble((*(it->second)).getMarble());
        it++;
    }
}
map<string, cell*> board::getCells() const {
    return cells;
}
board::~board() {
    for (int i = 0;i < 9;i++) {
        delete[] rows[i];
        rows[i] = nullptr;
    }
    cells.clear();
}

board::operator std::string() const {
    stringstream ss;
    char r = 'I';
    int d = 9;
    for (int i = 0;i < 9;i++) {
        if (i < 5) {
            ss << setw(5 - i) << r << " ";
            for (int k = 0;k < 5 + i;k++) {
                ss << rows[i][k].getMarble() << " ";
            }
            ss << endl;
            r--;
        }
        else {
            ss << setw(i - 3) << r << " ";
            for (int k = 0;k < 5 + 8 - i;k++) {
                ss << rows[i][k].getMarble() << " ";
            }
            ss << d << endl;
            r--;
            d--;
        }
    }
    d = 1;
    ss << setw(8) << d++ << " ";
    for (;d < 6;d++)
        ss << d << " ";
    ss << endl;
    for (int i = 0;i < woc;i++)
        ss << 'O' << " ";
    ss << endl;
    for (int i = 0;i < boc;i++)
        ss << '@' << " ";
    ss << endl;
    return ss.str();
}

string board::traverseDiagonal() const {
    map<string, cell*>::const_iterator it = cells.find(string("E1"));
    cell* start = it->second;
    return traverse(start, 0, 1, 2);
}
string board::traverseHorizontal() const {
    map<string, cell*>::const_iterator it = cells.find(string("A1"));
    cell* start = it->second;
    return traverse(start, 5, 0, 1);
}

string board::traverseAny(const string& corner, const int& pd, const int& sd, const int& td)const {
    map<string, cell*>::const_iterator it = cells.find(string(corner));
    cell* start = it->second;
    if (traverse(start, pd, sd, td).size() == 183) {
        return traverse(start, pd, sd, td);
    }
    else {
        return "The specified traversal is not possible";
    }
}

bool board::validateMove(const char& m, const string& l, const int& n, const int& fd, const int& md, int& mtype, bool& scoreMove) const {
    if (!cell::validateLocation(l))//confirm existence of location
        return false;
    else {
        cell* cp = cells.at(l);//initialize to leading cell
        const char marble = cp->getMarble();
        if (marble != m)//trying to move a space or a marble that is not yours
            return false;
        for (int i = 1;i < n;i++) {
            cp = cp->getAdjacent(fd);
            if (cp == nullptr || marble != cp->getMarble())//confirm availability and type of claimed resources
                return false;
        }
        if (fd == getAdjacentIndex(md)) {//in-line move
            cp = cells.at(l);//initialize to leading cell
            mtype = 0;//how many opponent marbles are in front of the leading marble i.e in the move direction
            for (int i = 0;i < n;i++) {
                cp = cp->getAdjacent(md);
                if (cp != nullptr) {//confirm availability of required resources
                    if (marble == cp->getMarble())//met own marble instead of space
                        return false;
                    if ('+' != cp->getMarble())//if it belongs to opponent
                        mtype++;//count opponent marbles
                    else
                        break;
                }
                else {
                    if (mtype > 0)//opponent's marble will be pushed off the board
                        scoreMove = true;
                    else//if trying to push your own marble off the board
                        return false;
                    break;
                }
            }
            if (mtype >= n)//should only push fewer than n of the opponent's marbles
                return false;
        }
        else {//broad-side move
            mtype = -1;
            if (n <= 1)//broad-side move must use at least 2 marbles
                return false;
            cp = cells.at(l);
            for (int i = 0;i < n;i++) {
                cell* dp = cp->getAdjacent(md);//destination pointer
                if (dp != nullptr && '+' == dp->getMarble())//is there an empty space to move to?
                    cp = cp->getAdjacent(fd);
                else
                    return false;
            }
        }
        return true;
    }
}

bool board::executeMove(const char& m, const string& l, const int& n, const int& fd, const int& md) {
    int moveType = 0;//default to in-line with no following marbles
    bool scoreMove = false;
    bool valid = validateMove(m, l, n, fd, md, moveType, scoreMove);
    if (valid) {
        if (moveType == -1) {//broad-side
            if (cells.find(l) == cells.end()) return false;
            cell* cp = cells.at(l);
            for (int i = 0;i < n;i++) {
                if (cp == nullptr) return false;
                cell* dp = cp->getAdjacent(md);//destination pointer
                if (dp == nullptr) return false;
                dp->setMarble(cp->getMarble());
                cp->setMarble('+');
                cp = cp->getAdjacent(fd);
            }
        }
        else {//in-line
            cell* cp = cells.at(l), * rearp = cp;
            const char marble = cp->getMarble();
            cell* frontp = cp->getAdjacent(md);
            for (int i = 1;i < n;i++) {
                rearp = rearp->getAdjacent(fd);
            }
            if (scoreMove) {
                frontp->setMarble(rearp->getMarble());
                rearp->setMarble('+');
                if (marble == 'O')
                    boc++;
                if (marble == '@')
                    woc++;
            }
            else {
                cell* tipp = frontp;
                for (int i = 0;i < moveType;i++) {
                    tipp = tipp->getAdjacent(md);//looking for empty space
                }
                if (moveType != 0)//tipp is already equal to frontp
                    tipp->setMarble(frontp->getMarble());
                frontp->setMarble(rearp->getMarble());
                rearp->setMarble('+');
            }
        }
    }
    return valid;
}
bool board::inPlay() const { // 6 marbles of the same type off the board terminate the game
    if ((woc < 6) && (boc < 6))
        return true;
    return false;
}
void board::refreshOffboardCounts(string& winner_) {
    int wOnBoard = 0;
    int bOnBoard = 0;

    map<string, cell*>::const_iterator it = cells.begin();
    while (it != cells.end()) {
        if ((cells[it->first])->getMarble() == 'O') {
            wOnBoard++;
        }
        if ((cells[it->first])->getMarble() == '@') {
            bOnBoard++;
        }
        it++;
    }

    woc = 14 - wOnBoard;
    boc = 14 - bOnBoard;
    if (boc == 6 && woc < 6) {
        winner_ = "white";
    }
    else if (woc == 6 && boc < 6) {
        winner_ = "black";
    }
}