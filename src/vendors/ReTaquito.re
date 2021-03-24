%raw
"
const { TezosToolkit, WalletOperation, OpKind } = require('@taquito/taquito');
const { RpcClient } = require ('@taquito/rpc');
const { InMemorySigner, importKey } = require('@taquito/signer');
";

let walletOperation = [%raw "WalletOperation"];
let opKind = [%raw "OpKind"];

let rpcClient = [%raw "RpcClient"];
let inMemorySigner = [%raw "InMemorySigner"];

%raw
"
function getBalance (endpoint, address) {
   const t = new TezosToolkit(endpoint);
   return t.tz.getBalance(address).then(v => v.toNumber());
}
";

[@bs.val]
external getBalance: (string, string) => Js.Promise.t(int) = "getBalance";

let getBalance = (endpoint, address) => {
  getBalance(endpoint, address)->FutureJs.fromPromise(Js.String.make);
};
