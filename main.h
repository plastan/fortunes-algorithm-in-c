#include <math.h>


typedef struct Point{
    int x;
    int y;
}Point;



typedef struct Arc Arc;
typedef struct Edge Edge;

typedef struct Event{
    Point * p;
    Point * vertex;
    bool type;
    Arc * caller;
    bool active;
}Event;



struct Arc{
    Point * p;
    Arc * left;
    Arc * right;
    Edge *le;
    Edge *re;
    Event * event;
};


struct Edge{
    Arc * caller;
    Point * start;
    Point * end;
    float m;
    float q;
    float startx;
    Point * left_arc_point;
    Point * right_arc_point;
};



Point * newPoint(int x , int y){
  Point * p = malloc(sizeof(Point));
  p->x = x;
  p->y = y;
  return p;

};


Arc * newArc(Point * p, Arc * left, Arc * right,Edge * le, Edge * re){
    Arc * a = malloc(sizeof(Arc));
    a->p = p;
    a->left = left;
    a->right = right;
    a->event = NULL;
    a->le = le;
    a->re = re;
    return a;
};


float getY(Edge * e1, float startx){
  float result ;
  if (e1->m == INFINITY){
    result = -1;
  }else{
    result = e1->m * startx + e1->q;
  }
/*printf("getY %f %f %f \n",result,e1->m,e1->q);*/
  return result;
};


float getX(Edge * e, float y){
  float result ;
  if (e->m == INFINITY){
    result = -1;
  }else{
    result = (float) (y - e->q)/e->m;
  }
  return result;
}

Event * newEvent(bool type , Point *p , Arc * caller, Point * vertex){
    Event * e = malloc(sizeof(Event));
    e->p = p;
    e->type = type;
    e->caller = caller;
    e->vertex = vertex;

    return e;

}




Edge * newEdge(Point * p1, Point * p2, float startx){
        Edge * e = malloc(sizeof(Edge));
        e->m = (float)-(p1->x - p2->x)/(p1->y - p2->y);
        e->q =(float) (0.5 *(float) (p1->x * p1->x - p2->x * p2->x  + p1->y * p1->y - p2->y * p2->y)) / (p1->y - p2->y);

        e->left_arc_point = p1;
        e->right_arc_point = p2;
        if (startx != -1 && e->m != INFINITY){
          e->start = newPoint(startx, getY(e,startx) );
        }

        return e;
}


//  y = m  * x + q;
//


