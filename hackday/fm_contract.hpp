#include <eosiolib/eosio.hpp>
#include <vector>
#include <tuple>
#include <list>

class [[eosio::contract]] familytree : public eosio::contract {

    public: 
        familytree(eosio::name receiver, eosio::name famname, eosio::datastream<const char*> ds)
            : eosio::contract(receiver, famname, ds),
                _fm(receiver, receiver.value),
                _users(receiver, receiver.value){}

        [[eosio::action]] void upsertfamtree(eosio::name creator,  eosio::name famlyname, std::list<std::tuple<uint64_t, std::list<std::string>, std::list<std::string>>> tree);

        [[eosio::action]] void login(eosio::name username);

    private:
    
    // struct [[eosio::table]] member {
    //     eosio::name username;
    //     std::string birth_date;
    //     std::string death_date;
    // };

    struct [[eosio::table]] fam_tree
    {
      eosio::name fam_name;

      eosio::name creator;

      std::vector<std::tuple<std::uint64_t, std::list<std::string>, std::list<std::string>>> fam;

      auto primary_key() { return fam_name.value; }
    };

    struct [[eosio::table]] user_info
    {
        eosio::name username;

        auto primary_key() const { return username.value; }
    };

    typedef eosio::multi_index<eosio::name("familytree"), fam_tree> fm;
    typedef eosio::multi_index<eosio::name("users"), user_info> users;

    fm _fm;
    users _users;
};