#include <stdio.h>
#include <stdint.h>
#define MAXSIZE 25
#define ROWS 5
#define COLS 5
typedef struct{
	int X;
	int Y;
}Point;

typedef struct{
	Point data[MAXSIZE];
	int top;
}STACK;
void initialize(STACK *s){
	s->top = 0;
}
int isEmpty(STACK *s){
	if(s->top == 0) return 1;
	else return 0;
}
int isFull(STACK *s){
	if(s->top == MAXSIZE) return 1;
	else return 0;
}
int push(STACK *s,int x, int y){
	if(isFull(s)) return 0;
	else{
		s->data[s->top].X = x;
		s->data[s->top].Y = y;
		s->top++;
	}
}
Point pop(STACK *s){
	if(isEmpty(s)) return (Point){-1, -1};// gecersiz bir deger oldugu icin
	else{
		return s->data[--s->top];
	}
}
Point peek(STACK *s){ 
	if(isEmpty(s)) return (Point){-1, -1};
	int k = s->top;
	return s->data[--k];
}


void printStack(STACK *s) {
    if (isEmpty(s)) {
        printf("Stack bos!\n");
        return;
    }
    int i ;
    printf("Stack ici (top to low):\n");
    for (i = s->top-1; i >= 0; i--) {
        printf("(%d, %d)\n", s->data[i].X, s->data[i].Y);
    }
}
int isValid(int x, int y, int map[ROWS][COLS], int visited[ROWS][COLS]){
	return (x<ROWS && x >= 0 && y< COLS && y>= 0 && !(visited[x][y]) && map[x][y] == 0);
}
int DFS(int startX, int startY, int endX, int endY, int map[ROWS][COLS]){	
	if(map[startX][startY] == 1 || map[endX][endY] == 1){
		return -1;
	}
	
	STACK s;
	initialize(&s);
	
	int visited[ROWS][COLS] = {0};
	push(&s, startX, startY);
	
	//			Yukari, Asagi, 	Sol, 	Sag
	int dx[] ={	 0,		 0,		-1		,1};
	int dy[] ={	-1,		 1,		 0		,0};
	
	int i,newX,newY;
	
	while(!isEmpty(&s)){
		Point current = pop(&s);
		int x = current.X;
		int y = current.Y;
		
		if(visited[x][y]){
			//zaten gecilmis
		}
		else if(x == endX && y == endY){
			return 1;
		}
		else{
			for(i=0;i<4;i++){
				newX = x + dx[i];
				newY = y + dy[i];
				if(isValid(newX,newY,map,visited)){
					push(&s, newX, newY);
				}
			}
			visited[x][y] = 1;
		}
	}
	return -1;
}


void generateMatrix(uint32_t num, int matrix[ROWS][COLS]) {
	int i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            int bitPos = i * COLS + j;
            matrix[i][j] = (num >> bitPos) & 1;
        }
    }
}

int main(){
	int counter = 0,i,j;
	uint32_t totalCases = 1 << (ROWS * COLS); // 2^25 = 33,554,432
    int matrix[ROWS][COLS];
	uint32_t num ;
    for (num = 100000; num < totalCases; num++) {
        generateMatrix(num, matrix);
        if(DFS(0,2,4,3,matrix)==1){
        	counter++;
		}
//		if(num % 1000000 == 0){
//			for (i = 0; i < ROWS; i++) {
//		        for ( j = 0; j < COLS; j++) {
//		            printf("%d ", matrix[i][j]);
//			    }
//			    printf("\n");
//			}
//			printf("-----------\n");
//		}
    }
    printf("\ngecerli harita sayisi : %d", counter);
	
	
	
	
	
	
	
	/*
	// *** Stack TEST ***
	push(&s, 1, 2);
    push(&s, 3, 4);
    push(&s, 5, 6);

    printStack(&s);

    Point top = peek(&s);
    printf("top eleman: (%d, %d)\n", top.X, top.Y);

    Point p = pop(&s);
    printf("pop eleman: (%d, %d)\n", p.X, p.Y);

    printStack(&s);*/
    /*
    //		DFS Algorithm TEST
	int map[ROWS][COLS]={	{0,0,1,0,1},
							{0,1,0,0,0},
							{0,1,0,1,0},
							{0,0,0,1,0},
							{0,0,1,0,0}};
	//map[2][0] = 1; // engel ile kapat
	if(DFS(0,1,4,3,map)==1){
		printf("\nhedefe ulasildi");
	}else{
		printf("\nYol Yok!");
	}*/
	
	
	
	return 0;
}
