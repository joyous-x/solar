const
  os = require('os'),
  path = require('path'),
  webpack = require('webpack')


const isProd = process.env.NODE_ENV === 'production'

module.exports = {
  entry: [
    '@iro/wechat-adapter',
    'core-js/modules/es.array.iterator',   
    './src/app.js'
  ],

  output: {
    path: path.resolve('dist/root'),
    filename: 'game.js',
    chunkFilename: '[name].[id].js', // .[chunkhash]
  },

  resolve: {
    alias: {
      '@': path.resolve('.')
    }
  },

  devtool: isProd ? false : 'source-map',

  stats: 'errors-only',

  
  optimization: {
    minimize: true,
    providedExports: true,
    usedExports: true,
    sideEffects: true,
    splitChunks: {
      cacheGroups: {
        // commons: {
        //   test: /[\\/]node_modules[\\/](@pixi)[\\/]/,  // |pixi.js
        //   name: 'vendors',
        //   chunks: 'initial'
        // }
      }
    },
  },

  module: {
    rules: [
      {
        test: /\.js$/,
        use: ['babel-loader'],
        exclude: /node_modules/
      },
      {
        test: /\.(vert|frag)$/,
        use: ['raw-loader']
      }
    ]
  },

  plugins: [
    new webpack.ProvidePlugin({
      PIXI: 'pixi.js-legacy',
    })
  ],

  mode: isProd ? 'production' : 'development'
}