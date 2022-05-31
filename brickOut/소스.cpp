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

bool game_over;// game over �Ǵ�
bool game_start;// game start �Ǵ�
bool ball_start; // ����� �Է��� ������ ���� ������

// ���� ȭ���� �Ǵ��� bool ����
bool window_intro;// ��Ʈ�� ȭ��
bool window_game_solution; // ���� ��� ȭ��
bool window_game_now;// ���� ���� ȭ��
bool window_game_over; // ���� ���� ȭ��
bool clear;// Ŭ���� ȭ��


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

// ������ ���� ��ġ
void item_init() {
	srand((unsigned int)time(NULL));

	int row, col;

	for (int i = 0; i < 8; i++) {
		row = rand() % STAGE1_BLOCK_ROW; // ������ ���� ���� ���� �� random �� ����
		col = rand() % 10; // 10���� �� ���� �� random �� ����

		// �������� ��ġ�� ���� ��ġ�Ѵ�.
		item.item_text[i].x = (brick_array[row][col].rectangle[0].x + brick_array[row][col].rectangle[3].x) / 2;
		item.item_text[i].y = brick_array[row][col].rectangle[1].y - item_radius;

		// �������� ���� ������ ��ġ�� ����Ѵ�.
		item.upper_brick_num[i].x = row;
		item.upper_brick_num[i].y = col;
	}
}

void init(void) {
	game_over = false; // game over �Ǵ�
	game_start = false; // game start �Ǵ�
	ball_start = false; // ����� �Է��� ������ ���� ������

	// ���� ȭ���� �Ǵ��� bool ����
	window_intro = true; // ��Ʈ�� ȭ��
	window_game_solution = false; // ���� ��� ȭ��
	window_game_now = false; // ���� ���� ȭ��
	window_game_over = false; // ���� ���� ȭ��
	clear = false; // Ŭ���� ȭ��

	item_radius = 20.0; // �������� ������ ���� ������
	moving_ball_radius = 15.0; // �����̴� ���� ������
	moving_ball.x = 305; // �����̴� ���� ���� x ��ǥ
	moving_ball.y = 170 + moving_ball_radius; // �����̴� ���� ���� y ��ǥ

	count = 0; // �� ������ ����

	// bar ��ǥ
	bar_x1 = 210; // ���� x��ǥ
	bar_x2 = 390; // ������ x��ǥ
	bar_y1 = 150; // �Ʒ��� y��ǥ
	bar_y2 = 170; // ���� y��ǥ

	velocity.x = -7.0; // 0.0 x ���� �ӵ�
	velocity.y = 7.0; // 0.05 y ���� �ӵ�

	int tmp_x_position = 10;
	int tmp_y_position = 890;


	// ���� ��ġ ����
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

	// ������ ���� ��ġ
	item_init();

}

// �Ÿ� ���ϴ� �Լ�
double distance(Point a, Point b) {
	return sqrt((pow(b.x - a.x, 2)) + (pow(b.y - a.y, 2)));
}

void MyReshape(int w, int h) {
	glViewport(0, 0, w, h); // ���ϴ��� ������ �Ǵ� ������ ���!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height); // mouse2()
}

// �� �׸���
void Modeling_Circle(float radius, Point CC) {
	float delta;
	delta = 2 * PI / polygon_num;

	glBegin(GL_POLYGON);
	for (int i = 0; i < polygon_num; i++)
		glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
	glEnd();
}

// bar ��ġ ����
void Modeling_bar_init() {
	// ���� ���� ��, �߰��� ���� bar�� ��ġ�� ������
	bar.rectangle[0] = Point(bar_x1, bar_y2);
	bar.rectangle[1] = Point(bar_x1, bar_y1);
	bar.rectangle[2] = Point(bar_x2, bar_y1);
	bar.rectangle[3] = Point(bar_x2, bar_y2);
}

// ����, �� �׸���
void Modeling_Brick() {
	// bar ��ġ ���� �Լ� ȣ��
	Modeling_bar_init();

	// bar �׸��� (ä�� �簢��), bar �� �׸���(LINE)
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


	// ���� �׸��� (ä�� �簢��), ���� �� �׸���(LINE)
	for (int i = 0; i < STAGE1_BLOCK_ROW; i++) {
		for (int j = 0; j < 10; j++) {
			glBegin(GL_QUADS);

			// ���� ������ �������� ������ ������ ��� ����
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
			// ���� ������ ����� ������ ��� ����
			else {
				glColor3f(1.0, 1.0, 1.0);
			}

			glVertex2d(brick_array[i][j].rectangle[0].x, brick_array[i][j].rectangle[0].y);
			glVertex2d(brick_array[i][j].rectangle[1].x, brick_array[i][j].rectangle[1].y);
			glVertex2d(brick_array[i][j].rectangle[2].x, brick_array[i][j].rectangle[2].y);
			glVertex2d(brick_array[i][j].rectangle[3].x, brick_array[i][j].rectangle[3].y);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glLineWidth(10.0); // LINE �β� ����
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

// ���� �� �浹
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

// bottom �浹 -> ���� ����
void Collision_Detection_to_game_over() {
	if (moving_ball.y - moving_ball_radius <= 0) { // ȭ�� �Ʒ��� ���� �Ѿ��
		// ���� �ӵ��� 0���� �����
		velocity.x = 0;
		velocity.y = 0;

		window_game_now = false; // ���� ���� ȭ�� ��Ȱ��ȭ
		window_game_over = true; // ���� ���� ȭ�� Ȱ��ȭ
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

		// retry�� ��츦 ����Ͽ� bar�� ��ġ�� �ʱ�ȭ��
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


// �����۰� �� �浹 (������ ȹ��)
void item_got_it() {
	for (int i = 0; i < 8; i++) {
		// bar�� ���� �ȿ� item�� �����ϸ�
		if (bar.rectangle[0].x <= item.item_text[i].x
			&& bar.rectangle[3].x >= item.item_text[i].x) {
			if (bar.rectangle[3].y >= item.item_text[i].y) {
				// item�� ��ǥ�� �������� ������
				item.item_text[i].x = 0;
				item.item_text[i].y = 0;
				// ȹ���� item�� ���� count
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

	// item falling
	for (int i = 0; i < 8; i++) {
		// item ������ ���� ��� ����
		if (item.flag[i] == true) {
			Modeling_Circle(item_radius, item.item_text[i]);
			// item falling �ӵ� ����
			item.item_text[i].y -= 3.0;
			if (item.item_text[i].y <= bar.rectangle[1].y) { // bar�� �Ʒ��κ� y ��ǥ���� ������ ���
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
			// item ������ �����ִ��� �˻�
			if (brick_array[tmp_x][tmp_y].rectangle[i].x == 0
				&& brick_array[tmp_x][tmp_y].rectangle[i].y == 0) {
				tmp++;
				// ������ ��� ��ǥ(4��)�� �������� ���ִٸ� item flag�� true�� ����
				// item flag�� true : item falling ����
				if (tmp == 4 && item.flag[j] == false) {
					item.flag[j] = true;
				}
			}
		}
	}

}

// game_start�� false�� �� ȣ���
void Game_Intro() { 
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


// ȭ�� ������
void RenderScene(void) {

	// 1. ���� ��Ʈ�� (window_intro)
	if (game_start == false && window_intro == true) { // ���� ��Ʈ�� ȭ��
		Game_Intro();
	}
	
	// 2. ���� ��� (window_game_solution)
	if (game_start == true && window_game_solution == true) {
		Game_Solution();
	}

	// 3. ���� ���� ȭ�� (window_game_now)
	// ���� ���� ���°� �ƴϰ�, ���� ���� ������ ��
	if (game_start == true && game_over != true && window_game_now == true) { // ���� ���� �߿��� ���� ȭ���� �׸�
		Game_Now();

		// �浹 �Լ��� ȣ��
		Collision_Detection_to_Walls(); // ��-�� �浹 �Լ�
		Collision_Detection_to_bricks(); // ��-���� �浹 �Լ�
		Collision_Detection_to_bar(); // ��-�� �浹 �Լ�

		// ����� ���� ��ġ�� bar ��ġ�� �̿��Ͽ� �׸�
		Modeling_Brick();

		// �����̴� �� �׸���
		glColor3f(1.0, 1.0, 1.0);
		Modeling_Circle(moving_ball_radius, moving_ball);

		// ������ Falling, ȹ�� �Լ� ȣ��
		Item_Falling();

		// game start ����, 'F2' key�� ���� ��� -> ball�� �ӷ��� �־������� ��
		if (ball_start) {
			// �����̴� ���� ��ġ ��ȭ
			moving_ball.x += velocity.x;
			moving_ball.y += velocity.y;
		}
	}

	// 4. ���� Clear ȭ�� (���� 70���� �� �� ���)
	if (count == 70) {
		window_game_now = false;
		clear = true;

		// 4-1. Clear : ������ 8���� �� ������ ���� ���
		if (item_count != 8) {
			Game_Clear1();
		}
		// 4-2. Clear : ������ 8���� ��� ���� ���
		else {
			Game_Clear2();
		}
	}

	// 5. ���� ���� ȭ�� (window_game_over)
	// bottom ������ ���� �������� game over ó�� -> ���� over ȭ������ ��ȯ
	Collision_Detection_to_game_over();

	glutSwapBuffers();
	glFlush();
}

void SpecialKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT: // ���� ����Ű
		if (game_start && ball_start) {
			if (bar_x1 >= 20) {
				bar_x1 -= 15;
				bar_x2 -= 15;
			}
		}
		break;
	case GLUT_KEY_RIGHT: // ������ ����Ű
		if (game_start && ball_start) {
			if (bar_x2 <= width - 20) {
				bar_x1 += 25;
				bar_x2 += 25;
			}
		}
		break;
	case GLUT_KEY_UP: // ���� ����Ű
		if (game_start && ball_start) {
			if (bar_y1 >= 20 && bar_y1 <= 280) {
				bar_y1 += 20;
				bar_y2 += 20;
			}
		}
		break;
	case GLUT_KEY_DOWN: // �Ʒ��� ����Ű
		if (game_start && ball_start) {
			if (bar_y1 >= 40 && bar_y1 <= 300) {
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

	// ���� Ȱ��ȭ 'F1' Key
	case GLUT_KEY_F1:
		if (window_intro) { // ��Ʈ�� ȭ���� �� F1�� ������
			game_start = true; // ���� ���� Ȱ��ȭ
			window_game_solution = true; // ���� ��� ȭ�� ���
			window_intro = false; // ��Ʈ�� ȭ�� ��Ȱ��ȭ
		}
		else if (window_game_solution) { // ���� ��� ȭ���� �� F1�� ������
			window_game_solution = false; // ���� ��� ȭ�� ��Ȱ��ȭ
			window_game_now = true; // ���� ȭ�� 
		}
		else if (game_over) { // ���� ���� ȭ���� �� F1�� ������
			game_over = false; // ���� ���� ��Ȱ��ȭ
			init(); // ���� retry
		}
		else if (clear) { // Ŭ���� ȭ���� �� F1�� ������
			clear = false; // Ŭ���� ��Ȱ��ȭ
			init(); // ���� retry
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
	
	// ȭ�� �籸��
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