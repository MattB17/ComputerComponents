// Some high-level architecture specific functions
#include "architecture.h"

void memWrite(uint16_t address, uint16_t addressVal)
{
  mem[address] = addressVal;
}

uint16_t memRead(uint16_t address)
{
  if (address == MR_KBSR)
  {
    if (checkKey())
    {
      // If a key is pressed we update the KBSR register to show its pressed
      // and read the character into KBDR.
      mem[MR_KBSR] = (1 << 15);
      mem[MR_KBDR] = getchar();
    }
    else
    {
      // Otherwise the keyboard is not pressed
      mem[MR_KBSR] = 0;
    }
  }
  // Get the value at the specified address
  return mem[address];
}

void disableInputBuffering()
{
  tcgetattr(STDIN_FILENO, &originalTio);
  struct termios newTio = originalTio;
  newTio.c_lflag &= ~ICANON & ~ECHO
  tcsetattr(STDIN_FILENO, TCSANOW, &newTio);
}

void restoreInputBuffering()
{
  tcsetattr(STDIN_FILENO, TCSANOW, &newTio);
}

uint16_t checkKey()
{
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  return select(1, &readfds, NULL, NULL, &timeout) != 0;
}
