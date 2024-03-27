#include "Client.hpp"

Client::Client(int server_fd)
    : _stream(*new SocketStream(server_fd)), _status(0)
{
  std::cout << "[Client] " << _stream.get_fd() << " connected" << std::endl;
}
Client::~Client()
{
  std::cout << "[Client] " << _stream.get_fd() << " disconnected" << std::endl;
  delete &_stream;
}

void Client::set_userinfo(const std::string &username,
                          const std::string &hostname,
                          const std::string &servername,
                          const std::string &realname)
{
  _username = username;
  _hostname = hostname;
  _servername = servername;
  _realname = realname;
}

std::string Client::read_buffer()
{
  std::string msg;
  _stream >> msg;

  return msg;
}
void Client::recv() { _stream.recv(); }
void Client::send() { _stream.send(); }

Message Client::get_msg()
{
  std::string msg;
  _stream >> msg;
  return Message(msg);
}
void Client::start_negotiation()
{
  // 이미 등록된 클라이언트거나, CAP 협상이 완료되었거나, 협상 중인 경우
  // TODO: 여기서 판단할지, 외부에서 판단할지 결정 필요
  // 외부에서 판단하고 이 함수 호출 대신 매크로 직접 사용하는게 좋을 듯
  if (IS_CAP_NEGOTIATED(*this) || IS_REGISTERED(*this) ||
      IS_IN_NEGOTIATION(*this))
    return;
  SET_IN_NEGOTIATION(*this);
}
void Client::finish_negotiation()
{
  // 이미 등록된 클라이언트거나, 협상 중이 아닌 경우
  // TODO: 여기서 판단할지, 외부에서 판단할지 결정 필요
  if (IS_REGISTERED(*this) || (!IS_IN_NEGOTIATION(*this)))
    return;
  UNSET_IN_NEGOTIATION(*this);
  SET_CAP_NEGOTIATED(*this);
  register_client();
}
void Client::confirm_password()
{
  SET_PASS_CONFIRMED(*this);
  register_client();
}
void Client::set_user(const std::string &username,
                      __unused const std::string &hostname,
                      __unused const std::string &servername,
                      const std::string &realname)
{
  if ((!IS_PASS_CONFIRMED(*this)))
    return;
  _username = username;
  _realname = realname;
  SET_USER_SET(*this);
  std::cout << "[Client] " << _stream.get_fd() << " set user" << std::endl;
  register_client();
}
void Client::register_client()
{
  if (IS_REGISTERED(*this) || (!IS_PASS_CONFIRMED(*this)) ||
      (!IS_NICK_SET(*this)) || (!IS_USER_SET(*this)) ||
      IS_IN_NEGOTIATION(*this))
  {
    return;
  }
  SET_REGISTERED(*this);

  _stream << RPL_WELCOME(*this);
}

unsigned int Client::get_status() const { return _status; }
bool Client::is_registered() const { return IS_REGISTERED(*this); }
const std::string &Client::get_nickname() const { return _nickname; }
const std::string &Client::get_username() const { return _username; }
const std::string &Client::get_realname() const { return _realname; }
const std::string &Client::get_hostname() const { return _hostname; }
int Client::get_fd() const { return _stream.get_fd(); }

void Client::set_nickname(const std::string &nickname)
{
  SET_NICK_SET(*this);
  _nickname = nickname;
  register_client();
}
void Client::set_username(const std::string &username)
{
  SET_USER_SET(*this);
  _username = username;
  register_client();
}
void Client::set_realname(const std::string &realname)
{
  _realname = realname;
  register_client();
}

bool Client::operator==(const Client &other)
{
  return _stream.get_fd() == other._stream.get_fd();
}
bool Client::operator!=(const Client &other)
{
  return _stream.get_fd() != other._stream.get_fd();
}
bool Client::operator<(const Client &other)
{
  return _stream.get_fd() < other._stream.get_fd();
}
bool Client::operator>(const Client &other)
{
  return _stream.get_fd() > other._stream.get_fd();
}
bool Client::operator<=(const Client &other)
{
  return _stream.get_fd() <= other._stream.get_fd();
}
bool Client::operator>=(const Client &other)
{
  return _stream.get_fd() >= other._stream.get_fd();
}
Client &Client::operator<<(const std::string &data)
{
  _stream << data;
  return *this;
}
Client &Client::operator>>(std::string &data)
{
  _stream >> data;
  return *this;
}
Client &Client::operator>>(std::vector<Message> &vec)
{
  while (true)
  {
    try
    {
      std::string msg;

      _stream >> msg;
      Message m(msg);
      vec.push_back(m);
    }
    catch (SocketStream::NoNewlineException &e)
    {
      break;
    }
    catch (SocketStream::MessageTooLongException &e)
    {
      // *this << ERR_MSGTOOLONG_STR;
    }
  }

  return *this;
}

Client::Client() // 사용하지 않는 생성자
    : _stream(*new SocketStream(0)), _status(0)
{
  std::cout << "[Client] default constructer called - need to fix" << std::endl;
}

Client::Client(const Client &src) // 사용하지 않는 복사 생성자
    : _nickname(src._nickname),
      _stream(*new SocketStream(0)),
      _status(src._status)
{
  std::cout << "[Client] copy constructer called - need to fix" << std::endl;
}

Client &Client::operator=(const Client &src) // 사용하지 않는 대입 연산자
{
  std::cout << "[Client] assign operater called - need to fix" << std::endl;
  _status = src._status;
  _nickname = src._nickname;
  // _stream = *new SocketStream(0);
  return *this;
}

std::vector<Channel *> &Client::get_channels() { return _channels; }

void Client::join_channel(Channel *channel)
{
  _channels.push_back(channel);
}

void Client::part_channel(Channel *channel)
{
  std::vector<Channel *>::iterator it = _channels.begin();
  while (it != _channels.end())
  {
    if (*it == channel)
    {
      it = _channels.erase(it);
      break;
    }
    it++;
  }
}

// Path: srcs/SocketStream.cpp
