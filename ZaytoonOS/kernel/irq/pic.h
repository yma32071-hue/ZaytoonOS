#ifndef ZAYTOONOS_IRQ_PIC_H
#define ZAYTOONOS_IRQ_PIC_H

#include "kernel/types.h"

void pic_remap(void);
void pic_mask(u8 irq_line);
void pic_unmask(u8 irq_line);
void pic_eoi(u8 irq);

#endif /* ZAYTOONOS_IRQ_PIC_H */
