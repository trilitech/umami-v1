
module.exports = {
  require: jest.fn(),
  match: jest.fn(),
  app: jest.fn(),
  window: {
    require: require,
  },
  dialog: jest.fn(),
};
