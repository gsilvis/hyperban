const path = require('path');

module.exports = {
  entry: './web/glue.js',
  output: {
    filename: 'renderer.js',
    path: path.resolve(__dirname, 'dist'),
  },
  resolve: {
    modules: ["./"]
  }
};
