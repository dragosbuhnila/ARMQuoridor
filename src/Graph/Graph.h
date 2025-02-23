typedef struct node *link;

typedef struct node { 
	int v;
	link next;
} node;

typedef struct graph { 
	int V; 
	int E; 
	link *ladj; 
	link z;
} graph;

typedef struct graph *Graph;

typedef struct edge {
	int v;
	int w;
} Edge;

Graph GRAPHinit(int V);
void  GRAPHfree(Graph G);

link  NEW(int v, link next);
Edge  EDGEcreate(int v, int w);

void  GRAPHinsertE(Graph G, int id1, int id2);
void  GRAPHremoveE(Graph G, int id1, int id2);
void  insertE(Graph G, Edge e);
void  removeE(Graph G, Edge e);

void GRAPHdfs(Graph G, int id, int* found, int turn);
void dfsR(Graph G, int id, int *visited, int* found, int turn);

void  GRAPHedges(Graph G, Edge *a);
