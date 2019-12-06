#include <kcc/ext.h>

aes_t *aes_init(const uint8_t *key)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(key);
    aes_t *ctx = __kcc_builtin_call_p(h, "aes_init");
    return ctx;
}

aes_t *aes_init_iv(const uint8_t *key, const uint8_t *iv)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(key);
    __kcc_builtin_add_arg_p(iv);
    aes_t *ctx = __kcc_builtin_call_p(h, "aes_init_iv");
    return ctx;
}

void aes_free(aes_t *ctx)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(ctx);
    __kcc_builtin_call(h, "aes_free");
}

void aes_set_iv(aes_t *ctx, const uint8_t *iv)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(ctx);
    __kcc_builtin_add_arg_p(iv);
    __kcc_builtin_call(h, "aes_set_iv");
}

void aes_ecb_encrypt(aes_t *ctx, const uint8_t *buf)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(ctx);
    __kcc_builtin_add_arg_p(buf);
    __kcc_builtin_call(h, "aes_ecb_encrypt");
}

void aes_ecb_decrypt(aes_t *ctx, const uint8_t *buf)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(ctx);
    __kcc_builtin_add_arg_p(buf);
    __kcc_builtin_call(h, "aes_ecb_decrypt");
}

void aes_cbc_encrypt(aes_t *ctx, const uint8_t *buf, int32_t len)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(ctx);
    __kcc_builtin_add_arg_p(buf);
    __kcc_builtin_add_arg_i(len);
    __kcc_builtin_call(h, "aes_cbc_encrypt");
}

void aes_cbc_decrypt(aes_t *ctx, const uint8_t *buf, int32_t len)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(ctx);
    __kcc_builtin_add_arg_p(buf);
    __kcc_builtin_add_arg_i(len);
    __kcc_builtin_call(h, "aes_cbc_decrypt");
}

void aes_ctr_xcrypt(aes_t *ctx, const uint8_t *buf, int32_t len)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(ctx);
    __kcc_builtin_add_arg_p(buf);
    __kcc_builtin_add_arg_i(len);
    __kcc_builtin_call(h, "aes_ctc_xcrypt");
}
