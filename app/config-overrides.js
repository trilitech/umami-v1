var webpack = require('webpack');

module.exports = {
  webpack: function (config, env) {
    return {
      ...config,
      "externals": {
        "child_process": "require('child_process')"
      }
    }
  },
  plugins: [
    new webpack.IgnorePlugin(/^\.\/(?!english)/, /bip39\/src\/wordlists$/),
  ]
}
