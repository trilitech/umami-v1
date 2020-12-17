const _secp256k1 = require('secp256k1-wasm');
const _uecc = require('@nomadic-labs/uecc-js');
const _hacl = require('hacl-wasm');
const _sdk = require('./TzSDK.js');

var XMLHttpRequest = require("xmlhttprequest").XMLHttpRequest;
global.XMLHttpRequest = XMLHttpRequest;

function init() {
  return new Promise((resolve) => {
    _secp256k1().then(function(SECP256K1) {
      global._SECP256K1 = SECP256K1;
      _uecc().then(function(UECC) {
        global._UECC = UECC;
        _hacl.setMyPrint(() => {return;});
        _hacl.getInitializedHaclModule().then((_hacl) => {
          global._HACL = _hacl;
          // Loads the caml runtime and initialise exports
          _sdk.camlInit(global);
          resolve(_sdk)
        })
      })
    })
  })
}
module.exports.init = init ;
