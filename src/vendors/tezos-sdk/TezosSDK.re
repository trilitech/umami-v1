%raw
"var SDK;";

let sdk = [%raw "SDK"];

/* console.log('LOADING SDK'); */

%raw
"
console.log('require sdk');
const initjs = require('tezos-sdk');
console.log(initjs);

initjs.init().then(function (api) {
  console.log('SDK LOADED');
  console.log(api.list_known_addresses(0));
  sdk = api
})

";

let list_known_addresses = () => {
  Js.log("toto");
};
