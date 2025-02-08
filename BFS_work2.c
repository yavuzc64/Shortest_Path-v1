#include <stdio.h>
#include <stdlib.h>
#define MAXSIZE 25
#define FILENAME "validMaps.bin"
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
	if(!newNode){return -1;	}	
	
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
	while(isEmpty(q)){
		Node* temp = dequeue(q);
		free(temp);
	}
}








int BFS(int matrix[ROWS][COLS], int biasMatrix[ROWS][COLS][4]){
	
}
void loadMatrixFromBinaryFile(int *matrix[ROWS][COLS], FILE *file){
	if(fread(matrix, sizeof(int), ROWS*COLS, file)){
		//	okudum
	}
}
void statisticCreate(){
	FILE *file = fopen(FILENAME, "rb");
	if(file == NULL) return;
	int matrix[ROWS][COLS];
	int biasMatrix[ROWS][COLS][4];// en kisa yol a gore doldurulacak matris
	//loadMatrixFromBinaryFile(&matrix , file);//loop da kullan sirayla matrisleri versin
	/*while(dosyanýn sonuna kadar){
		//dosyadan 1 matris cek
		//bfs ile oranlarini cikar
		//oranlari bfs deki adýmlara görebul ve matrise kaydet
	}*/
	
	//filelari kapat
	fclose(file);
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
    
    
    return 0;
	
	
	
	
	
	
	
	
	return 0;
}
