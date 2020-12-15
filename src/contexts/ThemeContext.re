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

let darkTheme = {
  dark: true,
  colors: {
    textHighEmphasis: Colors.Dark.highEmphasis,
    textMediumEmphasis: Colors.Dark.mediumEmphasis,
    textDisabled: Colors.Dark.disabled,
    textPositive: Colors.Dark.positive,
    textNegative: Colors.Dark.negative,
    textPrimary: Colors.Dark.primaryText,
    iconHighEmphasis: Colors.Dark.maxEmphasis,
    iconMediumEmphasis: Colors.Dark.mediumEmphasis,
    iconDisabled: Colors.Dark.disabled,
    iconPrimary: Colors.Dark.primary,
    background: Colors.Dark.background,
    barBackground: Colors.Dark.barBackground,
    scrim: Colors.Dark.scrim,
    error: Colors.Dark.error,
    statePressed: Colors.Dark.statePressed,
    stateHovered: Colors.Dark.stateHovered,
    stateActive: Colors.Dark.stateActive,
    primaryButtonBackground: Colors.Dark.primary,
    primaryTextHighEmphasis: Colors.Light.highEmphasis,
    primaryTextMediumEmphasis: Colors.Light.mediumEmphasis,
    primaryTextDisabled: Colors.Light.disabled,
    primaryIconHighEmphasis: Colors.Light.highEmphasis,
    primaryIconMediumEmphasis: Colors.Light.mediumEmphasis,
    primaryIconDisabled: Colors.Light.disabled,
    borderHighEmphasis: Colors.Dark.highEmphasis,
    borderMediumEmphasis: Colors.Dark.mediumEmphasis,
    borderDisabled: Colors.Dark.disabled,
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
