#ifndef _QGRAMS_PRIVATE_HEADER
#define _QGRAMS_PRIVATE_HEADER

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QGRAMS_STACK_INIT_SIZE 16

// Type declarations. //
struct int_stack_t;
struct queue_t;

typedef struct int_stack_t int_stack_t;
typedef struct queue_t queue_t;
typedef int_stack_t *index_t;

// Function declarations. //
int           int_stack_push (uint32_t, index_t *);
index_t     * make_index (FILE *, unsigned int);
index_t     * new_index (unsigned int);
int_stack_t * new_int_stack (void);
queue_t     * new_queue (unsigned int);
uint32_t      queue_pop (queue_t *);
int           queue_push (uint32_t, queue_t *);
void          scan (FILE *, unsigned int, unsigned int, unsigned int);
void          update_bins (char *, const int_stack_t *, uint32_t,
      int e, int);

// Constants. //
static const unsigned char CHARVAL[256] = {
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255, 0 ,255, 1 ,255,255,255, 2 ,255,255,255,255,255,255,255,255,
   255,255,255,255, 3 ,255,255,255,255,255,255,255,255,255,255,255,
   255, 0 ,255, 1 ,255,255,255, 2 ,255,255,255,255,255,255,255,255,
   255,255,255,255, 3 ,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
};

// Type definitions. //
struct int_stack_t
{
   size_t    space;   // Stack size.
   size_t    nitems;  // Number of items.
   uint32_t  items[]; // Items.
};

struct queue_t
{
   size_t    space;   // Queue size.
   size_t    first;   // First element.
   size_t    nitems;  // Number of items.
   uint32_t  items[]; // Items.
};

#endif
