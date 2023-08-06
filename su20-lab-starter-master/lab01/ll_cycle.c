#include <stddef.h>
#include "ll_cycle.h"

int ll_has_cycle(node *head) {
    /* your code here */
    node*p=head,*q=head;
    while(p&&q){
        p=p->next;
        if(q->next)
            q=q->next->next;
        else
            return 0;
        
        if(p==q&&p)
            return 1;

    }

    return 0;
}