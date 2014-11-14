#include "qgram.h"

#define min(a,b) (a)<(b)?(a):(b)

// Global variables. //
int ERROR;
static const uint32_t NOGRAM = -1;

// Function definitions. //
index_t *
make_index
(
         FILE * inputf,
   const unsigned int q
)
{

   index_t *index = new_index(q);
   if (index == NULL) abort();

   const unsigned int mask = (1 << 2*q) - 1;

   int skip = 1 << q;
   uint32_t qgramid = 0, c = 0;
   while ((c = fgetc(inputf)) != EOF) {
      if (CHARVAL[c] > 3) skip = 1 << q;
      qgramid = mask & (CHARVAL[c] + (qgramid << 2));
      if ((skip = skip >> 1)) continue;
      int_stack_push(ftell(inputf), index + qgramid);
   }

   return index;

}

void
update_bins
(
   char * bins,
   const int_stack_t * stack,
   const uint32_t j,
   const int e,
   const int value
)
{
   for (int i = 0 ; i < stack->nitems ; i++) {
      int d = stack->items[i] - j;
      // All 'e+1' consecutive bins have the q-gram.
      for (int k = 0 ; k < e+1 ; k++) {
         // Ignore 'bins[0]' (contains all the qgrams).
         if (d-k < 1) break;
         bins[d-k] += value;
      }
   }
}


void
scan
(
         FILE * inputf,
   const unsigned int q,
   const unsigned int n,
   const double       eps
)
{

   const unsigned int n0 = n;
   const unsigned int n1 = ceil((floor(eps*n0) + 1) / eps);
   const unsigned int tau =
      min(n0+1 - q*(floor(eps*n0)+1), n1+1 - q*(floor(eps*n1)+1));
   const unsigned int e = floor((2*(tau-1) + (q-1)) / (1/eps - q));
   const int unsigned w = (tau-1) + q*(e+1);

   fprintf(stderr, "tau = %d\n", tau);
   fprintf(stderr, "e = %d\n", e);
   fprintf(stderr, "w = %d\n", w);

   const unsigned int L = w-q+1;
   const unsigned int mask = (1 << 2*q) - 1;

   // At least one q-gram per parallelogram. //
   if (tau < 1) abort();

   fseek(inputf, 0, SEEK_SET);

   index_t *index = make_index(inputf, q);
   fprintf(stderr, "done indexing\n");

   const uint32_t nbins = ftell(inputf);
   char *bins = calloc(nbins, sizeof(char));
   if (bins == NULL) abort();

   queue_t *queue = new_queue(L);
   if (queue == NULL) abort();

   fseek(inputf, 0, SEEK_SET);

   // Fill the queue. //
   for (int i = 0 ; i < L ; i++) queue_push(NOGRAM, queue);

   // Scan the text. //
   int skip = q, c;
   uint32_t qgramin = 0, qgramout = 0;
   while ((c = fgetc(inputf)) != EOF) {

      size_t textpos = ftell(inputf);

      if (CHARVAL[c] > 3) skip = 1 << q;
      qgramin = mask & (CHARVAL[c] + (qgramin << 2));
      qgramout = queue_pop(queue);

      if (qgramout != NOGRAM) {
         update_bins(bins, index[qgramout], textpos-L, e, -1);
      }
      if ((skip = skip >> 1)) {
         queue_push(NOGRAM, queue);
      }
      else {
         update_bins(bins, index[qgramin], textpos, e, +1);
         queue_push(qgramin, queue);
      }

      // Check the BINS. //
      for (size_t j = 0 ; j < nbins - textpos ; j++) {
         if (bins[j] > tau-1) fprintf(stdout, "%ld:%ld (%d)\n",
            j, textpos, bins[j]);
      }

   }

}


queue_t *
new_queue
(
   unsigned int L
)
{
   // Allocate memory for a node array, with 'GSTACK_INIT_SIZE' slots.
   size_t base = sizeof(queue_t);
   size_t extra = L * sizeof(uint32_t);
   queue_t *new = malloc(base + extra);
   if (new == NULL) {
      ERROR = __LINE__;
      return NULL;
   }
   new->space = L;
   new->first = 0;
   new->nitems = 0;

   return new;

}


int_stack_t *
new_int_stack
(void)
{

   // Allocate memory for a node array, with 'GSTACK_INIT_SIZE' slots.
   size_t base = sizeof(int_stack_t);
   size_t extra = QGRAMS_STACK_INIT_SIZE * sizeof(uint32_t);
   int_stack_t *new = malloc(base + extra);
   if (new == NULL) {
      ERROR = __LINE__;
      return NULL;
   }
   new->space = QGRAMS_STACK_INIT_SIZE;
   new->nitems = 0;

   return new;

}


int
queue_push
(
   uint32_t   item,
   queue_t  * queue
)
{
   if (queue->nitems >= queue->space) return 1;
   queue->items[(queue->first + queue->nitems) % queue->space] = item;
   queue->nitems++;

   return 0;

}


uint32_t
queue_pop
(
   queue_t * queue
)
{
   
   if (queue->nitems < 1) return -1;
   uint32_t item = queue->items[queue->first];
   queue->first = (queue->first + 1) % queue->space;
   queue->nitems--;

   return item;

}


int
int_stack_push
(
   uint32_t   item,
   index_t  * stack_addr
)
{

   // Convenience variable for readability.
   int_stack_t *stack = *stack_addr;

   // Resize stack if needed.
   if (stack->nitems >= stack->space) {
      // If the stack has more items than slots, it is in a
      // locked state and will not receive more items. This
      // allows to 
      if (stack->nitems > stack->space) return 1;

      // The stack is not locked, allocate more memory.
      size_t new_space = 2 * stack->space;
      size_t base = sizeof(int_stack_t);
      size_t extra = new_space * sizeof(uint32_t);
      int_stack_t *ptr = realloc(stack, base + extra);
      if (ptr == NULL) {
         // Failed to add item to the stack. Increase
         // 'nitems' beyond 'nslots' to lock the stack.
         stack->nitems = stack->space + 1;
         ERROR = __LINE__;
         return 1;
      }
      *stack_addr = stack = ptr;
      stack->space = new_space;
   }

   // Push item and increase 'nitems'.
   stack->items[stack->nitems++] = item;

   return 0;

}


index_t *
new_index
(
   const unsigned int q
)
{
   const unsigned int Q = 1 << 2*q;
   index_t *new = malloc(Q * sizeof(int_stack_t *));
   if (new == NULL) {
      ERROR = __LINE__;
      return NULL;
   }
   for (int i = 0 ; i < Q ; i++) {
      new[i] = new_int_stack();
      if (new[i] == NULL) {
         while (--i > 0) free(new[i]);
         free(new);
         ERROR = __LINE__;
         return NULL;
      }
   }

   return new;

}
