const bs58check = require('bs58check');

function b58cencode(prefix: Uint8Array, value: Uint8Array) {
    const prefixedValue = new Uint8Array(prefix.length + value.length);
    prefixedValue.set(prefix);
    prefixedValue.set(value, prefix.length);
    return bs58check.encode(Buffer.from(prefixedValue.buffer));
}

const sodium = require('libsodium-wrappers');

export function edsk(seed: Uint8Array): Promise<string> {
    return sodium.ready.then(() => {
        const keyPair = sodium.crypto_sign_seed_keypair(seed.slice(0, 32));
        return b58cencode(new Uint8Array([43, 246, 78, 7]), keyPair.privateKey);
    });
}