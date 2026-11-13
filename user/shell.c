#include "shell.h"

#include <stdint.h>

#include "io.h"
#include "kernel/task.h"
#include "kernel/typedefs.h"
#include "portable/kernel_config.h"
#include "syscall.h"
#include "util/slab.h"
#include "util/util.h"

#define SHELL_BACKSPACE 0x08
#define SHELL_DEL 0x7F
#define SHELL_CR 0x0D
#define SHELL_LF 0x0A

struct args_for_tasks {
  char line[SHELL_MAX_LINE];
  task_args_t arg;
};

static slab_t shell_slab;
static struct args_for_tasks task_args[NTASKS];

static void Help(void* param);
static void Echo(void* param);
static void Task_List(void* param);
static void Zombie_Task(void* param);

typedef struct {
  const char* name;
  void (*fn)(void* param);
  const char* help;
} ShellCmd;

static const ShellCmd g_cmds[] = {
    {"help", Help, "show this help"},
    {"echo", Echo, "echo arguments"},
    {"ts", Task_List, "task list"},
    {"zombie", Zombie_Task, "create a zombie task"},
};
#define CMD_COUNT (sizeof(g_cmds) / sizeof(g_cmds[0]))

static void Send_Prompt(void) { Write_Str(SHELL_PROMPT); }

static void Read_Prompt(char* buf, uint32_t size) {
  uint32_t len = 0;

  if (size == 0) return;
  buf[0] = '\0';

  for (;;) {
    char c;
    Read_Char(&c);

    if (c == SHELL_CR || c == SHELL_LF) {
      Write_Str("\n");
      buf[len] = '\0';
      return;
    }

    if (c == SHELL_BACKSPACE || c == SHELL_DEL) {
      if (len > 0) {
        len--;
        Write_Str("\b \b");
      }
      continue;
    }

    if (c >= 0x20 && c < 0x7F && len + 1 < size) {
      buf[len++] = c;
      Write_Char(c);
    }
  }
}

static int tokenize(char* line, char** argv, int max_args) {
  int argc = 0;
  char* p = line;
  int in_token = 0;

  while (*p) {
    if (*p == ' ' || *p == '\t') {
      *p = '\0';
      in_token = 0;
    } else if (!in_token) {
      if (argc >= max_args) break;
      argv[argc++] = p;
      in_token = 1;
    }
    p++;
  }
  return argc;
}

static void Parse_Prompt(struct args_for_tasks* p) {
  p->arg.argc = tokenize(p->line, p->arg.argv, SHELL_MAX_ARGS);
  if (p->arg.argc == 0) return;

  for (uint32_t i = 0; i < CMD_COUNT; i++) {
    const char* a = g_cmds[i].name;
    const char* b = p->arg.argv[0];
    if (Util_Strcmp(a, b) == 0) {
      uint32_t tid = Create(g_cmds[i].name, g_cmds[i].fn, &p->arg);
      Write_Str("Created task with TID: ");
      Write_Int(tid);
      Write_Char('\n');
      return;
    }
  }

  Write_Str("unknown command: ");
  Write_Str(p->arg.argv[0]);
  Write_Str("\n");
}

void Shell_Start(void* param) {
  (void)param;
  Slab_Init(&shell_slab, task_args, sizeof(task_args), sizeof(task_args[0]));
  Write_Str("\nEnergOS shell. Type 'help' for commands.\n");

  for (;;) {
    struct args_for_tasks* p = Slab_Alloc(&shell_slab);
    if (p == 0) {
      Write_Str("Shell: Not enough memory to allocate another task.\n");
      // Task_Sleep();
      continue;
    }
    Send_Prompt();
    Read_Prompt(p->line, sizeof(p->line));
    Parse_Prompt(p);
  }
}

static void Help(void* param) {
  uint32_t i;
  (void)param;
  for (i = 0; i < CMD_COUNT; i++) {
    Write_Str("  ");
    Write_Str(g_cmds[i].name);
    Write_Str(" - ");
    Write_Str(g_cmds[i].help);
    Write_Str("\n");
  }
}

static void Echo(void* param) {
  task_args_t* text = param;
  for (int i = 1; i < text->argc; i++) Write_Str(text->argv[i]);
  Write_Str("\n");
}

static void Task_List(void* param) {
  (void)param;
  Print_Task_List();
}

static void Zombie_Task(void* param) {
  (void)param;
  while (1);
}
