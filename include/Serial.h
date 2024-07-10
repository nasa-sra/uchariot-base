#pragma once
#include <string>

class SerialConn {

private:
  int handle;
  std::string device_name;
  int baud;

public:

  SerialConn(std::string device_name, int baud);
  ~SerialConn();

  bool Send( unsigned char  * data,int len);
  bool Send(unsigned char value);
  bool Send( std::string value);
  int Receive( unsigned char  * data, int len);
  bool IsOpen(void);
  void Close(void);
  bool Open(std::string device_name, int baud);
  bool NumberByteRcv(int &bytelen);

  std::string ReceiveLine();
};
