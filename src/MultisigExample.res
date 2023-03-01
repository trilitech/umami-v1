let creatorAddress = PublicKeyHash.unsafeBuild("tz1UNer1ijeE9ndjzSszRduR3CzX49hoBUB3")
let address2 = PublicKeyHash.unsafeBuild("tz1Te4MXuNYxyyuPqmAQdnKwkD8ZgSF9M7d6")
let contractAddress = PublicKeyHash.unsafeBuild("KT1NYUDvzv85i4558nbgUEpMY8thJG3XDSeB")
let creatorAccount: Account.t = {
  address: creatorAddress,
  name: "hello",
  kind: Encrypted,
}

// 1 - Originate multisige with implicit address
let origination = Multisig.API.origination(
  ~source=creatorAccount, // TODO no need to pass account
  ~ownerAddresses=[creatorAccount.address, address2],
  ~threshold=ReBigNumber.fromInt(3),
)

// 2 - Fetch contracts that concern an implcit address (Only explorer and chain fields are needed)
let network: Network.t = {name: "foo", chain: #Ghostnet, explorer: "explorerHost", endpoint: "bar"}
let contracts = Multisig.API.getAddresses(network, ~addresses=[creatorAddress])

// 3 - Fetch proposals for a given multisig contract address
let proposals = MultisigApiRequest.Base.getPendingOperations(~network, ~address=contractAddress)

// 4 - Fetch multisig contracts concerning a list of addresses
let multisigs = MultisigApiRequest.Base.getMultisigsConcerningAddresses(
  ~network,
  ~addresses=[creatorAddress, address2],
)

// 5 - Approve/Execute multisig
let proposalId = ReBigNumber.fromInt(3)
let approval = ProtocolHelper.Multisig.approve(proposalId, contractAddress)
let execution = ProtocolHelper.Multisig.execute(proposalId, contractAddress)
