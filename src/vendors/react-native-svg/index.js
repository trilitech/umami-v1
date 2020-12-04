var ReactNative = require("react-native");
var StyleSheet = ReactNative.StyleSheet;
var createElement = ReactNative.unstable_createElement;

var styles = StyleSheet.create({
  svg: {
    display: "flex",
    flexBasis: "auto",
    flexGrow: 0,
    flexShrink: 0,
  },
});

exports.Svg = ({ style, ...props }) =>
  createElement("svg", { ...props, style: StyleSheet.compose(styles.svg, style) });

exports.Rect = (props) => createElement("rect", props);

exports.Circle = (props) => createElement("circle", props);

exports.Ellipse = (props) => createElement("ellipse", props);

exports.Line = (props) => createElement("line", props);

exports.Polygon = (props) => createElement("polygon", props);

exports.Polyline = (props) => createElement("polyline", props);

exports.Path = (props) => createElement("path", props);

exports.Text = (props) => createElement("text", props);

exports.TextPath = (props) => createElement("textPath", props);

exports.Tspan = (props) => createElement("tspan", props);

exports.Use = (props) => createElement("use", props);

exports.G = (props) => createElement("g", props);

exports.Symbol = (props) => createElement("symbol", props);

exports.Defs = (props) => createElement("defs", props);

exports.Image = (props) => createElement("image", props);

exports.ClipPath = (props) => createElement("clipPath", props);

exports.LinearGradient = (props) => createElement("linearGradient", props);

exports.Stop = (props) => createElement("stop", props);

exports.RadialGradient = (props) => createElement("radialGradient", props);

exports.Mask = (props) => createElement("mask", props);

exports.Pattern = (props) => createElement("pattern", props);

exports.Marker = (props) => createElement("marker", props);

exports.ForeignObject = (props) => createElement("foreignObject", props);
