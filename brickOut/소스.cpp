#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h> // (or others, depending on the system in use)
#include <math.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cstdlib>

#include "image_texture_mapping.hpp"

#define STAGE1_BLOCK_ROW	7
#define ITEM_NUMBER			8
#define	width				600
#define	height				900
#define	PI					3.1415
#define	polygon_num			50

int count;
int item_count = 0;

float radius1, moving_ball_radius;
float item_radius;

bool debug_mode = false;

bool game_over;// game over 판단
bool game_start;// game start 판단
bool ball_start; // 사용자 입력을 받으면 공이 움직임

// 현재 화면을 판단할 bool 변수
bool window_intro;// 인트로 화면
bool window_game_solution; // 게임 방법 화면
bool window_game_now;// 게임 진행 화면
bool window_game_over; // 게임 오버 화면
bool clear;// 클리어 화면


class Point {
public:
	float x;
	float y;

	Point(float a, float b) {
		this->x = a;
		this->y = b;
	}

	Point() {
		this->x = 0;
		this->y = 0;
	}
};

Point fixed_ball, moving_ball, velocity, rectangle[4];


// 벽돌의 위치를 저장하는 class
class BRICKS {
public:
	Point rectangle[4];
};

BRICKS brick_array[7][10]; // 7줄 (가로 10개씩)
BRICKS bar;

// 아이템 위치를 저장하는 class
class ITEM {
public:
	Point item_text[8];
	Point upper_brick_num[8];
	int flag[8] = { false, false, false, false ,
					false, false, false, false };
};

ITEM item;

// 바 -> w: 180, h: 20
int bar_x1, bar_x2, bar_y1, bar_y2;

// 아이템 랜덤 배치
void item_init() {
	srand((unsigned int)time(NULL));

	int row, col;

	for (int i = 0; i < 8; i++) {
		row = rand() % STAGE1_BLOCK_ROW; // 지정된 행의 개수 범위 내 random 수 생성
		col = rand() % 10; // 10개의 열 범위 내 random 수 생성

		// 아이템의 위치를 랜덤 배치한다.
		item.item_text[i].x = (brick_array[row][col].rectangle[0].x + brick_array[row][col].rectangle[3].x) / 2;
		item.item_text[i].y = brick_array[row][col].rectangle[1].y - item_radius;

		// 아이템이 나올 벽돌의 위치를 기억한다.
		item.upper_brick_num[i].x = row;
		item.upper_brick_num[i].y = col;
	}
}

void init(void) {
	game_over = false; // game over 판단
	game_start = false; // game start 판단
	ball_start = false; // 사용자 입력을 받으면 공이 움직임

	// 현재 화면을 판단할 bool 변수
	window_intro = true; // 인트로 화면
	window_game_solution = false; // 게임 방법 화면
	window_game_now = false; // 게임 진행 화면
	window_game_over = false; // 게임 오버 화면
	clear = false; // 클리어 화면

	item_radius = 20.0; // 떨어지는 아이템 공의 반지름
	moving_ball_radius = 15.0; // 움직이는 공의 반지름
	moving_ball.x = 305; // 움직이는 공의 시작 x 좌표
	moving_ball.y = 170 + moving_ball_radius; // 움직이는 공의 시작 y 좌표

	count = 0; // 깬 벽돌의 갯수

	// bar 좌표
	bar_x1 = 210; // 왼쪽 x좌표
	bar_x2 = 390; // 오른쪽 x좌표
	bar_y1 = 150; // 아래쪽 y좌표
	bar_y2 = 170; // 위쪽 y좌표

	velocity.x = -7.0; // 0.0 x 방향 속도
	velocity.y = 7.0; // 0.05 y 방향 속도

	int tmp_x_position = 10;
	int tmp_y_position = 890;


	// 벽돌 위치 저장
	for (int i = 0; i < STAGE1_BLOCK_ROW; i++) {
		int tmp_x_position = 10;
		for (int j = 0; j < 10; j++) {
			brick_array[i][j].rectangle[0].x = brick_array[i][j].rectangle[1].x = tmp_x_position;
			brick_array[i][j].rectangle[2].x = brick_array[i][j].rectangle[3].x = tmp_x_position + 50;

			brick_array[i][j].rectangle[0].y = brick_array[i][j].rectangle[3].y = tmp_y_position;
			brick_array[i][j].rectangle[1].y = brick_array[i][j].rectangle[2].y = tmp_y_position - 30;

			tmp_x_position += 59;
		}
		tmp_y_position -= 40;
	}

	// 아이템 랜덤 배치
	item_init();

}

// 거리 구하는 함수
double distance(Point a, Point b) {
	return sqrt((pow(b.x - a.x, 2)) + (pow(b.y - a.y, 2)));
}

void MyReshape(int w, int h) {
	glViewport(0, 0, w, h); // 좌하단이 원점이 되는 안전한 방법!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height); // mouse2()
}

// 원 그리기
void Modeling_Circle(float radius, Point CC) {
	float delta;
	delta = 2 * PI / polygon_num;

	glBegin(GL_POLYGON);
	for (int i = 0; i < polygon_num; i++)
		glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
	glEnd();
}

// bar 위치 지정
void Modeling_bar_init() {
	// 게임 시작 전, 중간에 있을 bar의 위치를 지정함
	bar.rectangle[0] = Point(bar_x1, bar_y2);
	bar.rectangle[1] = Point(bar_x1, bar_y1);
	bar.rectangle[2] = Point(bar_x2, bar_y1);
	bar.rectangle[3] = Point(bar_x2, bar_y2);
}

// 벽돌, 바 그리기
void Modeling_Brick() {
	// bar 위치 지정 함수 호출
	Modeling_bar_init();

	// bar 그리기 (채움 사각형), bar 선 그리기(LINE)
	glBegin(GL_QUADS);
	glColor3f(0.9, 0.7, 0.8);
	for (int i = 0; i < 4; i++) {
		glVertex2d(bar.rectangle[0].x, bar.rectangle[0].y);
		glVertex2d(bar.rectangle[1].x, bar.rectangle[1].y);
		glVertex2d(bar.rectangle[2].x, bar.rectangle[2].y);
		glVertex2d(bar.rectangle[3].x, bar.rectangle[3].y);
	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(0.5, 0.5, 0.5);
	for (int i = 0; i < 4; i++) {
		glVertex2d(bar.rectangle[0].x, bar.rectangle[0].y);
		glVertex2d(bar.rectangle[1].x, bar.rectangle[1].y);

		glVertex2d(bar.rectangle[1].x, bar.rectangle[1].y);
		glVertex2d(bar.rectangle[2].x, bar.rectangle[2].y);

		glVertex2d(bar.rectangle[2].x, bar.rectangle[2].y);
		glVertex2d(bar.rectangle[3].x, bar.rectangle[3].y);
	}
	glEnd();


	// 벽돌 그리기 (채움 사각형), 벽돌 선 그리기(LINE)
	for (int i = 0; i < STAGE1_BLOCK_ROW; i++) {
		for (int j = 0; j < 10; j++) {
			glBegin(GL_QUADS);

			// 현재 벽돌이 아이템이 나오는 벽돌일 경우 수행
			if ((item.upper_brick_num[0].x == i && item.upper_brick_num[0].y == j)
				|| (item.upper_brick_num[1].x == i && item.upper_brick_num[1].y == j)
				|| (item.upper_brick_num[2].x == i && item.upper_brick_num[2].y == j)
				|| (item.upper_brick_num[3].x == i && item.upper_brick_num[3].y == j)
				|| (item.upper_brick_num[4].x == i && item.upper_brick_num[4].y == j)
				|| (item.upper_brick_num[5].x == i && item.upper_brick_num[5].y == j)
				|| (item.upper_brick_num[6].x == i && item.upper_brick_num[6].y == j)
				|| (item.upper_brick_num[7].x == i && item.upper_brick_num[7].y == j)) {
				glColor3f(0.8, 0.6, 1.0);
			}
			// 현재 벽돌이 평범한 벽돌일 경우 수행
			else {
				glColor3f(1.0, 1.0, 1.0);
			}

			glVertex2d(brick_array[i][j].rectangle[0].x, brick_array[i][j].rectangle[0].y);
			glVertex2d(brick_array[i][j].rectangle[1].x, brick_array[i][j].rectangle[1].y);
			glVertex2d(brick_array[i][j].rectangle[2].x, brick_array[i][j].rectangle[2].y);
			glVertex2d(brick_array[i][j].rectangle[3].x, brick_array[i][j].rectangle[3].y);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glLineWidth(10.0); // LINE 두께 지정
			glColor3f(0.5, 0.5, 0.5);
			glVertex2d(brick_array[i][j].rectangle[0].x, brick_array[i][j].rectangle[0].y);
			glVertex2d(brick_array[i][j].rectangle[1].x, brick_array[i][j].rectangle[1].y);

			glVertex2d(brick_array[i][j].rectangle[1].x, brick_array[i][j].rectangle[1].y);
			glVertex2d(brick_array[i][j].rectangle[2].x, brick_array[i][j].rectangle[2].y);

			glVertex2d(brick_array[i][j].rectangle[2].x, brick_array[i][j].rectangle[2].y);
			glVertex2d(brick_array[i][j].rectangle[3].x, brick_array[i][j].rectangle[3].y);
			glEnd();
		}
	}

	glFlush();
}

// 공과 벽 충돌
void Collision_Detection_to_Walls(void) {
	// window bottom
	if (moving_ball.y <= moving_ball_radius) velocity.y = -velocity.y;
	// window top
	if (moving_ball.y >= height - moving_ball_radius - 2) velocity.y *= -1;
	// window left
	if (moving_ball.x <= moving_ball_radius) velocity.x *= -1;
	// window right
	if (moving_ball.x >= width - moving_ball_radius) velocity.x *= -1;
}

// 공과 바 충돌
void Collision_Detection_to_bar() {
	if (bar.rectangle[0].x <= moving_ball.x
		&& bar.rectangle[3].x >= moving_ball.x) {
		// top collision
		if (bar.rectangle[3].y >= moving_ball.y - moving_ball_radius
			&& velocity.y <= 0
			&& distance(Point(moving_ball.x, bar.rectangle[3].y), moving_ball) <= moving_ball_radius) {
			velocity.y *= -1;
		}

		// bottom collision
		if (bar.rectangle[1].y >= moving_ball.y - moving_ball_radius
			&& velocity.y >= 0
			&& distance(Point(moving_ball.x, bar.rectangle[1].y), moving_ball) <= moving_ball_radius) {
			velocity.y *= -1;
		}
	}

	if (bar.rectangle[0].y >= moving_ball.y
		&& bar.rectangle[1].y <= moving_ball.y) {
		// left collision
		if (bar.rectangle[0].y >= moving_ball.x + moving_ball_radius
			&& velocity.x >= 0
			&& distance(Point(bar.rectangle[0].x, moving_ball.y), moving_ball) <= moving_ball_radius) {
			velocity.x *= -1;
		}

		// right collision
		if (bar.rectangle[3].y <= moving_ball.x + moving_ball_radius
			&& velocity.x >= 0
			&& distance(Point(bar.rectangle[3].x, moving_ball.y), moving_ball) <= moving_ball_radius) {
			velocity.x *= -1;
		}
	}

	// top-left collision
	if (distance(Point(bar.rectangle[0]), Point(moving_ball)) <= moving_ball_radius) {
		if (velocity.x >= 0 && velocity.y <= 0) {
			velocity.x *= -1;
			velocity.y *= -1;
		}
		else if (velocity.x >= 0 && velocity.y >= 0) {
			velocity.x *= -1;
		}
		else if (velocity.x <= 0 && velocity.y <= 0) {
			velocity.y *= -1;
		}
	}

	// top-right collision
	if (distance(Point(bar.rectangle[3]), Point(moving_ball)) <= moving_ball_radius) {
		if (velocity.x >= 0 && velocity.y <= 0) {
			velocity.y *= -1;
		}
		else if (velocity.x <= 0 && velocity.y >= 0) {
			velocity.x *= -1;
			velocity.y *= -1;
		}
		else if (velocity.x <= 0 && velocity.y <= 0) {
			velocity.x *= -1;
			velocity.y *= -1;
		}
	}

	// bottom-left collision
	if (distance(Point(bar.rectangle[1]), Point(moving_ball)) <= moving_ball_radius) {
		if (velocity.x >= 0 && velocity.y <= 0) {
			velocity.x *= -1;
		}
		else if (velocity.x >= 0 && velocity.y >= 0) {
			velocity.x *= -1;
			velocity.y *= -1;
		}
		else if (velocity.x <= 0 && velocity.y >= 0) {
			velocity.y *= -1;
		}
	}

	// bottom-right collision
	if (distance(Point(bar.rectangle[2]), Point(moving_ball)) <= moving_ball_radius) {
		if (velocity.x >= 0 && velocity.y >= 0) {
			velocity.y *= -1;
		}
		else if (velocity.x <= 0 && velocity.y >= 0) {
			velocity.x *= -1;
			velocity.y *= -1;
		}
		else if (velocity.x <= 0 && velocity.y <= 0) {
			velocity.x *= -1;
		}
	}
}

// bottom 충돌 -> 게임 오버
void Collision_Detection_to_game_over() {
	if (moving_ball.y - moving_ball_radius <= 0) { // 화면 아래로 공이 넘어가면
		// 공의 속도를 0으로 만들고
		velocity.x = 0;
		velocity.y = 0;

		window_game_now = false; // 게임 진행 화면 비활성화
		window_game_over = true; // 게임 오버 화면 활성화
		char gameover[50] = "gameover.png";
		intro_image_texture(gameover);

		// game over 화면을 그림
		glBegin(GL_QUADS);
		glTexCoord2d(0.0, 0.0);
		glVertex2d(0, height);

		glTexCoord2d(0.0, 1.0);
		glVertex2d(0, 0);

		glTexCoord2d(1.0, 1.0);
		glVertex2d(width, 0);

		glTexCoord2d(1.0, 0.0);
		glVertex2d(width, height);

		// retry의 경우를 대비하여 bar의 위치를 초기화함
		Modeling_bar_init();

		game_over = true;
		ball_start = false;
		glEnd();
	}
}

// 공과 벽돌 충돌
void Collision_Detection_to_bricks() {
	for (int i = 0; i < STAGE1_BLOCK_ROW; i++) {
		for (int j = 0; j < 10; j++) {
			if (brick_array[i][j].rectangle[0].x <= moving_ball.x
				&& brick_array[i][j].rectangle[3].x >= moving_ball.x) {
				// top collision
				if (brick_array[i][j].rectangle[3].y >= moving_ball.y - moving_ball_radius
					&& velocity.y <= 0
					&& distance(Point(moving_ball.x, brick_array[i][j].rectangle[3].y), moving_ball) <= moving_ball_radius + 3) {
					// velocity.x *= -1;
					velocity.y *= -1;
					count++;
					for (int k = 0; k < 4; k++) {
						brick_array[i][j].rectangle[k] = Point(0, 0);
					}

				}

				// bottom collision
				if (brick_array[i][j].rectangle[1].y <= moving_ball.y + moving_ball_radius
					&& velocity.y >= 0
					&& distance(Point(moving_ball.x, brick_array[i][j].rectangle[1].y), moving_ball) <= moving_ball_radius + 3) {
					velocity.y *= -1;
					count++;
					for (int k = 0; k < 4; k++) {
						brick_array[i][j].rectangle[k] = Point(0, 0);
					}
				}
			}

			if (brick_array[i][j].rectangle[0].y >= moving_ball.y
				&& brick_array[i][j].rectangle[1].y <= moving_ball.y) {
				// left collision
				if (brick_array[i][j].rectangle[0].x >= moving_ball.x + moving_ball_radius
					&& distance(Point(brick_array[i][j].rectangle[0].x, moving_ball.y), moving_ball) <= moving_ball_radius) {
					velocity.x *= -1;
					count++;
					for (int k = 0; k < 4; k++) {
						brick_array[i][j].rectangle[k] = Point(0, 0);
					}
				}

				// right collision
				if (brick_array[i][j].rectangle[3].x <= moving_ball.x + moving_ball_radius
					&& distance(Point(brick_array[i][j].rectangle[3].x, moving_ball.y), moving_ball) <= moving_ball_radius) {
					velocity.x *= -1;
					count++;
					for (int k = 0; k < 4; k++) {
						brick_array[i][j].rectangle[k] = Point(0, 0);
					}
				}
			}

			// top-left collision
			if (distance(Point(brick_array[i][j].rectangle[0]), Point(moving_ball)) <= moving_ball_radius) {
				if (velocity.x >= 0 && velocity.y <= 0) {
					velocity.x *= -1;
					velocity.y *= -1;
					count++;
				}
				else if (velocity.x >= 0 && velocity.y >= 0) {
					velocity.x *= -1;
					count++;
				}
				else if (velocity.x <= 0 && velocity.y <= 0) {
					velocity.y *= -1;
					count++;
				}
				for (int k = 0; k < 4; k++) {
					brick_array[i][j].rectangle[k] = Point(0, 0);
				}
			}
				
			// top-right collision
			if (distance(Point(brick_array[i][j].rectangle[3]), Point(moving_ball)) <= moving_ball_radius) {
				if (velocity.x >= 0 && velocity.y <= 0) {
					velocity.y *= -1;
					count++;
				}
				else if (velocity.x <= 0 && velocity.y >= 0) {
					velocity.x *= -1;
					velocity.y *= -1;
					count++;
				}
				else if (velocity.x <= 0 && velocity.y <= 0) {
					velocity.x *= -1;
					velocity.y *= -1;
					count++;
				}
				for (int k = 0; k < 4; k++) {
					brick_array[i][j].rectangle[k] = Point(0, 0);
				}
			}

			// bottom-left collision
			if (distance(Point(brick_array[i][j].rectangle[1]), Point(moving_ball)) <= moving_ball_radius) {
				if (velocity.x >= 0 && velocity.y <= 0) {
					velocity.x *= -1;
					count++;
				}
				else if (velocity.x >= 0 && velocity.y >= 0) {
					velocity.x *= -1;
					velocity.y *= -1;
					count++;
				}
				else if (velocity.x <= 0 && velocity.y >= 0) {
					velocity.y *= -1;
					count++;
				}
				for (int k = 0; k < 4; k++) {
					brick_array[i][j].rectangle[k] = Point(0, 0);
				}
			}

			// bottom-right collision
			if (distance(Point(brick_array[i][j].rectangle[2]), Point(moving_ball)) <= moving_ball_radius) {
				if (velocity.x >= 0 && velocity.y >= 0) {
					velocity.y *= -1;
					count++;
				}
				else if (velocity.x <= 0 && velocity.y >= 0) {
					velocity.x *= -1;
					velocity.y *= -1;
					count++;
				}
				else if (velocity.x <= 0 && velocity.y <= 0) {
					velocity.x *= -1;
					count++;
				}
				for (int k = 0; k < 4; k++) {
					brick_array[i][j].rectangle[k] = Point(0, 0);
				}
			}
		}
	}
	
}


// 아이템과 바 충돌 (아이템 획득)
void item_got_it() {
	for (int i = 0; i < 8; i++) {
		// bar의 범위 안에 item이 존재하면
		if (bar.rectangle[0].x <= item.item_text[i].x
			&& bar.rectangle[3].x >= item.item_text[i].x) {
			if (bar.rectangle[3].y >= item.item_text[i].y) {
				// item의 좌표를 원점으로 보내고
				item.item_text[i].x = 0;
				item.item_text[i].y = 0;
				// 획득한 item의 개수 count
				item_count++;
				printf("아이템 먹었어! %d\n", item_count);
			}
		}
	}
}

// 아이템 falling -> 아이템 좌표 이동 (천천히 내려감)
void Item_Falling() {
	int tmp = 0;
	int tmp_x = -1, tmp_y = -1;

	// item falling
	for (int i = 0; i < 8; i++) {
		// item 벽돌이 깨진 경우 수행
		if (item.flag[i] == true) {
			Modeling_Circle(item_radius, item.item_text[i]);
			// item falling 속도 조절
			item.item_text[i].y -= 3.0;
			if (item.item_text[i].y <= bar.rectangle[1].y) { // bar의 아랫부분 y 좌표보다 내려갈 경우
				item.flag[i] = -1; // 획득 실패 처리
			}
			item_got_it();
		}
	}

	for (int j = 0; j < 8; j++) {
		tmp = 0;
		tmp_x = item.upper_brick_num[j].x;
		tmp_y = item.upper_brick_num[j].y;

		for (int i = 0; i < 4; i++) {
			// item 벽돌이 깨져있는지 검사
			if (brick_array[tmp_x][tmp_y].rectangle[i].x == 0
				&& brick_array[tmp_x][tmp_y].rectangle[i].y == 0) {
				tmp++;
				// 벽돌의 모든 좌표(4개)가 원점으로 가있다면 item flag를 true로 만듬
				// item flag가 true : item falling 수행
				if (tmp == 4 && item.flag[j] == false) {
					item.flag[j] = true;
				}
			}
		}
	}

}

// game_start가 false일 때 호출됨
void Game_Intro() { 
	char intro[50] = "intro.png";
	intro_image_texture(intro);

	// game intro 화면을 그림
	glBegin(GL_QUADS);

	glTexCoord2d(0.0, 0.0);
	glVertex2d(0, height);

	glTexCoord2d(0.0, 1.0);
	glVertex2d(0, 0);

	glTexCoord2d(1.0, 1.0);
	glVertex2d(width, 0);

	glTexCoord2d(1.0, 0.0);
	glVertex2d(width, height);
	glEnd();
}

void Game_Solution() {
	char gamesolution[50] = "gamesolution.png";
	intro_image_texture(gamesolution);

	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(0, height);

	glTexCoord2d(0.0, 1.0);
	glVertex2d(0, 0);

	glTexCoord2d(1.0, 1.0);
	glVertex2d(width, 0);

	glTexCoord2d(1.0, 0.0);
	glVertex2d(width, height);
	glEnd();
}

void Game_Now() {
	char game[50] = "game.png";
	intro_image_texture(game);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(0, height);

	glTexCoord2d(0.0, 1.0);
	glVertex2d(0, 0);

	glTexCoord2d(1.0, 1.0);
	glVertex2d(width, 0);

	glTexCoord2d(1.0, 0.0);
	glVertex2d(width, height);
	glEnd();
}

void Game_Clear1() {
	char clearNoAllItem[50] = "clear_no_all_item.png";
	intro_image_texture(clearNoAllItem);

	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(0, height);

	glTexCoord2d(0.0, 1.0);
	glVertex2d(0, 0);

	glTexCoord2d(1.0, 1.0);
	glVertex2d(width, 0);

	glTexCoord2d(1.0, 0.0);
	glVertex2d(width, height);
	glEnd();
}

void Game_Clear2() {
	char clearAllItem[50] = "clear_all_item.png";
	intro_image_texture(clearAllItem);

	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(0, height);

	glTexCoord2d(0.0, 1.0);
	glVertex2d(0, 0);

	glTexCoord2d(1.0, 1.0);
	glVertex2d(width, 0);

	glTexCoord2d(1.0, 0.0);
	glVertex2d(width, height);
	glEnd();
}


// 화면 렌더링
void RenderScene(void) {

	// 1. 게임 인트로 (window_intro)
	if (game_start == false && window_intro == true) { // 게임 인트로 화면
		Game_Intro();
	}
	
	// 2. 게임 방법 (window_game_solution)
	if (game_start == true && window_game_solution == true) {
		Game_Solution();
	}

	// 3. 게임 진행 화면 (window_game_now)
	// 게임 오버 상태가 아니고, 게임 시작 상태일 때
	if (game_start == true && game_over != true && window_game_now == true) { // 게임 진행 중에만 게임 화면을 그림
		Game_Now();

		// 충돌 함수들 호출
		Collision_Detection_to_Walls(); // 공-벽 충돌 함수
		Collision_Detection_to_bricks(); // 공-벽돌 충돌 함수
		Collision_Detection_to_bar(); // 공-바 충돌 함수

		// 저장된 벽돌 위치와 bar 위치를 이용하여 그림
		Modeling_Brick();

		// 움직이는 공 그리기
		glColor3f(1.0, 1.0, 1.0);
		Modeling_Circle(moving_ball_radius, moving_ball);

		// 아이템 Falling, 획득 함수 호출
		Item_Falling();

		// game start 이후, 'F2' key를 누른 경우 -> ball에 속력이 주어지도록 함
		if (ball_start) {
			// 움직이는 공의 위치 변화
			moving_ball.x += velocity.x;
			moving_ball.y += velocity.y;
		}
	}

	// 4. 게임 Clear 화면 (벽돌 70개를 다 깬 경우)
	if (count == 70) {
		window_game_now = false;
		clear = true;

		// 4-1. Clear : 아이템 8개를 다 모으지 못한 경우
		if (item_count != 8) {
			Game_Clear1();
		}
		// 4-2. Clear : 아이템 8개를 모두 모은 경우
		else {
			Game_Clear2();
		}
	}

	// 5. 게임 오버 화면 (window_game_over)
	// bottom 밑으로 공이 내려가면 game over 처리 -> 게임 over 화면으로 전환
	Collision_Detection_to_game_over();

	glutSwapBuffers();
	glFlush();
}

void SpecialKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT: // 왼쪽 방향키
		if (game_start && ball_start) {
			if (bar_x1 >= 20) {
				bar_x1 -= 15;
				bar_x2 -= 15;
			}
		}
		break;
	case GLUT_KEY_RIGHT: // 오른쪽 방향키
		if (game_start && ball_start) {
			if (bar_x2 <= width - 20) {
				bar_x1 += 25;
				bar_x2 += 25;
			}
		}
		break;
	case GLUT_KEY_UP: // 위쪽 방향키
		if (game_start && ball_start) {
			if (bar_y1 >= 20 && bar_y1 <= 280) {
				bar_y1 += 20;
				bar_y2 += 20;
			}
		}
		break;
	case GLUT_KEY_DOWN: // 아래쪽 방향키
		if (game_start && ball_start) {
			if (bar_y1 >= 40 && bar_y1 <= 300) {
				bar_y1 -= 20;
				bar_y2 -= 20;
			}
		}
		break;

	/*
		// 현재 화면을 판단할 bool 변수
		bool window_intro = true; // 인트로 화면
		bool window_game_solution = false;
		bool window_game_now = false; // 게임 진행 화면
		bool window_game_over = false; // 게임 오버 화면 
	*/

	// 게임 활성화 'F1' Key
	case GLUT_KEY_F1:
		if (window_intro) { // 인트로 화면일 때 F1을 누르면
			game_start = true; // 게임 시작 활성화
			window_game_solution = true; // 게임 방법 화면 출력
			window_intro = false; // 인트로 화면 비활성화
		}
		else if (window_game_solution) { // 게임 방법 화면일 때 F1을 누르면
			window_game_solution = false; // 게임 방법 화면 비활성화
			window_game_now = true; // 게임 화면 
		}
		else if (game_over) { // 게임 오버 화면일 때 F1을 누르면
			game_over = false; // 게임 오버 비활성화
			init(); // 게임 retry
		}
		else if (clear) { // 클리어 화면일 때 F1을 누르면
			clear = false; // 클리어 비활성화
			init(); // 게임 retry
		}
		break;

	case GLUT_KEY_F2:
		if (game_start) ball_start = true;
		break;

	// Debug Mode Key (End -> F3, F4, F5, F6)
	case GLUT_KEY_END:
		debug_mode = true;
		velocity.x = 0;
		velocity.y = 0;
		break;
	case GLUT_KEY_F3:
		if (debug_mode) moving_ball.x -= 3;
		break;
	case GLUT_KEY_F4:
		if (debug_mode) moving_ball.y += 3;
		break;
	case GLUT_KEY_F5:
		if (debug_mode) moving_ball.x += 3;
		break;
	case GLUT_KEY_F6:
		if (debug_mode) moving_ball.y -= 3;
		break;
	default:
		break;
	}
	
	// 화면 재구성
	glutPostRedisplay();
}


void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(660, 50);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Fly To The Sky");

	init();
	glutReshapeFunc(MyReshape);

	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutSpecialFunc(SpecialKey);
	glutMainLoop();
}