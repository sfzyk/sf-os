#include <kernel/input-event-codes.h>
#include <i386/irq.h>
#include <kernel/signal.h>
#include <i386/config.h>
#include <i386/io.h>

#define Q_TO_P(x) (code >= KEY_Q && code <= KEY_P)
#define A_TO_L(x) (code >= KEY_A && code <= KEY_L)
#define Z_TO_M(x) (code >= KEY_Z && code <= KEY_M)

#define QP_GAP KEY_Q
#define AL_GAP (KEY_A-KEY_P+QP_GAP-1)
#define ZM_GAP (KEY_Z-KEY_L+AL_GAP-1)

char keymap[26] = {
    'q','w','e','r','t','y','u','i','o','p',
    'a','s','d','f','g','h','j','k','l',
    'z','x','c','v','b','n','m'
};

char codeToChar(unsigned char code)
{
    if(Q_TO_P(code))
        return keymap[code - QP_GAP];
    else if(A_TO_L(code))
        return keymap[code - AL_GAP];
    else if(Z_TO_M(code))
        return keymap[code - ZM_GAP];
    return 0;
}

irqreturn_t kbd_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    unsigned char scancode = inb(0x60);
	unsigned char c = codeToChar(scancode);
	if(c >= 'a' && c <= 'z')
		printf("%c",c);
	return IRQ_HANDLED;
}

static struct irqaction irq1  = { kbd_interrupt, SA_INTERRUPT, CPU_MASK_NONE, "kbd", NULL, NULL};

void intr_init_hook(void){
	setup_irq(1,&irq1);
}