#define	width 			600
#define	height			900

// 이미지 파일 열기
unsigned char* LoadMeshFromFile(const char* texFile) {
	int w, h;
	GLuint texture; // 텍스쳐 버퍼
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

void intro_image_texture(char a[]) {
	GLuint texID;
	unsigned char* bitmap;
	a = (char*)a;
	bitmap = LoadMeshFromFile(a);

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

void intro_image_texture_blend(char a[]) {
	GLuint texID;
	unsigned char* bitmap;
	a = (char*)a;
	bitmap = LoadMeshFromFile(a);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_ZERO);

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glBlendFunc(GL_ONE, GL_ONE);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
	free(bitmap);
}