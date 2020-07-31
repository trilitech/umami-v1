module.exports = function override(config, env) {
  return {
    ...config,
    "externals": {
      "child_process": "require('child_process')"
    }
  }
}
