#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define __DS__

#define INITIAL_CAP 10
#define UPPER_THRESHOLD 0.75 // 75%
#define LOWER_THRESHOLD 0.25

struct Vertex {
  short x, y;
};


typedef struct Vertex Vertex;

struct DynArr {
  Vertex *arr;
  size_t  len;
  size_t  cap;
};

typedef struct DynArr DynArr;

int da_init(DynArr *);
int da_push(DynArr *, Vertex);
static int __resize(DynArr *);
void da_print(DynArr *);

unsigned int magnetude(Vertex *, Vertex *);

int da_init(DynArr *s) {
  if (!(s->arr = (Vertex *)malloc(INITIAL_CAP * sizeof(Vertex)))) {
    return 1;
  }
  s->cap = INITIAL_CAP;
  s->len = 0;
  return 0;
}

int da_push(DynArr *s, Vertex v) {
  __resize(s);

  s->arr[s->len++] = v;
  return 0;
}

static int __resize(DynArr *s) {
  if (s->len < s->cap * UPPER_THRESHOLD) {
    printf("--------resize: %zu/%zu\n", s->len, s->cap);
    return 0;
  }

  s->arr = (Vertex *)realloc(s->arr, s->cap * sizeof(Vertex) * 2);
  if (!s->arr) {
    perror("resizing fault");
    exit(1);
  }
  s->cap *= 2;
  return 0;
}

void da_print(DynArr *a) {
  printf("Dynamic Array: \n");
  for (int i = 0; i < a->len; ++i) {
    printf("%i %i\n", a->arr[i].x, a->arr[i].y);
  }
}

void da_deinit(DynArr *a) {
  free(a->arr);
  a->len = 0;
  a->cap = 0;
}

unsigned int magnetude(Vertex *v1, Vertex *v2) {
  int dx = v2->x - v1->x;
  int dy = v2->y - v2->y;
  return sqrt(dx * dx + dy * dy);
} 
