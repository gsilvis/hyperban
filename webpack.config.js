const path = require('path');

module.exports = {
  entry: ['babel-polyfill', './web/glue.js'],
  output: {
    filename: 'hyperban.js',
    path: path.resolve(__dirname, 'dist'),
  },
module: {
  rules: [
    {
      test: /\.js$/,
      exclude: /(node_modules|bower_components)/,
      use: {
        loader: 'babel-loader',
        options: {
          presets: ['@babel/preset-env'],
          plugins: [require("@babel/plugin-syntax-dynamic-import")],
        }
      }
    }
  ]
},
     stats: {
         colors: true
     },
     devtool: 'source-map'
};
