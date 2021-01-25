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

export const Svg = ({ style, ...props }) =>
  createElement("svg", { ...props, style: StyleSheet.compose(styles.svg, style) });

export const Rect = (props) => createElement("rect", props);

export const Circle = (props) => createElement("circle", props);

export const Ellipse = (props) => createElement("ellipse", props);

export const Line = (props) => createElement("line", props);

export const Polygon = (props) => createElement("polygon", props);

export const Polyline = (props) => createElement("polyline", props);

export const Path = (props) => createElement("path", props);

export const Text = (props) => createElement("text", props);

export const TextPath = (props) => createElement("textPath", props);

export const Tspan = (props) => createElement("tspan", props);

export const Use = (props) => createElement("use", props);

export const G = (props) => createElement("g", props);

export const Symbol = (props) => createElement("symbol", props);

export const Defs = (props) => createElement("defs", props);

export const Image = (props) => createElement("image", props);

export const ClipPath = (props) => createElement("clipPath", props);

export const LinearGradient = (props) => createElement("linearGradient", props);

export const Stop = (props) => createElement("stop", props);

export const RadialGradient = (props) => createElement("radialGradient", props);

export const Mask = (props) => createElement("mask", props);

export const Pattern = (props) => createElement("pattern", props);

export const Marker = (props) => createElement("marker", props);

export const ForeignObject = (props) => createElement("foreignObject", props);
