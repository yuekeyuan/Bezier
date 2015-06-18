//这个算法在原有的贝塞尔曲线生成方法的基础上进行改进，
//使它能够生成条连续的直线，没有重复的点

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
float (*srcPoint)[3];             //这里我们只能读取 srcPiont 但是不能修改 srcPoint 的值
float temPoint[255][3];           //这个数组是存储临时产生的点,但是其最长值是255 个点

int srcLength = 0;
int pointLength = 0;

bool isHeadEqualTail = true;       //这个是表示我们的head和tail是贝塞尔曲线的起点和终点， 并且他们两个重合
bool isHEqualT       = true;       //这个则仅仅表示我们传进来的点 h 和 t 在位置上市重合的， 但在 rate上并不是重合的

/************************************************************************/
/* 产生贝塞尔曲线点的函数                                                */
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

	//判断 head 和 tail 是否重合 并保存为全局参数
	isHeadEqualTail = ((head->x == tail->x) && (head->y == tail->y) && (head->z == tail->z));  
	pointLength = 2;

}

BezierPoint* GeneratePointCount(float src[][3], int len){
	initBezier(src, len);
	DieDai(head, tail);
	return head;
}

//TODO: 现在暂时没有考虑起始点和终止点重合的情况,在这种情况下是有bug的
void DieDai(BezierPoint* h, BezierPoint* t){

	//判断 h 和 t 是否重合
	isHEqualT = ((h->x == t->x) && ( h->y == t->y) && (h->z == t->z));
	//判断h 和 t 是否是临近点 (最好再加上h和t不是头点和尾点且相等)
	bool isNeighborPoint = (abs(h->x - t->x) < 2) && (abs(h->y - t->y) < 2) && (abs(h->z - t->z) < 2);
	 //表示两个点之间不需要别的点插进来， 就返回了，在这一步尽可能的多排除情况，使计算更加简单
	if (isHEqualT || isNeighborPoint){   
		return;
	}

	//生成下一个点,既然前面已经判断它可以产生一个值，那么它一定能够产生一个值
	BezierPoint* cp = generateCenterPoint(h, t);
	
	//对点进行插入
	h->next = cp;
	cp->prev = h;
	t->prev = cp;
	cp->next = t;
	pointLength ++;

	//继续迭代
	DieDai(h, cp);
	DieDai(cp, t);
}

//这里我们用分点来求点， 并判断点何不合格
BezierPoint* generateCenterPoint(BezierPoint* h, BezierPoint* t){
	GLfloat fenzi = 1.0f;   //通过 fenzi 和 分母之间的关系来获得不同定分点之间的点
	GLfloat fenmu = 2.0f;
	BezierPoint* point;

	while (true){
		GLfloat centerRate = (h->rate * fenzi + t->rate * (fenmu - fenzi))/ fenmu;
		point = getPoint(centerRate);
		
		bool isEqualHead = ((h->x == point->x) && (h->y == point->y) && (h->z == point->z));
		bool isEqualTail = ((t->x == point->x) && (t->y == point->y) && (t->z == point->z));      //判断得出的点是不是重合点

		if(isEqualHead && isEqualTail){
			//这是一个错误，我不知道会不会发生，先写在这里
			printf("error occurred\n");
			assert(false);     //时光时光你慢些吧！
			system("pause");
			exit(-1);
		}
		if (!isEqualHead && !isEqualTail){
			return point;
		}
		
		//这是一种比较冒险的做法，会使曲线变得很粗糙， 
		/*
		point->x = (int)((h->x + t->x)/2.0);
		point->y = (int)((h->y + t->y)/2.0);
		point->z = (int)((h->z + t->z)/2.0);
		return point;
		*/

		//下面注释掉的方法会好一些，但是费时
		
		if(fenzi < fenmu - 1.0f){
			fenzi = fenzi + 1;
		}else{
			fenmu = fenmu + 1;
			fenzi = 1.0;
		}

		//弥补一个bug， 往往会增加更多的bug！, 这个保证它必须产生出来一个可用的数据, 这里可以进行过调节，但是感觉没有太大必要
		if (fenmu > 4){
			point->x = (int)((h->x + t->x)/2.0);
			point->y = (int)((h->y + t->y)/2.0);
			point->z = (int)((h->z + t->z)/2.0);
			return point;
		}
		
		
	}

	
}

//这个是根据主程序传入的点计算单个点的大小
BezierPoint* getPoint(GLfloat rate){

	//拷贝srcPoint 到 temPoint
	for(int i=0;i<srcLength;i++){
		for(int j=0;j<3;j++){
			temPoint[i][j] = srcPoint[i][j];
		}
	}

	for(int i=srcLength-2;i>=0;i--){   //表示所要运行的次数
		for (int j=0;j<=i;j++)          //每次运行的起始点和结束点
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
		printf("内存无法分配");
		system("pause");
		exit(-2);
	}
	return point;
}

#endif

