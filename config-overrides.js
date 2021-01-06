var webpack = require('webpack');

module.exports = {
  webpack: function (config, env) {
    return {
      ...config,
      "externals": {
        "child_process": "require('child_process')",
        "fs": "require('fs')",
        "path": "require('path')",
      },
      "target": "electron-renderer",

    };
  },
  plugins: [
    new webpack.IgnorePlugin(/^\.\/(?!english)/, /bip39\/src\/wordlists$/),
  ]
}
