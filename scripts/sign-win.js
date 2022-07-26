// Script used for windows installer signing.
// By default, electron-builder will sign many things found in win-unpacked folder,
// leading to errors while signing vendored .exe for other platforms.
// This script filters out all but 'umami Setup {VERSION}.exe' file.
// See https://github.com/electron-userland/electron-builder/issues/6304

const { doSign } = require('app-builder-lib/out/codeSign/windowsCodeSign')

/**
 * @type {import("electron-builder").CustomWindowsSign} sign
 */
module.exports = async function sign(config, packager) {

    if (!(config.cscInfo && /umami Setup \d+\.\d+\.\d+(-\d+)?\.exe$/.test(config.path))) {
	console.log ('Skip ' + config.path + ' signing.');
	return
    }

    await doSign(config, packager)
}
