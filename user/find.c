#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

char *basename(char *path)
{
  char *p = path + strlen(path);

  while (p > path && *(p - 1) != '/')
      p--;

  return p;
}

void run_command(char* path, char** command_args)
{
  char* argv_child[MAXARG];
  int i;

  for (i = 0; command_args[i] != 0; i++)
  {
    if (i >= MAXARG - 1)
    {
        fprintf(2, "find: too many arguments\n");
        return;
    }
    
    argv_child[i] = command_args[i];
  }
  
  argv_child[i] = path;
  argv_child[i + 1] = 0;

  int pid = fork();

  if (pid < 0)    
  {
    fprintf(2, "find: fork failed\n");
    return;
  }

  if (pid == 0)
  {
    exec(argv_child[0], argv_child);
    fprintf(2, "find: exec\n");
    exit(1);
  }
  
  wait(0);
}

void handle_match(char* path, int exec_mode, char** command_args)
{
  if (exec_mode)
  {
    run_command(path, command_args);
  }
  else
  {
    printf("%s\n", path);
  }
}

void find(char* path, char* name, int exec_mode, char** command_args) // exec_mode = 0: chỉ in path, exec_mode = 1: chạy command
{
  char buf[MAXPATH], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if (stat(path, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    return;
  }

  if (st.type == T_DIR && strcmp(basename(path), name) == 0)
  {
    handle_match(path, exec_mode, command_args);
  }

  if (st.type == T_FILE || st.type == T_DEVICE)
  {
    if (strcmp(basename(path), name) == 0)
    {
      handle_match(path, exec_mode, command_args);
    }

    return;
  }
  
  if (st.type != T_DIR)
  {
    return;
  }
  
  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
  {
    fprintf(2, "find: path too long\n");
    return;
  }
  
  if ((fd = open(path, O_RDONLY)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(path);
  *p++ = '/';

  while (read(fd, &de, sizeof(de)) == sizeof(de))
  {
    if (de.inum == 0)
    {
      continue;
    }
    
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = '\0';

    if (strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
    {
      continue;
    }
    
    find(buf, name, exec_mode, command_args);
  }
  
  close(fd);
}

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    fprintf(2, "usage: find path name [-exec command ...]\n");
    exit(1);
  }
  
  int exec_mode = 0;
  char** command_args = 0;

  if (argc > 3)
  {
    if (strcmp(argv[3], "-exec") != 0 || argc < 5)
    {
      fprintf(2, "usage: find path name [-exec, command ...]\n");
      exit(1);
    }
    
    exec_mode = 1;
    command_args = &argv[4];
  }
  
  find(argv[1], argv[2], exec_mode, command_args);
  exit(0);
}
