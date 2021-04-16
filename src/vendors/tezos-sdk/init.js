const _rustc_bls12_381 = require('@dannywillems/rustc-bls12-381');
const _secp256k1 = require('@nomadic-labs/secp256k1-wasm-browser');
const _uecc = require('@nomadic-labs/uecc-wasm');
const _hacl = require('@nomadic-labs/hacl-wasm');
const _sdk = require('./TzSDK.js');

var XMLHttpRequest = require("@nomadic-labs/xmlhttprequest").XMLHttpRequest;
global.XMLHttpRequest = XMLHttpRequest;


function init() {
  return new Promise((resolve) => {
    _rustc_bls12_381.then(function (BLS12_381) {
      global._RUSTC_BLS12_381 = BLS12_381;
      return _secp256k1();
    }).then(function (SECP256K1) {
      global._SECP256K1 = SECP256K1;
      _uecc().then(function (UECC) {
        global._UECC = UECC;
        _hacl.setMyPrint(() => { return; });
        _hacl.getInitializedHaclModule().then((_hacl) => {
          global._HACL = _hacl;
          // Loads the caml runtime and initialise exports
          _sdk.camlInit(global);
          resolve(_sdk);
        });

      });
    });
  });
}
module.exports.init = init;

