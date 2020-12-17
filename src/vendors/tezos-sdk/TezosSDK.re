%raw
"var SDK;";

let sdk = [%raw "SDK"];

%raw
"
const _secp256k1 = require('@nomadic-labs/secp256k1-wasm');
const _hacl = require('@nomadic-labs/hacl-js');
const _uecc = require('@nomadic-labs/uecc-js');
const _sdk = require('./TzSDK.js');

console.log('just required');

function init() {
  return new Promise((resolve) => {
    _secp256k1().then(function(SECP256K1) {
      global._SECP256K1 = SECP256K1;
      _uecc().then(function(UECC) {
        global._UECC = UECC;
        _hacl().then((HACL) => {
          global._HACL = HACL;
          _sdk.camlInit(global);
          console.log('loaded all wasm');
          sdk = _sdk;
        })
      })
    })
  })
}

init();
";

let list_known_addresses = () => {
  "toto";
};

let print_addresses = () => Js.log(list_known_addresses());
