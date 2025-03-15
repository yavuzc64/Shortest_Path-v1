#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXSIZE 100
#define FILENAME "Maps_10k.bin"

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

int saveAsBinary(int *validMatrices, int newCount, const char *filename, int rows, int cols) {
    FILE *file = fopen(filename, "rb+");
    if (!file) {
		//dosyayı okumak icin bulamazsa yeni dosya olusturmak
        file = fopen(filename, "wb+");
        if (!file) {return -1;}

        int header[3] = {0, rows, cols}; // count = 0 olarak basla
        if(fwrite(header, sizeof(int), 3, file) != 3){
			fclose(file);
			return -1;
		}
        fflush(file);
    }
    int oldCount = 0;
    fseek(file, 0, SEEK_SET);
    fread(&oldCount, sizeof(int), 1, file);

    fseek(file, 0, SEEK_END);//sona ekle
    // for (int i = 0; i < newCount; i++) {
    //     fwrite(&validMatrices[i * rows * cols], sizeof(int), rows * cols, file);
    // }
	if(fwrite(validMatrices, sizeof(int), newCount * rows * cols, file) != (size_t)(newCount * rows * cols)){
        fclose(file);
        return -1;// yazma basarisiz olursa dosya kapat
    }	

    int newTotalCount = oldCount + newCount;
    printf("count %d\n",newTotalCount);
    fseek(file, 0, SEEK_SET);
    fwrite(&newTotalCount, sizeof(int), 1, file);
    
    fclose(file);
	return 0;
}


void generateRandomMap(unsigned long long int key, int *matrix, int rows, int cols){
	srand(key);
	int i, max = rows*cols;
	for ( i = 0; i < max; i++)
	{
		matrix[i] = rand() % 2;
	}
	
}

int main(int argc, char *argv[]) {
    /*	Argument Control	*/
	if (argc < 7) {
        printf("Usage: %s <rows> <cols> <StartX> <StartY> <EndX> <EndY>\n", argv[0]);
        return -1;
    }
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
	if (rows <= 0 || cols <= 0) {
		printf("rows ve cols pozitif olmalidir!\n");
		return -1;
	}
	int startX = atoi(argv[3]);
	int startY = atoi(argv[4]);
	int endX = atoi(argv[5]);
	int endY = atoi(argv[6]);
	if (startX < 0 || startX >= rows || startY < 0 || startY >= cols || endX < 0 || endX >= rows || endY < 0 || endY >= cols) {
		printf("Baslangic ve bitis noktalari matrisin icinde olmalidir!\n");
		return -1;
	}
	if (startX == endX && startY == endY) {
		printf("Baslangic ve bitis noktalari ayni olamaz!\n");
		return -1;
	}
	
	unsigned int parseSize = 2000;	// ValidMaastrices dizisinin boyutunu belirler.(realloc icin)
	unsigned int chunkSize = 100000;// Dosyaya ne siklikla yazilacagini belirler. 
	unsigned int counter = 0; 		// Islem sirasinda dosyaya kaydedilmemis && valid harita sayisi tutar.
	unsigned simpleCount = 10000;	// istatistigin kac harita ile yapilacigi
	
	/*	Memory allocation	*/
	int *validMatrices = (int*) malloc(parseSize * rows * cols * sizeof(int));
	if (!validMatrices) {	return -1;    }
    int *matrix = (int *) malloc(rows * cols * sizeof(int));
	if (!matrix) { return -1; }
	int *visited = (int *) malloc( rows * cols * sizeof(int));
	if (!visited) { return -1; }
	
	
	/*	Main Process	*/
	int i,j;
	unsigned long int key = 1;
	//long unsigned int sectionStep = 1; // key icin adim atlamada kullaniliyordu.
	//const unsigned long int threshold = (float)totalCases * 0.01f;

    while(simpleCount > 0 && key < 4290000000){
        generateRandomMap(key, matrix, rows, cols);
        if(DFS(startX, startY, endX, endY, matrix, visited, rows, cols)==1){
			if (counter > chunkSize)
			{
				int i = saveAsBinary(validMatrices, counter, FILENAME, rows, cols);
				if (i != 0) {//dosyaya yazma basarisiz olursa
					free(validMatrices);
					free(matrix);
					free(visited);
					return -1;
				}
				counter = 0;
			}
			
			if (counter >= parseSize) {
                parseSize += 20000;
				int *temp = (int*) realloc(validMatrices, parseSize * rows * cols * sizeof(int));
				if (!temp) {
					free(validMatrices);
					printf("realloc olmadi !\n");
					return -1;
				}
				validMatrices = temp;
            }
			
			for ( i = 0; i < rows; i++){
                for (j = 0; j < cols; j++){
                    validMatrices[counter * cols * rows + (i * cols + j)] = matrix[i * cols +j];
				}
			}

			counter++;
			simpleCount--;
		}
		key++;
    }
	
	/*	Exit Process	*/
	i = saveAsBinary(validMatrices, counter, FILENAME, rows, cols);
	free(validMatrices);
	free(matrix);
	free(visited);

	return (i == 0) ? 0 : -1;
}
