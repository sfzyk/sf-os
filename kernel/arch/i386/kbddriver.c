#include <kernel/input-event-codes.h>

#define Q_TO_P(x) (code >= KEY_Q && code <= KEY_P)
#define A_TO_L(x) (code >= KEY_A && code <= KEY_L)
#define Z_TO_M(x) (code >= KEY_Z && code <= KEY_M)

#define QP_GAP KEY_Q
#define AL_GAP (KEY_A+KEY_Q-KEY_P-1)
#define ZM_GAP (KEY_Z+KEY_A-KEY_L+KEY_Q-KEY_P-2)

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