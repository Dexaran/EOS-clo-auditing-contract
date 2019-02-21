#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class [[eosio::contract("example")]] example_contract : public eosio::contract {

public:
  using contract::contract;
  uint64_t autodelay = 60;
  
  example_contract(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

  [[eosio::action]]
  void ask(name _user, uint64_t _CPU, uint64_t _NET, uint64_t _timeframe) {

    multiauth(_user);
    
    delegate_resources( _user, _CPU, _NET, _timeframe );
  }

  [[eosio::action]]
  void cancel(name _user) {

    multiauth(_user);

    undelegate_resources( _user );
  }

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
  t.delay_sec = autodelay; // example - 1 minute delay for the tx

  uint64_t amount = 1;
  asset quantity(amount, symbol("EOS",4));

    t.actions.emplace_back(
      permission_level(name(get_self()), name("active")), 
      name("eosio"),
      name("undelegatebw"), 
      std::tuple(get_self(), _user, quantity, quantity, false));

    t.send(now(), name(get_self())); // Send the transaction with some ID derived from the memo
  };

void delegate_resources(name _user, uint64_t _CPU, uint64_t _NET, uint64_t _timeframe){

  asset CPU_to_delegate(_CPU, symbol("EOS",4));
  asset NET_to_delegate(_NET, symbol("EOS",4));
    action(
      permission_level(name(get_self()), name("active")), 
      name("eosio"),
      name("delegatebw"), 
      std::tuple(get_self(), _user, CPU_to_delegate, NET_to_delegate, false)
    ).send();
  };
};

EOSIO_DISPATCH( example_contract, (ask)(cancel) )
