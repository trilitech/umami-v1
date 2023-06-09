module.exports = {
  moduleDirectories: ["node_modules"],
  testMatch: ["**/__tests__/*_test.bs.js"],
  transformIgnorePatterns: [
    "node_modules/(?!(bs-platform|reason-react|reason-react-compat|reason-react-update|bs-webapi|bs-css|bs-css-emotion)/)",
  ],
  collectCoverageFrom: ["src/*.js"],
  testEnvironment: "jsdom",
  preset: "react-native-web",
  setupFilesAfterEnv: ["<rootDir>/jestSetup.js"],
  moduleNameMapper: {
    "^@ledgerhq/devices/hid-framing$": "@ledgerhq/devices/lib/hid-framing"
  },
};
