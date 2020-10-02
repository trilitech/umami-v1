# Portofoli

## User stories

 * As a user, I create aliases. E.g bob or alice. These accounts will each have generated keys.  
 * As a user, I want to be able to switch from mainnet to test network and inversely.
 * As a user, I want to use the faucet on test network to get tez on my accounts.
 * As a user, I can send tez to other addresses.
 * As a user, I can view the balance for all my accounts.
 * As a user, I can deploy contracts and name it.
 * As a user, I can list contracts that have been created with my aliases.
 * As a user, I have several commands to manage a multisig smart contract:
    * I can deploy it with several parameters (e.g. tez quantity, threshold, signatories, delegate, etc)
    * I can prepare a transfer from this contract to another destination
    * I can prepare a change of delegate (i.e. adding or withdrawing one)
    * I can prepare a change of threshold and signatories
    * I can sign for a validation of transfer using the secret key of an alias
    * I can sign for a validation of setting/withdrawing delegate using the secret key of an alias
    * I can sign for a validation of setting threshold and public keys
    * I can transfer from the contract using the necessary signatures
    * I can set/withdraw delegate of the contract with the necessary signatures
 * As a user, I am able to interact with the voting system:
    * I can show the voting period
    * I can show the different proposals
 * As a user, I am able to ask the node about any command-line client I want. For example, about a specific block:
    * Access the timestamp
    * List contracts
    * Get the storage of a specific contract
    * Get the delegate of a specific contract
    * Get entry points for a specific contract
    * Typecheck a specific script
    * Run a specific script
