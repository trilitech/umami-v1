jest.mock("react-native-svg", () =>
  require("react-native-svg-mock/build/index")
);

jest.mock("usb-detection", () => {});

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
