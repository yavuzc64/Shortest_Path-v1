#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define MAXSIZE 100
#define MAX_VALID_MAPS 2500000 //bunu belli bir sayiya geldikce realloc yaparak artacak sekilde duzenle
#define FILENAME "NewValidMaps.bin"
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
int isValid(int x, int y, int *map, int *visited, int rows, int cols){
	return (x<rows && x >= 0 && y< cols && y>= 0 && !(visited[(x*cols)+y]) && map[(x*cols)+y] == 0);
}
int DFS(int startX, int startY, int endX, int endY, int *map, int *visited, int rows, int cols){	
	if(map[startX * cols + startY] == 1 || map[endX * cols + endY] == 1){
		return -1;
	}
	
	STACK s;
	initialize(&s);
	
    memset(visited, 0, rows * cols * sizeof(int));

	push(&s, startX, startY);
	
	//			Yukari, Asagi, 	Sol, 	Sag
	int dx[] ={	 0,		 0,		-1		,1};
	int dy[] ={	-1,		 1,		 0		,0};
	
	int i,newX,newY;
	
	while(!isEmpty(&s)){
		Point current = pop(&s);
		int x = current.X;
		int y = current.Y;
		
		if(visited[(x*cols)+y]){
			//zaten gecilmis
		}
		else if(x == endX && y == endY){
			return 1;
		}
		else{
			for(i=0;i<4;i++){
				newX = x + dx[i];
				newY = y + dy[i];
				if(isValid(newX,newY,map,visited, rows, cols)){
					push(&s, newX, newY);
				}
			}
			visited[(x*cols)+y] = 1;
		}
	}
	return -1;
}

int saveAsBinary(int *matrices, int count, const char *filename, int rows, int cols) {
	FILE *file = fopen(filename, "wb");
    if (file == NULL) return -1;

	if (fwrite(&rows, sizeof(int), 1, file) != 1 ||
        fwrite(&cols, sizeof(int), 1, file) != 1 ||
        fwrite(&count, sizeof(int), 1, file) != 1 ||
        fwrite(matrices, sizeof(int), count * rows * cols, file) != (size_t)(count * rows * cols)) {
        fclose(file);
        return -1;// yazma basarisiz olursa dosya kapat
    }	
	fclose(file);
	return 0;
}

void generateMatrix(uint32_t num, int *matrix, int rows, int cols) {
	int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            int bitPos = i * cols + j;
            matrix[bitPos] = (num >> bitPos) & 1;
        }
    }
}
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <rows> <cols>\n", argv[0]);
        return -1;
    }
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

	int counter = 0,i,j;
	uint32_t totalCases = 1 << (rows * cols); // 2^25 = 33,554,432

	int (*validMatrices) = (int*) malloc(MAX_VALID_MAPS * sizeof(int) * rows * cols);
	if (!validMatrices) {	return -1;    }
	
    int *matrix = (int *) malloc(rows * cols * sizeof(int));
	if (!matrix) { return -1; }
	int *visited = (int *) malloc( rows * cols * sizeof(int));
	if (!visited) { return -1; }

	uint32_t num ;
    for (num = 0; num < totalCases; num++) {
        generateMatrix(num, matrix, rows, cols);
        if(DFS(0,2,4,3,matrix, visited, rows, cols)==1){
        	if (counter < MAX_VALID_MAPS) {// burayi sabit yapma 50serli realloc yap !
                for ( i = 0; i < rows; i++)
                    for (j = 0; j < cols; j++)
                        validMatrices[counter * cols * rows + (i * cols + j)] = matrix[i * cols +j];

                counter++;
            } else {
                printf("Maks matrices count!\n");
                break;//degistir
            }
		}
		// if(num % 1000000 == 0){
		// 	for (i = 0; i < ROWS; i++) {
		//         for ( j = 0; j < COLS; j++) {
		//             printf("%d ", matrix[i][j]);
		// 	    }
		// 	    printf("\n");
		// 	}
		// 	printf("-----------\n");
		// }
    }
    printf("\ngecerli harita sayisi : %d", counter);
	
	
	i = saveAsBinary(validMatrices, counter, FILENAME, rows, cols);
	free(validMatrices);
	free(matrix);
	free(visited);

	return i;
	return 0;//bir sey dondurmeme ihtimali varsa diye silmedim
}
