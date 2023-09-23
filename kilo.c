#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

struct termios orig_termios;

//异常处理
void die(const char *c)
{
  perror(c);
  exit(1);
}
void disableRawMode()
{
  if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&orig_termios) == -1)
  {
    die("tcsetattr");
  }
  
}
void enableRawMode()
{

  //获取终端设置
  if(tcgetattr(STDIN_FILENO, &orig_termios) == -1)
  {
    die("tcgetattr");
  }

  //使得改动保存到系统中
  atexit(disableRawMode);

  struct termios raw = orig_termios;  

  raw.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON);
  raw.c_iflag &= ~(IXON | ICRNL);
  raw.c_cflag &= ~(CS8);
  raw.c_oflag &= ~(OPOST);
  //ECHO: 取消回显
  //ICANON： 使用逐行输入，也就是不用按回车了
  // ISGG：取消ctrl+C等外键的中断影响
  raw.c_lflag &= ~(ECHO | IEXTEN | ICANON | ISIG);


  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  //应用终端设置
  if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw) == -1)
  {
    die("tcsetattr");
  }
}
int main()
{
  enableRawMode();

  while(1){
    char c = ' ';
    if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN )
    {
      die("read");
    }
    if(iscntrl(c))
    {
      printf("%d\r\n",c);
    }
    else
    {
      printf("%d('%c')\r\n",c,c);
    }
    if(c == 'q')
    {
      break;
    }
  }
  return 0;
}

