const webpack = require('webpack')
const path = require('path')
const HtmlWebpackPlugin = require('html-webpack-plugin')
const CopyPlugin = require('copy-webpack-plugin')

// Any directories you will be adding code/files into, need to be added to this array so webpack will pick them up
const defaultInclude = path.resolve(__dirname, 'src')

const SpeedMeasurePlugin = require("speed-measure-webpack-plugin");
const smp = new SpeedMeasurePlugin();

module.exports = smp.wrap({
  module: {
    rules: [
      {
        test: /\.jsx?$/,
        use: [{ loader: 'babel-loader' }],
        include: defaultInclude,
        exclude: path.resolve(__dirname, 'src', 'vendors', 'tezos-sdk')
      },
    ]
  },
  experiments: {
    syncWebAssembly: true,
    asyncWebAssembly: true,
  },
  target: 'electron-renderer',
  plugins: [

    new CopyPlugin({
      patterns: [
        { from: 'public', to: '.' },
      ],
    }),
    new HtmlWebpackPlugin({
      template: 'src/index.html',
    }),
    new webpack.DefinePlugin({
      'process.env.NODE_ENV': JSON.stringify('production')
    }),
  ],
  stats: {
    colors: true,
    children: false,
    chunks: false,
    modules: false
  },
  optimization: {
    minimize: true
  },
  devtool: "eval",
  resolve: {
    // This will only alias the exact import "react-native"
    alias: {
      'react-native$': 'react-native-web'
    },
    // If you're working on a multi-platform React Native app, web-specific
    // module implementations should be written in files using the extension
    // `.web.js`.
    extensions: ['.web.js', '.js', '.bs.js']
  },
  output: {
    path: path.resolve(__dirname, 'build'),
  },
  externals: {
    '@ledgerhq/hw-transport-node-hid-singleton': 'commonjs @ledgerhq/hw-transport-node-hid-singleton'
  }
});
