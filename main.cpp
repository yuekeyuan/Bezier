#include <gl/glut.h>
#include <stdio.h>
#include "MyBezier.h"

#define DRAW_MY_BEZIER true

static GLint  WINDOW_SIZE_X = 800;
static GLint  WINDOW_SIZE_Y = 600;

GLint nNumPoints = 0;
GLint maxNumPoints =0;
GLboolean isReadyDraw = false;

//最多点255个点
GLfloat ctrlPoints[255][3] = {0};

// 绘制控制点
void DrawPoints(void)
{
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for(int i = 0; i < nNumPoints; i++){
		printf("point %d (%f, %f)\n", i, ctrlPoints[i][0], ctrlPoints[i][1]);
		glVertex2fv(ctrlPoints[i]);
	}
	glEnd();
}

void RenderScene(void)
{
	int i;
	glClear(GL_COLOR_BUFFER_BIT);

	//绘制曲线
	if(isReadyDraw){
		if(DRAW_MY_BEZIER){                 //使用自己的贝塞尔曲线
		
		/*  
			//测试用例
			ctrlPoints[0][0] = 0;
			ctrlPoints[0][1] = 0;
			ctrlPoints[0][2] = 0;

			ctrlPoints[1][0] = 2;
			ctrlPoints[1][1] = 0;
			ctrlPoints[1][2] = 0;

			ctrlPoints[2][0] = 3;
			ctrlPoints[2][1] = 2;
			ctrlPoints[2][2] = 0;

			ctrlPoints[3][0] = 3;
			ctrlPoints[3][1] = 3;
			ctrlPoints[0][1] = 0;

			nNumPoints = 4;
		*/

			//因为我实现的贝塞尔曲线有一定的问题，所以当输入点数大于4个的时候，直接调用内部函数生成离散点

			if (nNumPoints <4)     //当点数小于4的时候
			{
				BezierPoint* head = GeneratePointCount(ctrlPoints, nNumPoints);
				glBegin(GL_POINTS);
				glPointSize(0.5);
				for(i = 0; i < pointLength; i++)   //总共包括起点和终点， 一共 n + 1 个点， 排除特殊情况
				{
					glVertex3f(head->x, head->y, head->z);
					head = head->next;
				}
				glEnd();
			}else       //点数大于等于 4
			{
				initBezier(ctrlPoints, nNumPoints);
				glLineWidth(2.0f);
				glBegin(GL_LINE_STRIP);
				for (float i=0.0f;i<=1;i=i+0.01)
				{
					BezierPoint* point = getPoint(i);
					glVertex3f(point->x, point->y, point->z);
					free(point);
				}
				glEnd();
			}

		}else {                             //绘制 Opengl 提供的 贝塞尔曲线
			glMap1f(GL_MAP1_VERTEX_3,	// Type of data generated
				0.0f,						// Lower u range
				100.0f,						// Upper u range
				3,							// Distance between points in the data
				nNumPoints,					// number of control points
				&ctrlPoints[0][0]);			// array of control points

			glEnable(GL_MAP1_VERTEX_3);
			glBegin(GL_LINE_STRIP);
			for(i = 0; i <= 100; i++)
			{
				glEvalCoord1f((GLfloat) i); 
			}
			glEnd();
		}
	}
	DrawPoints();
	glutSwapBuffers();
}

void SetupRC()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f );
	glColor3f(0.0f, 0.0f, 1.0f);	
}

void ChangeSize(int w, int h)
{
	WINDOW_SIZE_X = w;
	WINDOW_SIZE_Y = h;
	if(h == 0)
		h = 1;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(-w/2, w/2, -h/2, h/2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void mouseFun(int button, int state, int x, int y){
	
	if(nNumPoints < maxNumPoints){
		if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
			ctrlPoints[nNumPoints][0] = x - WINDOW_SIZE_X /2;
			ctrlPoints[nNumPoints][1] = WINDOW_SIZE_Y /2 - y;
			ctrlPoints[nNumPoints][2] = 0.0f;
			nNumPoints = nNumPoints + 1;
		}
	}else
	{
		isReadyDraw = true;
	}

	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glutCreateWindow("贝塞尔曲线");
	glutReshapeFunc(ChangeSize);
	glutMouseFunc(mouseFun);
	glutDisplayFunc(RenderScene);
	SetupRC();
	while (maxNumPoints < 2)
	{
		printf("请输入要绘制曲线的控制点数：\n");
		scanf("%d", &maxNumPoints);
	}
	glutMainLoop();
	return 0;
}
