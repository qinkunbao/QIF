void sample_encrypt_setup
               (encrypt_ctx *ctx,
               const unsigned char *key)
{
    int i, j, k;
    unsigned char *m, T[256];
    m = ctx->m;
    for(i = 0 ; i < 256; i++, k++)
    {
        // Secret Dependent Memory Access
        m[i] = Table[key[k] % 256];
        // Secret Dependent Control Flow Transfer
        if(key[k] == 0)
        {
            m[i] = k * key[i];
            m[i] = m[i] % 256;
        }
    }
}