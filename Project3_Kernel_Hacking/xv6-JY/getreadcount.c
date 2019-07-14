#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  if (argc < 2){
    printf(2,"Error:\nThis system call requires one argument.\nThis argument should either be:\n0,    if you want to keep the current system call counter.\n1-22, to reset the counter to a new system call number.\n23,   if you want to reset the counter.\n\n");
    exit(); }
  getreadcount(atoi(argv[1]));
  exit();
}
