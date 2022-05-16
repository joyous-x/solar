#include "rc4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void rc4_setup( rc4_state *self, unsigned char *key,  int keylen) {
    int i, index1, index2;

    for (i = 0; i < 256; i++) self->state[i] = i;
    self->x = 0; self->y = 0;
    index1 = 0; index2 = 0;
    for (i = 0; i < 256; i++)
    {
        int t;
        index2 = (key[index1] + self->state[i] + index2) % 256;
        t = self->state[i];
        self->state[i] = self->state[index2];
        self->state[index2] = t;
        index1 = (index1 + 1) % keylen;
    }
} 

void rc4_crypt( rc4_state * self, unsigned char * block, int len)
{
    int i, x = self->x, y = self->y;

    for (i = 0; i < len; i++)
    {
        x = (x + 1) % 256;
        y = (y + self->state[x]) % 256;
        {
            int t;		/* Exchange state[x] and state[y] */
            t = self->state[x];
            self->state[x] = self->state[y];
            self->state[y] = t;
        }
        {
            int xorIndex;	/* XOR the data with the stream data */
            xorIndex = (self->state[x] + self->state[y]) % 256;
            block[i] ^= self->state[xorIndex];
        }
    }
    self->x = x;
    self->y = y;
}

#if 0
int main()
{
    char key[128] = {"12345678"};
    char data[512] = {"asdfasdfasdfasf"};
    struct rc4_state *s;
    s=(struct rc4_state *) malloc (sizeof(struct rc4_state));
    
    printf("key : %s\n", key);
    printf("raw : %s\n", data);
    rc4_setup(s, (unsigned char *)key, strlen(key));  

    rc4_setup(s, (unsigned char *)key, strlen(key));  
    rc4_crypt(s, (unsigned char *)data, strlen(data));
    printf("encrypt  : %s\n",data);
    
    rc4_setup(s, (unsigned char *)key, strlen(key));  
    rc4_crypt(s, (unsigned char *)data, strlen(data));
    printf("decrypt  : %s\n",data);

    return 0;
}
#endif
