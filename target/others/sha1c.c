#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include <stdio.h>

// don't just forward declare as we want to pass it around by value
typedef struct _Sha1Digest
{
    uint32_t digest[5];
} Sha1Digest;
Sha1Digest Sha1Digest_fromStr (const char* src);
void       Sha1Digest_toStr   (const Sha1Digest* digest, char* dst);

// Streamable hashing
typedef struct _Sha1Ctx Sha1Ctx;
Sha1Ctx*   Sha1Ctx_create    (void);
void       Sha1Ctx_reset     (Sha1Ctx*);
void       Sha1Ctx_write     (Sha1Ctx*, const void* msg, uint64_t bytes);
Sha1Digest Sha1Ctx_getDigest (Sha1Ctx*);
void       Sha1Ctx_release   (Sha1Ctx*);

// Helper for one-off buffer hashing
Sha1Digest Sha1_get (const void* msg, uint64_t bytes);


static uint32_t rotl32(uint32_t x, int b)
{
    return (x << b) | (x >> (32-b));
}

// switch endianness
static uint32_t get32 (const void* p)
{
    const uint8_t *x = (const uint8_t*)p;
    return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}

static uint32_t f (int t, uint32_t b, uint32_t c, uint32_t d)
{
    assert(0 <= t && t < 80);

    if (t < 20)
        return (b & c) | ((~b) & d);
    if (t < 40)
        return b ^ c ^ d;
    if (t < 60)
        return (b & c) | (b & d) | (c & d);
    //if (t < 80)
        return b ^ c ^ d;
}

struct _Sha1Ctx
{
    uint8_t block[64];
    uint32_t h[5];
    uint64_t bytes;
    uint32_t cur;
};

void Sha1Ctx_reset (Sha1Ctx* ctx)
{
    ctx->h[0] = 0x67452301;
    ctx->h[1] = 0xefcdab89;
    ctx->h[2] = 0x98badcfe;
    ctx->h[3] = 0x10325476;
    ctx->h[4] = 0xc3d2e1f0;
    ctx->bytes = 0;
    ctx->cur = 0;
}

Sha1Ctx* Sha1Ctx_create (void)
{
    // TODO custom allocator support
    Sha1Ctx* ctx = (Sha1Ctx*)malloc(sizeof(Sha1Ctx));
    Sha1Ctx_reset(ctx);
    return ctx;
}

void Sha1Ctx_release (Sha1Ctx* ctx)
{
    free(ctx);
}

static void processBlock (Sha1Ctx* ctx)
{
    static const uint32_t k[4] =
    {
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };

    uint32_t w[16];
    uint32_t a = ctx->h[0];
    uint32_t b = ctx->h[1];
    uint32_t c = ctx->h[2];
    uint32_t d = ctx->h[3];
    uint32_t e = ctx->h[4];
    int t;

    for (t = 0; t < 16; t++)
        w[t] = get32(&((uint32_t*)ctx->block)[t]);

    for (t = 0; t < 80; t++)
    {
        int s = t & 0xf;
        uint32_t temp;
        if (t >= 16)
            w[s] = rotl32(w[(s + 13) & 0xf] ^ w[(s + 8) & 0xf] ^ w[(s + 2) & 0xf] ^ w[s], 1);

         temp = rotl32(a, 5) + f(t, b,c,d) + e + w[s] + k[t/20];

         e = d; d = c; c = rotl32(b, 30); b = a; a = temp;
    }

    ctx->h[0] += a;
    ctx->h[1] += b;
    ctx->h[2] += c;
    ctx->h[3] += d;
    ctx->h[4] += e;
}

void Sha1Ctx_write (Sha1Ctx* ctx, const void* msg, uint64_t bytes)
{
    ctx->bytes += bytes;

    const uint8_t* src = msg;
    while (bytes--)
    {
        // TODO: could optimize the first and last few bytes, and then copy
        // 128 bit blocks with SIMD in between
        ctx->block[ctx->cur++] = *src++;
        if (ctx->cur == 64)
        {
            processBlock(ctx);
            ctx->cur = 0;
        }
    }
}

Sha1Digest Sha1Ctx_getDigest (Sha1Ctx* ctx)
{
    // append separator
    ctx->block[ctx->cur++] = 0x80;
    if (ctx->cur > 56)
    {
        // no space in block for the 64-bit message length, flush
        memset(&ctx->block[ctx->cur], 0, 64 - ctx->cur);
        processBlock(ctx);
        ctx->cur = 0;
    }

    memset(&ctx->block[ctx->cur], 0, 56 - ctx->cur);
    uint64_t bits = ctx->bytes * 8;

    // TODO a few instructions could be shaven
    ctx->block[56] = (uint8_t)(bits >> 56 & 0xff);
    ctx->block[57] = (uint8_t)(bits >> 48 & 0xff);
    ctx->block[58] = (uint8_t)(bits >> 40 & 0xff);
    ctx->block[59] = (uint8_t)(bits >> 32 & 0xff);
    ctx->block[60] = (uint8_t)(bits >> 24 & 0xff);
    ctx->block[61] = (uint8_t)(bits >> 16 & 0xff);
    ctx->block[62] = (uint8_t)(bits >> 8  & 0xff);
    ctx->block[63] = (uint8_t)(bits >> 0  & 0xff);
    processBlock(ctx);

    {
        Sha1Digest ret;
        int i;
        for (i = 0; i < 5; i++)
            ret.digest[i] = get32(&ctx->h[i]);
        Sha1Ctx_reset(ctx);
        return ret;
    }
}

Sha1Digest Sha1_get (const void* msg, uint64_t bytes)
{
    Sha1Ctx ctx;
    Sha1Ctx_reset(&ctx);
    Sha1Ctx_write(&ctx, msg, bytes);
    return Sha1Ctx_getDigest(&ctx);
}



void print_hex(char* label, uint8_t* data, size_t len) {
    printf("%s", label);
    for(size_t i = 0; i < len; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");
}


int main()
{
    unsigned char input[29];

    memset(input, 0xaa, sizeof(input));

	Sha1Digest digest = Sha1_get(input, sizeof(input));

#if DEBUG
    print_hex("input ", input, sizeof(input));
    print_hex("digest ", (uint8_t*) digest.digest, 20); 
#endif
}



