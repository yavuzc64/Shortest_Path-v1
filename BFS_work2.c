#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INPUTFILENAME "Maps.bin"
#define OUTPUTFILENAME "PercentData.bin"
#define INDEX(x,y,z,rows,cols) (cols * x + y + (rows * cols * z))

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

int isValid(int x, int y, int *map, int *visited, int rows, int cols){
	if (x >= rows || x < 0 || y >= cols || y < 0) return 0;
	return (!(visited[(x*cols)+y]) && map[(x*cols)+y] == 0);
}

int BFS(int startX, int startY, int endX, int endY, int *matrix, int *biasMatrix, int *visited, int *backward, int rows, int cols){
	int maxStep = rows * cols;
	int i , j;
	
	Queue q;
	initQueue(&q);
	memset(visited, 0, rows * cols * sizeof(int));
	memset(backward, 0, 2 * rows * cols * sizeof(int));
	enqueue(&q, startX, startY);
	visited[cols*startX + startY] = 1;

	//baslangic noktasi -1 ile isaretlenir 
	backward[INDEX(startX,startY,0,rows,cols)]= -1; //backward[0][startX][startY] nin dengidir
	backward[INDEX(startX,startY,1,rows,cols)]= -1;
	
	//			Yukari, Asagi, 	Sol, 	Sag
	int dx[] ={	 0,		 0,		-1		,1};
	int dy[] ={	-1,		 1,		 0		,0};
	
	int newX,newY, isFinded=0, count = 0;
	while(!isEmpty(&q) && !isFinded && count < maxStep){
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
			
				if(isValid(newX, newY, matrix, visited, rows, cols)){
					enqueue(&q, newX, newY);
					backward[INDEX(newX,newY,0,rows,cols)] = x;
					backward[INDEX(newX,newY,1,rows,cols)] = y;
					visited[cols*newX+ newY] = 1;
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
	while(backward[INDEX(cx,cy,0,rows,cols)] !=-1 && count < maxStep){
		int px = backward[INDEX(cx,cy,0,rows,cols)];
		int py = backward[INDEX(cx,cy,1,rows,cols)];
		if(cx == px){ 			//yatayda hareket
			if(cy < py){// Sol
				biasMatrix[INDEX(px,py,2,rows,cols)]++;	// biasMatrix[px][py][2] nin muadili
				
			}else{		// Sag
				biasMatrix[INDEX(px,py,3,rows,cols)]++;
			}
		}else{					//dikey hareket
			if(cx < px){// Yukari
				biasMatrix[INDEX(px,py,0,rows,cols)]++;
			}else{		// Asagi
				biasMatrix[INDEX(px,py,1,rows,cols)]++;
			}
		}
		cx = px;
		cy = py;
		count++;
	}
	return 1;
}

void statisticCreate(int rows, int cols, int *biasMatrix){ // bias matrisi kaydetme
	
	long unsigned int total;
	int i,j,k;
	for(i= 0; i<rows; i++){
		for(j= 0; j<cols; j++){
			total = biasMatrix[INDEX(i,j,0,rows,cols)] +
			        biasMatrix[INDEX(i,j,1,rows,cols)] +
			        biasMatrix[INDEX(i,j,2,rows,cols)] +
			        biasMatrix[INDEX(i,j,3,rows,cols)];
			if (total == 0) {
			    biasMatrix[INDEX(i,j,0,rows,cols)] = 0;
			    biasMatrix[INDEX(i,j,1,rows,cols)] = 0;
			    biasMatrix[INDEX(i,j,2,rows,cols)] = 0;
			    biasMatrix[INDEX(i,j,3,rows,cols)] = 0;
			} else {
			    for(k = 0; k < 4; k++){
			        biasMatrix[INDEX(i,j,k,rows,cols)] *= 100;
			        biasMatrix[INDEX(i,j,k,rows,cols)] /= total;
			    }
			}
		}
	}
	//	.BIN		istatistigin yuzdesel degerlerini binary olarak yazdirmak icin
	FILE *binFile = fopen(OUTPUTFILENAME, "rb+");
    if (!binFile) {
        binFile = fopen(OUTPUTFILENAME, "wb+");
        if (!binFile) {return;}

        int header[3] = {0, rows, cols};// 0 bos
        if(fwrite(header, sizeof(int), 3, binFile) != 3){
			fclose(binFile);
			return;
		}
        fflush(binFile);
    }
    if(fwrite(biasMatrix, sizeof(int), 4 * rows * cols, binFile) != (size_t)(4 * rows * cols)){
        fclose(binFile);
        return;
    }
    fclose(binFile);
		
	/*//	.TEXT		istatistigin sayisal degerlerini text olarak yazdirmak icin
	FILE *textFile = fopen("directionData2.txt", "w");
	if(textFile == NULL) return;
	
	fprintf(textFile, "Yukari\tAsagi\tSol\tSag\n");
	for(i= 0; i<rows; i++){
		for(j= 0; j<cols; j++){
			if(i == 0 && j == 2){
				fprintf(textFile,"X");
			}
			if(i == 4 && j == 3){
				fprintf(textFile,"X");
			}
			fprintf(textFile, "[%d, %d, %d, %d]\t", biasMatrix[INDEX(i,j,0,rows,cols)], biasMatrix[INDEX(i,j,1,rows,cols)], 
				biasMatrix[INDEX(i,j,2,rows,cols)], biasMatrix[INDEX(i,j,3,rows,cols)]);
		}
		fprintf(textFile,"\n");
	}
	if(textFile != NULL)//bellek sizintisi riskiymis
		fclose(textFile);
	*/
	
}

int main(int argc, char *argv[]){
	if (argc < 4) {
        printf("Usage: %s <StartX> <StartY> <EndX> <EndY>\n", argv[0]);
        return -1;
    }
	int startX = atoi(argv[1]);
	int startY = atoi(argv[2]);
	int endX = atoi(argv[3]);
	int endY = atoi(argv[4]);
	FILE *file = fopen(INPUTFILENAME, "rb");
	if(file == NULL) {
		printf("dosya acilamadi");
		return -1;
	}else{
		printf("dosya acildi\n");
	}
	
	int measures[3];
	fread(measures, sizeof(int), 3, file);// dosyanin basina rows, cols ve count sayilarinin ekledim atlamasi icin
	int rows = measures[1];
	int cols = measures[2];
	
	/*	Memory allocation	*/
	int *biasMatrix = (int*) malloc(4 * rows * cols * sizeof(int));// en kisa yolda doldurulacak yon matrisi 	
	if (!biasMatrix) {	return -1;    }
    int *matrix = (int *) malloc(rows * cols * sizeof(int));
	if (!matrix) { free(biasMatrix);return -1; }
	int *visited = (int *) malloc( rows * cols * sizeof(int));
	if (!visited) { free(biasMatrix);free(matrix);return -1; };
	int *backward = (int*) malloc(2 * rows * cols * sizeof(int));// en kisa yol bulmak icin matris
	if (!backward) {	
		free(biasMatrix);
		free(matrix);
		free(visited);
		return -1;
	}
	memset(biasMatrix, 0, 4 * rows * cols * sizeof(int));
	int maksimum = measures[0];
	int counter = 0, percentage = 0;
	//fread() fonksiyonu (dogru calismis ve sonda degilse) okunan veri sayisini geri verir.
	while(fread(matrix, sizeof(int), rows*cols, file) == rows*cols && counter<maksimum){ 
		if (counter > 1000)// ilk basta bosa yakin olusturuyor. onlari atlamak icin
		{
			BFS(startX,startY,endX,endY,matrix,biasMatrix,visited,backward,rows,cols);
		}
		
		
		counter++;
		if(counter%(maksimum/100) == 0){
			percentage++;
			if(percentage%5 == 0)
				printf("yuzde %d\n",percentage);
		}
	}
	fclose(file);
	printf("DOSYAYA KAYIT YAPILIYOR\n");
	statisticCreate(rows, cols, biasMatrix);
	free(matrix);
	free(visited);
	free(biasMatrix);
	free(backward);


	return 0;
}
