#include <stdio.h>
#include <time.h>
#include <stdlib.h>
//#include "minigrind.h"
static int knTESTmatrix[5][5] = {
  {1, 2, 3, 4, 5},
  {2, 3, 4, 5, 6},
  {3, 4, 5, 6, 7},
  {4, 5, 6, 7, 8},
  {5, 6, 7, 8, 9},
};

static char *filename = "instructions.txt";

typedef struct route_node{
  int x,y;
  struct route_node *next, *prev;
}routeStep;

typedef struct k{
  int amplitude, frequency, n;
} k;

routeStep *addStep(routeStep *firststep, int x, int y){
  routeStep *newStep;
  newStep = (routeStep*) malloc (sizeof(routeStep));
  newStep->x = x;
  newStep->y = y;
  if (firststep == NULL){
    newStep->next = NULL;
    newStep->prev = NULL;
    return newStep;
  }
  routeStep *curr = firststep;
  while(1){
    if (curr->next == NULL){break;}
    else {curr = curr->next;}
  }
  curr->next = newStep;
  newStep->prev = curr;
  newStep->next = NULL;
  return firststep;
}

routeStep *removeLastStep(routeStep *firststep){
  if (firststep == NULL){return NULL;}
  routeStep *curr = firststep;
  if (curr->next == NULL){
    free(curr);
    return NULL;
  }
  if ((curr->next)->next == NULL){
    free(curr->next);
    curr->next = NULL;
    return firststep;
  }
  while(1){
    if ((curr->next)->next != NULL){curr = curr->next;}
    else if ((curr->next)->next == NULL){
      free(curr->next);
      curr->next = NULL;
      return firststep;
    }
  }
}

void printRoute(routeStep *firststep){
  if (firststep == NULL){printf("Empty\n"); return;}
  routeStep *curr = firststep;
  while(curr != NULL){
    printf("%i, %i\n", curr->x, curr->y);
    if (curr->next == NULL){return;}
    else {curr = curr->next;}
  }
}

void printRouteToFile(routeStep *firststep, char *filename){
  FILE *fp = fopen(filename, "w");
  if (fp == NULL){printf("Error opening file\n"); return;}
  routeStep *curr = firststep;
  while(curr != NULL){
    fprintf(fp, "%i%i", curr->x, curr->y);
    if (curr->next == NULL){return;}
    else {curr = curr->next;}
  }
  fclose(fp);
}

routeStep *deleteAllList(routeStep *firststep){
  while (1){
    if (firststep == NULL){return firststep;}
    firststep = removeLastStep(firststep);
  }
}

int bin_to_range(bit2, bit1, bit0){
  int sol = 0;
  sol = sol + bit0;
  sol = sol + (bit1 * 2);
  sol = sol + (bit2 * 4);
  return sol;
}

void printMatrix(k *matrix[]){
  for (int i = 0; i<6; i++){
    for (int j = 0; j<6 ; j++){
      printf("  %i, %i ;", matrix[i][j].amplitude, matrix[i][j].frequency);
    }
    printf("\n");
  }
}

routeStep *importListFromFile(routeStep *firststep, char *filename){
  FILE *fp = fopen(filename, "r");
  if (fp == NULL){printf("Error opening file\n"); return NULL;}
  int i, x, y, postracker = 0;
  i = getc(fp);
  while(i != EOF){
    if (postracker == 0){
      x = i - '0'; postracker++;
    } else if (postracker == 1){
      y = i - '0';
      firststep = addStep(firststep, x, y);
      postracker = 0;
    }
    i = getc(fp);
  }
  return firststep;
}


int main(void) {
  float time_taken = 0.0;
  char cmd;
  printf("Use 'r' to read and 'g' to generate: ");
  scanf(" %c", &cmd);
  routeStep *firststep = NULL;
  switch(cmd){
    case 'g':{
      printf("This will delete instructions in previous \"instructions.txt\" file!\nChange file name to keep the current data\nPress ENTER to continue.\n");
      int a = getchar();
      a = getchar();
      clock_t begin = clock();
      int currx = 5; int curry = 5;
      firststep = addStep(firststep, 5, 5);
      printf("Generating Matrix...");
      k matrix[5][5];
      for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
          matrix[i][j].amplitude = i+1;
          matrix[i][j].frequency = j+1;
          matrix[i][j].n = knTESTmatrix[i][j];
        }
      }
      printf("Matrix generated\n");
      printf("Finding route...\n");
      while (1){
        //When in top start go left
        if (curry == 1){
          firststep = addStep(firststep, currx-1, curry);
          currx--;
          printf("LEFT(LIM) New step found: x=%i y=%i\n", currx, curry);
        }
        //Go up
        else if (matrix[curry-2][currx-1].n < matrix[curry-1][currx-1].n){
          firststep = addStep(firststep, currx, curry-1);
          curry--;
          printf("UP New step found: x=%i y=%i\n", currx, curry);
        }
        //Go left
        else if (matrix[curry-1][currx-2].n < matrix[curry-1][currx-1].n){
          firststep = addStep(firststep, currx-1, curry);
          currx--;
          printf("LEFT New step found: x=%i y=%i\n", currx, curry);
        }
        else {
          //Go up to equal
          if (matrix[curry-2][currx-1].n <= matrix[curry-1][currx-1].n){
            firststep = addStep(firststep, currx, curry-1);
            curry--;
            printf("UP (EQ) New step found: x=%i y=%i\n", currx, curry);
          }
          //Go left to equal
          else if (matrix[curry-1][currx-2].n <= matrix[curry-1][currx-1].n){
            firststep = addStep(firststep, currx-1, curry);
            currx--;
            printf("LEFT (EQ) New step found: x=%i y=%i\n", currx, curry);
          }
        }

        if (currx == 1 && curry == 1){
          break;
        }
      }
      printRoute(firststep);
      printRouteToFile(firststep, filename);
      clock_t end = clock();
      time_taken += (double)(end - begin) / CLOCKS_PER_SEC;
      printf("%f", time_taken);
      break;
    }
    case 'r':{
      printf("Deleting potential previous data...\n");
      firststep = deleteAllList(firststep);
      firststep = importListFromFile(firststep, filename);
      printRoute(firststep);
      break;
    }
    default:
      printf("Invalid command\n");
  }

  //Free heap before ending
  firststep = deleteAllList(firststep);
  printf("Exiting...\n");
}
