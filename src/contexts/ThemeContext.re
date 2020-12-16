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

let lightTheme = {
  dark: false,
  colors: {
    textHighEmphasis: Colors.Light.highEmphasis,
    textMediumEmphasis: Colors.Light.mediumEmphasis,
    textDisabled: Colors.Light.disabled,
    textPositive: Colors.Light.positive,
    textNegative: Colors.Light.negative,
    textPrimary: Colors.Light.primary,
    iconHighEmphasis: Colors.Light.maxEmphasis,
    iconMediumEmphasis: Colors.Light.mediumEmphasis,
    iconDisabled: Colors.Light.disabled,
    iconPrimary: Colors.Light.primary,
    background: Colors.Light.background,
    barBackground: Colors.Light.barBackground,
    scrim: Colors.Light.scrim,
    error: Colors.error,
    statePressed: Colors.Light.statePressed,
    stateHovered: Colors.Light.stateHovered,
    stateActive: Colors.Light.stateActive,
    primaryButtonBackground: Colors.Light.primary,
    primaryTextHighEmphasis: Colors.Dark.highEmphasis,
    primaryTextMediumEmphasis: Colors.Dark.mediumEmphasis,
    primaryTextDisabled: Colors.Dark.disabled,
    primaryIconHighEmphasis: Colors.Dark.highEmphasis,
    primaryIconMediumEmphasis: Colors.Dark.mediumEmphasis,
    primaryIconDisabled: Colors.Dark.disabled,
    borderHighEmphasis: Colors.Light.highEmphasis,
    borderMediumEmphasis: Colors.Light.mediumEmphasis,
    borderDisabled: Colors.Light.disabled,
  },
};

let darkTheme = {
  dark: true,
  colors: {
    textHighEmphasis: Colors.Dark.highEmphasis,
    textMediumEmphasis: Colors.Dark.mediumEmphasis,
    textDisabled: Colors.Dark.disabled,
    textPositive: Colors.Dark.positive,
    textNegative: Colors.Dark.negative,
    textPrimary: Colors.Dark.primary,
    iconHighEmphasis: Colors.Dark.maxEmphasis,
    iconMediumEmphasis: Colors.Dark.mediumEmphasis,
    iconDisabled: Colors.Dark.disabled,
    iconPrimary: Colors.Dark.primary,
    background: Colors.Dark.background,
    barBackground: Colors.Dark.barBackground,
    scrim: Colors.Dark.scrim,
    error: Colors.error,
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

let initialState = lightTheme;

let context = React.createContext(initialState);

module Provider = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

// Final Provider

[@bs.val] external window: 'a = "window";
let mediaQueryColorSchemeDark =
  window##matchMedia("(prefers-color-scheme: dark)");

[@react.component]
let make = (~children) => {
  let (dark, setDark) =
    React.useState(_ => mediaQueryColorSchemeDark##matches);

  let listener =
    React.useCallback1(event => {setDark(_ => event##matches)}, [|setDark|]);

  React.useEffect1(
    () => {
      mediaQueryColorSchemeDark##addEventListener("change", listener)->ignore;
      Some(
        () => {
          mediaQueryColorSchemeDark##removeEventListener("change", listener)
          ->ignore
        },
      );
    },
    [|listener|],
  );

  <Provider value={dark ? darkTheme : lightTheme}> children </Provider>;
};

// Hooks

let useTheme = () => React.useContext(context);
