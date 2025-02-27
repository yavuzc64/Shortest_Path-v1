#include <stdio.h>
#include <stdlib.h>
#define MAXSIZE 25
#define MAX_STEP (MAXSIZE)
#define FILENAME "NewValidMaps.bin"
#define COLS 5
#define ROWS 5

typedef struct Node{
	int X;
	int Y;
	struct Node* next;
}Node;
typedef struct{
	Node* front;
	Node* rear;
}Queue;

void initQueue(Queue *q){
	q->front = NULL;
	q->rear = NULL;
}
int isEmpty(Queue *q){return(q->front == NULL);}

void enqueue(Queue *q, int x, int y){
	Node* newNode = (Node*)malloc(sizeof(Node));
	if(!newNode){return ;	}	
	
	newNode->X = x;
	newNode->Y = y;
	newNode->next = NULL;
	
	if(isEmpty(q)){
		q->front = newNode;
		q->rear = newNode;
	}
	else{
		q->rear->next = newNode;
		q->rear = newNode;
	}
}
Node* dequeue(Queue *q){
	if(isEmpty(q)){
		return NULL;
	}
	Node* temp = q->front;
	q->front = q->front->next;
	if(q->front == NULL){
		q->rear = NULL;
	}
	return temp;
}
void freeQueue(Queue *q){
	while(!isEmpty(q)){
		Node* temp = dequeue(q);
		free(temp);
	}
}

int isValid(int x, int y, int map[ROWS][COLS], int visited[ROWS][COLS]){
	return ( x >= 0 && x<ROWS && y>= 0 && y< COLS && !(visited[x][y]) && map[x][y] == 0);
}

int BFS(int matrix[ROWS][COLS], int biasMatrix[ROWS][COLS][4], int startX, int startY, int endX, int endY){
	
	Queue q;
	initQueue(&q);
	
	int visited[ROWS][COLS] = {0};
	int backward[ROWS][COLS][2];
	enqueue(&q, startX, startY);
	visited[startX][startY] = 1;
	//baslangic noktasi -1 ile isaretlenir 
	backward[startX][startY][0] = -1;
	backward[startX][startY][1] = -1;
	
	//			Yukari, Asagi, 	Sol, 	Sag
	int dx[] ={	 0,		 0,		-1		,1};
	int dy[] ={	-1,		 1,		 0		,0};
	
	int i,newX,newY, isFinded=0, count = 0;
	while(!isEmpty(&q) && !isFinded && count < MAX_STEP){
		Node* node = dequeue(&q);
		if(node == NULL) continue;	/* DEBUG */
		int x = node->X;
		int y = node->Y;
		free(node);
		
		if(x == endX && y == endY){ // bu satirdan once kayit yapilmali
			freeQueue(&q);
			isFinded = 1;
		}
		else{
			for(i = 0; i<4 ; i++){
				newX = x + dx[i];
				newY = y + dy[i];
			
				if(isValid(newX, newY, matrix, visited)){
					enqueue(&q, newX, newY);
					backward[newX][newY][0] = x;
					backward[newX][newY][1] = y;
					visited[newX][newY] = 1;
				}
			}
		}
		count++;
	}
	if(!isFinded){
		printf("yol yok !!!");
		freeQueue(&q);
		return -1;
	}
	
	
	int cx = endX, cy = endY;count = 0;
	while(backward[cx][cy][0] !=-1 && count < MAX_STEP){
		int px = backward[cx][cy][0];
		int py = backward[cx][cy][1];
		
		if(cx == px){ 	//yatayda hareket
			if(cy < py){// Sol
				biasMatrix[px][py][2]++;
				//printf("sol\n");
			}else{// Sag
				biasMatrix[px][py][3]++;
				//printf("sag\n");
			}
		}else{			//dikey hareket
			if(cx < px){// Yukari
				biasMatrix[px][py][0]++;
				//printf("yukari\n");
			}else{		// Asagi
				biasMatrix[px][py][1]++;
				//printf("asagi\n");
			}
		}
		cx = px;
		cy = py;
		count++;
	}
	return 1;
}

void loadMatrixFromBinaryFile(int *matrix[ROWS][COLS], FILE *file){
	if(fread(matrix, sizeof(int), ROWS*COLS, file)){
		//	okudum
	}
}
void statisticCreate(){
	FILE *file = fopen(FILENAME, "rb");
	if(file == NULL) {
		printf("dosya acilamadi");
		return;
	}
	int matrix[ROWS][COLS];// harita
	int biasMatrix[ROWS][COLS][4] = {0};// en kisa yol a gore doldurulacak matris 	
	
	int maksimum = 2355000; //yaklasik 2,355M
	int counter = 0, percentage = 0;
	
	int trash[3];
	fread(&trash, sizeof(int), 3, file);// dosyanin basina rows, cols ve count sayilarinin ekledim atlamasi icin
	
	while(fread(&matrix, sizeof(int), ROWS*COLS, file) == ROWS*COLS && counter<maksimum){ //fread() fonksiyonu (dogru calismis ve sonda degilse) okunan veri say�s�n� geri verir.
		BFS(matrix, biasMatrix, 0, 2, 4, 3);
		counter++;
		if(counter%(maksimum/100) == 0){
			percentage++;
			if(percentage%5 == 0)
				printf("yuzde %d\n",percentage);
		}
	}
	fclose(file);
	
	FILE *textFile = fopen("TempDirectionData.txt", "w");
	if(textFile == NULL) return;
	
	fprintf(textFile, "Yukari\tAsagi\tSol\tSag\n");
	int i, j;
	for(i= 0; i<ROWS; i++){
		for(j= 0; j<COLS; j++){
			fprintf(textFile, "[%d,%d,%d,%d]\t\t\t\t", biasMatrix[i][j][0], biasMatrix[i][j][1], biasMatrix[i][j][2], biasMatrix[i][j][3]);
		}
		fprintf(textFile,"\n");
	}	
	fclose(textFile);
}


int main(){
	/* TEST
	Queue q;
    initQueue(&q);
    enqueue(&q, 2, 3);
    enqueue(&q, 4, 5);
    Node* node = dequeue(&q);
    if (node) {
        printf("Output: (%d, %d)\n", node->X, node->Y);
        free(node);
    }
    freeQueue(&q);*/
    
//	int matrix[ROWS][COLS] = {
//        {0, 0, 0, 1, 0},
//        {1, 1, 0, 1, 0},
//        {0, 0, 0, 0, 0},
//        {0, 1, 1, 1, 0},
//        {0, 0, 0, 0, 0}
//    };
//
//    int biasMatrix[ROWS][COLS][4] = {0};
//
//    BFS(matrix, biasMatrix, 0, 0, 4, 4);
	/*int i,j ;
	printf("Bias Matrix:\n");
    for (i = 0; i < ROWS; i++) {
        for ( j = 0; j < COLS; j++) {
            printf("[%d%d%d%d] ", biasMatrix[i][j][0], biasMatrix[i][j][1], biasMatrix[i][j][2], biasMatrix[i][j][3]);
        }
        printf("\n");
    }*/
	statisticCreate();
	
	
	return 0;
}
