#include "fm_contract.hpp"

void familytree::upsertfamtree(eosio::name creator,  eosio::name famlyname, std::list<std::tuple<uint64_t, std::list<std::string>, std::list<std::string>>> tree) {

    require_auth(creator);

    auto itr = _fm.find(famlyname.value);

    if(itr == _fm.end()){
        itr = _fm.emplace(creator, [&](auto &r) {
            r.creator = creator;
            r.fam_name = famlyname;
            for (auto part : tree) {
                r.fam.emplace_back(part);
            }
        });

    }

}


EOSIO_DISPATCH(familytree, (login)(upsertfamtree))
