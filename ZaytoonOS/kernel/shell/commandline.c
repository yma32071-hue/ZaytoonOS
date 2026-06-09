#include "kernel/shell/commandline.h"
#include "kernel/apps/capp.h"
#include "kernel/drivers/console.h"
#include "kernel/fs/vfs.h"
#include "kernel/kernel/printk.h"
#include "kernel/kernel/sched.h"
#include "kernel/shell/string.h"

#define SHELL_MAX_ARGS 8
#define SHELL_LINE_MAX 128

static void shell_writeln(const char *line)
{
    console_write(line);
    console_write("\n");
}

static void shell_prompt(void)
{
    console_write(vfs_cwd());
    console_write("> ");
}

static int tokenize(char *line, const char **argv)
{
    int argc = 0;
    char *p = line;
    while (*p && argc < SHELL_MAX_ARGS) {
        while (*p == ' ' || *p == '\t') {
            *p++ = '\0';
        }
        if (!*p) {
            break;
        }
        argv[argc++] = p;
        while (*p && *p != ' ' && *p != '\t') {
            ++p;
        }
    }
    return argc;
}

static bool has_capp_extension(const char *name)
{
    u32 len = shell_strlen(name);
    return len > 5 && shell_streq_ci(name + len - 5, ".capp");
}

static void cmd_help(void)
{
    shell_writeln("Commands: HELP, VER, CLS, DIR/LS, CD, TYPE, ECHO, RUN/EXEC, MEM, PS, UPTIME, EXIT");
    shell_writeln("DOS style works beside Linux style: DIR == ls, CD == cd, RUN app.capp == ./app.capp");
}

static void cmd_type(const char *path)
{
    if (!path || shell_streq_ci(path, "README.TXT")) {
        shell_writeln("ZaytoonOS RAM drive. Built-in C executables live in A:/BIN as .capp files.");
        return;
    }
    shell_writeln("File not found");
}

static void shell_exec_line(const char *input)
{
    char line[SHELL_LINE_MAX];
    const char *argv[SHELL_MAX_ARGS];
    shell_copy(line, sizeof(line), input);

    shell_prompt();
    shell_writeln(input);

    int argc = tokenize(line, argv);
    if (argc == 0) {
        return;
    }

    if (shell_streq_ci(argv[0], "HELP") || shell_streq_ci(argv[0], "MAN")) {
        cmd_help();
    } else if (shell_streq_ci(argv[0], "VER") || shell_streq_ci(argv[0], "UNAME")) {
        shell_writeln("ZaytoonOS advanced kernel preview x86_64");
    } else if (shell_streq_ci(argv[0], "CLS") || shell_streq_ci(argv[0], "CLEAR")) {
        shell_writeln("[screen cleared]");
    } else if (shell_streq_ci(argv[0], "DIR") || shell_streq_ci(argv[0], "LS")) {
        vfs_list_dir(argc > 1 ? argv[1] : vfs_cwd(), shell_writeln);
    } else if (shell_streq_ci(argv[0], "CD")) {
        if (argc < 2) {
            shell_writeln(vfs_cwd());
        } else if (!vfs_chdir(argv[1])) {
            shell_writeln("The system cannot find the path specified.");
        }
    } else if (shell_streq_ci(argv[0], "TYPE") || shell_streq_ci(argv[0], "CAT")) {
        cmd_type(argc > 1 ? argv[1] : 0);
    } else if (shell_streq_ci(argv[0], "ECHO")) {
        for (int i = 1; i < argc; ++i) {
            console_write(argv[i]);
            console_write(i + 1 < argc ? " " : "\n");
        }
        if (argc == 1) {
            shell_writeln("");
        }
    } else if (shell_streq_ci(argv[0], "RUN") || shell_streq_ci(argv[0], "EXEC")) {
        if (argc < 2) {
            shell_writeln("Usage: RUN program.capp [args]");
        } else if (capp_exec(argv[1], argc - 1, &argv[1]) != 0) {
            shell_writeln("Bad command or executable.");
        }
    } else if (shell_streq_ci(argv[0], "MEM")) {
        shell_writeln("Heap: bump allocator online; paging: 2 MiB page mapper online; scheduler: round-robin.");
    } else if (shell_streq_ci(argv[0], "PS")) {
        shell_writeln("PID  NAME       STATE");
        shell_writeln("1    shell      running");
        shell_writeln("2    idle       ready");
    } else if (shell_streq_ci(argv[0], "UPTIME")) {
        char buf[64];
        shell_format_u64(buf, sizeof(buf), "ticks=", sched_ticks());
        shell_writeln(buf);
    } else if (has_capp_extension(argv[0])) {
        if (capp_exec(argv[0], argc, argv) != 0) {
            shell_writeln("Bad command or executable.");
        }
    } else if (shell_starts_with_ci(argv[0], "./") && has_capp_extension(argv[0] + 2)) {
        if (capp_exec(argv[0] + 2, argc, argv) != 0) {
            shell_writeln("Bad command or executable.");
        }
    } else {
        shell_writeln("Bad command or file name. Try HELP.");
    }
}

void shell_init(void)
{
    printk("[shell] DOS/Linux hybrid shell ready");
}

void shell_run_demo(void)
{
    static const char *script[] = {
        "ver",
        "help",
        "dir",
        "cd bin",
        "ls",
        "run commandline.capp from kernel shell",
        "run hello.capp from kernel shell",
        "./sysinfo.capp",
        "ticks.capp",
    };

    shell_writeln("Zaytoon Command Processor [DOS/Linux hybrid]");
    for (u32 i = 0; i < sizeof(script) / sizeof(script[0]); ++i) {
        shell_exec_line(script[i]);
    }
    shell_writeln("Interactive keyboard input is the next driver milestone; demo script complete.");
}
