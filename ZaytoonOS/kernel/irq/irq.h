#ifndef IRQ_H
#define IRQ_H

#include "kernel/types.h"

typedef void (*irq_handler_t)(void);

void irq_init(void);
void irq_disable(void);
void register_irq_handler(unsigned int irq, irq_handler_t handler);
void irq_dispatch(unsigned int irq);
void irq_enable(void);

#endif /* IRQ_H */
