const notarizing = require("./notarize.js");
const snap = require("./snap.js");
const isCI = require('is-ci');

exports.default = async function afterSign(context) {
    // GitlabCI cannot build snaps for the moment
    if (!isCI) {
        await snap.default(context);
    }
    await notarizing.default(context);
}
