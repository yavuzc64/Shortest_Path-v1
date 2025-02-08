#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define THREAD_COUNT 6
#define BATCH_SIZE 1000 

#define MAXSIZE 25
#define MAX_STEP (MAXSIZE)
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

typedef struct {
	int matrices[BATCH_SIZE][ROWS][COLS];
	int count;
}ThreadData;

int biasMatrix_main[ROWS][COLS][4] = {0};
pthread_mutex_t mutex;

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

void saveBiasData(){
	FILE *textFile = fopen("Fast_BiasData.txt", "w");
	if(textFile == NULL) return;
	
	fprintf(textFile, "Yukari\tAsagi\tSol\tSag\n");
	int i, j;
	for(i= 0; i<ROWS; i++){
		for(j= 0; j<COLS; j++){
			fprintf(textFile, "[%d,%d,%d,%d]\t\t\t\t", biasMatrix_main[i][j][0], biasMatrix_main[i][j][1], biasMatrix_main[i][j][2], biasMatrix_main[i][j][3]);
		}
		fprintf(textFile,"\n");
	}	
	fclose(textFile);
}

void* processMatrices(void* arg) {
	ThreadData* data = (ThreadData*)arg;
	int biasMatrix_thread[ROWS][COLS][4] = {0};
	int i,j,k;
	for(i = 0; i<data->count; i++){// son batch de array tam dolmayabilir ! bu nedenle count kullan !
		BFS(data->matrices[i],biasMatrix_thread, 0,2,4,3);
	}
	/*
		syncronized olmasi icin mutex kullanilir
	*/
	pthread_mutex_lock(&mutex);		//	!!!
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            for (k = 0; k < 4; k++) {
                biasMatrix_main[i][j][k] += biasMatrix_thread[i][j][k];
            }
        }
    }
    pthread_mutex_unlock(&mutex);	//	!!!
	
	
	
	pthread_exit(NULL);
}

int main(){// 2,229 second (unmultithread)	//yaklasik 2,355M matris // 6 thread 1,05 second
	FILE *file = fopen(FILENAME, "rb");
	if(file == NULL) { return;	}
	
	pthread_t threads[THREAD_COUNT];
    ThreadData threadData[THREAD_COUNT];

    int threadIndex = 0, i;  // Hangi threadData bloðunu kullanacaðýmýz
    int totalMatrices = 0;
    int matrix[ROWS][COLS];

    pthread_mutex_init(&mutex, NULL);

    while (fread(&matrix, sizeof(int), ROWS * COLS, file) == ROWS * COLS) {
        // Okunan matrisi ilgili ThreadData bloðuna ekle
        int batchIndex = totalMatrices % BATCH_SIZE;
        threadData[threadIndex].matrices[batchIndex][0][0] = matrix[0][0]; // Pointer atanmalý
        memcpy(threadData[threadIndex].matrices[batchIndex], matrix, sizeof(matrix));

        totalMatrices++;

        // Eðer 1000 matris dolduysa, thread baþlat
        if ((totalMatrices % BATCH_SIZE) == 0) {
            threadData[threadIndex].count = BATCH_SIZE;
            pthread_create(&threads[threadIndex], NULL, processMatrices, &threadData[threadIndex]);
            threadIndex++;

            // Eðer 4 thread baþlatýlmýþsa, önce bunlarýn bitmesini bekle
            if (threadIndex == THREAD_COUNT) {
                for (i = 0; i < THREAD_COUNT; i++) {
                    pthread_join(threads[i], NULL);
                }
                threadIndex = 0;
            }
        }
    }
    // Kalan matrisi iþleyen threadleri baþlat
    if (totalMatrices % BATCH_SIZE != 0) {
        threadData[threadIndex].count = totalMatrices % BATCH_SIZE;
        pthread_create(&threads[threadIndex], NULL, processMatrices, &threadData[threadIndex]);
        threadIndex++;
    }
    // Kalan threadleri bitir
    for (i = 0; i < threadIndex; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(file);
    pthread_mutex_destroy(&mutex);
		
	saveBiasData();
	return 0;
}
