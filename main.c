#include <stdio.h>
#include <stdbool.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#include "priorityq.c"

Point sites[5] = {
    {100,100},  
    {150,150},  
    {170,200},
    {10,203},
    {200,201}
};
int sites_len= 5;

float  parabola_intersection(int y,Point f1,Point f2); 
Node * pq = NULL;

Point vertecies[100];
int vi=0;

Edge edges[100];
int ei=0;


// Beach line
Arc * BL = NULL;

Point * Edge_intersection(Edge * el, Edge * er){
  // printf("(%d,%d),(%d,%d)\n",el->start->x,el->start->y,er->start->x,er->start->y);
  // printf("[%f, %f] \n ",el->m,er->m);
  if (el == NULL || er == NULL) return NULL;
  if (el->m == INFINITY) return newPoint(el->startx,getY(el,er->startx));
  else if (er->m == INFINITY) return newPoint(er->startx,getY(er,el->startx));
  else{
    float mdif = er->m  - el->m;
    if (mdif == 0){
      return NULL;
    }
    float value_x  = (float)(el->q - er->q ) / mdif;
    return newPoint(
      value_x,
       getY(el,value_x)
        );
  }
return NULL;

}

  
  
  
 void AddCircleEvent(Arc * arc, Point * p){
  // printf("circle Event \n");
  if (arc->left != NULL && arc->right !=NULL){
   Point a = *arc -> left -> p;
   Point b = *arc -> p;
   Point c = *arc -> right -> p;
   // printf("a  : (%d,%d)\n b: (%d,%d)\n c : (%d,%d)\n\n",a.x,a.y,b.x,b.y,c.x,c.y);
    //determining weather the point's bisectors intersects
    
    int value  =(b.x-a.x) * (c.y-a.y) - (c.x-a.x) * (b.y-a.y);
    // printf("%d , %d, %d, %d\n",(b.x-a.x) , (c.y-a.y) , (c.x-a.x), (b.y-a.y)); 
    
    // printf("value : %d \n",value);
    if(value > 0 ) {
        // printf("The point is inside the circle\n");
        Point *  edge_intersec = Edge_intersection(arc->le,arc->re);
      printf("  edge_intersec : %d,%d\n",edge_intersec->x,edge_intersec->y);
        if (edge_intersec != NULL){
        // printf("edge_intersec is not NULL");
        float circle_radius = sqrt(pow(edge_intersec->x - arc->p->x,2)+ pow(edge_intersec->y - arc->p->y,2));
        float vertex_y_value = circle_radius + edge_intersec->y;
          Point * event_position = newPoint(edge_intersec->x,vertex_y_value);
          // printf("(%d %d)",event_position->x,event_position->y);
          push(&pq,newEvent(true,event_position,arc,edge_intersec));
          // printf("pushing event\n");
        }
      } 
     
    }

  } 
  
  
void PointEvent(Event * e){
    // printf("PointEvent\n");

    Point * p = e->p;

    Arc *head  = BL;
    
    if (head == NULL){
      BL = newArc(p,NULL,NULL,NULL,NULL);

      return ;
    }

    //
    // if (p->x  < head->p -> x){
    //   Edge * re = newEdge(p,head->p,p->x);
    //   Arc * n = newArc(p,NULL, head,NULL,re);
    //   head->left = n;
    //   head = n;
    //   head->right->le = re;
    // BL = head;
    // }

    // finding the arc below the inserting Point
    
    while ( head->right != NULL && parabola_intersection(p->y,*head->p,*head->right->p)<= p->x){
    // while ( head->right != NULL && head->p->x <= p->x){
      head = head->right;

    }

    
    Edge * el = newEdge(head->p,p,p->x);
    Edge * er = newEdge(p,head->p,p->x);


    edges[ei] = *el;
    ei++;
    edges[ei] = *el;
    ei++;




    Arc * arc_p = newArc(p,head,NULL,el,er);
    Arc * arc_hr = newArc(head->p,arc_p,head->right,el,head->re);
    if (head->right != NULL){
    head->right->left = arc_p;
  }

    arc_p->right = arc_hr;
    head->right = arc_p;
    
    head->re = el;
    
    AddCircleEvent(head,p);
    AddCircleEvent(arc_hr,p);

  }
float  parabola_intersection(int y,Point f1,Point f2) {
    float fyDiff = f1.y - f2.y;
    if (fyDiff == 0) return (f1.x + f2.x) / 2;
    float fxDiff = f1.x - f2.x;
    float b1md = f1.y - y; //Difference btw parabola 1 fy and directrix
    float b2md = f2.y - y; //Difference btw parabola 2 fy and directrix
    float h1 = (-f1.x * b2md + f2.x * b1md) / fyDiff;
    float  h2 = sqrt(b1md * b2md * (pow(fxDiff , 2) +pow(fyDiff , 2))) /
      fyDiff;

    return h1 + h2; //Returning the left x coord of intersection. Remember top of canvas is 0 hence parabolas are facing down
  }

void CircleEvent(Event * e){
  /*printf("CircleEvent\n");  */

  Arc * arc = e->caller;
  Point *p = e->p;


  Edge *new_edge = newEdge(arc->left->p,arc->right->p,-1);


  if (arc->left->event != NULL){
    arc->left->event->active = false;
  }

  if (arc->right->event != NULL){
    arc->right->event->active = false;
  }


  arc->left->re = new_edge;
  arc->right->le = new_edge;


  arc->left->right = arc->right;
  arc->right->left = arc->left;
  
  edges[ei] = *new_edge;
  ei++;
  vertecies[vi] = *e->vertex;
  vi++;


  arc->le->end  = e->vertex;
  arc->re->end = e->vertex;


  new_edge->start = e->vertex;

  AddCircleEvent(arc->left,p);
  AddCircleEvent(arc->right,p);



};









// ________________________________________________________________________________________________________________

void seeBeachLine(Arc * a){
  if (a->right){
    printf("(%d,%d)[%d,%d] -> ",a->p->x,a->p->y,a->le ? 1:0,a->re?1:0);
    seeBeachLine(a->right);
  }else{
    return ;
  }
}




int main(){

for (int i = 0; i < sites_len; i++){
 Event * e = malloc(sizeof(Event));
  e->p = newPoint(sites[i].x, sites[i].y);
  e->type = 0;
  push(&pq,e);

}


while(!isEmpty(&pq)){


    Event * e = top(&pq);
    // printf("(%d, %d)\t\n",e->p->x,e->p->y);
    pop(&pq);


    if (e->type == 0){
      PointEvent(e);
    }else{
      CircleEvent(e);
    }

   // seeBeachLine(BL);
  }

  // for (int l = 0; l < ei ; l++){
  //   printf("(%f,%f) -\n ",edges[l].m,edges[l].q);
  // }
  // printf("number of vertices calculated : %d",vi);
  //
  // for (int j = 0;j < vi; j++){
  //   printf("[  %d  ,  %d  ]  \n",vertecies[j].x,vertecies[j].y);
  // }
  //


  return 0;
}