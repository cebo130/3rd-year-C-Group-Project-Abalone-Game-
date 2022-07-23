/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <time.h>
#include "agent.h"
using namespace std;

movement::movement(const string& ll, const int& nn, const int& f, const int& m, const int& d, const int& mt, const bool& sm) :
    l(ll), n(nn), fd(f), md(m), d2e(d), mtype(mt), scoreMove(sm) {}
movement::movement(const movement& copy) : l(copy.l), n(copy.n), fd(copy.fd), md(copy.md), d2e(copy.d2e), mtype(copy.mtype), scoreMove(copy.scoreMove) {}

agent::agent(const char& c) : symbol(c) {
    for (int n = 1;n < 4;n++) {
        for (int fd = 0;fd < 6;fd++) {//all 6 directions
            if (n == 1) {//will create 6 moves for n=1
                vector<int> tv;
                tv.push_back(n);
                tv.push_back(fd);
                tv.push_back(getAdjacentIndex(fd));
                pmd.push_back(tv);
            }
            else {//will create a total of 60 moves for n=2 and n=3
                for (int md = 0;md < 6;md++) {
                    if (md != fd) {
                        vector<int> tv;
                        tv.push_back(n);
                        tv.push_back(fd);
                        tv.push_back(md);
                        pmd.push_back(tv);
                    }
                }
            }
        }
    }
}
vector<movement> agent::findMoves(const board& bd) const {
    vector<movement> mv;
    map<string, cell*> cells = bd.getCells();
    for (map<string, cell*>::iterator it = cells.begin();it != cells.end();it++) {
        string l = it->first;
        cell* cp = it->second;
        if (cp != nullptr && cp->getMarble() == symbol) {
            set<int> mdSet;//directions in which neighbors either have a space or an opponent marble
            for (int md = 0;md < 6;md++) {
                cell* np = cp->getAdjacent(md);
                if (np != nullptr && np->getMarble() != symbol) {
                    mdSet.insert(md);
                }
            }
            for (vector<int> tv : pmd) {
                int md = tv[2];
                if (mdSet.find(md) != mdSet.end()) {
                    int mtype = 0;//default to in-line with no following marbles
                    bool scoreMove = false;
                    if (bd.validateMove(symbol, l, tv[0], tv[1], md, mtype, scoreMove))
                        mv.push_back(movement(l, tv[0], tv[1], md, cp->getDistanceToEdge(md), mtype, scoreMove));
                }
            }
        }
    }
    return mv;
}
double score(const movement& a) {
    double maxScore = 0;
    if (a.mtype < 0)
        maxScore = 0.6;
    else if (a.mtype > 0)
        maxScore = 0.2;
    else
        maxScore = 0.8;
    if (a.mtype > 0) {
        return maxScore * (1 + (1 - a.d2e) / 9.0);
    }
    else {
        return maxScore * (1 - abs(5 - a.d2e) / 5.0);
    }
}
double scoreB(const movement& a) {
    double maxScore = 0;
    if (a.mtype == 3 && a.scoreMove == true) {
        maxScore = 20;
        return maxScore;
    }
    else if (a.mtype == 2 && a.scoreMove == true) {
        maxScore = 15;
        return maxScore;
    }
}


randAgent::randAgent(const char& c) :agent(c) {}
movement randAgent::move(const movement* const om, board bd) {
    vector<movement> mv = findMoves(bd);
    if (mv.size() == 0) {
        throw string("Error! There are no available moves!");
    }
    //cout <<"The size of the movements vector is: " << mv.size();
    int i = rand() % mv.size();
    return mv[i];
}
distAgent::distAgent(const char& c) :agent(c) {}

movement distAgent::move(movement const* const om, board bd) {
    vector<movement> mv = findMoves(bd);
    sort(mv.begin(), mv.end(), moveTNDPriority());
    const int bm2c = 3;//consider only these top best moves.
    if (mv.size() == 0) {
        throw string("Error! There are no available moves!");
    }
    if (mv.size() < bm2c)
        return mv[0];
    else {
        int i = rand() % bm2c;
        return mv[i];
    }
}
// humanAgent
humanAgent::humanAgent(const char& c) :agent(c) {}

movement humanAgent::move(movement const* const om, board bd) {
    //vector<movement> mv = findMoves(bd);
    string l;int n, f, m;
    int mtype = 0;bool sm = false;
    bool valid = false;
    cout << "Enter your move in the ff order" << endl;
    cout << "location n fd md -->: ";cin >> l >> n >> f >> m;
    valid = bd.validateMove(symbol, l, n, f, m, mtype, sm);
    while (valid == false) {
        int mtype = 0;bool sm = false;
        cout << "Entered move can not be executed under the current board state!" << endl;
        cout << "Enter your move in the ff order" << endl;
        cout << "location n fd md -->: ";cin >> l >> n >> f >> m;
        valid = bd.validateMove(symbol, l, n, f, m, mtype, sm);
    }
    map<string, cell*> cellsMap = bd.getCells();
    cell* cp = cellsMap.at(l);
    int d2edge = cp->getDistanceToEdge(m);
    return movement(l, n, f, m, d2edge, mtype, sm);
}

//new functions

//function that gives a score to a movement
double rating(const movement& a) {
    double score = 0;
    //set scores according to types of moves and number of marbles pushed
    if (a.n == 3 && a.mtype == 2 && a.scoreMove) {
        score = 4.5;
    }
    else if (a.n == 2 && a.mtype == 1 && a.scoreMove) {
        score = 4;
    }
    else if (a.n == 3 && a.mtype == 1 && a.scoreMove) {
        score = 3.5;
    }
    else if (a.n == 3 && a.mtype == 2) {
        score = 3;
    }
    else if (a.n == 2 && a.mtype == 1) {
        score = 2.5;
    }
    else if (a.n == 3 && a.mtype == 1) {
        score = 2;
    }
    else if (a.n == 3 && a.mtype == -1 || a.mtype == 0) {//inline and broadside moves with 3 marbles
        score = 1.5;
    }
    else if (a.n == 2 && a.mtype == -1 || a.mtype == 0) {
        score = 1;
    }
    else {
        score = 0.5;
    }
    return score;
}
// function that gives preference according to which movement is closer to the leading marble in the opponents previous move
double distBetweenMovements(const string& location1, const string& location2) {
    board bd;
    double dis = 0; // the smallest distance is 1(between neighbors) and the greatest distance is 8(between opposite corners)
    map<string, cell*> cellsMap = bd.getCells();
    cell* cellA = cellsMap.at(location1);
    cell* cellB = cellsMap.at(location2);
    for (int i = 0; i < 6; i++) {
        int num = (cellA->getDistanceToEdge(i) - cellB->getDistanceToEdge(i));
        if (num < 0) {
            num = -1 * num;
        }
        dis += num;
    }
    return ceil(dis / 4);// the lowest number that can be returned by the function is 1 and the highest is 8
}
newAgent::newAgent(const char& c) : agent(c) {};

movement newAgent::move(movement const* const om, board bd)
{
    vector<movement> mv = findMoves(bd);
    string loc = om->l;
    sort(mv.begin(), mv.end(), compare());//compare(loc)
    const int bm2c = 4; // consider only these top best moves.
    if (mv.size() == 0)
    {
        throw string("Error! There are no available moves!");
    }
    if (mv.size() < bm2c)
        return mv[0];
    else
    {
        int i = rand() % bm2c;
        return mv[i];
    }
}


//function that facilitates the game between a user versus bot
int userVbot(const char& char1, vector<string>& states, vector<string>& moves,bool bi) {
    if (char1 != '@' && char1 != 'O') {
        throw string("Parameter must be O or @ only!");
    }
    char bot;
    if (char1 == '@') {
        bot = 'O';
    }
    else if (char1 == 'O') {
        bot = '@';
    }
    
    if (bi == true)
    {
        system(CL);//**********************************234
    }
    SLP(1);
    time_t now = time(NULL);
    srand(now);
    cout << "Seed: " << now << endl;
    board abalone;
    string state(abalone);
    moves.push_back("Initial State");
    states.push_back(state);
    cout << "Initial State:" << endl << state;
    agent* w = new humanAgent(char1);
    agent* b = new distAgent(bot);
    //int x = 0;bool y = false;
    movement* pom = nullptr;//new movement("I5",0,0,0,0,0,false); // previous opponent's move
    movement* mym = nullptr; // player's chosen move
    char c = char1;
    int i = 1;
    string winner = "No player";
    string yon= "y";
    while (abalone.inPlay())
    {
        try
        {
            if (c == char1)
            {
                movement mm = w->move(pom, abalone);
                mym = new movement(mm);
                if (pom)
                    delete pom;
                pom = new movement(mm);
            }
            else
            {
                movement mm = b->move(pom, abalone);
                mym = new movement(mm);
                if (pom)
                    delete pom;
                pom = new movement(mm);
            }
        }
        catch (const string& s)
        {
            cout << s;
            return 1;
        }
        bool valid = abalone.executeMove(c, mym->l, mym->n, mym->fd, mym->md);
        if (bi == true)
        {
            system(CL);//**********************************283
        }
        if (valid)
        {
            cout << "Move " << i << ": " << c << "," << mym->l << "," << mym->n << "," << mym->fd << "," << mym->md << " (Bot)" << endl;
            string state(abalone);
            cout << "Next State:" << endl << state;
            string moveStr = "Move " + to_string(i) + " : " + c + "," + mym->l + "," + to_string(mym->n) + "," + to_string(mym->fd) + ", Bot )";
            moves.push_back(moveStr); //store the moves
            states.push_back(state); //parallelly store the corresponding state
        }
        else
        {
            cout << "Invalid move!!" << endl;
            string state(abalone);
            cout << "Next State:" << endl << state;
            string moveStr = "Move " + to_string(i) + " : " + c + "," + mym->l + "," + to_string(mym->n) + "," + to_string(mym->fd) + "," + to_string(mym->md) + "(" + "Bot" + ")";
            moves.push_back(moveStr + "\nInvalid Move!"); //store the moves
            states.push_back(state); //parallelly store the corresponding state
        }

        delete mym;
        abalone.refreshOffboardCounts(winner);
        if (c == char1)
            c = bot;
        else
            c = char1;
        i++;
    }
    if (pom)
        delete pom;
    delete w;
    delete b;
    if (bi == true)
    {
        system(CL);//***********************************************318
    }
    string final_state(abalone);
    cout << "Final State: " << endl << final_state;
    cout << "The total number of moves made is: " << i - 1 << endl;
    cout << winner << " wins this round of the game!" << endl;
    return 0;
}
//ending of user vs bot
//start of bot vs bot
int botVbot(vector<string>& states, vector<string>& moves,double speed,bool bi) {
    /*start*/
   // string newLine = "y";
    string winBet;
    string yon = "y";
    int movesRange1, movesRange2 = 0;
    string movCheck = "";
    double diff = 0.0;
    double balance = 1500.0;
    cout << "Your initial balance is R1 500.00" << endl;
    cout << "Betting costs R1 500.00, would you like to bet [y/n]?:" << endl;
    cin >> yon;
    if (yon=="y" || yon=="yes")//*******************************************************
    {
        //start
        cout << "and who do you place your bet on, black or white (B/W)?: ";
        cin >> winBet;
        for (auto& ch : winBet)
        {
            ch = tolower(ch);
        }
        if (winBet == "b" || winBet =="black") {
            winBet = "black";
        }
        else if (winBet == "w" || winBet == "white") {
            winBet = "white";
        }
        cout << winBet << endl;
        cout << "How many moves will there be, A-[50-99],B-[100-199],C-[200-299] or D-[300-399] ?: ";
        cin >> movCheck;
        //string test = "Hello World";
        for (auto& ch : movCheck)
        {
            ch = tolower(ch);
        }

        //cout << movCheck << endl;

        if (movCheck == "a") {
            movesRange1 = 50;
            movesRange2 = 99;
        }
        else if (movCheck == "b") {
            movesRange1 = 100;
            movesRange2 = 199;
        }
        else if (movCheck == "c") {
            movesRange1 = 200;
            movesRange2 = 299;
        }
        else if (movCheck == "d") {
            movesRange1 = 300;
            movesRange2 = 399;
        }
        /*end*/
        if (bi == true)
        {
            system(CL);
        }
        //remember to uncomment
        SLP(1);
        time_t now = time(NULL);
        srand(now);
        board abalone;
        string state(abalone);
        cout << "Seed: " << now << endl;
        cout << "Initial State:" << endl << state;
        //SLP(1);
        if (bi == true)
        {
            system(CL);
        }
        SLP(1);
        moves.push_back("Initial State");
        states.push_back(state);
        agent* w = new newAgent('O');
        agent* b = new distAgent('@');
        movement* pom = new movement("I5", 0, 0, 0, 0, 0, false); // previous opponent's move
        movement* mym = nullptr; // player's chosen move
        char c = 'O';
        int i = 1;
        string winner = "No player";

        while (abalone.inPlay())
        {
            try
            {
                if (c == 'O')
                {
                    movement mm = w->move(pom, abalone);
                    mym = new movement(mm);
                    if (pom)
                        delete pom;
                    pom = new movement(mm);
                }
                else
                {
                    movement mm = b->move(pom, abalone);
                    mym = new movement(mm);
                    if (pom)
                        delete pom;
                    pom = new movement(mm);
                }
            }
            catch (const string& s)
            {
                cout << s;
                return 1;
            }
            bool valid = abalone.executeMove(c, mym->l, mym->n, mym->fd, mym->md);
            string player;
            if (i % 2 == 0) {
                player = "Bot 2";
            }
            else {
                player = "Bot 1";
            }

            if (valid)
            {
                string state(abalone);
                cout << "Move " << i << ": " << c << "," << mym->l << "," << mym->n << "," << mym->fd << "," << mym->md << "(" << player << ")" << endl;
                string moveStr = "Move " + to_string(i) + " : " + c + "," + mym->l + "," + to_string(mym->n) + "," + to_string(mym->fd) + "," + to_string(mym->md) + "(" + player + ")";
                moves.push_back(moveStr); //store the moves
                states.push_back(state); //parallelly store the corresponding state
                cout << "Next State:" << endl << state;
                //remember to uncomment
                SLP(speed);
                if (bi == true)
                {
                    system(CL);
                }
                
                //SLP(0.4);
            }
            else
            {
                cout << "Invalid move!!" << endl;
                //remember to uncomment
                //SLP(0.1);
                if (bi == true)
                {
                    system(CL);
                }
                //SLP(0.1);
                string moveStr = "Move " + to_string(i) + " : " + c + "," + mym->l + "," + to_string(mym->n) + "," + to_string(mym->fd) + "," + to_string(mym->md) + "(" + player + ")";
                moves.push_back(moveStr + "\nInvalid Move!"); //store the moves
                states.push_back(state); //parallelly store the corresponding state
            }
            if (c == 'O')
                c = '@';
            else
                c = 'O';
            delete mym;
            abalone.refreshOffboardCounts(winner);
            //SLP(1);
            //system(CL);
            i++;
        }
        if (pom)
            delete pom;
        delete w;
        delete b;
        //cout << "Seed: " << now << endl;
        //cout << "Initial State: " << endl << initialState;
        string final_state(abalone);
        cout << "Final State: " << endl << final_state;
        int numOfMoves = i - 1;
        if (winner == winBet) {
            cout << "Hooray, your player won !!" << endl;
            balance = balance + 500.0;
            diff = balance - 1500;
            cout << "You've just earned R " << diff << endl;
            if (numOfMoves > movesRange1 && numOfMoves < movesRange2) {
                cout << "The range " << movesRange1 << "- to -" << movesRange2 << " moves ,that you chose is correct" << endl;
                balance = balance + 500;
                cout << "Here's an extra R500 for that." << endl;
            }
            else {
                cout << "Eish, but you chose the wrong range..." << endl;
                cout << "The range " << movesRange1 << "- to -" << movesRange2 << " moves ,that you chose is incorrect" << endl;
                cout << "Because " << numOfMoves << " moves is not within that range of moves you chose." << endl;
            }

        }
        else {
            cout << "Ooops , your player lost ." << endl;
            if (numOfMoves > movesRange1 && numOfMoves < movesRange2) {
                cout << "The range " << movesRange1 << "- to -" << movesRange2 << " moves ,that you chose is correct" << endl;
                balance = balance + 500;
                cout << "Here's R500 for that." << endl;
            }
            else {
                cout << "Eish, you also chose the wrong range..." << endl;
                cout << "The range " << movesRange1 << "- to -" << movesRange2 << " moves ,that you chose is incorrect" << endl;
                cout << "Because " << numOfMoves << " moves is not within that range of moves you chose." << endl;
            }
            balance = balance - 1500.0;
        }
        cout << "Your new balance is R " << balance << endl;
        cout << "The total number of moves made is: " << i - 1 << endl;
        cout << winner << " wins this round of the game!" << endl;
        //end
    }//******************************************************************************************
    else if (yon=="n" || yon=="no")
    {
    if (bi == true)
    {
        if (bi == true)
        {
            system(CL);
        }
    }
        SLP(1);
        time_t now = time(NULL);
        srand(now);
        board abalone;
        string state(abalone);
        cout << "Seed: " << now << endl;
        cout << "Initial State:" << endl << state;
        //SLP(1);
        if (bi == true)
        {
            system(CL);
        }
        SLP(1);
        moves.push_back("Initial State");
        states.push_back(state);
        agent* w = new newAgent('O');
        agent* b = new distAgent('@');
        movement* pom = new movement("I5", 0, 0, 0, 0, 0, false); // previous opponent's move
        movement* mym = nullptr; // player's chosen move
        char c = 'O';
        int i = 1;
        string winner = "No player";

        while (abalone.inPlay())
        {
            try
            {
                if (c == 'O')
                {
                    movement mm = w->move(pom, abalone);
                    mym = new movement(mm);
                    if (pom)
                        delete pom;
                    pom = new movement(mm);
                }
                else
                {
                    movement mm = b->move(pom, abalone);
                    mym = new movement(mm);
                    if (pom)
                        delete pom;
                    pom = new movement(mm);
                }
            }
            catch (const string& s)
            {
                cout << s;
                return 1;
            }
            bool valid = abalone.executeMove(c, mym->l, mym->n, mym->fd, mym->md);
            string player;
            if (i % 2 == 0) {
                player = "Bot 2";
            }
            else {
                player = "Bot 1";
            }

            if (valid)
            {
                string state(abalone);
                cout << "Move " << i << ": " << c << "," << mym->l << "," << mym->n << "," << mym->fd << "," << mym->md << "(" << player << ")" << endl;
                string moveStr = "Move " + to_string(i) + " : " + c + "," + mym->l + "," + to_string(mym->n) + "," + to_string(mym->fd) + "," + to_string(mym->md) + "(" + player + ")";
                moves.push_back(moveStr); //store the moves
                states.push_back(state); //parallelly store the corresponding state
                cout << "Next State:" << endl << state;
                //remember to uncomment
                SLP(speed);
                if (bi == true)
                {
                    system(CL);
                }
                //SLP(0.4);
            }
            else
            {
                cout << "Invalid move!!" << endl;
                //remember to uncomment
                //SLP(0.1);
                if (bi == true)
                {
                    system(CL);
                }
                //SLP(0.1);
                string moveStr = "Move " + to_string(i) + " : " + c + "," + mym->l + "," + to_string(mym->n) + "," + to_string(mym->fd) + "," + to_string(mym->md) + "(" + player + ")";
                moves.push_back(moveStr + "\nInvalid Move!"); //store the moves
                states.push_back(state); //parallelly store the corresponding state
            }
            if (c == 'O')
                c = '@';
            else
                c = 'O';
            delete mym;
            abalone.refreshOffboardCounts(winner);
            //SLP(1);
            //system(CL);
            i++;
        }
        if (pom)
            delete pom;
        delete w;
        delete b;
        //cout << "Seed: " << now << endl;
        //cout << "Initial State: " << endl << initialState;
        string final_state(abalone);
        cout << "Final State: " << endl << final_state;
        cout << "The total number of moves made is: " << i - 1 << endl;
        cout << winner << " wins this round of the game!" << endl;
        //end
    }
    else {
    
    }


    return 0;
}
