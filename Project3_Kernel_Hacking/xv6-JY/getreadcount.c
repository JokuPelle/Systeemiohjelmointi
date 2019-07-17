#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  if (argc < 2){
    getreadcount(23);
    exit();
    }
  getreadcount(atoi(argv[1]));
  exit();
}
