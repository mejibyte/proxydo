// Definition of the Socket class

#ifndef Socket_class
#define Socket_class


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>


const int MAXHOSTNAME = 256;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 1024 * 8; //8kb

class Socket
{
 public:
  Socket();
  virtual ~Socket();

  // Server initialization
  bool create();
  bool bind ( const int port );
  bool listen() const;
  bool accept ( Socket& ) const;

  // Client initialization
  bool connect ( const std::string host, const int port );

  // Data Transimission
  bool send ( const std::string ) const;
  int recv ( std::string& ) const;
  int send (char * buffer, int size) const;
  int recv(char * buffer, int size) const;
  std::string readLine() const;


  void set_non_blocking ( const bool );

  bool is_valid() const { return m_sock != -1; }

  int m_sock;
 private:

  sockaddr_in m_addr;


};


#endif
