#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

class [[eosio::contract]] cardgame : public eosio::contract {

public:
  cardgame(name receiver, name code, datastream<const char *> ds)
      : contract(receiver, code, ds),
        _users(receiver, receiver.value) {}

  [[eosio::action]]
  void login(name username);

private:
  struct [[eosio::table]] user_info {
    name username;
    uint16_t win_count = 0;
    uint16_t lost_count = 0;

    auto primary_key() const { return username.value; }
  };

  typedef eosio::multi_index<name("users"), user_info> users_table;

  users_table _users;
};
