#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class [[eosio::contract("example")]] example_contract : public eosio::contract {

public:
  using contract::contract;
  //uint32_t standard_delay = 24 * 60 * 60; // 1 day in seconds
  uint32_t standard_delay = 180; // 3 minute in seconds for testing purpose only
  uint64_t request_index = 0;
  
  example_contract(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}


  struct [[eosio::table]] request {
    //uint64_t id;
    name username;
    uint64_t CPU;
    uint64_t NET;
    uint32_t time;

    uint64_t primary_key() const { return username.value; }

    EOSLIB_SERIALIZE( request, (username)(CPU)(NET)(time))
  };

  typedef eosio::multi_index<name("request"), request> requests;

  [[eosio::action]]
  void ask(name _user, uint64_t _CPU, uint64_t _NET) {

    multiauth(_user);

    requests reqest_instance(_code, _code.value);
    auto iterator = reqest_instance.find(_user.value);   
    if( iterator == reqest_instance.end() )
    {
      reqest_instance.emplace(_user, [&]( auto& row ) {
        row.username = _user;
        row.CPU = _CPU;
        row.NET = _NET;
        row.time = now() + standard_delay;
      });
      delegate_resources( _user, _CPU, _NET, now() + standard_delay);

      auto str = std::to_string(now() + standard_delay);
      printmsg(_user, "delegated CPU and NET until " + str);
    }
    else {
      return;
    }

    auto timestamp = now();
  }

  [[eosio::action]]
  void cancel(name _user) {

    multiauth(_user);

    undelegate_resources( _user );
  }

  [[eosio::action]]
  void printmsg(name _user, std::string _msg) {
    require_recipient(_user);

    action(
      permission_level(name(get_self()), name("active")),
      get_self(),
      name("notify"),
      std::make_tuple(_user, _msg)
    ).send();
  }

  [[eosio::action]]
  void eraserecord(name _user){
    require_auth(get_self());
    requests reqest_instance(_code, _code.value);
    auto iterator = reqest_instance.find(_user.value);
    eosio_assert(iterator != reqest_instance.end(), "Record does not exist");
    reqest_instance.erase(iterator);
  };


private:

// Bad practice of multiauth implementation.
void multiauth(name _user)
{
    if (_user == name("dexaraniiznx"))
    {
      require_auth(name("dexaraniiznx"));
    }
    else if (_user == name("walletzzzzzz"))
    {
      require_auth(name("walletzzzzzz"));
    }
    else
    {
      require_auth(name(get_self()));
    }
};

void undelegate_resources(name _user){
  eosio::transaction t{};
  t.delay_sec = standard_delay;

  uint64_t amount = 1;
  asset quantity(amount, symbol("EOS",4));

    t.actions.emplace_back(
      permission_level(name(get_self()), name("active")), 
      name("eosio"),
      name("undelegatebw"), 
      std::tuple(get_self(), _user, quantity, quantity, false));

    t.send(now(), name(get_self()));
  };

void delegate_resources(name _user, uint64_t _CPU, uint64_t _NET, uint64_t _timeframe){

  asset CPU_delegated(_CPU, symbol("EOS",4));
  asset NET_delegated(_NET, symbol("EOS",4));
  action(
    permission_level(name(get_self()), name("active")), 
    name("eosio"),
    name("delegatebw"), 
    std::tuple(get_self(), _user, NET_delegated, CPU_delegated, false)
  ).send();

  eosio::transaction tx{};
  tx.delay_sec = standard_delay;

  tx.actions.emplace_back(
      permission_level(name(get_self()), name("active")), 
      name("eosio"),
      name("undelegatebw"), 
      std::tuple(get_self(), _user, NET_delegated, CPU_delegated, false));

  tx.actions.emplace_back(
      permission_level(name(get_self()), name("active")),
      get_self(),
      name("notify"),
      std::make_tuple(_user, "deferred transaction: automated undelegation of bandwidth in 3 minutes"));

  tx.actions.emplace_back(
      permission_level(name(get_self()), name("active")),
      get_self(),
      name("eraserecord"),
      std::make_tuple(_user));

  tx.send(now(), name(get_self()));
  };
};

EOSIO_DISPATCH( example_contract, (ask)(cancel)(eraserecord) )
