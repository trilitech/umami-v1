type colors = {
  textHighEmphasis: string,
  textMediumEmphasis: string,
  textDisabled: string,
  textPositive: string,
  textNegative: string,
  textPrimary: string,
  iconHighEmphasis: string,
  iconMediumEmphasis: string,
  iconDisabled: string,
  iconPrimary: string,
  background: string,
  barBackground: string,
  scrim: string,
  error: string,
  statePressed: string,
  stateHovered: string,
  stateActive: string,
  primaryButtonBackground: string,
  primaryTextHighEmphasis: string,
  primaryTextMediumEmphasis: string,
  primaryTextDisabled: string,
  primaryIconHighEmphasis: string,
  primaryIconMediumEmphasis: string,
  primaryIconDisabled: string,
  borderHighEmphasis: string,
  borderMediumEmphasis: string,
  borderDisabled: string,
};

type theme = {
  dark: bool,
  colors,
};

// Themes

// let darkTheme = {
//   dark: true,
//   colors: {
//     textHighEmphasis: "rgba(255,255,255,0.87)",
//     textMediumEmphasis: "rgba(255,255,255,0.6)",
//     textDisabled: "rgba(255,255,255,0.38)",
//     textPositive: "#76e59b",
//     textNegative: "#f97977",
//     textPrimary: "#ff9270",
//     iconHighEmphasis: "#FFF",
//     iconMediumEmphasis: "rgba(255,255,255,0.6)",
//     iconDisabled: "rgba(255,255,255,0.38)",
//     iconPrimary: "#f69679",
//     background: "#121212",
//     barBackground: "#212121",
//     error: "#bb3838",
//     statePressed: "rgba(255, 255, 255, 0.1)",
//     stateHover: "rgba(255, 255, 255, 0.04)",
//     stateActive: "rgba(255, 255, 255, 0.12)",
//     primaryButtonBackground: "#f69679",
//     primaryTextHighEmphasis: "rgba(0,0,0,0.87)",
//     primaryTextMediumEmphasis: "rgba(0,0,0,0.74)",
//     primaryTextDisabled: "rgba(0,0,0,0.38)",
//     primaryIconHighEmphasis: "rgba(0,0,0,0.87)",
//     primaryIconMediumEmphasis: "rgba(0,0,0,0.74)",
//     primaryIconDisabled: "rgba(0,0,0,0.38)",
//     border: "rgba(255,255,255,0.38)",
//   },
// };

let darkTheme = {
  dark: true,
  colors: {
    textHighEmphasis: "pink",
    textMediumEmphasis: "gold",
    textDisabled: "purple",
    textPositive: "green",
    textNegative: "red",
    textPrimary: "aqua",
    iconHighEmphasis: "hotpink",
    iconMediumEmphasis: "gold",
    iconDisabled: "purple",
    iconPrimary: "cyan",
    background: "dimgrey",
    barBackground: "grey",
    scrim: "rgba(92,92,92,0.32)",
    error: "darkred",
    statePressed: "rgba(30, 144, 255, 0.1)",
    stateHovered: "rgba(255, 20, 147, 0.04)",
    stateActive: "rgba(34, 139, 34, 0.12)",
    primaryButtonBackground: "cyan",
    primaryTextHighEmphasis: "darkorchid",
    primaryTextMediumEmphasis: "darkslateblue",
    primaryTextDisabled: "mediumvioletred",
    primaryIconHighEmphasis: "darkorchid",
    primaryIconMediumEmphasis: "darkslateblue",
    primaryIconDisabled: "mediumvioletred",
    borderHighEmphasis: "pink",
    borderMediumEmphasis: "gold",
    borderDisabled: "purple",
  },
};

// Context and Provider

let initialState = darkTheme;

let context = React.createContext(initialState);

module Provider = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

// Final Provider

[@react.component]
let make = (~children) => {
  let value = darkTheme;

  <Provider value> children </Provider>;
};

// Hooks

let useTheme = () => React.useContext(context);
