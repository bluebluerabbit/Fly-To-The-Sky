#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h> // (or others, depending on the system in use)
#include <math.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cstdlib>

#include "image_texture_mapping.hpp"

#define STAGE1_BLOCK_ROW 7
#define ITEM_NUMBER 8
#define	width 			600
#define	height			900
#define	PI				3.1415
#define	polygon_num		50

int	left = 0;
int	bottom = 0;

int	collision_count = 0;

int count = 0;
int item_count = 0;

float radius1, moving_ball_radius;
float item_radius;

bool debug_mode = false;

bool collision = false;
bool game_over = false; // game over �Ǵ�
bool game_start = false; // game start �Ǵ�
bool ball_start = false; // ����� �Է��� ������ ���� ������

// ���� ȭ���� �Ǵ��� bool ����
bool window_intro = true; // ��Ʈ�� ȭ��
bool window_game_solution = false;
bool window_game_now = false; // ���� ���� ȭ��
bool window_game_over = false; // ���� ���� ȭ�� 

// �� ���������� �Ǵ��� bool ����
bool stage[3] = { false, false, false };

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


// ������ ��ġ�� �����ϴ� class
class BRICKS {
public:
	Point rectangle[4];
};

BRICKS brick_array[7][10]; // 7�� (���� 10����)
BRICKS bar;

// ������ ��ġ�� �����ϴ� class
class ITEM {
public:
	Point item_text[8];
	Point upper_brick_num[8];
	int flag[8] = { false, false, false, false ,
					false, false, false, false };
};

ITEM item;

// �� -> w: 180, h: 20
int bar_x1, bar_x2, bar_y1, bar_y2;

void item_init() {
	srand((unsigned int)time(NULL));

	int row, col;

	for (int i = 0; i < 8; i++) {
		row = rand() % 7;
		col = rand() % 10;

		item.item_text[i].x = (brick_array[row][col].rectangle[0].x + brick_array[row][col].rectangle[3].x) / 2;
		item.item_text[i].y = brick_array[row][col].rectangle[1].y - item_radius;

		item.upper_brick_num[i].x = row;
		item.upper_brick_num[i].y = col;
	}
}

void init(void) {
	radius1 = 20.0;

	item_radius = 20.0; // �������� ������ ���� ������
	moving_ball_radius = 15.0; // �����̴� ���� ������
	moving_ball.x = 305; // �����̴� ���� ���� x ��ǥ
	moving_ball.y = 170 + moving_ball_radius; // �����̴� ���� ���� y ��ǥ

	bar_x1 = 210;
	bar_x2 = 390;
	bar_y1 = 150;
	bar_y2 = 170;

	velocity.x = -7.0; // 0.0 x ���� �ӵ�
	velocity.y = 7.0; // 0.05 y ���� �ӵ�

	int tmp_x_position = 10;
	int tmp_y_position = 890;


	// ���� ��ġ ����
	for (int i = 0; i < 7; i++) {
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

	item_init();

	collision_count = 1;

}

// �Ÿ� ���ϴ� �Լ�
double distance(Point a, Point b) {
	return sqrt((pow(b.x - a.x, 2)) + (pow(b.y - a.y, 2)));
}

void MyReshape(int w, int h) {
	glViewport(0, 0, w, h); // ���ϴ��� ������ �Ǵ� ������ ���!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left, left + width, bottom, bottom + height); // mouse2()
}

// �� �׸���
void Modeling_Circle(float radius, Point CC) {
	float	delta;
	
	delta = 2 * PI / polygon_num;
	glBegin(GL_POLYGON);
	for (int i = 0; i < polygon_num; i++)
		glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
	glEnd();
}

// bar �׸���
void Modeling_bar_init() {
	// ���� ���� ��, �߰��� ���� bar�� �׸�
	bar.rectangle[0] = Point(bar_x1, bar_y2);
	bar.rectangle[1] = Point(bar_x1, bar_y1);
	bar.rectangle[2] = Point(bar_x2, bar_y1);
	bar.rectangle[3] = Point(bar_x2, bar_y2);
}

// ����, �� �׸���
void Modeling_Brick() {
	Modeling_bar_init();

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

	/*
	brick_array[5][0].rectangle[0].x = 200;
	brick_array[5][0].rectangle[0].y = 540;

	brick_array[5][0].rectangle[1].x = 200;
	brick_array[5][0].rectangle[1].y = 300;

	brick_array[5][0].rectangle[2].x = 300;
	brick_array[5][0].rectangle[2].y = 300;

	brick_array[5][0].rectangle[3].x = 300;
	brick_array[5][0].rectangle[3].y = 540;
	*/

	// brick
	for (int i = 0; i < STAGE1_BLOCK_ROW; i++) {
		for (int j = 0; j < 10; j++) {
			glBegin(GL_QUADS);
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
			else {
				glColor3f(1.0, 1.0, 1.0);
			}

			glVertex2d(brick_array[i][j].rectangle[0].x, brick_array[i][j].rectangle[0].y);
			glVertex2d(brick_array[i][j].rectangle[1].x, brick_array[i][j].rectangle[1].y);
			glVertex2d(brick_array[i][j].rectangle[2].x, brick_array[i][j].rectangle[2].y);
			glVertex2d(brick_array[i][j].rectangle[3].x, brick_array[i][j].rectangle[3].y);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glLineWidth(10.0);
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

// ���� �� �浹
void Collision_Detection_Between_Balls(void) {
	float	distance;

	float minusX = (fixed_ball.x - moving_ball.x);
	float minusY = (fixed_ball.y - moving_ball.y);

	distance = sqrt(pow(minusX, 2) + pow(minusY, 2));

	float R1PlusR2 = radius1 + moving_ball_radius;

	// std::cout << distance << "\n";
}

// ���� �� �浹
void Collision_Detection_to_Walls(void) {
	if (moving_ball.y <= moving_ball_radius) velocity.y = -velocity.y;
	if (moving_ball.y >= height - moving_ball_radius - 2) velocity.y *= -1;

	if (moving_ball.x <= moving_ball_radius) velocity.x *= -1;
	if (moving_ball.x >= width - moving_ball_radius) velocity.x *= -1;
}

// ���� �� �浹
void Collision_Detection_to_bar() {
	if (bar.rectangle[0].x <= moving_ball.x
		&& bar.rectangle[3].x >= moving_ball.x) {
		if (bar.rectangle[3].y >= moving_ball.y - moving_ball_radius
			&& velocity.y < 0
			&& distance(Point(moving_ball.x, bar.rectangle[3].y), moving_ball) <= moving_ball_radius) {
			// velocity.x *= -1;
			velocity.y *= -1;
		}
	}
}

// �ٴ� �浹 -> ���� ����
void Collision_Detection_to_game_over() {
	if (moving_ball.y - moving_ball_radius <= 0) { // ȭ�� �Ʒ��� ���� �Ѿ��
		// ���� �ӵ��� 0���� �����
		velocity.x = 0;
		velocity.y = 0;

		window_game_now = false;
		window_game_over = true;
		char gameover[50] = "gameover.png";
		intro_image_texture(gameover);

		// game over ȭ���� �׸�
		glBegin(GL_QUADS);
		glTexCoord2d(0.0, 0.0);
		glVertex2d(0, height);

		glTexCoord2d(0.0, 1.0);
		glVertex2d(0, 0);

		glTexCoord2d(1.0, 1.0);
		glVertex2d(width, 0);

		glTexCoord2d(1.0, 0.0);
		glVertex2d(width, height);

		Modeling_bar_init();
		game_over = true;
		ball_start = false;
		glEnd();
	}
}

// ���� ���� �浹
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
					printf("top\n");
					printf("%d\n", count);
					for (int k = 0; k < 4; k++) {
						brick_array[i][j].rectangle[k] = Point(0, 0);
					}

				}

				// bottom collision
				else if (brick_array[i][j].rectangle[0].x <= moving_ball.x
					&& brick_array[i][j].rectangle[3].x >= moving_ball.x) {
					if (brick_array[i][j].rectangle[1].y <= moving_ball.y + moving_ball_radius
						&& velocity.y >= 0
						&& distance(Point(moving_ball.x, brick_array[i][j].rectangle[1].y), moving_ball) <= moving_ball_radius + 3) {
						velocity.y *= -1;
						count++;
						printf("bottom\n");
						printf("%d\n", count);
						for (int k = 0; k < 4; k++) {
							brick_array[i][j].rectangle[k] = Point(0, 0);
						}
					}
				}


			}

			// left collision
			if (brick_array[i][j].rectangle[0].y >= moving_ball.y + moving_ball_radius
				&& brick_array[i][j].rectangle[1].y <= moving_ball.y - moving_ball_radius) {
				if (brick_array[i][j].rectangle[0].x >= moving_ball.x + moving_ball_radius
					&& distance(Point(brick_array[i][j].rectangle[0].x, moving_ball.y), moving_ball) <= moving_ball_radius + 3) {
					velocity.x *= -1;
					count++;
					printf("left\n");
					printf("%d\n", count);
					for (int k = 0; k < 4; k++) {
						brick_array[i][j].rectangle[k] = Point(0, 0);
					}
				}
			}

			// right collision
			if (brick_array[i][j].rectangle[0].y >= moving_ball.y + moving_ball_radius
				&& brick_array[i][j].rectangle[1].y <= moving_ball.y - moving_ball_radius) {
				if (brick_array[i][j].rectangle[3].x <= moving_ball.x + moving_ball_radius
					&& distance(Point(brick_array[i][j].rectangle[3].x, moving_ball.y), moving_ball) <= moving_ball_radius + 3) {
					velocity.x *= -1;
					count++;
					printf("right\n");
					printf("%d\n", count);
					for (int k = 0; k < 4; k++) {
						brick_array[i][j].rectangle[k] = Point(0, 0);
					}
				}
			}

			// top-left collision
			if (((brick_array[i][j].rectangle[0].x) - (moving_ball.x + moving_ball_radius) <= 3) &&
				((brick_array[i][j].rectangle[0].y) - (moving_ball.y - moving_ball_radius)) <= 3 &&
				distance(Point(brick_array[i][j].rectangle[0]), Point(moving_ball)) <= moving_ball_radius) {
				if (velocity.x >= 0 && velocity.y <= 0) {
					velocity.x *= -1;
					velocity.y *= -1;
					printf("�»��1\n");
				}
				else if (velocity.x >= 0 && velocity.y >= 0) {
					velocity.x *= -1;
					printf("�»��2\n");
				}
				else if (velocity.x <= 0 && velocity.y <= 0) {
					velocity.y *= -1;
					printf("�»��3\n");
				}
				for (int k = 0; k < 4; k++) {
					brick_array[i][j].rectangle[k] = Point(0, 0);
				}
			}

				
			// top-right collision

			// bottom-left collision

			// bottom-right collision
		}
	}
	
}


// �����۰� �� �浹 (������ ����)
void item_got_it() {
	for (int i = 0; i < 8; i++) {
		if (bar.rectangle[0].x <= item.item_text[i].x
			&& bar.rectangle[3].x >= item.item_text[i].x) {
			if (bar.rectangle[3].y >= item.item_text[i].y) {
				item.item_text[i].x = 0;
				item.item_text[i].y = 0;
				item_count++;
				printf("������ �Ծ���! %d\n", item_count);
			}
		}
	}
}

// ������ falling -> ������ ��ǥ �̵� (õõ�� ������)
void Item_Falling() {
	int tmp = 0;
	int tmp_x = -1, tmp_y = -1;

	// 'LOVE' falling
	for (int i = 0; i < 8; i++) {
		if (item.flag[i] == true) {
			Modeling_Circle(item_radius, item.item_text[i]);
			item.item_text[i].y -= 5.0;
			if (item.item_text[i].y <= bar.rectangle[1].y) { // ���� ������
				item.flag[i] = -1; // ȹ�� ���� ó��
			}
			item_got_it();
		}
	}

	for (int j = 0; j < 8; j++) {
		tmp = 0;
		tmp_x = item.upper_brick_num[j].x;
		tmp_y = item.upper_brick_num[j].y;
		for (int i = 0; i < 4; i++) {
			if (brick_array[tmp_x][tmp_y].rectangle[i].x == 0
				&& brick_array[tmp_x][tmp_y].rectangle[i].y == 0) {
				tmp++;
				if (tmp == 4 && item.flag[j] == false) {
					item.flag[j] = true;
				}
			}
		}
	}

}


void Game_Intro() { // game_start�� false�� �� ȣ���
	char intro[50] = "intro.png";
	intro_image_texture(intro);
	// game intro ȭ���� �׸�
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

// ȭ�� ������
void RenderScene(void) {
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// ���� ���� ���°� �ƴ� ��
	if (game_start == false && window_intro == true) { // ���� ��Ʈ�� ȭ��
		Game_Intro();
	}

	if (game_start == true && window_game_solution == true) {
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

	/*
		// ���� ȭ���� �Ǵ��� bool ����
		bool window_intro = true; // ��Ʈ�� ȭ��
		bool window_game_solution = false;
		bool window_game_now = false; // ���� ���� ȭ��
		bool window_game_over = false; // ���� ���� ȭ��
	*/


	// ���� ���� ���°� �ƴϰ�, ���� ���� ������ ��
	if (game_start == true && game_over != true && window_game_now == true) { // ���� ���� �߿��� ���� ȭ���� �׸�
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

		// �浹 ó�� �κ�
		Collision_Detection_Between_Balls(); // ��-�� �浹 �Լ� 
		Collision_Detection_to_Walls(); // ��-�� �浹 �Լ�
		Collision_Detection_to_bricks(); // ��-���� �浹 �Լ�
		Collision_Detection_to_bar(); // ��-�� �浹 �Լ�

		Modeling_Brick(); // ����� ���� ��ġ �迭�� ���� �迭�� �׸�


		// �����̴� �� �׸���
		glColor3f(1.0, 1.0, 1.0);
		Modeling_Circle(moving_ball_radius, moving_ball);

		// ������ Falling
		Item_Falling();



		if (ball_start) {
			// �����̴� ���� ��ġ ��ȭ
			moving_ball.x += velocity.x;
			moving_ball.y += velocity.y;
		}
	}

	int item_clear = 0;

	// ���� 70���� �� ����
	if (count == 70) {
		window_game_now = false;

		if (item_count != 8) {
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
		else {
			char clearNoAllItem[50] = "intro.png";
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
	}

	// bottom ������ ���� �������� game over ó�� -> ���� over ȭ������ ��ȯ
	// Collision_Detection_to_game_over();

	glutSwapBuffers();
	glFlush();
}

void SpecialKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
		if (game_start && ball_start) {
			if (bar_x1 >= 20) {
				bar_x1 -= 15;
				bar_x2 -= 15;
			}
		}
		break;

	case GLUT_KEY_RIGHT:
		if (game_start && ball_start) {
			if (bar_x2 <= width - 20) {
				bar_x1 += 25;
				bar_x2 += 25;
			}
		}
		break;
	case GLUT_KEY_UP:
		if (game_start && ball_start) {
			if (bar_y1 >= 150 && bar_y1 <= 280) {
				bar_y1 += 20;
				bar_y2 += 20;
			}
		}
		break;
	case GLUT_KEY_DOWN:
		if (game_start && ball_start) {
			if (bar_y1 >= 170 && bar_y1 <= 300) {
				bar_y1 -= 20;
				bar_y2 -= 20;
			}
		}
		break;


	/*
		// ���� ȭ���� �Ǵ��� bool ����
		bool window_intro = true; // ��Ʈ�� ȭ��
		bool window_game_solution = false;
		bool window_game_now = false; // ���� ���� ȭ��
		bool window_game_over = false; // ���� ���� ȭ�� 
	*/
	case GLUT_KEY_F1:
		if (window_intro) { // ��Ʈ�� ȭ���� �� F1�� ������
			game_start = true; // ���� ���� Ȱ��ȭ
			window_game_solution = true; // ���� ��� ȭ�� ���
			window_intro = false; // ��Ʈ�� ȭ�� ��Ȱ��ȭ
		}
		else if (window_game_solution) { // ���� ��� ȭ���� �� F1�� ������
			window_game_solution = false; // ���� ��� ȭ�� ��Ȱ��ȭ
			window_game_now = true; // ���� ȭ�� Ȱ��ȭ
		}
		else if (game_over) { // ���� ���� ȭ���� �� F1�� ������
			game_over = false; // ���� ���� ��Ȱ��ȭ
			init(); // ���� retry
		}
		break;
	case GLUT_KEY_F2:
		if (game_start) ball_start = true;
		break;
	case GLUT_KEY_END:
		debug_mode = true;
		velocity.x = 0;
		velocity.y = 0;
		break;

	case GLUT_KEY_F3:
		if (debug_mode) moving_ball.x -= 10;
		break;
	case GLUT_KEY_F4:
		if (debug_mode) moving_ball.y += 10;
		break;
	case GLUT_KEY_F5:
		if (debug_mode) moving_ball.x += 10;
		break;
	case GLUT_KEY_F6:
		if (debug_mode) moving_ball.y -= 10;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}


void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Bouncing Ball & Wall");

	init();
	glutReshapeFunc(MyReshape);

	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutSpecialFunc(SpecialKey);
	glutMainLoop();
}