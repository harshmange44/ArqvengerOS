#include "system/processQueue.h"
#include "type.h"

#define QUEUE_SIZE      256

struct QueueNode* freeNodes = NULL;

static void free_node(struct QueueNode* node);

static struct QueueNode* alloc_node(void);

void free_node(struct QueueNode* node) {
    node->next = freeNodes;
    freeNodes = node;
}

struct QueueNode* alloc_node(void) {

    struct QueueNode* node;
    if (freeNodes == NULL) {
        node = kalloc(sizeof(struct QueueNode));
        if (node == NULL) {
            return NULL;
        }
    } else {
        node = freeNodes;
        freeNodes = node->next;
    }

    return node;
}

void process_queue_push(struct ProcessQueue* queue, struct Process* process) {

    struct QueueNode* node = alloc_node();
    node->process = process;
    node->acumPriority = 1;

    if (queue->first == NULL) {
        queue->first = queue->last = node;
    } else {
        queue->last->next = node;
        queue->last = node;
    }

    node->next = NULL;
}

void process_queue_remove(struct ProcessQueue* queue, struct Process* process) {

    struct QueueNode* prev = NULL;
    struct QueueNode* node = NULL;

    if (queue->first == NULL) {
        return;
    }

    node = queue->first;
    while (node != NULL) {

        if (node->process->pid == process->pid) {

            if (prev != NULL) {
                prev->next = node->next;
            } else {
                queue->first = node->next;
            }

            free_node(node);
            return;
        }

        prev = node;
        node = node->next;
    }
}


