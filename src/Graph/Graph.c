#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Graph.h"

static Edge EDGEcreate(int v, int w) {
  Edge e;
  e.v = v;
  e.w = w;
  return e;
}

static link NEW(int v, link next) {
  link x = malloc(sizeof *x);
  if (x == NULL)
    return NULL;
  x->v = v;
  x->next = next;
  return x;
}

Graph GRAPHinit(int V) {
  int v;
  Graph G = malloc(sizeof *G);
  if (G == NULL)
    return NULL;

  G->V = V;
  G->E = 0;
  G->z = NEW(-1, NULL);
  if (G->z == NULL)
    return NULL;
  G->ladj = malloc(G->V*sizeof(link));
  if (G->ladj == NULL)
    return NULL;
  for (v = 0; v < G->V; v++)
    G->ladj[v] = G->z;
  return G;
}

void GRAPHfree(Graph G) {
  int v;
  link t, next;
  for (v=0; v < G->V; v++)
    for (t=G->ladj[v]; t != G->z; t = next) {
      next = t->next;
      free(t);
    }
  free(G->ladj);
  free(G->z);
  free(G);
}

static void  insertE(Graph G, Edge e) {
  int v = e.v, w = e.w;

  G->ladj[v] = NEW(w, G->ladj[v]);
  G->E++;
}

void GRAPHinsertE(Graph G, int id1, int id2) {
  insertE(G, EDGEcreate(id1, id2));
}

void GRAPHremoveE(Graph G, int id1, int id2) {
  removeE(G, EDGEcreate(id1, id2));
}

static void  removeE(Graph G, Edge e) {
  int v = e.v, w = e.w;
  link x;
  if (G->ladj[v]->v == w) {
    G->ladj[v] = G->ladj[v]->next;
    G->E--;
  }
  else
    for (x = G->ladj[v]; x != G->z; x = x->next)
      if (x->next->v == w) {
        x->next = x->next->next;
        G->E--;
  }
}

void dfsR(Graph G, int id, int *visited, int* found, int turn) {
  link t;

  if (visited[id] == 1 || *found == 1)
    return;
  if ( (turn == 1 && id%10 == 0) || (turn == 2 && id%10 == 6) ) {
    *found = 1;
    return;
  }
    
  visited[id] = 1;
  
  for (t = G->ladj[id]; t != G->z; t = t->next) {
    dfsR(G, t->v, visited, found, turn);
  }
}

void GRAPHdfs(Graph G, int id, int* found, int turn) {
  int i, *visited;
  visited = malloc(G->V * sizeof(int));
  if (visited == NULL)
    return;

  for (i=0; i < G->V; i++)
    visited[i] = -1;

  dfsR(G, id, visited, found, turn);
	free(visited);
}

void  GRAPHedges(Graph G, Edge *a) {
  int v, E = 0;
  link t;
  for (v=0; v < G->V; v++)
    for (t=G->ladj[v]; t != G->z; t = t->next)
      a[E++] = EDGEcreate(v, t->v);
}
