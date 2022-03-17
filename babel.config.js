module.exports = function (api) {
  const presets = ["@babel/preset-react"];
  api.cache.never();

  return { presets };
};
