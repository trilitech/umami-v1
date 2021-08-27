(function (global, factory) {
    typeof exports === 'object' && typeof module !== 'undefined' ? factory(exports, require('libsodium-wrappers'), require('@taquito/utils'), require('typedarray-to-buffer'), require('elliptic'), require('pbkdf2'), require('bip39')) :
    typeof define === 'function' && define.amd ? define(['exports', 'libsodium-wrappers', '@taquito/utils', 'typedarray-to-buffer', 'elliptic', 'pbkdf2', 'bip39'], factory) :
    (global = typeof globalThis !== 'undefined' ? globalThis : global || self, factory(global.taquitoSigner = {}, global.sodium, global.utils, global.toBuffer, global.elliptic, global.pbkdf2, global.bip39));
}(this, (function (exports, sodium, utils, toBuffer, elliptic, pbkdf2, bip39) { 'use strict';

    function _interopDefaultLegacy (e) { return e && typeof e === 'object' && 'default' in e ? e : { 'default': e }; }

    var sodium__default = /*#__PURE__*/_interopDefaultLegacy(sodium);
    var toBuffer__default = /*#__PURE__*/_interopDefaultLegacy(toBuffer);
    var elliptic__default = /*#__PURE__*/_interopDefaultLegacy(elliptic);
    var pbkdf2__default = /*#__PURE__*/_interopDefaultLegacy(pbkdf2);

    /*! *****************************************************************************
    Copyright (c) Microsoft Corporation.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
    REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
    AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
    INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
    OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.
    ***************************************************************************** */

    function __awaiter(thisArg, _arguments, P, generator) {
        function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
        return new (P || (P = Promise))(function (resolve, reject) {
            function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
            function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
            function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
            step((generator = generator.apply(thisArg, _arguments || [])).next());
        });
    }

    function __generator(thisArg, body) {
        var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
        return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
        function verb(n) { return function (v) { return step([n, v]); }; }
        function step(op) {
            if (f) throw new TypeError("Generator is already executing.");
            while (_) try {
                if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
                if (y = 0, t) op = [op[0] & 2, t.value];
                switch (op[0]) {
                    case 0: case 1: t = op; break;
                    case 4: _.label++; return { value: op[1], done: false };
                    case 5: _.label++; y = op[1]; op = [0]; continue;
                    case 7: op = _.ops.pop(); _.trys.pop(); continue;
                    default:
                        if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                        if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                        if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                        if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                        if (t[2]) _.ops.pop();
                        _.trys.pop(); continue;
                }
                op = body.call(thisArg, _);
            } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
            if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
        }
    }

    /**
     * @description Provide signing logic for ed25519 curve based key (tz1)
     */
    var Tz1 = /** @class */ (function () {
        /**
         *
         * @param key Encoded private key
         * @param encrypted Is the private key encrypted
         * @param decrypt Decrypt function
         */
        function Tz1(key, encrypted, decrypt) {
            this.key = key;
            var keyPrefix = key.substr(0, encrypted ? 5 : 4);
            if (!utils.isValidPrefix(keyPrefix)) {
                throw new Error('key contains invalid prefix');
            }
            this._key = decrypt(utils.b58cdecode(this.key, utils.prefix[keyPrefix]));
            this._publicKey = this._key.slice(32);
            if (!this._key) {
                throw new Error('Unable to decode key');
            }
            this.isInit = this.init();
        }
        Tz1.prototype.init = function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, publicKey, privateKey;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0: return [4 /*yield*/, sodium__default['default'].ready];
                        case 1:
                            _b.sent();
                            if (this._key.length !== 64) {
                                _a = sodium__default['default'].crypto_sign_seed_keypair(new Uint8Array(this._key), 'uint8array'), publicKey = _a.publicKey, privateKey = _a.privateKey;
                                this._publicKey = publicKey;
                                this._key = privateKey;
                            }
                            return [2 /*return*/, true];
                    }
                });
            });
        };
        /**
         *
         * @param bytes Bytes to sign
         * @param bytesHash Blake2b hash of the bytes to sign
         */
        Tz1.prototype.sign = function (bytes, bytesHash) {
            return __awaiter(this, void 0, void 0, function () {
                var signature, signatureBuffer, sbytes;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, this.isInit];
                        case 1:
                            _a.sent();
                            signature = sodium__default['default'].crypto_sign_detached(new Uint8Array(bytesHash), new Uint8Array(this._key));
                            signatureBuffer = toBuffer__default['default'](signature);
                            sbytes = bytes + utils.buf2hex(signatureBuffer);
                            return [2 /*return*/, {
                                    bytes: bytes,
                                    sig: utils.b58cencode(signature, utils.prefix.sig),
                                    prefixSig: utils.b58cencode(signature, utils.prefix.edsig),
                                    sbytes: sbytes,
                                }];
                    }
                });
            });
        };
        /**
         * @returns Encoded public key
         */
        Tz1.prototype.publicKey = function () {
            return __awaiter(this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, this.isInit];
                        case 1:
                            _a.sent();
                            return [2 /*return*/, utils.b58cencode(this._publicKey, utils.prefix['edpk'])];
                    }
                });
            });
        };
        /**
         * @returns Encoded public key hash
         */
        Tz1.prototype.publicKeyHash = function () {
            return __awaiter(this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, this.isInit];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, sodium__default['default'].ready];
                        case 2:
                            _a.sent();
                            return [2 /*return*/, utils.b58cencode(sodium__default['default'].crypto_generichash(20, new Uint8Array(this._publicKey)), utils.prefix.tz1)];
                    }
                });
            });
        };
        /**
         * @returns Encoded private key
         */
        Tz1.prototype.secretKey = function () {
            return __awaiter(this, void 0, void 0, function () {
                var key, privateKey;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, this.isInit];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, sodium__default['default'].ready];
                        case 2:
                            _a.sent();
                            key = this._key;
                            privateKey = sodium__default['default'].crypto_sign_seed_keypair(new Uint8Array(key).slice(0, 32), 'uint8array').privateKey;
                            key = toBuffer__default['default'](privateKey);
                            return [2 /*return*/, utils.b58cencode(key, utils.prefix["edsk"])];
                    }
                });
            });
        };
        return Tz1;
    }());

    var pref = {
        p256: {
            pk: utils.prefix['p2pk'],
            sk: utils.prefix['p2sk'],
            pkh: utils.prefix.tz3,
            sig: utils.prefix.p2sig,
        },
        secp256k1: {
            pk: utils.prefix['sppk'],
            sk: utils.prefix['spsk'],
            pkh: utils.prefix.tz2,
            sig: utils.prefix.spsig,
        },
    };
    /**
     * @description Provide signing logic for elliptic curve based key (tz2, tz3)
     */
    var ECKey = /** @class */ (function () {
        /**
         *
         * @param curve Curve to use with the key
         * @param key Encoded private key
         * @param encrypted Is the private key encrypted
         * @param decrypt Decrypt function
         */
        function ECKey(curve, key, encrypted, decrypt) {
            this.curve = curve;
            this.key = key;
            var keyPrefix = key.substr(0, encrypted ? 5 : 4);
            if (!utils.isValidPrefix(keyPrefix)) {
                throw new Error('key contains invalid prefix');
            }
            this._key = decrypt(utils.b58cdecode(this.key, utils.prefix[keyPrefix]));
            var keyPair = new elliptic__default['default'].ec(this.curve).keyFromPrivate(this._key);
            var keyPairY = keyPair.getPublic().getY().toArray();
            var parityByte = keyPairY.length < 32 ? keyPairY[keyPairY.length - 1] : keyPairY[31];
            var pref = parityByte % 2 ? 3 : 2;
            var pad = new Array(32).fill(0);
            this._publicKey = toBuffer__default['default'](new Uint8Array([pref].concat(pad.concat(keyPair.getPublic().getX().toArray()).slice(-32))));
        }
        /**
         *
         * @param bytes Bytes to sign
         * @param bytesHash Blake2b hash of the bytes to sign
         */
        ECKey.prototype.sign = function (bytes, bytesHash) {
            return __awaiter(this, void 0, void 0, function () {
                var key, sig, signature, sbytes;
                return __generator(this, function (_a) {
                    key = new elliptic__default['default'].ec(this.curve).keyFromPrivate(this._key);
                    sig = key.sign(bytesHash, { canonical: true });
                    signature = sig.r.toString('hex', 64) + sig.s.toString('hex', 64);
                    sbytes = bytes + signature;
                    return [2 /*return*/, {
                            bytes: bytes,
                            sig: utils.b58cencode(signature, utils.prefix.sig),
                            prefixSig: utils.b58cencode(signature, pref[this.curve].sig),
                            sbytes: sbytes,
                        }];
                });
            });
        };
        /**
         * @returns Encoded public key
         */
        ECKey.prototype.publicKey = function () {
            return __awaiter(this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    return [2 /*return*/, utils.b58cencode(this._publicKey, pref[this.curve].pk)];
                });
            });
        };
        /**
         * @returns Encoded public key hash
         */
        ECKey.prototype.publicKeyHash = function () {
            return __awaiter(this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, sodium__default['default'].ready];
                        case 1:
                            _a.sent();
                            return [2 /*return*/, utils.b58cencode(sodium__default['default'].crypto_generichash(20, new Uint8Array(this._publicKey)), pref[this.curve].pkh)];
                    }
                });
            });
        };
        /**
         * @returns Encoded private key
         */
        ECKey.prototype.secretKey = function () {
            return __awaiter(this, void 0, void 0, function () {
                var key;
                return __generator(this, function (_a) {
                    key = this._key;
                    return [2 /*return*/, utils.b58cencode(key, pref[this.curve].sk)];
                });
            });
        };
        return ECKey;
    }());
    /**
     * @description Tz3 key class using the p256 curve
     */
    var Tz3 = ECKey.bind(null, 'p256');
    /**
     * @description Tz3 key class using the secp256k1 curve
     */
    var Tz2 = ECKey.bind(null, 'secp256k1');

    /**
     *
     * @description Import a key to sign operation with the side-effect of setting the Tezos instance to use the InMemorySigner provider
     *
     * @param toolkit The toolkit instance to attach a signer
     * @param privateKeyOrEmail Key to load in memory
     * @param passphrase If the key is encrypted passphrase to decrypt it
     * @param mnemonic Faucet mnemonic
     * @param secret Faucet secret
     */
    function importKey(toolkit, privateKeyOrEmail, passphrase, mnemonic, secret) {
        return __awaiter(this, void 0, void 0, function () {
            var signer, pkh, op, ex_1, isInvalidActivationError, signer;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!(privateKeyOrEmail && passphrase && mnemonic && secret)) return [3 /*break*/, 8];
                        signer = InMemorySigner.fromFundraiser(privateKeyOrEmail, passphrase, mnemonic);
                        toolkit.setProvider({ signer: signer });
                        return [4 /*yield*/, signer.publicKeyHash()];
                    case 1:
                        pkh = _a.sent();
                        op = void 0;
                        _a.label = 2;
                    case 2:
                        _a.trys.push([2, 4, , 5]);
                        return [4 /*yield*/, toolkit.tz.activate(pkh, secret)];
                    case 3:
                        op = _a.sent();
                        return [3 /*break*/, 5];
                    case 4:
                        ex_1 = _a.sent();
                        isInvalidActivationError = ex_1 && ex_1.body && /Invalid activation/.test(ex_1.body);
                        if (!isInvalidActivationError) {
                            throw ex_1;
                        }
                        return [3 /*break*/, 5];
                    case 5:
                        if (!op) return [3 /*break*/, 7];
                        return [4 /*yield*/, op.confirmation()];
                    case 6:
                        _a.sent();
                        _a.label = 7;
                    case 7: return [3 /*break*/, 10];
                    case 8: return [4 /*yield*/, InMemorySigner.fromSecretKey(privateKeyOrEmail, passphrase)];
                    case 9:
                        signer = _a.sent();
                        toolkit.setProvider({ signer: signer });
                        _a.label = 10;
                    case 10: return [2 /*return*/];
                }
            });
        });
    }

    // IMPORTANT: THIS FILE IS AUTO GENERATED! DO NOT MANUALLY EDIT OR CHECKIN!
    /* tslint:disable */
    var VERSION = {
        "commitHash": "33283ebb64d801dcbb0926ad199d024ad7189861",
        "version": "10.1.0"
    };
    /* tslint:enable */

    /**
     * @description A local implementation of the signer. Will represent a Tezos account and be able to produce signature in its behalf
     *
     * @warn If running in production and dealing with tokens that have real value, it is strongly recommended to use a HSM backed signer so that private key material is not stored in memory or on disk
     *
     * @warn Calling this constructor directly is discouraged as it do not await for sodium library to be loaded.
     *
     * Consider doing:
     *
     * ```const sodium = require('libsodium-wrappers'); await sodium.ready;```
     *
     * The recommended usage is to use InMemorySigner.fromSecretKey('edsk', 'passphrase')
     */
    var InMemorySigner = /** @class */ (function () {
        /**
         *
         * @param key Encoded private key
         * @param passphrase Passphrase to decrypt the private key if it is encrypted
         *
         */
        function InMemorySigner(key, passphrase) {
            var encrypted = key.substring(2, 3) === 'e';
            var decrypt = function (k) { return k; };
            if (encrypted) {
                if (!passphrase) {
                    throw new Error('Encrypted key provided without a passphrase.');
                }
                decrypt = function (constructedKey) {
                    var salt = toBuffer__default['default'](constructedKey.slice(0, 8));
                    var encryptedSk = constructedKey.slice(8);
                    var encryptionKey = pbkdf2__default['default'].pbkdf2Sync(passphrase, salt, 32768, 32, 'sha512');
                    return sodium__default['default'].crypto_secretbox_open_easy(new Uint8Array(encryptedSk), new Uint8Array(24), new Uint8Array(encryptionKey));
                };
            }
            switch (key.substr(0, 4)) {
                case 'edes':
                case 'edsk':
                    this._key = new Tz1(key, encrypted, decrypt);
                    break;
                case 'spsk':
                case 'spes':
                    this._key = new Tz2(key, encrypted, decrypt);
                    break;
                case 'p2sk':
                case 'p2es':
                    this._key = new Tz3(key, encrypted, decrypt);
                    break;
                default:
                    throw new Error('Unsupported key type');
            }
        }
        InMemorySigner.fromFundraiser = function (email, password, mnemonic) {
            var seed = bip39.mnemonicToSeedSync(mnemonic, "" + email + password);
            var key = utils.b58cencode(seed.slice(0, 32), utils.prefix.edsk2);
            return new InMemorySigner(key);
        };
        InMemorySigner.fromSecretKey = function (key, passphrase) {
            return __awaiter(this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, sodium__default['default'].ready];
                        case 1:
                            _a.sent();
                            return [2 /*return*/, new InMemorySigner(key, passphrase)];
                    }
                });
            });
        };
        /**
         *
         * @param bytes Bytes to sign
         * @param watermark Watermark to append to the bytes
         */
        InMemorySigner.prototype.sign = function (bytes, watermark) {
            return __awaiter(this, void 0, void 0, function () {
                var bb, bytesHash;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            bb = utils.hex2buf(bytes);
                            if (typeof watermark !== 'undefined') {
                                bb = utils.mergebuf(watermark, bb);
                            }
                            // Ensure sodium is ready before calling crypto_generichash otherwise the function do not exists
                            return [4 /*yield*/, sodium__default['default'].ready];
                        case 1:
                            // Ensure sodium is ready before calling crypto_generichash otherwise the function do not exists
                            _a.sent();
                            bytesHash = toBuffer__default['default'](sodium__default['default'].crypto_generichash(32, bb));
                            return [2 /*return*/, this._key.sign(bytes, bytesHash)];
                    }
                });
            });
        };
        /**
         * @returns Encoded public key
         */
        InMemorySigner.prototype.publicKey = function () {
            return __awaiter(this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    return [2 /*return*/, this._key.publicKey()];
                });
            });
        };
        /**
         * @returns Encoded public key hash
         */
        InMemorySigner.prototype.publicKeyHash = function () {
            return __awaiter(this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    return [2 /*return*/, this._key.publicKeyHash()];
                });
            });
        };
        /**
         * @returns Encoded private key
         */
        InMemorySigner.prototype.secretKey = function () {
            return __awaiter(this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    return [2 /*return*/, this._key.secretKey()];
                });
            });
        };
        return InMemorySigner;
    }());

    exports.InMemorySigner = InMemorySigner;
    exports.VERSION = VERSION;
    exports.importKey = importKey;

    Object.defineProperty(exports, '__esModule', { value: true });

})));
//# sourceMappingURL=taquito-signer.umd.js.map
