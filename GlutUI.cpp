//#include "GlutUI.h"
//
//GLuint texture_id;
//static const float green[] = { 1,1,1,1 };
//float m_rotX = 0, m_rotY = 0, m_scale = 1.0f;
//float angle_satellite = 0;
//double scale = 1.0;
//GLfloat aspect = 1.0;
//vector<Vec3> linkS;
//vector<Vec3> linkT;
//vector<Vec3> grounds;
//vector<Vec3> satellites;
//string bmpfile = "OpenGL/2k_earth_daymap.bmp";
//
//bool mouseLeftDown;
//float mouseX, mouseY;
//float cameraAngleX;
//float cameraAngleY;
//
//void AddGround(double x, double y, double z)
//{
//	Vec3 p(x, y, z);
//	grounds.push_back(p);
//}
//
//void AddSatellite(double x, double y, double z)
//{
//	Vec3 p(x, y, z);
//	satellites.push_back(p);
//}
//
//void AddLink(double xs, double ys, double zs, double xt, double yt, double zt)
//{
//	Vec3 s(xs, ys, zs);
//	linkS.push_back(s);
//	Vec3 t(xt, yt, zt);
//	linkT.push_back(t);
//}
//
////经纬度转换(弧度)
//BLH XYZtoLB(double X, double Y, double Z)
//{
//	BLH res(0, 0, 0);
//	double B = 0.0, N = 0.0, H = 0.0, R0, R1, deltaH, deltaB;
//	R0 = sqrt(pow(X, 2) + pow(Y, 2));
//	R1 = sqrt(pow(X, 2) + pow(Y, 2) + pow(Z, 2));
//	//经度直接求解
//	res.L = atan2(Y, X);
//	//迭代求大地维度和大地高
//	N = aa;
//	H = R1 - aa;
//	B = atan2(Z * (N + H), R0 * (N * (1 - e2) + H));
//	do
//	{
//		deltaH = N;//判断收敛所用
//		deltaB = B;
//		N = aa / sqrt(1 - e2 * pow(sin(B), 2));
//		H = R0 / cos(B) - N;
//		B = atan2(Z * (N + H), R0 * (N * (1 - e2) + H));
//	} while (fabs(deltaH - H) > 1.0e-3 && fabs(deltaB - B) > 1.0e-9);
//	res.B = B;
//	res.H = H;
//	return res;
//}
//
//BLH XYZToLatLon(double X, double Y, double Z)
//{
//	BLH blh(0, 0, 0);
//	blh.H = aa;
//	blh.B = asin(Z / blh.H);
//	blh.L = asin((Z / blh.H) / cos(blh.B));
//	return blh;
//}
//
//int LoadTexture(const char* filename, int width, int height, int depth, GLenum color_type, GLenum filter_type)
//{
//	GLubyte* raw_bitmap;
//	FILE* file;
//	errno_t err = fopen_s(&file, filename, "rb");
//	if (err != 0)
//	{
//		return 1;
//	}
//
//	raw_bitmap = (GLubyte*)malloc(width * height * depth * sizeof(GLubyte));
//	if (raw_bitmap == NULL)
//	{
//		fclose(file);
//		return 2;
//	}
//
//	fread(raw_bitmap, width * height * depth, 1, file);
//	fclose(file);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_type);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_type);
//
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, color_type, GL_UNSIGNED_BYTE, raw_bitmap);
//	//    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, color_type, GL_UNSIGNED_BYTE, raw_bitmap);
//	free(raw_bitmap);
//
//	return 0;
//}
//
//void LoadTextures(GLuint texture_id, int MaxNrofTextures)
//{
//	string name_earth = bmpfile;
//	glGenTextures(MaxNrofTextures, &texture_id);
//	glBindTexture(GL_TEXTURE_2D, 1);
//	if (LoadTexture(name_earth.c_str(), 2048, 1024, 3, GL_BGR_EXT, GL_LINEAR))
//	{
//		cout << "load texture error";
//		return;
//	}
//}
//
//void drawSatellite(Vec3 _p)
//{
//	glPushMatrix();
//
//	glTranslated(_p.x, _p.y, _p.z);
//	glDisable(GL_LIGHTING);
//	glDisable(GL_TEXTURE_2D);
//	glEnable(GL_DEPTH_TEST);
//
//	glScaled(1000.0f, 1000.0f, 1000.0f);
//	glBegin(GL_QUADS);
//	glColor3f(1.0f, 0.0f, 0.0f);
//	//Front Face
//	glNormal3f(0.0f, 0.0f, 1.0f);
//	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
//	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
//	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
//	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
//	//Back Face
//	glNormal3f(0.0f, 0.0f, -1.0f);
//	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
//	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
//	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
//	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
//	//Top Face
//	glNormal3f(0.0f, 1.0f, 0.0f);
//	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
//	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
//	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
//	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
//	//Bottom Face
//	glNormal3f(0.0f, -1.0f, 0.0f);
//	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
//	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
//	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
//	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
//	//Right Face
//	glNormal3f(1.0f, 0.0f, 0.0f);
//	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
//	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
//	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
//	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
//	//Left Face
//	glNormal3f(-1.0f, 0.0f, 0.0f);
//	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
//	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
//	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
//	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
//	glEnd();
//
//
//
//	glPopMatrix();
//	glEnable(GL_TEXTURE_2D);
//	/*glDisable(GL_DEPTH_TEST);
//	glEnable(GL_LIGHTING);*/
//	glColor3f(1.0f, 1.0f, 1.0f);
//}
//
//void drawGround(Vec3 _p)
//{
//	glPushMatrix();
//
//	glTranslated(_p.x, _p.y, _p.z);
//	glDisable(GL_LIGHTING);
//	glDisable(GL_TEXTURE_2D);
//	glEnable(GL_DEPTH_TEST);
//
//	glScaled(500.0f, 500.0f, 500.0f);
//	glBegin(GL_QUADS);
//	glColor3f(0.0f, 1.0f, 0.0f);
//	//Front Face
//	glNormal3f(0.0f, 0.0f, 1.0f);
//	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
//	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
//	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
//	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
//	//Back Face
//	glNormal3f(0.0f, 0.0f, -1.0f);
//	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
//	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
//	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
//	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
//	//Top Face
//	glNormal3f(0.0f, 1.0f, 0.0f);
//	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
//	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
//	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
//	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
//	//Bottom Face
//	glNormal3f(0.0f, -1.0f, 0.0f);
//	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
//	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
//	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
//	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
//	//Right Face
//	glNormal3f(1.0f, 0.0f, 0.0f);
//	glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, -1.0f, -1.0f);
//	glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, -1.0f);
//	glTexCoord2f(0.0f, 1.0f);   glVertex3f(1.0f, 1.0f, 1.0f);
//	glTexCoord2f(0.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 1.0f);
//	//Left Face
//	glNormal3f(-1.0f, 0.0f, 0.0f);
//	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, -1.0f);
//	glTexCoord2f(1.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 1.0f);
//	glTexCoord2f(1.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, 1.0f);
//	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, -1.0f);
//	glEnd();
//
//
//
//	glPopMatrix();
//	glEnable(GL_TEXTURE_2D);
//	/*glDisable(GL_DEPTH_TEST);
//	glEnable(GL_LIGHTING);*/
//	glColor3f(1.0f, 1.0f, 1.0f);
//}
//
//void drawEarth3D(float r)
//{
//	glBindTexture(GL_TEXTURE_2D, 1);
//	glPushMatrix();
//	glRotatef(86, 0, 0, 1);
//	GLUquadricObj* sphere = gluNewQuadric();
//	gluQuadricDrawStyle(sphere, GLU_FILL);
//	gluQuadricNormals(sphere, GLU_SMOOTH);
//	gluQuadricTexture(sphere, GL_TRUE);
//	gluSphere(sphere, r, 100, 100);
//
//	glPopMatrix();
//}
//
//void drawLine3D(float lineWidth)
//{
//	glLineWidth(lineWidth);
//	glPushMatrix();
//	for (int i = 0; i < linkS.size(); i++)
//	{
//		glBegin(GL_LINE_STRIP);
//		glVertex3f(linkS[i].x, linkS[i].y, linkS[i].z);
//		glColor3f(1, 1, 1);
//		glVertex3f(linkT[i].x, linkT[i].y, linkT[i].z);
//		glColor3f(1, 1, 1);
//		glEnd();
//	}
//	glPopMatrix();
//}
//
//void display()
//{
//	glEnable(GL_DEPTH_TEST);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除颜色缓存区和深度缓冲区
//
//	float earthR = aa;
//	glRotatef(cameraAngleX, 1, 0, 0);
//	glRotatef(cameraAngleY, 0, 1, 0);
//
//	/*for (int i = 0; i < updateXYZ.size(); i++)
//	{
//		updateXYZ[i];
//	}*/
//
//	glPushMatrix();
//
//	drawEarth3D(earthR);//draw earth
//	for(int i=0;i<satellites.size();i++)
//		drawSatellite(satellites[i]);// load satellite
//	for (int i = 0; i < grounds.size(); i++)
//	{
//		drawGround(grounds[i]);//load ground
//	}
//	drawLine3D();// draw track lines
//	glPopMatrix();
//
//	glutSwapBuffers();
//}
//
//void InitWindow(int argc, char* argv[], int width, int height, const char* title)
//{
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
//	glutInitWindowSize(width, height);//指定窗口大小
//	glutInitWindowPosition(100, 100);//指定窗口左上角位置
//	glutCreateWindow(title);//创建openGL渲染窗口
//
//	glClearColor(0, 0, 0, 1);//设置清除颜色
//	glClearDepth(1.0f);
//	glDepthFunc(GL_LEQUAL);
//	glShadeModel(GL_SMOOTH);
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//	LoadTextures(texture_id, 3);
//	glEnable(GL_TEXTURE_2D);
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_ALPHA_TEST);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//}
//
//void lookat(int type)
//{
//	switch (type) {
//	case 1:
//		glMatrixMode(GL_MODELVIEW);
//		glLoadIdentity();
//		gluLookAt(3 * grounds[0].x, 3 * grounds[0].y, 3 * grounds[0].z, grounds[0].x, grounds[0].y, grounds[0].z, 0, 1, 0);
//		break;
//	default:
//		break;
//	}
//}
//
//void reshape(int w, int h)
//{
//	aspect = (GLfloat)w / (GLfloat)h;
//	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluPerspective(45.0, aspect, 0.1, 3e5);
//	lookat(1);
//}
//
////鼠标事件
//void MouseEvent(int button, int state, int x, int y)
//{
//	mouseX = x;
//	mouseY = y;
//
//	if (button == GLUT_LEFT_BUTTON)
//	{
//		if (state == GLUT_DOWN)
//		{
//			mouseLeftDown = true;
//		}
//		else if (state == GLUT_UP)
//			mouseLeftDown = false;
//	}
//}
////鼠标移动事件
//void Motion( int x, int y)
//{
//	if (mouseLeftDown)
//	{
//		cameraAngleY += (x - mouseX) * 0.1;
//		cameraAngleX += (y - mouseY) * 0.1;
//		mouseX = x;
//		mouseY = y;
//	}
//
//	// 请求重新绘制场景
//	glutPostRedisplay();
//}
//
//
//
//
//void IdleEvent()
//{
//	glutPostRedisplay();
//}
//
//void beginWindow()
//{
//	glutDisplayFunc(display);//添加显示回调函数
//
//	glutIdleFunc(IdleEvent);
//	glutReshapeFunc(reshape);
//
//	glutMouseFunc(MouseEvent);
//	glutMotionFunc(Motion);
//	glutMainLoop();//启动程序
//}
//int test(int argc, char* argv[])
//{
//	InitWindow(argc, argv, 1920, 1080, "earth");
//	return 1;
//}