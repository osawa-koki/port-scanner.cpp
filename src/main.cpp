#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <omp.h>

const char* HOSTNAME = "example.com";
const unsigned short PORT_START = 50;
const unsigned short PORT_END = 100;

int main()
{
  // ホスト名からIPアドレスを取得する
  addrinfo hints;
  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  addrinfo* res;
  int error = getaddrinfo(HOSTNAME, nullptr, &hints, &res);
  if (error) {
    std::cerr << "Error: " << gai_strerror(error) << std::endl;
    return 1;
  }

  // IPアドレスを表示する
  char ip_str[INET_ADDRSTRLEN];
  for (addrinfo* p = res; p; p = p->ai_next) {
    sockaddr_in* sockaddr = reinterpret_cast<sockaddr_in*>(p->ai_addr);
    inet_ntop(AF_INET, &sockaddr->sin_addr, ip_str, sizeof(ip_str));
    std::cout << "IP Address: " << ip_str << std::endl;
  }

  // 各ポートに対して接続を試みる
  #pragma omp parallel for
  for (unsigned short port = PORT_START; port <= PORT_END; ++port) {
    std::cout << "Port " << port << ": Scanning..." << std::endl;
    // ソケットを作成する
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
      std::cerr << "Error: Failed to create socket" << std::endl;
      continue;
    }

    // タイムアウトの設定
    timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
      std::cerr << "Error: Failed to set timeout" << std::endl;
      continue;
    }

    // ソケットを接続する
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = reinterpret_cast<sockaddr_in*>(res->ai_addr)->sin_addr;
    if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0) {
      // 接続成功
      std::cout << "Port " << port << ": Open" << std::endl;
    }

    // ソケットを閉じる
    close(sock);
  }

  // 後処理
  freeaddrinfo(res);

  return 0;
}
