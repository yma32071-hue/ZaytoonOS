#include "kernel/apps/capp.h"
#include "kernel/apps/capp_format.h"
#include "kernel/drivers/console.h"
#include "kernel/kernel/printk.h"
#include "kernel/shell/string.h"
#include "kernel/kernel/sched.h"

static int sysinfo_main(const capp_context_t *ctx);
static int hello_main(const capp_context_t *ctx);
static int ticks_main(const capp_context_t *ctx);

static const capp_image_t native_capps[] = {
    { CAPP_MAGIC, CAPP_ABI_VERSION, sizeof(capp_image_t), "hello.capp", "native sample C application", hello_main },
    { CAPP_MAGIC, CAPP_ABI_VERSION, sizeof(capp_image_t), "sysinfo.capp", "native kernel and runtime information", sysinfo_main },
    { CAPP_MAGIC, CAPP_ABI_VERSION, sizeof(capp_image_t), "ticks.capp", "native scheduler uptime report", ticks_main },
};

static void capp_write(const char *message)
{
    console_write(message);
}

static void capp_writeln(const char *message)
{
    console_write(message);
    console_write("\n");
}

static u64 capp_ticks(void)
{
    return sched_ticks();
}

static const char *basename(const char *path)
{
    const char *name = path;
    if (!path) {
        return NULL;
    }

    for (const char *p = path; *p; ++p) {
        if (*p == '/' || *p == '\\') {
            name = p + 1;
        }
    }
    return name;
}

static bool native_valid(const capp_image_t *image)
{
    return image && image->magic == CAPP_MAGIC && image->abi_version == CAPP_ABI_VERSION &&
           image->header_size == sizeof(capp_image_t) && image->entry;
}

static const capp_image_t *native_find(const char *path)
{
    const char *name = basename(path);
    if (!name || !*name) {
        return NULL;
    }

    for (u32 i = 0; i < (u32)(sizeof(native_capps) / sizeof(native_capps[0])); ++i) {
        if (shell_streq_ci(name, native_capps[i].name)) {
            return &native_capps[i];
        }
    }
    return NULL;
}

static bool blob_header_valid(const capp_blob_t *blob, const capp_file_header_t **header_out)
{
    if (!blob || !blob->data || blob->size < sizeof(capp_file_header_t)) {
        return false;
    }

    const capp_file_header_t *header = (const capp_file_header_t *)blob->data;
    if (header->magic != CAPP_FILE_MAGIC || header->abi_version != CAPP_FILE_ABI_VERSION ||
        header->header_size != sizeof(capp_file_header_t) || header->record_offset < sizeof(capp_file_header_t) ||
        header->record_offset > blob->size) {
        return false;
    }

    *header_out = header;
    return true;
}

static const capp_blob_t *blob_find(const char *path, const capp_file_header_t **header_out)
{
    const char *name = basename(path);
    if (!name || !*name) {
        return NULL;
    }

    for (u32 i = 0; i < generated_capp_count; ++i) {
        const capp_file_header_t *header = NULL;
        if (!blob_header_valid(&generated_capps[i], &header)) {
            continue;
        }
        if (shell_streq_ci(name, header->name) || shell_streq_ci(name, generated_capps[i].filename)) {
            *header_out = header;
            return &generated_capps[i];
        }
    }
    return NULL;
}

static void write_args(const capp_context_t *ctx)
{
    for (int i = 1; i < ctx->argc; ++i) {
        ctx->write(" ");
        ctx->write(ctx->argv[i]);
    }
    ctx->writeln("");
}

static int blob_exec(const capp_blob_t *blob, const capp_file_header_t *header, const capp_context_t *ctx)
{
    u32 offset = header->record_offset;
    int exit_code = 0;

    for (u32 record_index = 0; record_index < header->record_count; ++record_index) {
        if (offset + sizeof(capp_record_header_t) > blob->size) {
            printk("[capp] truncated record table in %s", header->name);
            return -1;
        }

        const capp_record_header_t *record = (const capp_record_header_t *)(blob->data + offset);
        offset += sizeof(capp_record_header_t);
        if (offset + record->length > blob->size) {
            printk("[capp] truncated record payload in %s", header->name);
            return -1;
        }

        const char *payload = (const char *)(blob->data + offset);
        switch (record->opcode) {
        case CAPP_OP_PRINT:
            ctx->write(payload);
            break;
        case CAPP_OP_PRINTLN:
            ctx->writeln(payload);
            break;
        case CAPP_OP_ARGS:
            write_args(ctx);
            break;
        case CAPP_OP_TICKS: {
            char buf[64];
            shell_format_u64(buf, sizeof(buf), "ticks=", ctx->uptime_ticks());
            ctx->writeln(buf);
        } break;
        case CAPP_OP_EXIT:
            if (record->length >= sizeof(int)) {
                exit_code = *(const int *)payload;
            }
            return exit_code;
        default:
            printk("[capp] unsupported opcode %u in %s", record->opcode, header->name);
            return -1;
        }

        offset += record->length;
    }

    return exit_code;
}

void capp_init(void)
{
    printk("[capp] registered %u application packages (%u packaged, %u native)", capp_count(), generated_capp_count,
           (u32)(sizeof(native_capps) / sizeof(native_capps[0])));
}

u32 capp_count(void)
{
    return generated_capp_count + (u32)(sizeof(native_capps) / sizeof(native_capps[0]));
}

bool capp_exists(const char *path)
{
    const capp_file_header_t *header = NULL;
    return blob_find(path, &header) != NULL || native_find(path) != NULL;
}

void capp_list(void (*writer)(const char *line))
{
    for (u32 i = 0; i < generated_capp_count; ++i) {
        const capp_file_header_t *header = NULL;
        if (blob_header_valid(&generated_capps[i], &header)) {
            writer(header->name);
        }
    }

    for (u32 i = 0; i < (u32)(sizeof(native_capps) / sizeof(native_capps[0])); ++i) {
        writer(native_capps[i].name);
    }
}

int capp_exec(const char *path, int argc, const char *const *argv)
{
    capp_context_t ctx = {
        .argc = argc,
        .argv = argv,
        .write = capp_write,
        .writeln = capp_writeln,
        .uptime_ticks = capp_ticks,
    };

    const capp_file_header_t *header = NULL;
    const capp_blob_t *blob = blob_find(path, &header);
    if (blob) {
        printk("[capp] exec packaged %s", header->name);
        return blob_exec(blob, header, &ctx);
    }

    const capp_image_t *image = native_find(path);
    if (!native_valid(image)) {
        printk("[capp] invalid or missing package: %s", path ? path : "<null>");
        return -1;
    }

    printk("[capp] exec native %s", image->name);
    return image->entry(&ctx);
}

static int hello_main(const capp_context_t *ctx)
{
    ctx->writeln("Hello from a native .capp executable written in C.");
    if (ctx->argc > 1) {
        ctx->write("Arguments:");
        for (int i = 1; i < ctx->argc; ++i) {
            ctx->write(" ");
            ctx->write(ctx->argv[i]);
        }
        ctx->writeln("");
    }
    return 0;
}

static int sysinfo_main(const capp_context_t *ctx)
{
    char buf[96];
    ctx->writeln("ZaytoonOS CAPP ABI v1");
    shell_format_u64(buf, sizeof(buf), "uptime ticks: ", ctx->uptime_ticks());
    ctx->writeln(buf);
    shell_format_u64(buf, sizeof(buf), "registered capps: ", capp_count());
    ctx->writeln(buf);
    return 0;
}

static int ticks_main(const capp_context_t *ctx)
{
    char buf[64];
    shell_format_u64(buf, sizeof(buf), "ticks=", ctx->uptime_ticks());
    ctx->writeln(buf);
    return 0;
}
