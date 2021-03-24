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
function getBalance (endpoint, address, options) {
   const t = new RpcClient(endpoint);
   return t.getBalance(address, options).then(v => v.toNumber());
}
";

module Balance = {
  type params = {block: string};

  [@bs.val]
  external get: (string, string, option(params)) => Js.Promise.t(int) =
    "getBalance";

  let get = (endpoint, ~address, ~options=?, ()) => {
    get(endpoint, address, options)->FutureJs.fromPromise(Js.String.make);
  };
};
