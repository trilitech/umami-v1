const MockDate = require("mockdate");
MockDate.set(100);

jest.mock("react-native-svg", () =>
  require("react-native-svg-mock/build/index")
);

jest.mock("@electron/remote", () => ({
  app: {
    getPath: jest.fn()
  }
}));

// https://stackoverflow.com/a/65286435
const localStorageMock = (function() {
  let store = {}
  return {
    getItem: function(key) {
      return store[key] || null
    },
    setItem: function(key, value) {
      store[key] = value.toString()
    },
    removeItem: function(key) {
      delete store[key]
    },
    clear: function() {
      store = {}
    }
  }
})()
Object.defineProperty(global, 'localStorage', {
  value: localStorageMock
})

// https://jestjs.io/docs/manual-mocks#mocking-methods-which-are-not-implemented-in-jsdom
//
// Also had to add global.window instead of window
// because bellow code breaks in non jsDom environment of Wallet_test.re
Object.defineProperty(global.window || {}, "matchMedia", {
  writable: true,
  value: jest.fn().mockImplementation((query) => ({
    matches: false,
    media: query,
    onchange: null,
    addListener: jest.fn(), // deprecated
    removeListener: jest.fn(), // deprecated
    addEventListener: jest.fn(),
    removeEventListener: jest.fn(),
    dispatchEvent: jest.fn(),
  })),
});
