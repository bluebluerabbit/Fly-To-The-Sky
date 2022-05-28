#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h> // (or others, depending on the system in use)
#include <math.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cstdlib>

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

float radius1, moving_ball_radius;
float item_radius;

bool collision = false;
bool game_over = false; // game over �Ǵ�
bool game_start = false; // game start �Ǵ�
bool ball_start = false; // ����� �Է��� ������ ���� ������

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
	Point item_text[8]; // 'L', 'O', 'V', 'E' 2����
	Point upper_brick_num[8];
	int flag[8] = { false, false, false, false ,
					false, false, false, false };
};

ITEM LOVE, HOPE, LUCK;

// �� -> w: 180, h: 20
int bar_x1 = 210;
int bar_x2 = 390;
int bar_y1 = 50;
int bar_y2 = 70;

void item_init() {
	srand((unsigned int)time(NULL));

	int row, col;

	for (int i = 0; i < 8; i++) {
		row = rand() % 7;
		col = rand() % 10;

		LOVE.item_text[i].x = (brick_array[row][col].rectangle[0].x + brick_array[row][col].rectangle[3].x) / 2;
		LOVE.item_text[i].y = brick_array[row][col].rectangle[1].y - item_radius;

		LOVE.upper_brick_num[i].x = row;
		LOVE.upper_brick_num[i].y = col;
	}
}

void init(void) {
	radius1 = 20.0;

	item_radius = 20.0; // �������� ������ ���� ������
	moving_ball_radius = 10.0; // �����̴� ���� ������
	moving_ball.x = 300; // �����̴� ���� ���� x ��ǥ
	moving_ball.y = 70 + moving_ball_radius; // �����̴� ���� ���� y ��ǥ
	
	velocity.x = -1.0; // 0.0 x ���� �ӵ�
	velocity.y = 1.0; // 0.05 y ���� �ӵ�

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

void Modeling_bar_init() {
	// ���� ���� ��, �߰��� ���� bar�� �׸�
	bar.rectangle[0] = Point(bar_x1, bar_y2);
	bar.rectangle[1] = Point(bar_x1, bar_y1);
	bar.rectangle[2] = Point(bar_x2, bar_y1);
	bar.rectangle[3] = Point(bar_x2, bar_y2);
}

// ����, �� �׸���
void Modeling_Brick() {
	glBegin(GL_QUADS);
	glColor3f(1.0, 0.0, 0.0);

	Modeling_bar_init();

	// bar
	glVertex2d(bar.rectangle[0].x, bar.rectangle[0].y);
	glVertex2d(bar.rectangle[1].x, bar.rectangle[1].y);
	glVertex2d(bar.rectangle[2].x, bar.rectangle[2].y);
	glVertex2d(bar.rectangle[3].x, bar.rectangle[3].y);

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
			if ((LOVE.upper_brick_num[0].x == i && LOVE.upper_brick_num[0].y == j)
				|| (LOVE.upper_brick_num[1].x == i && LOVE.upper_brick_num[1].y == j)
				|| (LOVE.upper_brick_num[2].x == i && LOVE.upper_brick_num[2].y == j)
				|| (LOVE.upper_brick_num[3].x == i && LOVE.upper_brick_num[3].y == j)
				|| (LOVE.upper_brick_num[4].x == i && LOVE.upper_brick_num[4].y == j)
				|| (LOVE.upper_brick_num[5].x == i && LOVE.upper_brick_num[5].y == j)
				|| (LOVE.upper_brick_num[6].x == i && LOVE.upper_brick_num[6].y == j)
				|| (LOVE.upper_brick_num[7].x == i && LOVE.upper_brick_num[7].y == j)) {

				glColor3f(0.0, 1.0, 0.0);
			}
			else {
				glColor3f(1.0, 1.0, 1.0);
			}
			glVertex2d(brick_array[i][j].rectangle[0].x, brick_array[i][j].rectangle[0].y);
			glVertex2d(brick_array[i][j].rectangle[1].x, brick_array[i][j].rectangle[1].y);
			glVertex2d(brick_array[i][j].rectangle[2].x, brick_array[i][j].rectangle[2].y);
			glVertex2d(brick_array[i][j].rectangle[3].x, brick_array[i][j].rectangle[3].y);
		}
	}

	glEnd();
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

	if (moving_ball.y >= bottom + height - moving_ball_radius) { // bottom + height = top
		velocity.y *= -1;
		velocity.x = velocity.x;
	}
	if (moving_ball.x <= left + moving_ball_radius) {
		velocity.y = velocity.y;
		velocity.x *= -1;
	}
	if (moving_ball.x >= left + width - moving_ball_radius) { // left + width = right
		velocity.y = velocity.y;
		velocity.x *= -1;
	}
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

// �����۰� �� �浹 (������ ����
void item_got_it() {
	for (int i = 0; i < 8; i++) {
		if (LOVE.flag[i]) {
			if (bar.rectangle[0].x <= LOVE.item_text[i].x
				&& bar.rectangle[3].x >= LOVE.item_text[i].x) {
				if (bar.rectangle[3].y >= LOVE.item_text[i].y) {
					LOVE.item_text[i].x = 0;
					LOVE.item_text[i].y = 0;
				}
			}
		}
	}
}

// �ٴ� �浹 -> ���� ����
void Collision_Detection_to_game_over() {
	if (moving_ball.y - moving_ball_radius <= 0) { // ȭ�� �Ʒ��� ���� �Ѿ��
		// ���� �ӵ��� 0���� �����
		velocity.x = 0;
		velocity.y = 0;

		// game over ȭ���� �׸�
		glBegin(GL_QUADS);
		glColor3f(0.0, 0.0, 0.0);
		glVertex2d(0, height);
		glVertex2d(0, 0);
		glVertex2d(width, 0);
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
					&& velocity.y < 0
					&& distance(Point(moving_ball.x, brick_array[i][j].rectangle[3].y), moving_ball) <= moving_ball_radius) {
					// velocity.x *= -1;
					velocity.y *= -1;
					//printf("top\n");
					count++;
					for (int k = 0; k < 4; k++) {
						brick_array[i][j].rectangle[k] = Point(0, 0);
					}

				}

				// left collision
				if (brick_array[i][j].rectangle[0].y >= moving_ball.y
					&& brick_array[i][j].rectangle[1].y <= moving_ball.y) {
					if (brick_array[i][j].rectangle[0].x >= moving_ball.x
						&& velocity.x > 0
						&& distance(Point(brick_array[i][j].rectangle[0].x, moving_ball.y), moving_ball) <= moving_ball_radius) {
						velocity.x *= -1;
						//printf("left\n");
						count++;
						for (int k = 0; k < 4; k++) {
							brick_array[i][j].rectangle[k] = Point(0, 0);
						}
					}
				}

				// right collision
				if (brick_array[i][j].rectangle[0].y >= moving_ball.y
					&& brick_array[i][j].rectangle[1].y <= moving_ball.y) {
					if (brick_array[i][j].rectangle[3].x <= moving_ball.x
						&& velocity.x < 0
						&& distance(Point(brick_array[i][j].rectangle[3].x, moving_ball.y), moving_ball) <= moving_ball_radius) {
						velocity.x *= -1;
						//printf("right\n");
						count++;
						for (int k = 0; k < 4; k++) {
							brick_array[i][j].rectangle[k] = Point(0, 0);
						}
					}
				}

				// bottom collision
				if (brick_array[i][j].rectangle[0].x <= moving_ball.x
					&& brick_array[i][j].rectangle[3].x >= moving_ball.x) {
					if (brick_array[i][j].rectangle[1].y <= moving_ball.y + moving_ball_radius
						&& velocity.y > 0
						&& distance(Point(moving_ball.x, brick_array[i][j].rectangle[1].y), moving_ball) <= moving_ball_radius) {
						/*
						printf("���� �ذ� ���� �߽� ���� �Ÿ� : %lf\n", distance(brick_array[i][j].rectangle[1], moving_ball));
						printf("bottom collision :: [%d][%d] x : (%f ~ %f) -> %f / %f , %f\n",
							i, j, brick_array[i][j].rectangle[0].x, brick_array[i][j].rectangle[3].x,
							moving_ball.x + moving_ball_radius, brick_array[i][j].rectangle[1].y, moving_ball.y + moving_ball_radius);
						*/
						// velocity.x *= -1;
						velocity.y *= -1;
						//printf("bottom\n");
						count++;
						for (int k = 0; k < 4; k++) {
							brick_array[i][j].rectangle[k] = Point(0, 0);
						}
					}
				}

				// top-left collision
				
				// top-right collision

				// bottom-left collision

				// bottom-right collision
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
		if (LOVE.flag[i] == true) {
			Modeling_Circle(item_radius, LOVE.item_text[i]);
			LOVE.item_text[i].y -= 0.2;
			if (LOVE.item_text[i].y <= item_radius) {
				LOVE.flag[i] = -1;
			}
		}
	}

	for (int j = 0; j < 8; j++) {
		tmp = 0;
		tmp_x = LOVE.upper_brick_num[j].x;
		tmp_y = LOVE.upper_brick_num[j].y;
		for (int i = 0; i < 4; i++) {
			if (brick_array[tmp_x][tmp_y].rectangle[i].x == 0
				&& brick_array[tmp_x][tmp_y].rectangle[i].y == 0) {
				tmp++;
				if (tmp == 4 && LOVE.flag[j] == false) {
					LOVE.flag[j] = true;
				}
			}
		}
	}
}

// �̹��� ���� ����
unsigned char* LoadMeshFromFile(const char* texFile) {
	int w, h;

	GLuint texture; // �ؽ��� ����
	glGenTextures(1, &texture);

	FILE* fp = NULL;
	if (fopen_s(&fp, texFile, "rb")) {
		printf("ERROR : No %s. \n fail to bind %d\n", texFile, texture);
		return (unsigned char*)false;
	}

	int channel;
	unsigned char* image = stbi_load_from_file(fp, &w, &h, &channel, 4);
	fclose(fp);
	return image;
}

// �̹��� ���� ����
void intro_image_texture() {
	GLuint texID;

	unsigned char* bitmap;
	bitmap = LoadMeshFromFile((char*)"intro.png");
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	free(bitmap);
}

void Game_Intro() { // game_start�� false�� �� ȣ���
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
	glClear(GL_COLOR_BUFFER_BIT);

	// ������ Falling
	Item_Falling();

	// ������ ȹ��
	item_got_it();

	// bottom ������ ���� �������� game over ó�� -> ���� over ȭ������ ��ȯ
	// Collision_Detection_to_game_over();

	// ���� ���� ���°� �ƴ� ��
	if (game_start == false) { // ���� ��Ʈ�� ȭ��
		Game_Intro();
	}


	// ���� ���� ���°� �ƴϰ�, ���� ���� ������ ��
	else if (game_over != true && game_start != false) { // ���� ���� �߿��� ���� ȭ���� �׸�
		glClearColor(0.0, 0.0, 0.0, 0.0); // black background

		// �浹 ó�� �κ�
		Collision_Detection_Between_Balls(); // ��-�� �浹 �Լ� 
		Collision_Detection_to_Walls(); // ��-�� �浹 �Լ�
		Collision_Detection_to_bricks(); // ��-���� �浹 �Լ�
		Collision_Detection_to_bar(); // ��-�� �浹 �Լ�

		// �����̴� �� �׸��� 
		glColor3f(1.0, 1.0, 1.0);
		Modeling_Circle(moving_ball_radius, moving_ball);

		Modeling_Brick(); // ����� ���� ��ġ �迭�� ���� �迭�� �׸�

		if (ball_start) {
			// �����̴� ���� ��ġ ��ȭ
			moving_ball.x += velocity.x;
			moving_ball.y += velocity.y;
		}
	}

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
				bar_x1 += 15;
				bar_x2 += 15;
			}
		}
		break;
	case GLUT_KEY_UP:
		if (game_start && ball_start) {
			if (bar_y1 >= 40 && bar_y1 <= 160) {
				bar_y1 += 10;
				bar_y2 += 10;
				printf("%d\n", bar_y1);
				printf("%d\n", bar_y2);
			}
		}
		break;
	case GLUT_KEY_DOWN:
		if (game_start && ball_start) {
			if (bar_y1 >= 50 && bar_y1 <= 200) {
				bar_y1 -= 10;
				bar_y2 -= 10;
				printf("%d\n", bar_y1);
				printf("%d\n", bar_y2);
			}
		}
		break;

	case GLUT_KEY_F1:
		game_start = true;
		if (game_over) {
			game_over = false;
			bar_x1 = 210;
			bar_x2 = 390;
			init();
		}
		break;
	case GLUT_KEY_F2:
		if (game_start) ball_start = true;
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
	intro_image_texture();

	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutSpecialFunc(SpecialKey);
	glutMainLoop();
}