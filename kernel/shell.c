#include "shell.h"

#include "io.h"
#include "kernel_config.h"
#include "stdint.h"
#include "syscall.h"
#include "task.h"

#define SHELL_MAX_LINE 128u
#define SHELL_MAX_ARGS 16u
#define SHELL_BACKSPACE 0x08
#define SHELL_DEL 0x7F
#define SHELL_CR 0x0D
#define SHELL_LF 0x0A

static void Help(void* param);
static void Echo(void* param);
static void Zombie_Task(void* param);
extern void Print_Tasks(void* param);

typedef struct {
  const char* name;
  void (*fn)(void* param);
  const char* help;
} ShellCmd;

static const ShellCmd g_cmds[] = {
    {"help", Help, "show this help"},
    {"echo", Echo, "echo arguments"},
    {"ts", Print_Tasks, "task list"},
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
      Write_Str("\r\n");
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

static void Parse_Prompt(const char* cmd) {
  char line[SHELL_MAX_LINE];
  char* argv[SHELL_MAX_ARGS];
  int argc;
  uint32_t i;

  for (i = 0; i + 1 < sizeof(line) && cmd[i]; i++) {
    line[i] = cmd[i];
  }
  line[i] = '\0';

  argc = tokenize(line, argv, SHELL_MAX_ARGS);
  if (argc == 0) return;

  for (i = 0; i < CMD_COUNT; i++) {
    const char* a = g_cmds[i].name;
    const char* b = argv[0];
    while (*a && *a == *b) {
      a++;
      b++;
    }
    if (*a == '\0' && *b == '\0') {
      uint32_t tid = Create(g_cmds[i].name, g_cmds[i].fn, 0);
      Write_Str("Created task with TID: ");
      Write_Int(tid);
      Write_Char('\n');
      return;
    }
  }

  Write_Str("unknown command: ");
  Write_Str(argv[0]);
  Write_Str("\r\n");
}

void Shell_Start(void* param) {
  (void)param;
  char line[SHELL_MAX_LINE];

  Write_Str("\r\nEnergOS shell. Type 'help' for commands.\r\n");

  for (;;) {
    Send_Prompt();
    Read_Prompt(line, sizeof(line));
    Parse_Prompt(line);
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
    Write_Str("\r\n");
  }
}

static void Echo(void* param) {
  const char* text = param;
  if (text != 0) Write_Str(text);
  Write_Str("\r\n");
}
static void Zombie_Task(void* param) {
  (void)param;
  while (1);
}
