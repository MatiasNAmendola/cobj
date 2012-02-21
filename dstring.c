#include "dstring.h"

dstring
dstring_new(const char *init)
{
    size_t initlen = (init == NULL) ? 0 : strlen(init);
    return dstring_newl(init, initlen);
}

dstring
dstring_newl(const void *init, size_t initlen)
{
    struct dstringhdr *hdr;
    hdr = xmalloc(sizeof(struct dstringhdr) + initlen + 1);
    hdr->len = initlen;
    hdr->capacity = initlen;
    if (hdr->len) {
        if (init) {
            memcpy(hdr->buf, init, initlen);
        } else {
            memset(hdr->buf, 0, initlen);
        }
    }
    hdr->buf[initlen] = '\0';
    return (char *)hdr->buf;
}

size_t
dstring_len(const dstring s)
{
    struct dstringhdr *hdr;
    hdr = (void *)(s - (sizeof(struct dstringhdr)));
    return hdr->len;
}

void
dstring_free(dstring s)
{
    if (s == NULL)
        return;
    free(s - sizeof(struct dstringhdr));
}

size_t
dstring_avail(const dstring s)
{
    struct dstringhdr *hdr;
    hdr = (void *)(s - (sizeof(struct dstringhdr)));
    return hdr->capacity - hdr->len;
}

static dstring
__dstring_autoincr(dstring s, size_t incrlen)
{
    struct dstringhdr *hdr;
    size_t newlen;

    hdr = (void *)(s - (sizeof(struct dstringhdr)));
    if (hdr->capacity - hdr->len >= incrlen)
        return s;
    newlen = hdr->len + incrlen;
    hdr = xrealloc(hdr, sizeof(struct dstringhdr) + newlen + 1);
    hdr->capacity = newlen;
    return hdr->buf;
}

dstring
dstring_catl(dstring s, char *t, size_t len)
{
    struct dstringhdr *hdr;
    s = __dstring_autoincr(s, len);
    hdr = (void *)(s - (sizeof(struct dstringhdr)));
    memcpy(s + hdr->len, t, len);
    hdr->len = hdr->len + len;
    s[hdr->len] = '\0';
    return s;
}

dstring
dstring_cat(dstring s, char *t)
{
    return dstring_catl(s, t, strlen(t));
}

dstring
dstring_copyl(dstring s, void *t, size_t len)
{
    struct dstringhdr *hdr;
    hdr = (void *)(s - (sizeof(struct dstringhdr)));
    if (hdr->capacity < len) {
        s = __dstring_autoincr(s, len - hdr->len);
        hdr = (void *)(s - (sizeof(struct dstringhdr)));
    }
    memcpy(s, t, len);
    s[len] = '\0';
    hdr->len = len;
    return s;
}

dstring
dstring_copy(dstring s, char *t)
{
    return dstring_copyl(s, t, strlen(t));
}

dstring
dstring_getline(FILE * fp)
{
    int ch;
    struct dstringhdr *hdr;

    dstring s = dstring_newl(NULL, 0);
    if (feof(fp))
        return NULL;

    while ((ch = fgetc(fp)) != EOF) {
        s = __dstring_autoincr(s, 1);
        hdr = (void *)(s - (sizeof(struct dstringhdr)));
        s[hdr->len++] = ch;
        if (ch == '\n')
            break;
    }
    if (ch == EOF && hdr->len == 0) {
        return NULL;
    }
    s[hdr->len] = '\0';
    return s;
}

dstring
dstring_readfd(int fd)
{
    dstring s = dstring_newl(NULL, 0);
    struct dstringhdr *hdr;

    for (;;) {
        ssize_t cnt;
        s = __dstring_autoincr(s, 8192);
        hdr = (void *)(s - (sizeof(struct dstringhdr)));
        cnt = xread(fd, hdr->buf + hdr->len, 8192);
        if (cnt < 0) {
            dstring_free(s);
            return NULL;
        }
        if (!cnt)
            break;
        hdr->len += cnt;
    }
    hdr->buf[hdr->len] = '\0';

    return s;
}

dstring
dstring_readfile(const char *path)
{
    int fd;
    fd = open(path, O_RDONLY);
    if (fd < 0)
        return NULL;
    dstring s;
    s = dstring_readfd(fd);
    close(fd);
    return s;
}
