#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdlib.h>

using namespace std;

const int NROW = 8;  // # rows = rows + 2 + 2
const int NCOL = 10;  // # cols = cols + 2 + 2
const int NDOGS = 2;  //number of sausages


const int water = -1;  
const int ground = 0;
const int fire = 1; 

typedef struct state{
    int x[NDOGS];
    int y[NDOGS];
    int cooked[NDOGS][2][2];  // top[0][1][1]: 第0个香肠的上面的第二个格子
    bool is_top[NDOGS]; //初始为0：一开始bot面朝下，当滚动一次后，变为1， cooked[][]进行操作
    int player_x;
    int player_y;
    int player_direction;

    bool operator==(const state &p)  const {
        for (int i = 0; i < NDOGS; i++) {
            if (x[i] != p.x[i]|| y[i] != p.y[i]) 
                return false;
            if (cooked[i][0][0] != p.cooked[i][0][0] || cooked[i][0][1] != p.cooked[i][0][1] || 
            cooked[i][1][0] != p.cooked[i][1][0] || cooked[i][1][1] != p.cooked[i][1][1])
                return false;
        }
        if (player_x != p.player_x || player_y != p.player_y || 
        player_direction != p.player_direction)
            return false;
        return true;
    }
} state;

struct state_hash {
size_t operator () (const state & st) const 
{
    return hash<int>()(st.player_x * 2 + st.player_y + 3 * 
    st.player_direction + 5 * st.x[0] + 7 * st.y[0]);
}
};


int board[NROW][NCOL];
int sausage_shape[NDOGS]; 
state st;
unordered_map<state, int, state_hash> visited;


void initialize(){
    // board for Fiery Jut 
    for (int i = 0; i < NROW; i++)
    for (int j = 0; j < NCOL; j++)
    board[i][j] = water;
    for (int i = 2; i < 6; i++)
    for (int j = 2; j < 5; j++){
        board[i][j] = fire;
    }
    board[3][5] = ground;
    board[3][6] = ground;
    board[4][5] = ground;
    board[4][6] = ground;
    for (int i = 2; i < 6; i++)
        board[i][7] = ground;
    // sausage shape  0: horizontal  1 : vertical 
    sausage_shape[0] = 1;
    sausage_shape[1] = 1;
    //state 
    st.x[0] = 3;
    st.x[1] = 3;
    st.y[0] = 5;
    st.y[1] = 6;
    for (int i = 0; i < NDOGS; i++) 
        for (int j = 0; j < 1; j++)
            for (int k = 0; k < 1; k++)
                st.cooked[i][j][k] = 0;
    st.player_x = 4;
    st.player_y = 7;
    st.player_direction= 2;
    visited.clear();
}

bool is_finish(state st){ // 检测是否满足游戏结束条件
    for (int i = 0; i < NDOGS; i ++)
        if (st.cooked[i][0][0] != 1 || st.cooked[i][0][1] != 1 ||
         st.cooked[i][1][0] != 1 || st.cooked[i][1][1] != 1)
            return false;
    return true;
}


void apply_force (state & st, int dir, int x_value, int y_value, int moved[]) {


    for (int i = 0; i < NDOGS; i ++){
        if (moved[i] == false)
            if (sausage_shape[i] == 0) {//水平摆放  [x] [x]
                if (st.x[i] == x_value && st.y[i] == y_value || st.x[i] == x_value && st.y[i] + 1 == y_value) {
                    moved[i] = true;
                    if (dir == 1 || dir == 3) {// 水平摆放，力是左右方向的，新的力只有一个方向 
                        if (dir == 1) {//力向左
                            st.y[i] = st.y[i] - 1;
                            apply_force(st, dir, x_value, st.y[i], moved);
                        }
                        else { //力向右
                            st.y[i] = st.y[i] + 1;
                            apply_force(st, dir, x_value, st.y[i] + 1, moved);
                        }
                    }
                    else { // 水平摆放，力是上下，有2个新的力坐标
                        st.is_top[i] = 1 - st.is_top[i]; // 1 -> 0 , 0 -> 1
                        if (dir == 2) //力向下
                            st.x[i] = st.x[i] + 1;
                        else  //力向上
                            st.x[i] = st.x[i] - 1;
                        apply_force(st, dir, st.x[i], st.y[i], moved);
                        apply_force(st, dir, st.x[i], st.y[i] + 1, moved);
                    }
                }
            }
            else { //竖直摆放
                if (st.x[i] == x_value && st.y[i] == y_value || st.x[i] + 1 == x_value && st.y[i] == y_value) {
                    moved[i] = true;
                    if (dir == 2 || dir == 4) {// 上下摆放，力是上下方向的，新的力只有一个方向 
                        if (dir == 2) {//力向下
                            st.x[i] = st.x[i] + 1;
                            apply_force(st, dir, st.x[i] + 1, y_value, moved);
                        }
                        else { //力向上
                            st.x[i] = st.x[i] - 1;
                            apply_force(st, dir, st.x[i], y_value, moved);
                        }
                    }
                    else { // 上下摆放，力是左右，有2个新的力坐标
                       st.is_top[i] = 1 - st.is_top[i];
                        if (dir == 1) //力向左
                            st.y[i] = st.y[i] - 1;
                        else  //力向右
                            st.y[i] = st.y[i] + 1;
                        apply_force(st, dir, st.x[i], st.y[i], moved);
                        apply_force(st, dir, st.x[i] + 1, st.y[i], moved);
                    }
            }
        }
    }
    
  
}

void roast (state & st, int moved[]) {
    for (int i = 0; i < NDOGS; i++) {
        if (moved[i]) {
            if (board[st.x[i]][st.y[i]] == fire) 
               st.cooked[i][st.is_top[i]][0] ++; 
            if (sausage_shape[i] == 0 && board[st.x[i]][st.y[i]+1] == fire
            || sausage_shape[i] == 1 && board[st.x[i]+1][st.y[i]] == fire) 
                st.cooked[i][st.is_top[i]][1] ++; 
        }
    }
}

void restore (state & st, int dir){
    if ((st.player_direction == dir || abs(st.player_direction - dir) == 2) && 
    board[st.player_x][st.player_y] == fire) { 
        if (dir == 1) 
            st.player_y = st.player_y + 1;
        else if (dir == 2)
            st.player_x = st.player_x - 1;
        else if (dir == 3)
            st.player_y = st.player_y - 1;
        else 
            st.player_x = st.player_x + 1;
    }
}


//core function     
state is_valid_next_step(state st, int direction) { // 1 = 左 2 = 下 3 = 右 4 = 上
        // change 1
    state new_st;
    new_st = st;
    int f_x,f_y;
    
    //首先检测是否会走出地图
    if (st.player_direction == direction || abs(st.player_direction - direction) == 2) { //如果相同或者相反方向：（下一步会导致角色位移）

        if (direction == 1) {
            new_st.player_x = st.player_x;
            f_x = st.player_x;
            new_st.player_y = st.player_y - 1;
            f_y = st.player_y - 2;
        } 
        else if (direction == 2) {
            new_st.player_x = st.player_x + 1;
            f_x = st.player_x + 2;
            new_st.player_y = st.player_y;
            f_y = st.player_y;
        }
        else if (direction == 3) {
            new_st.player_x = st.player_x;
            f_x = st.player_x;
            new_st.player_y = st.player_y + 1;
            f_y = st.player_y + 2;
        }
        else {
            new_st.player_x = st.player_x - 1;
            f_x = st.player_x - 2;
            new_st.player_y = st.player_y;
            f_y = st.player_y;
        }

        if (board[new_st.player_x][new_st.player_y] == water)  {
            new_st.player_direction = -1;
            return new_st; 
        }
    }   


    int moved[NDOGS]; //记录第i个香肠有没有被移动过,初始为false ， 都没有移动过
    for (int i = 0; i < NDOGS; i++)
        moved[i] = false;

    // 当人物移动带来香肠移动，或者香肠移动带来香肠移动的时候，继续循环，直到结束，并且更新new_state  
    if (abs(st.player_direction - direction) == 2) // 倒退：判定新的人物位置
        apply_force(new_st, direction, new_st.player_x, new_st.player_y,moved);
    else if (st.player_direction == direction) //　前进了一个格子　,判定： 新的fork位置
        apply_force(new_st, direction, f_x, f_y, moved);
    else {
        //接下来是旋转的情况，分八种， 2个方向，4个不同的情况
        int xx,yy,dd; // 代表旋转叉子碰撞的坐标和方向
        if (st.player_direction == 1 && direction == 4) {//初始朝左，顺时针 
            xx = st.player_x - 1; //1st
            yy = st.player_y - 1;
            dd = 4;
            apply_force(new_st, dd, xx, yy, moved);
            xx = st.player_x - 1; // 2nd rotation
            yy = st.player_y;
            dd = 3;
            apply_force(new_st, dd, xx, yy, moved);
        }

        else if (st.player_direction == 1 && direction == 2) {//初始朝左，逆时针
            xx = st.player_x + 1; //1st
            yy = st.player_y - 1;
            dd = 2;
            apply_force(new_st, dd, xx, yy, moved);
            xx = st.player_x + 1; // 2nd rotation
            yy = st.player_y;
            dd = 3;
            apply_force(new_st, dd, xx, yy, moved);
        }

        else if (st.player_direction == 2 && direction == 1) {//初始朝下，顺时针
            xx = st.player_x + 1; //1st
            yy = st.player_y - 1;
            dd = 1;
            apply_force(new_st, dd, xx, yy, moved);
            xx = st.player_x; // 2nd rotation
            yy = st.player_y - 1;
            dd = 4;
            apply_force(new_st, dd, xx, yy, moved);
        }

        else if (st.player_direction == 2 && direction == 3) {//初始朝下，逆时针
            xx = st.player_x + 1; //1st
            yy = st.player_y + 1;
            dd = 3;
            apply_force(new_st, dd, xx, yy, moved);
            xx = st.player_x; // 2nd rotation
            yy = st.player_y + 1;
            dd = 4;
            apply_force(new_st, dd, xx, yy, moved);
        }

        else if (st.player_direction == 3 && direction == 2) {//初始朝右，顺时针
            xx = st.player_x + 1; //1st
            yy = st.player_y + 1;
            dd = 2;
            apply_force(new_st, dd, xx, yy, moved);
            xx = st.player_x + 1; // 2nd rotation
            yy = st.player_y;
            dd = 1;
            apply_force(new_st, dd, xx, yy, moved);
        }

        else if (st.player_direction == 3 && direction == 4) {//初始朝右，逆时针
            xx = st.player_x - 1; //1st
            yy = st.player_y + 1;
            dd = 4;
            apply_force(new_st, dd, xx, yy, moved);
            xx = st.player_x - 1; // 2nd rotation
            yy = st.player_y;
            dd = 1;
            apply_force(new_st, dd, xx, yy, moved);
        }

        else if (st.player_direction == 4 && direction == 3) {//初始朝上，顺时针
            xx = st.player_x - 1; //1st
            yy = st.player_y + 1;
            dd = 3;
            apply_force(new_st, dd, xx, yy, moved);
            xx = st.player_x; // 2nd rotation
            yy = st.player_y + 1;
            dd = 2;
            apply_force(new_st, dd, xx, yy, moved);
        }

        else if (st.player_direction == 4 && direction == 1) {//初始朝上，逆时针
            xx = st.player_x - 1; //1st
            yy = st.player_y - 1;
            dd = 1;
            apply_force(new_st, dd, xx, yy, moved);
            xx = st.player_x; // 2nd rotation
            yy = st.player_y - 1;
            dd = 2;
            apply_force(new_st, dd, xx, yy, moved);
        }
    } //移动香肠完毕

    roast(new_st, moved); //根据目前位置烤香肠


    
    
    for (int i = 0; i < NDOGS; i++) { // 检测有没有烤焦 或者碰到水
        if (moved[i]) {
            for (int ii = 0; ii < 2; ii++)
                for (int jj = 0; jj < 2; jj++)
                    if (new_st.cooked[i][ii][jj] > 1) {
                        new_st.player_direction = -1;
                        return new_st;
                    }
            if (sausage_shape[i] == 0 && board[new_st.x[i]][new_st.y[i]] == water && board[new_st.x[i]][new_st.y[i]+1] == water) {
                new_st.player_direction = -1;
                return new_st;
            }
            if (sausage_shape[i] == 1 && board[new_st.x[i]][new_st.y[i]] == water && board[new_st.x[i]+1][new_st.y[i]] == water) {
                new_st.player_direction = -1;
                return new_st;
            }
        }
    }
    

    restore(new_st, direction);
    
    if (direction - new_st.player_direction == 1 || direction - new_st.player_direction == -3 
    || direction - new_st.player_direction == -1 || direction - new_st.player_direction == 3)
        new_st.player_direction = direction;

            //如果重复步骤，则返回 -1 => invalid 
    if (visited.find(new_st) != visited.end()) {
        new_st.player_direction = -1;
        return new_st;
    }

    return new_st;
}



void DFSUtil(state sta, int path[], int path_index , int cmd) {
    // 如果找到任何满足游戏结束条件的状态，打印并且退出
    visited.insert(pair<state, int>(sta, 1));
    path[path_index] = cmd;
    path_index ++;
    if (is_finish(sta)) {
        cout << endl;
        cout << "-------START-------" << "\n";
        for (int i = 1; i < path_index; i++) {
            cout << "step " << i << ": ";
            if (path[i] == 1)
                cout << "left" << "\n";
            else if (path[i] == 2)
                cout << "down" << "\n";
            else if (path[i] == 3)
                cout << "right" << "\n";
            else if (path[i] == 4)
                cout << "up" << "\n";
        }
        cout << "-------END-------\n" << endl;
        exit(0);
    } 
    // step : 1 = 左 2 = 下 3 = 右 4 = 上 
    for (int dir = 1; dir < 5; dir ++)  {
        state next_st = is_valid_next_step(sta, dir);
        if (next_st.player_direction != -1) {
            DFSUtil(next_st, path, path_index, dir);
        }
    }
    path_index --;
    // visited.erase(sta);
}

void DFS(state st) 
{     

    // Call the recursive helper function 
    // to print DFS traversal 
    int * path = new int[500];
    int path_index = 0; // Initialize path[] as empty

    DFSUtil(st, path, path_index, 0);  // 0 = initial move 
} 



int main() {
    initialize();  
    DFS(st);
}