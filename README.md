# Example contract that will be used to test EOS security auditors at Callisto

IMPORTANT: Relevant for EOS version 1.6.x

Compile: `eosio-cpp -o resource_delegating.wasm resource_delegating.cpp --abigen`

Deploy:  `cleos -u https://api.eosdetroit.io:443 set contract <your_account> '/home/user_name/folder_name/example/'`

Invoke:  `cleos -u https://api.eosdetroit.io:443 push action <contract_account> <actoin_name> [<params>] -p <your_account>@active`
