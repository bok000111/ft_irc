#include "../includes/Channel.hpp"
// #include "Channel.hpp"

Channel::Channel(std::string name, std::string key, bool scope, Client *host)
      : _name(name), _key(key), _max_clients(1) {
  add_client(host);
  add_operator(host);
  if (scope == false) //public
    _mode[SCOPE] = false;
  else //private
    _mode[SCOPE] = true;
  _mode[INVITE_ONLY] = false;
}

Channel::~Channel() {}

std::string Channel::get_name() const { return _name; }

std::string Channel::get_key() const { return _key; }

std::string Channel::get_topic() const { return _topic; }

int Channel::get_max_clients() const { return _max_clients; }

void Channel::set_key(const std::string &key) { _key = key; }

void Channel::set_max_clients(int max_clients) { _max_clients = max_clients; }

void Channel::set_topic(const std::string &topic) { _topic = topic; }

void Channel::add_client(Client *client) {
  if (_clients.size() >= _max_clients) {
    std::cout << "Channel is full" << std::endl;
    return;
  }
  _clients.insert(client);
}

void Channel::remove_client(Client *client) {
  std::set<Client *>::iterator it = _clients.find(client); 
  if (it != _clients.end()) {
    _clients.erase(it);
  }
  else {
    std::cout << "Client not in channel" << std::endl;
  }
}

bool Channel::is_client_in_channel(Client *client) const {
  return _clients.find(client) != _clients.end();
}

void Channel::add_operator(Client *client) {
  if (is_client_in_channel(client) == false) {
    std::cout << "Client not in channel" << std::endl;
    return;
  }
  if (is_operator(client) == true) {
    std::cout << "Client already operator" << std::endl;
    return;
  }
  _operators.insert(client);
}

void Channel::remove_operator(Client *client) {
  std::set<Client *>::iterator it = _operators.find(client);
  if (it != _operators.end()) {
    _operators.erase(it);
  }
  else {
    std::cout << "Client not operator" << std::endl;
  }
}

bool Channel::is_operator(Client *client) const {
  return _operators.find(client) != _operators.end();
}

void Channel::invite_client(Client *client) {
  if (is_invited(client) == true) {
    std::cout << "Client already invited" << std::endl;
    return;
  }
  if (_clients.find(client) == _invited.end()) {
    _invited.insert(client);
  }
}

bool Channel::is_invited(Client *client) const {
  return _invited.find(client) != _invited.end();
}

void Channel::set_channel_mode(bool scope, bool invite_only) {
  _mode[SCOPE] = scope;
  _mode[INVITE_ONLY] = invite_only;
}
