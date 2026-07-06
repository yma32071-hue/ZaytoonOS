#include "kernel/shell/commandline.h"
#include "kernel/apps/capp.h"
#include "kernel/drivers/console.h"
#include "kernel/drivers/keyboard.h"
#include "kernel/fs/vfs.h"
#include "kernel/kernel/printk.h"
#include "kernel/kernel/sched.h"
#include "kernel/shell/string.h"

#define SHELL_MAX_ARGS 8
#define SHELL_LINE_MAX 128

static void sh_write(const char *s)
{
    console_write(s);
}

static void sh_writeln(const char *s)
{
    console_write(s);
    console_write("\n");
}

static void sh_prompt(void)
{
    console_write(vfs_cwd());
    console_write("> ");
}

static int tokenize(char *line, const char **argv)
{
    int argc = 0;
    char *p = line;
    while (*p && argc < SHELL_MAX_ARGS) {
        while (*p == ' ' || *p == '\t') *p++ = '\0';
        if (!*p) break;
        argv[argc++] = p;
        while (*p && *p != ' ' && *p != '\t') ++p;
    }
    return argc;
}

static bool has_capp_ext(const char *name)
{
    u32 len = shell_strlen(name);
    return len > 5 && shell_streq_ci(name + len - 5, ".capp");
}

/* ─── built-in commands ─────────────────────────────────────────────────── */

static void cmd_help(void)
{
    sh_writeln("  HELP / ?       show this list");
    sh_writeln("  VER            kernel version");
    sh_writeln("  CLS            clear screen");
    sh_writeln("  DIR / LS       list directory");
    sh_writeln("  CD <path>      change directory");
    sh_writeln("  ECHO <text>    print text");
    sh_writeln("  TYPE <file>    show file contents");
    sh_writeln("  MEM            memory & heap info");
    sh_writeln("  PS             running tasks");
    sh_writeln("  UPTIME         scheduler ticks");
    sh_writeln("  RUN <app.capp> execute a .capp program");
    sh_writeln("  <app.capp> [args] run .capp directly by name with optional args");
    sh_writeln("");
    sh_writeln("EXAMPLES:");
    sh_writeln("  hello.capp");
    sh_writeln("  startup.capp (runs on boot if exists)");
}

static void cmd_cls(void)
{
    /* reuse console_init scroll trick: just blank the screen */
    for (int i = 0; i < 25; ++i) console_write("\n");
}

static void cmd_type(const char *path)
{
    if (!path) {
        sh_writeln("Usage: TYPE <file>");
        return;
    }
    if (shell_streq_ci(path, "README.TXT")) {
        sh_writeln("ZaytoonOS RAM drive  —  built-in executables live in A:/BIN as .capp files.");
        sh_writeln("Type  DIR         to list the root.");
        sh_writeln("Type  CD BIN      then  DIR  to see all programs.");
        sh_writeln("Type  RUN <name>  or just  <name>  to launch a program.");
        return;
    }
    sh_writeln("File not found.");
}

static void exec_line(char *buf, int argc, const char **argv)
{
    if (shell_streq_ci(argv[0], "HELP") || shell_streq_ci(argv[0], "?")) {
        cmd_help();
    } else if (shell_streq_ci(argv[0], "VER")) {
        sh_writeln("ZaytoonOS  kernel v1.0  (x86 32-bit, GRUB2 multiboot2)");
    } else if (shell_streq_ci(argv[0], "CLS") || shell_streq_ci(argv[0], "CLEAR")) {
        cmd_cls();
    } else if (shell_streq_ci(argv[0], "DIR") || shell_streq_ci(argv[0], "LS")) {
        vfs_list_dir(argc > 1 ? argv[1] : vfs_cwd(), sh_writeln);
    } else if (shell_streq_ci(argv[0], "CD")) {
        if (argc < 2) sh_writeln(vfs_cwd());
        else if (!vfs_chdir(argv[1])) sh_writeln("Path not found.");
    } else if (shell_streq_ci(argv[0], "TYPE") || shell_streq_ci(argv[0], "CAT")) {
        cmd_type(argc > 1 ? argv[1] : 0);
    } else if (shell_streq_ci(argv[0], "ECHO")) {
        for (int i = 1; i < argc; ++i) {
            sh_write(argv[i]);
            if (i + 1 < argc) sh_write(" ");
        }
        sh_write("\n");
    } else if (shell_streq_ci(argv[0], "MEM")) {
        sh_writeln("Heap  : bump allocator (1 MiB region)");
        sh_writeln("Paging: flat identity map (32-bit protected mode)");
        sh_writeln("Sched : preemptive round-robin, timer IRQ 0");
    } else if (shell_streq_ci(argv[0], "PS")) {
        sh_writeln("PID  NAME            STATE");
        sh_writeln("  1  shell           running");
        sh_writeln("  2  idle            ready");
    } else if (shell_streq_ci(argv[0], "UPTIME")) {
        char tmp[64];
        shell_format_u64(tmp, sizeof(tmp), "ticks = ", sched_ticks());
        sh_writeln(tmp);
    } else if (shell_streq_ci(argv[0], "RUN") || shell_streq_ci(argv[0], "EXEC")) {
        if (argc < 2) sh_writeln("Usage: RUN <program.capp> [args]");
        else if (capp_exec(argv[1], argc - 1, &argv[1]) != 0)
            sh_writeln("Error: program exited with non-zero status.");
    } else if (has_capp_ext(argv[0])) {
        if (capp_exec(argv[0], argc, argv) != 0)
            sh_writeln("Error: program exited with non-zero status.");
    } else if (shell_starts_with_ci(argv[0], "./") && has_capp_ext(argv[0] + 2)) {
        if (capp_exec(argv[0] + 2, argc, argv) != 0)
            sh_writeln("Error: program exited with non-zero status.");
    } else {
        sh_write("Unknown command: ");
        sh_writeln(argv[0]);
        sh_writeln("Type HELP for a list of commands.");
    }
    (void)buf;
}

/* ─── interactive input loop ────────────────────────────────────────────── */

void shell_init(void)
{
    keyboard_init();
}

void shell_run(void)
{
    char line[SHELL_LINE_MAX];
    const char *argv[SHELL_MAX_ARGS];
    u32 pos = 0;

    console_write("\n");
    sh_writeln("ZaytoonOS Command Processor v1.0");
    sh_writeln("Type HELP for available commands.");
    console_write("\n");

    /* Try to run startup.capp if it exists */
    if (capp_exists("startup.capp")) {
        sh_writeln("Executing startup.capp...");
        const char *startup_argv[] = { "startup.capp" };
        capp_exec("startup.capp", 1, startup_argv);
        console_write("\n");
    }

    sh_prompt();

    for (;;) {
        char c = keyboard_getchar();

        if (c == '\n' || c == '\r') {
            line[pos] = '\0';
            console_write("\n");
            if (pos > 0) {
                char buf[SHELL_LINE_MAX];
                shell_copy(buf, sizeof(buf), line);
                int argc = tokenize(buf, argv);
                if (argc > 0) exec_line(buf, argc, argv);
            }
            pos = 0;
            sh_prompt();

        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                console_write_char('\b');
            }

        } else if ((unsigned char)c >= 32 && pos < SHELL_LINE_MAX - 1) {
            line[pos++] = c;
            console_write_char(c);
        }
    }
}
