//����㷨��ԭ�еı������������ɷ����Ļ����Ͻ��иĽ���
//ʹ���ܹ�������������ֱ�ߣ�û���ظ��ĵ�

#ifndef __MYBEZIER_H
#define __MYBEZIER_H
#include <gl/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct BezierPoint
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat rate;
	BezierPoint* prev;
	BezierPoint* next;
} BezierPoint;

BezierPoint* generateBezierPoint();
BezierPoint* generateCenterPoint(BezierPoint* h, BezierPoint* t);
void DieDai(BezierPoint* h, BezierPoint* t);
BezierPoint* getPoint(GLfloat rate);

BezierPoint *head, *tail;
float (*srcPoint)[3];             //��������ֻ�ܶ�ȡ srcPiont ���ǲ����޸� srcPoint ��ֵ
float temPoint[255][3];           //��������Ǵ洢��ʱ�����ĵ�,�������ֵ��255 ����

int srcLength = 0;
int pointLength = 0;

bool isHeadEqualTail = true;       //����Ǳ�ʾ���ǵ�head��tail�Ǳ��������ߵ������յ㣬 �������������غ�
bool isHEqualT       = true;       //����������ʾ���Ǵ������ĵ� h �� t ��λ�������غϵģ� ���� rate�ϲ������غϵ�

/************************************************************************/
/* �������������ߵ�ĺ���                                                */
/************************************************************************/

void initBezier(float src[][3], int len){
	srcPoint = src;
	srcLength = len;

	head = generateBezierPoint();
	head->x = src[0][0];
	head->y = src[0][1];
	head->z = src[0][2];
	head->rate = 0.0f;

	tail = generateBezierPoint();
	tail->x = src[srcLength - 1][0];
	tail->y = src[srcLength - 1][1];
	tail->z = src[srcLength - 1][2];
	tail->rate = 1.0f;

	head->prev = NULL;
	head->next = tail;
	tail->prev = head;
	tail->next = NULL;

	//�ж� head �� tail �Ƿ��غ� ������Ϊȫ�ֲ���
	isHeadEqualTail = ((head->x == tail->x) && (head->y == tail->y) && (head->z == tail->z));  
	pointLength = 2;

}

BezierPoint* GeneratePointCount(float src[][3], int len){
	initBezier(src, len);
	DieDai(head, tail);
	return head;
}

//TODO: ������ʱû�п�����ʼ�����ֹ���غϵ����,���������������bug��
void DieDai(BezierPoint* h, BezierPoint* t){

	//�ж� h �� t �Ƿ��غ�
	isHEqualT = ((h->x == t->x) && ( h->y == t->y) && (h->z == t->z));
	//�ж�h �� t �Ƿ����ٽ��� (����ټ���h��t����ͷ���β�������)
	bool isNeighborPoint = (abs(h->x - t->x) < 2) && (abs(h->y - t->y) < 2) && (abs(h->z - t->z) < 2);
	 //��ʾ������֮�䲻��Ҫ��ĵ������� �ͷ����ˣ�����һ�������ܵĶ��ų������ʹ������Ӽ�
	if (isHEqualT || isNeighborPoint){   
		return;
	}

	//������һ����,��Ȼǰ���Ѿ��ж������Բ���һ��ֵ����ô��һ���ܹ�����һ��ֵ
	BezierPoint* cp = generateCenterPoint(h, t);
	
	//�Ե���в���
	h->next = cp;
	cp->prev = h;
	t->prev = cp;
	cp->next = t;
	pointLength ++;

	//��������
	DieDai(h, cp);
	DieDai(cp, t);
}

//���������÷ֵ�����㣬 ���жϵ�β��ϸ�
BezierPoint* generateCenterPoint(BezierPoint* h, BezierPoint* t){
	GLfloat fenzi = 1.0f;   //ͨ�� fenzi �� ��ĸ֮��Ĺ�ϵ����ò�ͬ���ֵ�֮��ĵ�
	GLfloat fenmu = 2.0f;
	BezierPoint* point;

	while (true){
		GLfloat centerRate = (h->rate * fenzi + t->rate * (fenmu - fenzi))/ fenmu;
		point = getPoint(centerRate);
		
		bool isEqualHead = ((h->x == point->x) && (h->y == point->y) && (h->z == point->z));
		bool isEqualTail = ((t->x == point->x) && (t->y == point->y) && (t->z == point->z));      //�жϵó��ĵ��ǲ����غϵ�

		if(isEqualHead && isEqualTail){
			//����һ�������Ҳ�֪���᲻�ᷢ������д������
			printf("error occurred\n");
			assert(false);     //ʱ��ʱ������Щ�ɣ�
			system("pause");
			exit(-1);
		}
		if (!isEqualHead && !isEqualTail){
			return point;
		}
		
		//����һ�ֱȽ�ð�յ���������ʹ���߱�úֲܴڣ� 
		/*
		point->x = (int)((h->x + t->x)/2.0);
		point->y = (int)((h->y + t->y)/2.0);
		point->z = (int)((h->z + t->z)/2.0);
		return point;
		*/

		//����ע�͵��ķ������һЩ�����Ƿ�ʱ
		
		if(fenzi < fenmu - 1.0f){
			fenzi = fenzi + 1;
		}else{
			fenmu = fenmu + 1;
			fenzi = 1.0;
		}

		//�ֲ�һ��bug�� ���������Ӹ����bug��, �����֤�������������һ�����õ�����, ������Խ��й����ڣ����Ǹо�û��̫���Ҫ
		if (fenmu > 4){
			point->x = (int)((h->x + t->x)/2.0);
			point->y = (int)((h->y + t->y)/2.0);
			point->z = (int)((h->z + t->z)/2.0);
			return point;
		}
		
		
	}

	
}

//����Ǹ�����������ĵ���㵥����Ĵ�С
BezierPoint* getPoint(GLfloat rate){

	//����srcPoint �� temPoint
	for(int i=0;i<srcLength;i++){
		for(int j=0;j<3;j++){
			temPoint[i][j] = srcPoint[i][j];
		}
	}

	for(int i=srcLength-2;i>=0;i--){   //��ʾ��Ҫ���еĴ���
		for (int j=0;j<=i;j++)          //ÿ�����е���ʼ��ͽ�����
		{
			temPoint[j][0] = (int)(temPoint[j][0]* (1-rate) + temPoint[j+1][0] * rate + 0.5);
			temPoint[j][1] = (int)(temPoint[j][1]* (1-rate) + temPoint[j+1][1] * rate + 0.5);
			temPoint[j][2] = (int)(temPoint[j][2]* (1-rate) + temPoint[j+1][2] * rate + 0.5);
		}
	}
	BezierPoint* point = generateBezierPoint();
	point->x = temPoint[0][0];
	point->y = temPoint[0][1];
	point->z = temPoint[0][2];
	point->rate = rate;
	return point;
}

BezierPoint* generateBezierPoint(){
	BezierPoint* point = (BezierPoint *)malloc(sizeof(BezierPoint));
	if(point == NULL){
		printf("�ڴ��޷�����");
		system("pause");
		exit(-2);
	}
	return point;
}

#endif

