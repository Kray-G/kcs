#include <kcc/dll.h>
#include <kcc/dllcore.h>
#include "../lib/aes/aes.h"

/* ---------------------------------------------------------------------------------------------
    AES encryption/decryption
--------------------------------------------------------------------------------------------- */

DLLEXPORT void* aes_init(int argc, arg_type_t* argv)
{
    if (argc != 1 || !(argv[0].type ==  C_PTR || argv[0].type ==  C_STR)) {
        return NULL;
    }

    const uint8_t *key = argv[0].type == C_PTR ? (uint8_t *)argv[0].value.p : (uint8_t *)argv[0].value.s;
    struct AES_ctx *ctx = (struct AES_ctx *)calloc(1, sizeof(struct AES_ctx));
    if (!ctx || !key) {
        free(ctx);
        return NULL;
    }
    AES_init_ctx(ctx, key);
    return (void*)ctx;
}

DLLEXPORT void* aes_init_iv(int argc, arg_type_t* argv)
{
    if (argc != 2 || !(argv[0].type ==  C_PTR || argv[0].type ==  C_STR) || !(argv[1].type ==  C_PTR || argv[1].type ==  C_STR)) {
        return NULL;
    }

    const uint8_t *key = argv[0].type == C_PTR ? (uint8_t *)argv[0].value.p : (uint8_t *)argv[0].value.s;
    const uint8_t *iv = argv[1].type == C_PTR ? (uint8_t *)argv[1].value.p : (uint8_t *)argv[1].value.s;
    struct AES_ctx *ctx = (struct AES_ctx *)calloc(1, sizeof(struct AES_ctx));
    if (!ctx || !key || !iv) {
        free(ctx);
        return NULL;
    }
    AES_init_ctx_iv(ctx, key, iv);
    return (void*)ctx;
}

DLLEXPORT void aes_free(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return;
    }

    struct AES_ctx *ctx = (struct AES_ctx *)argv[0].value.p;
    free(ctx);
}

DLLEXPORT void aes_set_iv(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_PTR || !(argv[1].type ==  C_PTR || argv[1].type ==  C_STR)) {
        return;
    }

    struct AES_ctx *ctx = (struct AES_ctx *)argv[0].value.p;
    const uint8_t *iv = argv[1].type == C_PTR ? (uint8_t *)argv[1].value.p : (uint8_t *)argv[1].value.s;
    if (!ctx || !iv) {
        return;
    }

    AES_ctx_set_iv(ctx, iv);
}

DLLEXPORT void aes_cbc_encrypt(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || !(argv[1].type ==  C_PTR || argv[1].type ==  C_STR) || argv[2].type !=  C_INT) {
        return;
    }

    struct AES_ctx *ctx = (struct AES_ctx *)argv[0].value.p;
    uint8_t *buf = argv[1].type == C_PTR ? (uint8_t *)argv[1].value.p : (uint8_t *)argv[1].value.s;
    uint32_t len = (uint32_t)argv[2].value.i;
    if (!ctx || !buf) {
        return;
    }

    AES_CBC_encrypt_buffer(ctx, buf, len);
}

DLLEXPORT void aes_cbc_decrypt(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || !(argv[1].type ==  C_PTR || argv[1].type ==  C_STR) || argv[2].type !=  C_INT) {
        return;
    }

    struct AES_ctx *ctx = (struct AES_ctx *)argv[0].value.p;
    uint8_t *buf = argv[1].type == C_PTR ? (uint8_t *)argv[1].value.p : (uint8_t *)argv[1].value.s;
    uint32_t len = (uint32_t)argv[2].value.i;
    if (!ctx || !buf) {
        return;
    }

    AES_CBC_decrypt_buffer(ctx, buf, len);
}

DLLEXPORT void aes_ctc_xcrypt(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || !(argv[1].type ==  C_PTR || argv[1].type ==  C_STR) || argv[2].type !=  C_INT) {
        return;
    }

    struct AES_ctx *ctx = (struct AES_ctx *)argv[0].value.p;
    uint8_t *buf = argv[1].type == C_PTR ? (uint8_t *)argv[1].value.p : (uint8_t *)argv[1].value.s;
    uint32_t len = (uint32_t)argv[2].value.i;
    if (!ctx || !buf) {
        return;
    }

    AES_CTR_xcrypt_buffer(ctx, buf, len);
}
