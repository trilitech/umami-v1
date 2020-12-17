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
  logBackground: string,
  scrim: string,
  error: string,
  statePressed: string,
  stateHovered: string,
  stateActive: string,
  borderHighEmphasis: string,
  borderMediumEmphasis: string,
  borderDisabled: string,
  borderPrimary: string,
  primaryButtonBackground: string,
  primaryTextHighEmphasis: string,
  primaryTextMediumEmphasis: string,
  primaryTextDisabled: string,
  primaryIconHighEmphasis: string,
  primaryIconMediumEmphasis: string,
  primaryIconDisabled: string,
  primaryStatePressed: string,
  primaryStateHovered: string,
  primaryStateActive: string,
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
    iconHighEmphasis: Colors.Light.highEmphasis,
    iconMediumEmphasis: Colors.Light.mediumEmphasis,
    iconDisabled: Colors.Light.disabled,
    iconPrimary: Colors.Light.primary,
    background: Colors.Light.background,
    barBackground: Colors.Light.barBackground,
    logBackground: Colors.Dark.background,
    scrim: Colors.Light.scrim,
    error: Colors.error,
    statePressed: Colors.Light.statePressed,
    stateHovered: Colors.Light.stateHovered,
    stateActive: Colors.Light.stateActive,
    borderHighEmphasis: Colors.Light.highEmphasis,
    borderMediumEmphasis: Colors.Light.mediumEmphasis,
    borderDisabled: Colors.Light.disabled,
    borderPrimary: Colors.Light.primary,
    primaryButtonBackground: Colors.Light.primary,
    primaryTextHighEmphasis: Colors.Dark.maxEmphasis,
    primaryTextMediumEmphasis: Colors.Dark.mediumEmphasis,
    primaryTextDisabled: Colors.Dark.disabled,
    primaryIconHighEmphasis: Colors.Dark.maxEmphasis,
    primaryIconMediumEmphasis: Colors.Dark.mediumEmphasis,
    primaryIconDisabled: Colors.Dark.disabled,
    primaryStatePressed: Colors.Dark.statePressed,
    primaryStateHovered: Colors.Dark.stateHovered,
    primaryStateActive: Colors.Dark.stateActive,
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
    iconHighEmphasis: Colors.Dark.highEmphasis,
    iconMediumEmphasis: Colors.Dark.mediumEmphasis,
    iconDisabled: Colors.Dark.disabled,
    iconPrimary: Colors.Dark.primary,
    background: Colors.Dark.background,
    barBackground: Colors.Dark.barBackground,
    logBackground: Colors.Light.background,
    scrim: Colors.Dark.scrim,
    error: Colors.error,
    statePressed: Colors.Dark.statePressed,
    stateHovered: Colors.Dark.stateHovered,
    stateActive: Colors.Dark.stateActive,
    borderHighEmphasis: Colors.Dark.highEmphasis,
    borderMediumEmphasis: Colors.Dark.mediumEmphasis,
    borderDisabled: Colors.Dark.disabled,
    borderPrimary: Colors.Dark.primary,
    primaryButtonBackground: Colors.Dark.primary,
    primaryTextHighEmphasis: Colors.Light.maxEmphasis,
    primaryTextMediumEmphasis: Colors.Light.mediumEmphasis,
    primaryTextDisabled: Colors.Light.disabled,
    primaryIconHighEmphasis: Colors.Light.maxEmphasis,
    primaryIconMediumEmphasis: Colors.Light.mediumEmphasis,
    primaryIconDisabled: Colors.Light.disabled,
    primaryStatePressed: Colors.Light.statePressed,
    primaryStateHovered: Colors.Light.stateHovered,
    primaryStateActive: Colors.Light.stateActive,
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
