#include <bangtal>
#include <iostream>
#include <Windows.h>


using namespace bangtal;
using namespace std;


enum class State {
	BLANK,
	BLACK,
	WHITE,
	POSSIBLE
};

enum class Turn {
	WHITE,
	BLACK
};
Turn turn = Turn::BLACK;


class Stone
{
private:
	State state;   // Blank : 0, White: 1, Black: -1, possible: 2
	ObjectPtr stoneImg;

public:
	Stone() {
		state = State::BLANK;
	}
	void setStone(string stonePath, ScenePtr scene, int x, int y) {
		stoneImg = Object::create(stonePath, scene, 40 + x * 80, 40 + y * 80);
	}
	void setState(State inputState) {
		switch (inputState) {
		case State::BLANK: stoneImg->setImage("images/blank.png"); break;
		case State::POSSIBLE: this->stoneImg->setImage(turn == Turn::BLACK ? "images/black possible.png" : "images/white possible.png"); break;
		case State::BLACK: this->stoneImg->setImage("images/black.png"); break;
		case State::WHITE: this->stoneImg->setImage("images/white.png"); break;
		}
		state = inputState;
	}
	/// POSSIBLE 체크 및 변경
	bool checkPossible(int x, int y, int dx, int dy) {
		State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
		State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

		int count = 0;
		
		bool possible = false;
		for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
			if (stone[y][x].getState() == other) {
				possible = true;
				count++;
			}
			else if (stone[y][x].getState() == self){
				return possible;
			}
			else {
				return false;
			}
		}
		return false;
	}
	bool checkPossible(int x, int y) {
		if (this->state == State::BLACK) return false;
		if (this->state == State::WHITE) return false;
		setState(State::BLANK);

		int delta[8][2] = {
			{0,1},
			{1,1},
			{1,0},
			{1,-1},
			{0,-1},
			{-1,-1},
			{-1,0},
			{-1,1}
		};
		bool possible = false;
		for (auto d : delta) {
			if (checkPossible(x, y, d[0], d[1])) possible = true;
		}
		return possible;
	}
	/// 돌 뒤집기
	void reverse(int x, int y, int dx, int dy) {
		State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
		State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;
		bool possible = false;
		for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
			if (stone[y][x].getState() == other) {
				possible = true;
			}
			else if (stone[y][x].getState() == self) {
				if (possible) {
					for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {
						if (stone[y][x].getState() == other) {
							stone[y][x].setState(self);
						}
						else {
							return;
						}
					}
				}
			}
			else {
				return ;
			}
		}
	}
	void reverse(int x, int y) {
		int delta[8][2] = {
			{0,1},
			{1,1},
			{1,0},
			{1,-1},
			{0,-1},
			{-1,-1},
			{-1,0},
			{-1,1}
		};
		bool possible = false;
		for (auto d : delta) {
			reverse(x, y, d[0], d[1]);
		}
	}
	ObjectPtr getStone() {
		return stoneImg;
	}
	State getState() {
		return state;
	}
}stone[8][8];
// 8x8 모든 위치에 대해서 놓여질 수 있으면 possible로 바꾼다.
int biggest = 0;
int bigY = 0;
int bigX = 0;
bool setPossible() {
	bool possible = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (stone[y][x].checkPossible(x, y)) {
				stone[y][x].setState(State::POSSIBLE);
				possible = true;
			}
		}
	}
	return true;
}

//점수 계산
int white_point, black_point = 0;

void drawPoints(int n, ObjectPtr oneDigit, ObjectPtr tenDigit) {

	
		string oneDigitName = "images/L" + to_string(n%10) + ".png";
		string tenDigitName = "images/L" + to_string(n / 10) + ".png";

		oneDigit->setImage(oneDigitName);
		tenDigit->setImage(tenDigitName);

}

void calPoints(ObjectPtr b_o_point, ObjectPtr b_t_point, ObjectPtr w_o_point,ObjectPtr w_t_point) {
	black_point = 0;
	white_point = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (stone[y][x].getState() == State::BLACK) {
				black_point++;
			}
			else if (stone[y][x].getState() == State::WHITE) {
				white_point++;
		}
		}
	}
	drawPoints(black_point, b_o_point, b_t_point);
	drawPoints(white_point, w_o_point, w_t_point);
}

// 백돌 진행
int whitePC(int x, int y, int dx, int dy) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	int count = 0;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (stone[y][x].getState() == other) {
			count++;
		}
		else if (stone[y][x].getState() == self) {
			return count;
		}
		else {
			return 0;
		}
	}
	return count;
}
void whitePC() {
	int delta[8][2] = {
			{0,1},
			{1,1},
			{1,0},
			{1,-1},
			{0,-1},
			{-1,-1},
			{-1,0},
			{-1,1}
	};
	bool possible = false;
	int biggest = 0;
	int biggestX = 0;
	int biggestY = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (stone[y][x].getState() == State::POSSIBLE) {
				for (auto d : delta) {
					int temp = whitePC(x, y, d[0], d[1]);
					if (temp > biggest) {
						biggestX = x;
						biggestY = y;
						biggest = temp;
					}
				}
			}
		}
	}
	if (biggest == 0) {
		cout << 11111111111 << endl;
		showMessage(black_point > white_point? "게임 종료! 승리하셨습니다!" : "게임종료! 패배하셨습니다!");
	}
	cout << biggest << endl;
	stone[biggestY][biggestX].setState(State::WHITE);
	stone[biggestY][biggestX].reverse(biggestX, biggestY);
	turn = Turn::BLACK;
}


//메인 함수
int main() {
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	ScenePtr scene = Scene::create("오델로 게임", "images/Background.png");

	auto black_One_Point = Object::create("images/L2.png", scene, 830, 230);
	auto black_Ten_Point = Object::create("images/L0.png", scene, 760, 230);

	auto white_One_Point = Object::create("images/L2.png", scene, 1150, 230);	
	auto white_Ten_Point = Object::create("images/L0.png", scene, 1080, 230);

	auto white = Object::create("images/logo.png", scene, 400, 400);

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			stone[y][x].setStone("images/blank.png", scene, x, y);
			stone[y][x].getStone()->setOnMouseCallback([&,x,y](ObjectPtr object, int, int, MouseAction action)->bool {
				if (stone[y][x].getState() == State::POSSIBLE) {
					if (turn == Turn::BLACK) {
						stone[y][x].setState(State::BLACK);
						stone[y][x].reverse(x, y);
						turn = Turn::WHITE;
					}
					else {
						stone[y][x].setState(State::WHITE);
						stone[y][x].reverse(x, y);
						turn = Turn::BLACK;
					}
					//돌을 둘 곳이 없으면 상대 전환, 또 없으면 게임종료!
					if (!setPossible()) {
						whitePC();
						setPossible();
						if (!setPossible()) {
							showMessage(black_point > white_point ? "게임 종료! 승리하셨습니다!" : "게임종료! 패배하셨습니다!");
						}
					}
					setPossible();
					whitePC();
					setPossible();
					calPoints(black_One_Point, black_Ten_Point, white_One_Point, white_Ten_Point);
					

				}
				return true;
				});

		}
	}

	stone[3][3].setState(State::BLACK);
	stone[4][4].setState(State::BLACK);
	stone[4][3].setState(State::WHITE);
	stone[3][4].setState(State::WHITE);

	setPossible();
	

	

	startGame(scene);
}



