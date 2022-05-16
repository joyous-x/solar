#ifndef _RC4_H
#define _RC4_H

typedef struct __st_rc4_state {
    unsigned char state[256];
    unsigned char x, y;
} rc4_state;

void rc4_setup( rc4_state *s, unsigned char *key,  int length );
void rc4_crypt( rc4_state *s, unsigned char *data, int length );

/* Encryption and decryption are symmetric */
#define rc4_decrypt rc4_crypt	

#endif
