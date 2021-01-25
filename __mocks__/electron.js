
module.exports = {
  require: jest.fn(),
  match: jest.fn(),
  app: jest.fn(),
  window: {
    require: require,
  },
  remote: {
    app: {
      getPath: jest.fn(),
    },
  },
  dialog: jest.fn(),
};
