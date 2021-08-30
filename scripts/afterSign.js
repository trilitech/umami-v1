const notarizing = require("./notarize.js");
const snap = require("./snap.js");
const isCI = require('is-ci');

exports.default = async function afterSign(context) {
    await notarizing.default(context);
}
