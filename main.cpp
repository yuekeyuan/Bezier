#include <gl/glut.h>
#include <stdio.h>
#include "MyBezier.h"

#define DRAW_MY_BEZIER true

static GLint  WINDOW_SIZE_X = 800;
static GLint  WINDOW_SIZE_Y = 600;

GLint nNumPoints = 0;
GLint maxNumPoints =0;
GLboolean isReadyDraw = false;

//����255����
GLfloat ctrlPoints[255][3] = {0};

// ���ƿ��Ƶ�
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

	//��������
	if(isReadyDraw){
		if(DRAW_MY_BEZIER){                 //ʹ���Լ��ı���������
		
		/*  
			//��������
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

			//��Ϊ��ʵ�ֵı�����������һ�������⣬���Ե������������4����ʱ��ֱ�ӵ����ڲ�����������ɢ��

			if (nNumPoints <4)     //������С��4��ʱ��
			{
				BezierPoint* head = GeneratePointCount(ctrlPoints, nNumPoints);
				glBegin(GL_POINTS);
				glPointSize(0.5);
				for(i = 0; i < pointLength; i++)   //�ܹ����������յ㣬 һ�� n + 1 ���㣬 �ų��������
				{
					glVertex3f(head->x, head->y, head->z);
					head = head->next;
				}
				glEnd();
			}else       //�������ڵ��� 4
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

		}else {                             //���� Opengl �ṩ�� ����������
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
	glutCreateWindow("����������");
	glutReshapeFunc(ChangeSize);
	glutMouseFunc(mouseFun);
	glutDisplayFunc(RenderScene);
	SetupRC();
	while (maxNumPoints < 2)
	{
		printf("������Ҫ�������ߵĿ��Ƶ�����\n");
		scanf("%d", &maxNumPoints);
	}
	glutMainLoop();
	return 0;
}
