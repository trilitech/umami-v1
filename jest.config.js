module.exports = {
    "moduleDirectories": [
        "node_modules"
    ],
    "testMatch": [
        "**/*_test.bs.js"
    ],
    "transformIgnorePatterns": [
        "node_modules/(?!(bs-platform|reason-react|reason-react-compat|reason-react-update|bs-webapi|bs-css|bs-css-emotion)/)"
    ],
    "collectCoverageFrom": [
	"src/*.js"
    ]
}
