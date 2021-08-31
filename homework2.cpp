
#include <iostream>
#include <bits/stdc++.h>
#include <chrono>
#include <unistd.h>

using namespace std;

struct valid_move
{
    char action;
    int curr_row;
    int curr_col;
    int next_row;
    int next_col;
};

struct boards_and_moves
{
    vector<vector<valid_move>> moves;
    vector<vector<string>> boards;
};

int depth;
double MAX = 50000,MIN = -50000,remaining_time;


bool is_white_king(char c){
    if(c=='W') return true;
    else return false;
}

bool is_black_king(char c){
    if(c=='B') return true;
    else return false;
}

bool is_white_pawn(char c)
{
    if(c=='w') return true;
    else return false;
}

bool is_black_pawn(char c)
{
    if(c=='b') return true;
    else return false;
}

bool is_white_vulnerable(vector<string> board,int i,int j)
{
    if(i+1>=0 and i+1<8 and i-1>=0 and i-1<8 and j+1>=0 and j+1<8 and j-1>=0 and j-1<8) //check the boundary conditions
    {
        if(board[i-1][j-1]=='b' and board[i+1][j+1]=='.') //case 1
        {
            return true;
        }

        if(board[i-1][j+1]=='b' and board[i+1][j-1]=='.') //case 2
        {
            return true;
        }

        if(board[i-1][j-1]=='B' and board[i+1][j+1]=='.') //case 3
        {
            return true;
        }

        if(board[i-1][j+1]=='B' and board[i+1][j-1]=='.') //case 4
        {
            return true;
        }

        if(board[i-1][j-1]=='.' and board[i+1][j+1]=='B') //case 5
        {
            return true;
        }

        if(board[i-1][j+1]=='.' and board[i+1][j-1]=='B') //case 6
        {
            return true;
        }
    }

    return false;

}
double backrow_score(vector<string> board)
{
    double res = 0;
    int i,j;
    i=7;
    for(j=0;j<8;j++)
    {
       if(is_white_pawn(board[i][j])) res++;

    }

    i=0;
    for(j=0;j<8;j++)
    {
        if(is_black_pawn(board[i][j])) res--;
    }

    return res;
}

double middlebox_score(vector<string> board)
{
    int i,j;
    double res = 0;
    for(i=3;i<=4;i++)
    {
        for(j=2;j<=5;j++)
        {
            if(is_white_pawn(board[i][j]) or is_white_king(board[i][j])) res++;
            if(is_black_pawn(board[i][j]) or is_black_king(board[i][j])) res--;
        }
    }

    return res;
}

int middlerow_score(vector<string> board)
{
    int i,j;
    double res = 0;
    for(i=3;i<=4;i++)
    {
        for(j=0;j<=1;j++)
        {
            if(is_white_pawn(board[i][j]) or is_white_king(board[i][j])) res++;
            if(is_black_pawn(board[i][j]) or is_black_king(board[i][j])) res--;
        }
    }

    for(i=3;i<=4;i++)
    {
        for(j=6;j<=7;j++)
        {
            if(is_white_pawn(board[i][j]) or is_white_king(board[i][j])) res++;
            if(is_black_pawn(board[i][j]) or is_black_king(board[i][j])) res--;
        }
    }

    return res;
}

bool is_black_vulnerable(vector<string> board,int i,int j)
{
    if(i+1>=0 and i+1<8 and i-1>=0 and i-1<8 and j+1>=0 and j+1<8 and j-1>=0 and j-1<8) //check the boundary conditions
    {
        if(board[i+1][j-1]=='w' and board[i-1][j+1]=='.') //case 1
        {
            return true;
        }

        if(board[i+1][j+1]=='w' and board[i-1][j-1]=='.') //case 2
        {
            return true;
        }

        if(board[i-1][j-1]=='W' and board[i+1][j+1]=='.') //case 3
        {
            return true;
        }

        if(board[i-1][j+1]=='W' and board[i+1][j-1]=='.') //case 4
        {
            return true;
        }

        if(board[i-1][j-1]=='.' and board[i+1][j+1]=='W') //case 5
        {
            return true;
        }

        if(board[i-1][j+1]=='.' and board[i+1][j-1]=='W') //case 6
        {
            return true;
        }
    }

    return false;

}

bool is_white_safe(vector<string> board,int i,int j)
{


    if(i+1>=0 and i+1<8 and j+1>=0 and j+1<8 and j-1>=0 and j-1<8 )
    {
        if( is_white_pawn(board[i+1][j+1]) or is_white_king(board[i+1][j+1]) or is_black_pawn(board[i+1][j+1]) )
        {
            if( is_white_pawn(board[i+1][j-1]) or is_white_king(board[i+1][j-1]) or is_black_pawn(board[i+1][j-1]) )
            {
                return true;
            }
        }
    }

    return false;


}

bool is_black_safe(vector<string> board,int i,int j)
{

    if(i-1>=0 and i-1<8 and j+1>=0 and j+1<8 and j-1>=0 and j-1<8 )
    {
        if( is_white_pawn(board[i-1][j+1]) or is_black_king(board[i-1][j+1]) or is_black_pawn(board[i-1][j+1]) )
        {
            if( is_white_pawn(board[i-1][j-1]) or is_black_king(board[i-1][j-1]) or is_black_pawn(board[i-1][j-1]) )
            {
                return true;
            }
        }
    }

    return false;
}

bool black_wins(vector<string> board)
{
    int row,col,i,j,k;
    for(i=0;i<8;i++)
    {
        for(j=0;j<8;j++)
        {
            if(is_white_king(board[i][j])) // check for the moves
            {
                for(row=-1;row<=1;row=row+2) // checking for all the four directions simple move
                {
                    for(col=-1;col<=1;col=col+2)
                    {
                        if(i-row>=0 and i-row<8 and j-col>=0 and j-col<8) // check for the boundary
                        {
                            if(board[i-row][j-col] == '.')
                            {
                                return false;
                            }
                        }
                    }
                }
                if(i-2>=0 and i-2<8 and j+2>=0 and j+2<8) //whether right forward jump possible or not ..
                {
                    if(board[i-1][j+1]=='b' or board[i-1][j+1]=='B') //kill piece available or not
                    {
                        if(board[i-2][j+2]=='.') //empty place after the jump
                        {
                            return false;

                        }
                    }
                }
                if(i-2>=0 and i-2<8 and j-2>=0 and j-2<8) //left forward jump possible or not ..
                {
                    if(board[i-1][j-1]=='b' or board[i-1][j-1]=='B') //kill piece available or not
                    {
                        if(board[i-2][j-2]=='.') //empty place after the jump
                        {
                            return false;

                        }
                    }
                }
                //------checking for the possibility of the left jump
                if(i+2>=0 and i+2<8 and j+2>=0 and j+2<8) //whether right backward jump possible or not ..
                {
                    if(board[i+1][j+1]=='b' or board[i+1][j+1]=='B') //kill piece available or not
                    {
                        if(board[i+2][j+2]=='.') //empty place after the jump
                        {

                            return false;
                        }
                    }
                }
                if(i+2>=0 and i+2<8 and j-2>=0 and j-2<8) //left forward jump possible or not ..
                {
                    if(board[i+1][j-1]=='b' or board[i+1][j-1]=='B') //kill piece available or not
                    {
                        if(board[i+2][j-2]=='.') //empty place after the jump
                        {

                            return false;
                        }
                    }
                }



            }
            if(is_white_pawn((board[i][j])))
            {
                for(col=-1;col<=1;col=col+2) //check for the simple move
                {
                    if(i-1>=0 and i-1<8 and j-col>=0 and j-col<8) // check for the boundary
                    {

                        if(board[i-1][j-col] == '.')
                        {
                            return false;
                        }
                    }
                }
                if(i-2>=0 and i-2<8 and j+2>=0 and j+2<8) //whether right jump possible or not ..
                {
                    if(board[i-1][j+1]=='b' or board[i-1][j+1]=='B') //kill piece available or not
                    {

                        if(board[i-2][j+2]=='.') //empty place after the jump
                        {

                            return false;
                        }
                    }
                }
                if(i-2>=0 and i-2<8 and j-2>=0 and j-2<8) //right jump possible or not ..
                {
                    if(board[i-1][j-1]=='b' or board[i-1][j-1]=='B') //kill piece available or not
                    {
                        if(board[i-2][j-2]=='.') //empty place after the jump
                        {

                            return false;
                        }
                    }
                }




            }

        }
    }

    return true;
}

bool white_wins(vector<string> board)
{
    int i,j,row,col;
    for(i=0;i<8;i++)
    {
        for(j=0;j<8;j++)
        {
            if(is_black_king(board[i][j]))
            {

                for(row=-1;row<=1;row=row+2) // checking for all the four directions simple move
                {
                    for(col=-1;col<=1;col=col+2)
                    {
                        if(i-row>=0 and i-row<8 and j-col>=0 and j-col<8) // check for the boundary
                        {
                            if(board[i-row][j-col] == '.')
                            {
                                return false;
                            }
                        }
                    }
                }

                if(i-2>=0 and i-2<8 and j+2>=0 and j+2<8) //whether right forward jump possible or not ..
                {
                    if(board[i-1][j+1]=='w' or board[i-1][j+1]=='W') //kill piece available or not
                    {
                        if(board[i-2][j+2]=='.') //empty place after the jump
                        {
                          return false;

                        }
                    }
                }



                if(i-2>=0 and i-2<8 and j-2>=0 and j-2<8) //left forward jump possible or not ..
                {
                    if(board[i-1][j-1]=='w' or board[i-1][j-1]=='W') //kill piece available or not
                    {
                        if(board[i-2][j-2]=='.') //empty place after the jump
                        {
                           return false;

                        }
                    }
                }



                //------checking for the possibility of the left jump
                if(i+2>=0 and i+2<8 and j+2>=0 and j+2<8) //whether right backward jump possible or not ..
                {
                    if(board[i+1][j+1]=='w' or board[i+1][j+1]=='W') //kill piece available or not
                    {
                        if(board[i+2][j+2]=='.') //empty place after the jump
                        {
                            return false;

                        }
                    }
                }



                if(i+2>=0 and i+2<8 and j-2>=0 and j-2<8) //left forward jump possible or not ..
                {
                    if(board[i+1][j-1]=='w' or board[i+1][j-1]=='W') //kill piece available or not
                    {
                        if(board[i+2][j-2]=='.') //empty place after the jump
                        {
                            return false;

                        }
                    }
                }



            }

            if(is_black_pawn(board[i][j]))
            {
                for(col=-1;col<=1;col=col+2)
                {
                    if(i+1>=0 and i+1<8 and j-col>=0 and j-col<8) // check for the boundary
                    {

                        if(board[i+1][j-col] == '.')
                        {
                            return false;
                        }
                    }
                }


                if(i+2>=0 and i+2<8 and j+2>=0 and j+2<8) //whether right jump possible or not ..
                {
                    if(board[i+1][j+1]=='w' or board[i+1][j+1]=='W') //kill piece available or not
                    {

                        if(board[i+2][j+2]=='.') //empty place after the jump
                        {
                            return false;

                        }
                    }
                }


                if(i+2>=0 and i+2<8 and j-2>=0 and j-2<8) //right jump possible or not ..
                {
                    if(board[i+1][j-1]=='w' or board[i+1][j-1]=='W') //kill piece available or not
                    {
                        if(board[i+2][j-2]=='.') //empty place after the jump
                        {
                           return false;

                        }
                    }
                }



            }
        }
    }


        return true;

};

void move_white(vector<string> board,int row,int col,vector<vector<valid_move>> &res,vector<vector<string>> &board_state)
{
    valid_move temp;
    vector<valid_move> move;
    int j;
    vector<string> b;
    for(j=-1;j<=1;j=j+2)
    {
        if(row-1>=0 and row-1<8 and col-j>=0 and col-j<8) // check for the boundary
        {

            if(board[row-1][col-j] == '.')
            {
                b = board;
                temp.action = 'E';
                temp.curr_row = row;
                temp.curr_col = col;
                temp.next_row = row-1;
                temp.next_col = col-j;
                b[row][col] = '.';
                if(row-1 == 0) b[row-1][col-j] = 'W';  //make it king if it has entered last row
                else b[row-1][col-j] = 'w';
                move.emplace_back(temp); //add move to the results
                res.emplace_back(move); // add move to the results
                board_state.emplace_back(b);
                move.clear(); // add the board configuration to the result
            }
        }
    }
}



void move_black(vector<string> board,int row,int col,vector<vector<valid_move>> &res,vector<vector<string>> &board_state)
{
    valid_move temp;
    vector<valid_move> move;
    int j;
    vector<string> b;
    for(j=-1;j<=1;j=j+2)
    {
        if(row+1>=0 and row+1<8 and col-j>=0 and col-j<8) // check for the boundary
        {

            if(board[row+1][col-j] == '.')
            {
                b = board;
                temp.action = 'E';
                temp.curr_row = row;
                temp.curr_col = col;
                temp.next_row = row+1;
                temp.next_col = col-j;
                b[row][col] = '.';
                if(row+1 == 7) b[row+1][col-j] = 'B';  //make it king if it has entered last row
                else b[row+1][col-j] = 'b';
                move.emplace_back(temp); //add move to the results
                res.emplace_back(move); // add move to the results
                board_state.emplace_back(b);
                move.clear(); // add the board configuration to the result
            }
        }
    }

}



void jump_white(vector<string> board,int i,int j,vector<valid_move> move,vector<vector<valid_move>> &res,vector<vector<string>> &board_state)
{
    valid_move temp;
    char piece = board[i][j];
    bool is_right=false,is_left = false;
    vector<string> b;
    //------checking for the possibility of the left jump
    if(i-2>=0 and i-2<8 and j+2>=0 and j+2<8) //whether right jump possible or not ..
    {
        if(board[i-1][j+1]=='b' or board[i-1][j+1]=='B') //kill piece available or not
        {
            b = board;
            if(board[i-2][j+2]=='.') //empty place after the jump
            {
                temp.action = 'J';
                temp.curr_row = i;
                temp.curr_col = j;
                temp.next_row = i-2;
                temp.next_col = j+2;
                move.emplace_back(temp);
                b[i][j] = '.';     //update the board as per the given one
                b[i-1][j+1] = '.';
                b[i-2][j+2] = piece;
                is_right = true;
                jump_white(b,i-2,j+2,move,res,board_state);

            }
        }
    }

    if(is_right) move.pop_back();  // if a move has been added remove it

    if(i-2>=0 and i-2<8 and j-2>=0 and j-2<8) //right jump possible or not ..
    {
        if(board[i-1][j-1]=='b' or board[i-1][j-1]=='B') //kill piece available or not
        {
            if(board[i-2][j-2]=='.') //empty place after the jump
            {
                b = board;
                temp.action = 'J';
                temp.curr_row = i;
                temp.curr_col = j;
                temp.next_row = i-2;
                temp.next_col = j-2;
                move.emplace_back(temp);
                b[i][j] = '.';     //update the board as per the given one
                b[i-1][j-1] = '.';
                b[i-2][j-2] = piece;
                is_left = true;
                jump_white(b,i-2,j-2,move,res,board_state);

            }
        }
    }



    if(is_left==false and is_right==false) //terminating condition
    {
        if(i==0) board[i][j] = 'W'; //make it king if it has reached the last state

        if(move.size()>0)
        {
            res.emplace_back(move);
            board_state.emplace_back(board);  //add the current board_state
            return;
        }
    }


}

void jump_black(vector<string> board,int i,int j,vector<valid_move> move,vector<vector<valid_move>> &res,vector<vector<string>> &board_state)
{
    valid_move temp;
    char piece = board[i][j];
    bool is_right=false,is_left = false;
    vector<string> b;
    //------checking for the possibility of the left jump
    if(i+2>=0 and i+2<8 and j+2>=0 and j+2<8) //whether right jump possible or not ..
    {
        if(board[i+1][j+1]=='w' or board[i+1][j+1]=='W') //kill piece available or not
        {
            b = board;
            if(board[i+2][j+2]=='.') //empty place after the jump
            {
                temp.action = 'J';
                temp.curr_row = i;
                temp.curr_col = j;
                temp.next_row = i+2;
                temp.next_col = j+2;
                move.emplace_back(temp);
                b[i][j] = '.';     //update the board as per the given one
                b[i+1][j+1] = '.';
                b[i+2][j+2] = piece;
                is_right = true;
                jump_black(b,i+2,j+2,move,res,board_state);

            }
        }
    }

    if(is_right) move.pop_back();  // if a move has been added remove it

    if(i+2>=0 and i+2<8 and j-2>=0 and j-2<8) //right jump possible or not ..
    {
        if(board[i+1][j-1]=='w' or board[i+1][j-1]=='W') //kill piece available or not
        {
            if(board[i+2][j-2]=='.') //empty place after the jump
            {
                b = board;
                temp.action = 'J';
                temp.curr_row = i;
                temp.curr_col = j;
                temp.next_row = i+2;
                temp.next_col = j-2;
                move.emplace_back(temp);
                b[i][j] = '.';     //update the board as per the given one
                b[i+1][j-1] = '.';
                b[i+2][j-2] = piece;
                is_left = true;
                jump_black(b,i+2,j-2,move,res,board_state);

            }
        }
    }



    if(is_left==false and is_right==false) //terminating condition
    {
        if(i==7) board[i][j] = 'B'; //make it king if it has reached the last state

        if(move.size()>0)
        {
            res.emplace_back(move);
            board_state.emplace_back(board);  //add the current board_state
            return;
        }
    }


}





void jump_white_king(vector<string> board,int i,int j,vector<valid_move> move,vector<vector<valid_move>> &res,vector<vector<string>> &board_state)
{
    valid_move temp;
    char piece = board[i][j];
    bool right_forward=false,right_backward = false,left_forward = false,left_backward = false;
    vector<string> b;
    //------checking for the possibility of the left jump
    if(i-2>=0 and i-2<8 and j+2>=0 and j+2<8) //whether right forward jump possible or not ..
    {
        if(board[i-1][j+1]=='b' or board[i-1][j+1]=='B') //kill piece available or not
        {
            if(board[i-2][j+2]=='.') //empty place after the jump
            {
                b = board;
                temp.action = 'J';
                temp.curr_row = i;
                temp.curr_col = j;
                temp.next_row = i-2;
                temp.next_col = j+2;
                move.emplace_back(temp);
                b[i][j] = '.';     //update the board as per the given one
                b[i-1][j+1] = '.';
                b[i-2][j+2] = piece;
                right_forward = true;
                jump_white_king(b,i-2,j+2,move,res,board_state);

            }
        }
    }

    if(right_forward) move.pop_back();  // if a move has been added remove it

    if(i-2>=0 and i-2<8 and j-2>=0 and j-2<8) //left forward jump possible or not ..
    {
        if(board[i-1][j-1]=='b' or board[i-1][j-1]=='B') //kill piece available or not
        {
            if(board[i-2][j-2]=='.') //empty place after the jump
            {
                b = board;
                temp.action = 'J';
                temp.curr_row = i;
                temp.curr_col = j;
                temp.next_row = i-2;
                temp.next_col = j-2;
                move.emplace_back(temp);
                b[i][j] = '.';     //update the board as per the given one
                b[i-1][j-1] = '.';
                b[i-2][j-2] = piece;
                left_forward = true;
                jump_white_king(b,i-2,j-2,move,res,board_state);

            }
        }
    }

    if(left_forward) move.pop_back();

    //------checking for the possibility of the left jump
    if(i+2>=0 and i+2<8 and j+2>=0 and j+2<8) //whether right backward jump possible or not ..
    {
        if(board[i+1][j+1]=='b' or board[i+1][j+1]=='B') //kill piece available or not
        {
            if(board[i+2][j+2]=='.') //empty place after the jump
            {
                b = board;
                temp.action = 'J';
                temp.curr_row = i;
                temp.curr_col = j;
                temp.next_row = i+2;
                temp.next_col = j+2;
                move.emplace_back(temp);
                b[i][j] = '.';     //update the board as per the given one
                b[i+1][j+1] = '.';
                b[i+2][j+2] = piece;
                right_backward = true;
                jump_white_king(b,i+2,j+2,move,res,board_state);

            }
        }
    }

    if(right_backward) move.pop_back();

    if(i+2>=0 and i+2<8 and j-2>=0 and j-2<8) //left forward jump possible or not ..
    {
        if(board[i+1][j-1]=='b' or board[i+1][j-1]=='B') //kill piece available or not
        {
            if(board[i+2][j-2]=='.') //empty place after the jump
            {
                b = board;
                temp.action = 'J';
                temp.curr_row = i;
                temp.curr_col = j;
                temp.next_row = i+2;
                temp.next_col = j-2;
                move.emplace_back(temp);
                b[i][j] = '.';     //update the board as per the given one
                b[i+1][j-1] = '.';
                b[i+2][j-2] = piece;
                left_backward = true;
                jump_white_king(b,i+2,j-2,move,res,board_state);

            }
        }
    }

    if(right_forward==false and right_backward==false and left_forward==false and left_backward==false) //terminating condition
    {


        if(move.size()>0)
        {
            res.emplace_back(move);
            board_state.emplace_back(board);  //add the current board_state
            return;
        }
    }

}
void jump_black_king(vector<string> board,int i,int j,vector<valid_move> move,vector<vector<valid_move>> &res,vector<vector<string>> &board_state)
{
    valid_move temp;
    char piece = board[i][j];
    bool right_forward=false,right_backward = false,left_forward = false,left_backward = false;
    vector<string> b;
    //------checking for the possibility of the left jump
    if(i-2>=0 and i-2<8 and j+2>=0 and j+2<8) //whether right forward jump possible or not ..
    {
        if(board[i-1][j+1]=='w' or board[i-1][j+1]=='W') //kill piece available or not
        {
            if(board[i-2][j+2]=='.') //empty place after the jump
            {
                b = board;
                temp.action = 'J';
                temp.curr_row = i;
                temp.curr_col = j;
                temp.next_row = i-2;
                temp.next_col = j+2;
                move.emplace_back(temp);
                b[i][j] = '.';     //update the board as per the given one
                b[i-1][j+1] = '.';
                b[i-2][j+2] = piece;
                right_forward = true;
                jump_black_king(b,i-2,j+2,move,res,board_state);

            }
        }
    }

    if(right_forward) move.pop_back();  // if a move has been added remove it

    if(i-2>=0 and i-2<8 and j-2>=0 and j-2<8) //left forward jump possible or not ..
    {
        if(board[i-1][j-1]=='w' or board[i-1][j-1]=='W') //kill piece available or not
        {
            if(board[i-2][j-2]=='.') //empty place after the jump
            {
                b = board;
                temp.action = 'J';
                temp.curr_row = i;
                temp.curr_col = j;
                temp.next_row = i-2;
                temp.next_col = j-2;
                move.emplace_back(temp);
                b[i][j] = '.';     //update the board as per the given one
                b[i-1][j-1] = '.';
                b[i-2][j-2] = piece;
                left_forward = true;
                jump_black_king(b,i-2,j-2,move,res,board_state);

            }
        }
    }

    if(left_forward) move.pop_back();

    //------checking for the possibility of the left jump
    if(i+2>=0 and i+2<8 and j+2>=0 and j+2<8) //whether right backward jump possible or not ..
    {
        if(board[i+1][j+1]=='w' or board[i+1][j+1]=='W') //kill piece available or not
        {
            if(board[i+2][j+2]=='.') //empty place after the jump
            {
                b = board;
                temp.action = 'J';
                temp.curr_row = i;
                temp.curr_col = j;
                temp.next_row = i+2;
                temp.next_col = j+2;
                move.emplace_back(temp);
                b[i][j] = '.';     //update the board as per the given one
                b[i+1][j+1] = '.';
                b[i+2][j+2] = piece;
                right_backward = true;
                jump_black_king(b,i+2,j+2,move,res,board_state);

            }
        }
    }

    if(right_backward) move.pop_back();

    if(i+2>=0 and i+2<8 and j-2>=0 and j-2<8) //left forward jump possible or not ..
    {
        if(board[i+1][j-1]=='w' or board[i+1][j-1]=='W') //kill piece available or not
        {
            if(board[i+2][j-2]=='.') //empty place after the jump
            {
                b = board;
                temp.action = 'J';
                temp.curr_row = i;
                temp.curr_col = j;
                temp.next_row = i+2;
                temp.next_col = j-2;
                move.emplace_back(temp);
                b[i][j] = '.';     //update the board as per the given one
                b[i+1][j-1] = '.';
                b[i+2][j-2] = piece;
                left_backward = true;
                jump_black_king(b,i+2,j-2,move,res,board_state);

            }
        }
    }

    if(right_forward==false and right_backward==false and left_forward==false and left_backward==false) //terminating condition
    {


        if(move.size()>0)
        {
            res.emplace_back(move);
            board_state.emplace_back(board);  //add the current board_state
            return;
        }
    }

}




void king_move(vector<string> board,int row,int col,vector<vector<valid_move>> &res,vector<vector<string>> &board_state)
{
    // right forward jump for the king
    valid_move temp;
    vector<valid_move> move;
    vector<string> b;
    char piece = board[row][col];
    int i,j;
    for(i=-1;i<=1;i=i+2) // checking for all the four directions
    {
        for(j=-1;j<=1;j=j+2)
        {
            if(row-i>=0 and row-i<8 and col-j>=0 and col-j<8) // check for the boundary
            {
                if(board[row-i][col-j] == '.')
                {
                    b = board;
                    temp.action = 'E';
                    temp.curr_row = row;
                    temp.curr_col = col;
                    temp.next_row = row-i;
                    temp.next_col = col-j;
                    b[row][col] = '.';
                    b[row-i][col-j] = piece;
                    move.emplace_back(temp); //add move to the results
                    res.emplace_back(move); // add move to the results
                    board_state.emplace_back(b); // add the board configuration to the result
                    move.clear();
                }
            }
        }
    }
}

boards_and_moves generate_moves(vector<string> board,string color)
{
    boards_and_moves final;
    vector<vector<valid_move>> res; //stores all the valid moves for the given state of the board
    vector<valid_move> move,final_move;
    valid_move temp;
    vector<vector<string>> board_state; //stores all the states of the board
    int i,j;

//----------------------------------------------------------------------- generating all the valid moves for white-----
   if(color=="WHITE")
   {
       for(i=0;i<8;i++) // checking for the jump moves first
       {
           for(j=0;j<8;j++)
           {
               //res.clear();
               //board_state.clear();
               if(board[i][j]=='w') // for a pawn
               {

                   jump_white(board,i,j,move,res,board_state);
               }

               if(board[i][j]=='W') // for the king
               {
                   jump_white_king(board,i,j,move,res,board_state);
               }


           }

       }

       if(res.size()==0) //if no jumping moves are available
       {
           for(i=0;i<8;i++) // checking for the simple moves
           {
               for(j=0;j<8;j++)
               {
                   //res.clear();
                   //board_state.clear();
                   if(is_white_pawn(board[i][j])) // simple move for a pawn
                   {
                       move_white(board,i,j,res,board_state);
                   }

                   if(is_white_king(board[i][j])) // for the king
                   {
                       king_move(board,i,j,res,board_state);

                   }

               }
           }
       }
   }
   //--------------generating all valid_moves for the black pieces--------------------------------------
   if(color == "BLACK")
   {
       for(i=0;i<8;i++) // checking for the jump moves first
       {
           for(j=0;j<8;j++)
           {
               //res.clear();
               //board_state.clear();
               if(is_black_pawn(board[i][j])) // for a pawn
               {

                   jump_black(board,i,j,move,res,board_state);
               }

               if(is_black_king(board[i][j])) // for the king
               {
                   jump_black_king(board,i,j,move,res,board_state);
               }


           }

       }

       if(res.size()==0) //if no jumping moves are available
       {
           for(i=0;i<8;i++) // checking for the simple moves
           {
               for(j=0;j<8;j++)
               {
                   //res.clear();
                   //board_state.clear();
                   if(is_black_pawn(board[i][j])) // simple move for a pawn
                   {
                       move_black(board,i,j,res,board_state);
                   }

                   if(is_black_king(board[i][j])) // for the king
                   {
                       king_move(board,i,j,res,board_state);

                   }

               }
           }
       }


   }

        final.boards = board_state;
        final.moves = res;
        return final;

}
double evaluate(vector<string> board)
{
    int i,j;
    double res = 0;
    for(i=0;i<8;i++)
    {
        for(j=0;j<8;j++)
        {
            if(is_white_pawn(board[i][j])) //white pawn 
            {
                if(i>=0 and i<=3)
                {
                    res = res+9;
                }
                else res = res+7;
            }
            if(is_black_pawn(board[i][j])) //black pawn
            {
                if(i>=4 and i<=7)
                {
                    res = res-9;
                }
                else res = res-7;
            }
            if(is_white_king(board[i][j])) res = res+12;
            if(is_black_king(board[i][j])) res = res-12;
        }
    }


    return res;
}

double minimax_white(vector<string> board,int h,bool is_max,double alpha,double beta)
{

    boards_and_moves res;
    if(black_wins(board)) return -10000; // black wins
    if(white_wins(board)) return 10000; //white wins
    double score;
    if(depth==h)
    {
        score = evaluate(board);
        return score; //terminating condition
    }

    double best,value;
    int i,j;

    if(is_max) //maximizing player(white)
    {
        best = MIN;
        res = generate_moves(board,"WHITE");
        for(i=0;i<res.boards.size();i++)
        {
            value = minimax_white(res.boards[i],h+1,false,alpha,beta);
            best = max(best,value);
            alpha = max(alpha,best);
            if(alpha>=beta) //prune the nodes
            {
                break;
            }
        }
        return best;

    }

    if(is_max==false)  //minimizing player(black)
    {
        best = MAX;
        res = generate_moves(board,"BLACK");
        for(i=0;i<res.boards.size();i++)
        {
            value = minimax_white(res.boards[i],h+1,true,alpha,beta);
            best = min(best,value);
            beta = min(beta,best); //update beta
            if(alpha>=beta) //prune the nodes
            {
                break;
            }
        }
        return best;
    }

}



boards_and_moves find_move_white(vector<string> board,double alpha,double beta)
{

    double best=MIN,value;
    int i;
    boards_and_moves res,final;
    res = generate_moves(board,"WHITE"); //get all the valid moves of white
    vector<valid_move> best_move;
    vector<string> best_board;
    for(i=0;i<res.boards.size();i++)
    {
        value = minimax_white(res.boards[i],1,false,alpha,beta);
        if(value>best)
        {
            best = value;
            best_move = res.moves[i]; // update the best move
            best_board = res.boards[i]; //update the best board
        }
        alpha = max(best,alpha);
        if(alpha>=beta)
        {
            break; //prune the nodes
        }
    }
    final.moves.emplace_back(best_move); //it gets updated everything
    final.boards.emplace_back(best_board); //it gets updated everything

    return final;

}

double minimax_black(vector<string> board,int h,bool is_max,double alpha,double beta)
{

    boards_and_moves res;
    if(black_wins(board)) return 10000; // black wins
    if(white_wins(board)) return -10000; //white wins
    double score;
    if(depth==h)
    {
        score = evaluate(board);
        return -score;  // terminating conditions
    }

    double best,value;
    int i,j;

    if(is_max) //maximizing player(white)
    {
        best = MIN;
        res = generate_moves(board,"BLACK");
        for(i=0;i<res.boards.size();i++)
        {
            value = minimax_black(res.boards[i],h+1,false,alpha,beta);
            best = max(best,value);
            alpha = max(alpha,best);
            if(alpha>=beta) //prune the nodes
            {
                break;
            }
        }
        return best;

    }

    if(is_max==false)  //minimizing player(black)
    {
        best = MAX;
        res = generate_moves(board,"WHITE");
        for(i=0;i<res.boards.size();i++)
        {
            value = minimax_black(res.boards[i],h+1,true,alpha,beta);
            best = min(best,value);
            beta = min(beta,best); //update beta
            if(alpha>=beta) //prune the nodes
            {
                break;
            }
        }
        return best;
    }

}


boards_and_moves find_move_black(vector<string> board,double alpha,double beta)
{

    double best=MIN,value;
    int i;
    boards_and_moves res,final;
    res = generate_moves(board,"BLACK"); //get all the valid moves of white
    vector<valid_move> best_move;
    vector<string> best_board;
    for(i=0;i<res.boards.size();i++)
    {
        value = minimax_black(res.boards[i],1,false,alpha,beta);
        if(value>best)
        {
            best = value;
            best_move = res.moves[i]; // update the best move
            best_board = res.boards[i]; //update the best board
        }
        alpha = max(best,alpha);
        if(alpha>=beta)
        {
            break; //prune the nodes
        }
    }
    //cout<<alpha;
    final.moves.emplace_back(best_move); //it gets updated everything
    final.boards.emplace_back(best_board); //it gets updated everything

    return final;

}



int main(){

    ifstream fin;
    ofstream fout;
    fin.open("input.txt");
    fout.open("output.txt");
    double time;


    string game_type;
    string color;
    vector<string> board;
    vector<double> v;
    vector<double>::iterator low;
    int index;
    fin>>game_type; //reading the type of game single or agent
    fin>>color; //color of piece
    fin>>remaining_time;


    int i,j,m;
    unordered_map<int,int> row_map;
    unordered_map<int,char> col_map;
    for(m=0;m<8;m++)
        row_map[m] = 8-m;

    col_map[0] = 'a';
    col_map[1] = 'b';
    col_map[2] = 'c';
    col_map[3] = 'd';
    col_map[4] = 'e';
    col_map[5] = 'f';
    col_map[6] = 'g';
    col_map[7] = 'h';


    for(i=0;i<8;i++) //reading the charecters
    {
        string temp;
        fin>>temp;
        board.emplace_back(temp);
    }
    fin.close(); //close the input txt file

    // cout<<game_type;
    // cout<<endl;
    // cout<<color<<endl;
    // cout<<remaining_time<<endl;
    
    int k,l;
    // for(i=0;i<8;i++)
    // {
    //     for(j=0;j<8;j++)
    //     {
    //         cout<<board[i][j];
    //     }

    //     cout<<endl;
    // }

    //----------- ENTER THE GAME------------------------------------------------------------------------------
    boards_and_moves res,final;
    vector<valid_move> move;
    vector<valid_move>::iterator it;
    vector<vector<valid_move>> moves;
    vector<vector<string>> board_state;
    if(game_type=="SINGLE")  //VALID OUTPUT FOR ONLY THE SINGLE MOVE
    {
        depth = 1;
        if(color=="WHITE")
        {
            final = find_move_white(board,MIN,MAX);
            moves = final.moves;
            board_state = final.boards;

        }

        else if(color =="BLACK")
        {
            final = find_move_black(board,MIN,MAX);
            moves = final.moves;
            board_state = final.boards;
        }

        

    }

    else
    {
        fin.open("calibration.txt");
        //........ if the file exist....................................................................................
        if(fin)
        {
                while(fin>>time)
            {
                v.emplace_back(time);
            }

            //cout<<"max depth is "<<v.size()<<endl;
            if(remaining_time==300) //set the depth to one for the first move
            {
                depth = 1;


            }

            else if(remaining_time>=100 and remaining_time<300)
            {
                low = lower_bound(v.begin(),v.end(),7);//time for the move is less than 7
                index = low - v.begin();
                if(index<=7)
                {
                    depth = index;
                }

                else if(index>10)
                {
                    depth  = index;
                }
                else
                {
                    depth = 10;
                }    
                
            }

            else if(remaining_time<100 and remaining_time>=30)
            {
                low = lower_bound(v.begin(),v.end(),3); //time for the move is less than 3 seconds
                index = low - v.begin();
                depth = index;
            }

            else if(remaining_time<30 and remaining_time>=4)
            {
                low = lower_bound(v.begin(),v.end(),1); //time for the move is less than 3 seconds
                index = low - v.begin();
                depth = index;
            }

            else if(remaining_time<4 and remaining_time>=0.05)
            {
                depth = 2;
            }

            else // at the last stage set the depth to one
            {
                depth = 1;
            }
        }

        else
        {
            depth = 7; // if the calibration file does not exists then select this depth  
        }
       


        //-----choose the color of the play-----------------------------------
        if(color=="WHITE")
        {
            final = find_move_white(board,MIN,MAX);
            moves = final.moves;
            board_state = final.boards;

        }

        else if(color =="BLACK")
        {
            final = find_move_black(board,MIN,MAX);
            moves = final.moves;
            board_state = final.boards;

        }



        
    }


        
            move = moves[0];
            

            for(it=move.begin();it!=move.end();it++)  // print the move ---------------------------------
            {
                fout<<it->action<<" "<<col_map[it->curr_col]<<row_map[it->curr_row]<<" "<<col_map[it->next_col]<<row_map[it->next_row];
                if(it!=move.end()-1)
                {
                    fout<<'\n';
                }

            }
           
            vector<string> b;
            b=board_state[0];
            //cout<<b.size();
           




   







   // cout<<"depth is "<<depth;
    fin.close(); //close the calibration file
    fout.close();
    return 0;
}