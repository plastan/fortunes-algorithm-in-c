#include <stdbool.h>
#include <stdio.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#include "priorityq.c"

// TODO :
//  1. Make the whole thing memmory safe!!!
//  2. Add documentation.
//  3. Make raylib visualization.

Point sites[5] = {{100, 100}, {150, 150}, {170, 200}, {10, 203}, {200, 201}};
int sites_len = 5;

float parabola_intersection(int y, Point f1, Point f2);
Node *pq = NULL;

Point vertecies[100];
int vi = 0;

Edge *edges[100];
int ei = 0;

Edge final_edges[100];
int final_ei = 0;
// Beach line
Arc *BL = NULL;

Point *Edge_intersection(Edge *el, Edge *er) {

  if (el == NULL || er == NULL)
    return NULL;
  if (el->m == INFINITY)
    return newPoint(el->startx, getY(el, er->startx));
  else if (er->m == INFINITY)
    return newPoint(er->startx, getY(er, el->startx));
  else {
    float mdif = er->m - el->m;
    if (mdif == 0) {
      return NULL;
    }
    float value_x = (float)(el->q - er->q) / mdif;
    return newPoint(value_x, getY(el, value_x));
  }
  return NULL;
}

void AddCircleEvent(Arc *arc, Point *p) {

  if (arc->left != NULL && arc->right != NULL) {
    Point a = *arc->left->p;
    Point b = *arc->p;
    Point c = *arc->right->p;

    // determining weather the point's bisectors intersects

    int value = (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);

    if (value > 0) {
      Point *edge_intersec = Edge_intersection(arc->le, arc->re);
      if (edge_intersec != NULL) {
        float circle_radius = sqrt(pow(edge_intersec->x - arc->p->x, 2) +
                                   pow(edge_intersec->y - arc->p->y, 2));
        float vertex_y_value = circle_radius + edge_intersec->y;
        Point *event_position = newPoint(edge_intersec->x, vertex_y_value);
        push(&pq, newEvent(true, event_position, arc, edge_intersec));
      }
    }
  }
}

// poitn outside
bool Bounday_check(Point *p) {
  if (p->x < 0 || p->x > SCREEN_WIDTH || p->y < 0 || p->y > SCREEN_HEIGHT) {
    return true;
  }
  return false;
}

void PointEvent(Event *e) {
  // printf("PointEvent\n");

  Point *p = e->p;

  Arc *head = BL;

  if (head == NULL) {
    BL = newArc(p, NULL, NULL, NULL, NULL);

    return;
  }
  // finding the arc below the inserting Point

  while (head->right != NULL &&
         parabola_intersection(p->y, *head->p, *head->right->p) <= p->x) {
    // while ( head->right != NULL && head->p->x <= p->x){
    head = head->right;
  }

  Edge *el = newEdge(head->p, p, p->x);
  Edge *er = newEdge(p, head->p, p->x);

  edges[ei] = el;
  ei++;
  edges[ei] = er;
  ei++;

  Arc *arc_p = newArc(p, head, NULL, el, er);
  Arc *arc_hr = newArc(head->p, arc_p, head->right, el, head->re);
  if (head->right != NULL) {
    head->right->left = arc_p;
  }

  arc_p->right = arc_hr;
  head->right = arc_p;

  head->re = el;

  AddCircleEvent(head, p);
  AddCircleEvent(arc_hr, p);
}
float parabola_intersection(int y, Point f1, Point f2) {
  float fyDiff = f1.y - f2.y;
  if (fyDiff == 0)
    return (f1.x + f2.x) / 2;
  float fxDiff = f1.x - f2.x;
  float b1md = f1.y - y; // Difference btw parabola 1 fy and directrix
  float b2md = f2.y - y; // Difference btw parabola 2 fy and directrix
  float h1 = (-f1.x * b2md + f2.x * b1md) / fyDiff;
  float h2 = sqrt(b1md * b2md * (pow(fxDiff, 2) + pow(fyDiff, 2))) / fyDiff;

  return h1 + h2; // Returning the left x coord of intersection. Remember top of
                  // canvas is 0 hence parabolas are facing down
}

void CircleEvent(Event *e) {

  Arc *arc = e->caller;
  Point *p = e->p;

  Edge *new_edge = newEdge(arc->left->p, arc->right->p, -1);

  if (arc->left->event != NULL) {
    arc->left->event->active = false;
  }

  if (arc->right->event != NULL) {
    arc->right->event->active = false;
  }

  arc->left->re = new_edge;
  arc->right->le = new_edge;

  arc->left->right = arc->right;
  arc->right->left = arc->left;

  edges[ei] = new_edge;
  ei++;

  if (!Bounday_check(e->vertex)) {
    vertecies[vi] = *e->vertex;
    vi++;
  }

  // printf("Vertex %d %d\n", e->vertex->x, e->vertex->y);
  arc->le->end = e->vertex;
  arc->re->end = e->vertex;
  new_edge->start = e->vertex;

  AddCircleEvent(arc->left, p);
  AddCircleEvent(arc->right, p);
};

Point *edge_end(Edge *e, float y_limit) {
  float x = fmin(SCREEN_WIDTH, fmax(0, getX(e, y_limit)));
  float y = getY(e, x);
  if (y != -1)
    y = y_limit;
  Point *p = newPoint(x, y);
  vertecies[vi] = *p;
  vi++;
  return p;
}

void complete_edges(Point *last) {
  Arc *head = BL;

  while (head->right != NULL) {

    Edge *e = head->re;
    float x = parabola_intersection(last->y * 1.1, *e->left_arc_point,
                                    *e->right_arc_point);
    float y = getY(e, x);
    // printf("      : x %d y %d\n", x, y);
    // printf("start : x %d y %d\n", e->start->x, e->start->y);
    // printf("end   : x %d y %d\n", e->end ? e->end->x : -1,
    //        e->end ? e->end->y : -1);

    // finding  end point

    if ((e->start->y < 0 && y < e->start->y) ||
        (e->start->x > 0 && x < e->start->x) ||
        (e->start->x > SCREEN_WIDTH && x > e->start->x)) {
      e->end = e->start;
    } else {
      if (e->m == 0) {
        x = x - e->start->x <= 0 ? 0 : SCREEN_WIDTH;
        e->end = newPoint(x, e->start->y);
        vertecies[vi] = *e->end;

        vi++;

      } else {

        if (e->m == INFINITY) {

          y = SCREEN_HEIGHT;
        } else {
          y = e->m * (x - e->start->x) <= 0 ? 0 : SCREEN_HEIGHT;
        }
        printf("edge_end ( % d)", y);
        e->end = edge_end(e, y);

        printf("end : x %d y %d\n", e->end->x, e->end->y);
      }
    }
    head = head->right;
    int option = 0;
    for (int i = 0; i < ei; i++) {
      Edge *e = edges[i];

      option = 1 * Bounday_check(e->start) + 2 * Bounday_check(e->end);

      if (option == 3) {
        // both points are inside the SCREEN
        edges[ei] = NULL;
      } else if (option == 2) {
        // only start point is inside the SCREEN
        float y = e->end->y <= e->start->y ? 0 : SCREEN_HEIGHT;
        e->end = edge_end(e, y);
      } else if (option == 1) {
        // only end point is inside the SCREEN
        float y = e->start->y <= e->end->y ? 0 : SCREEN_HEIGHT;
        e->end = edge_end(e, y);
      } else {
        break;
      }
    }
  }
}

// ________________________________________________________________________________________________________________

void seeBeachLine(Arc *a) {
  if (a->right) {
    printf("(%d,%d)[%d,%d] -> ", a->p->x, a->p->y, a->le ? 1 : 0,
           a->re ? 1 : 0);
    seeBeachLine(a->right);
  } else {
    return;
  }
}

int main() {

  for (int i = 0; i < sites_len; i++) {
    Event *e = malloc(sizeof(Event));
    e->p = newPoint(sites[i].x, sites[i].y);
    e->type = 0;
    push(&pq, e);
  }
  Event *e = NULL;
  while (!isEmpty(&pq)) {

    e = top(&pq);
    pop(&pq);

    if (e->type == 0) {
      PointEvent(e);
    } else {
      CircleEvent(e);
    }
  }

  complete_edges(e->p);
  //
  for (int i = 0; i < ei; i++) {
    Edge *e = edges[i];
    if (e == NULL || e->end == NULL)
      continue;
    printf("end  (%d , %d) \n", e->end ? e->end->x : -1,
           e->end ? e->end->y : -1);
    //
  }
  //
  //

  // for (int i = 0; i < vi; i++) {
  //   printf("(%d,%d)\n", vertecies[i].x, vertecies[i].y);
  // }
  return 0;
}
