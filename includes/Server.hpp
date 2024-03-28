#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <ctime>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "NumericReply.hpp"
#include "ft_irc.hpp"

class Client;
class Channel;

class Server {
 private:
  const int _port;
  const std::string _password;
  const std::string _created_at;
  int _server_fd;
  fd_set _master_fds;
  fd_set _read_fds;
  struct sockaddr_in _addr;

  std::set<Client *> _clients;
  std::set<Channel *> _channels;

  Server();
  Server(const Server &src);
  Server &operator=(const Server &src);

  // private utils
  std::vector<std::string> split_tokens(const std::string &msg,
                                        const char delim);
  bool check_channel_name(const std::vector<std::string> &channel_tokens) const;
  Channel *get_target_channel(const std::string &channel_name) const;

  void accept_new_client();
  void read_client(Client *client);
  void write_client(Client *client);

  // private command handlers
  void register_client(Client *client);
  bool is_valid_nickname(const std::string &nickname);
  bool is_nick_in_use(const std::string &nickname);
  int 

  // 명령어 처리
  void CAP(Client *client, const std::vector<std::string> &params);
  void PASS(Client *client, const std::vector<std::string> &params);
  void NICK(Client *client, const std::vector<std::string> &params);
  void USER(Client *client, const std::vector<std::string> &params);

  // JOIN 명령어 처리
  void join_channel(Client *client, std::string msg);

  // MODE 명령어 처리: ongoing...
  void MODE(Client *client, const std::vector<std::string> &params);

 public:
  Server(int port, std::string password);
  ~Server();

  void run();
};

#endif
