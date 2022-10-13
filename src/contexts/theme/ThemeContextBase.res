/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

type colors = {
  textMaxEmphasis: string,
  textHighEmphasis: string,
  textMediumEmphasis: string,
  textDisabled: string,
  textPositive: string,
  textNegative: string,
  textPrimary: string,
  iconMaxEmphasis: string,
  iconHighEmphasis: string,
  iconMediumEmphasis: string,
  iconDisabled: string,
  iconPrimary: string,
  background: string,
  barBackground: string,
  backgroundMediumEmphasis: string,
  logBackground: string,
  elevatedBackground: string,
  cardBackground: string,
  scrim: string,
  error: string,
  valid: string,
  statePressed: string,
  stateDisabled: string,
  stateHovered: string,
  stateActive: string,
  stateFocusedOutline: string,
  stateRowHovered: string,
  borderHighEmphasis: string,
  borderMediumEmphasis: string,
  borderDisabled: string,
  borderPrimary: string,
  primaryButtonBackground: string,
  primaryButtonOutline: string,
  primaryTextHighEmphasis: string,
  primaryTextMediumEmphasis: string,
  primaryTextDisabled: string,
  primaryIconHighEmphasis: string,
  primaryIconMediumEmphasis: string,
  primaryIconDisabled: string,
  primaryStatePressed: string,
  primaryStateHovered: string,
  primaryStateActive: string,
  primaryStateDisabled: string,
  toastIconMediumEmphasis: string,
  toastError: string,
  toastValid: string,
  surfaceButtonBackground: string,
  surfaceButtonOutline: string,
}

type theme = {
  dark: bool,
  colors: colors,
}

// Themes

let lightTheme = {
  dark: false,
  colors: {
    textMaxEmphasis: Colors.Light.maxEmphasis,
    textHighEmphasis: Colors.Light.highEmphasis,
    textMediumEmphasis: Colors.Light.mediumEmphasis,
    textDisabled: Colors.Light.disabled,
    textPositive: Colors.Light.positive,
    textNegative: Colors.Light.negative,
    textPrimary: Colors.Light.primary,
    iconMaxEmphasis: Colors.Light.maxEmphasis,
    iconHighEmphasis: Colors.Light.highEmphasis,
    iconMediumEmphasis: Colors.Light.mediumEmphasis,
    iconDisabled: Colors.Light.disabled,
    iconPrimary: Colors.Light.primary,
    background: Colors.Light.background,
    barBackground: Colors.Light.barBackground,
    backgroundMediumEmphasis: Colors.Light.backgroundMediumEmphasis,
    logBackground: Colors.Dark.background,
    elevatedBackground: Colors.Light.background,
    cardBackground: Colors.Light.cardBackground,
    scrim: Colors.Light.scrim,
    error: Colors.Light.error,
    valid: Colors.Light.valid,
    statePressed: Colors.Light.statePressed,
    stateHovered: Colors.Light.stateHovered,
    stateFocusedOutline: Colors.Light.stateFocusedOutline,
    stateActive: Colors.Light.stateActive,
    stateDisabled: Colors.Light.stateDisabled,
    stateRowHovered: Colors.Light.stateSmallHovered,
    borderHighEmphasis: Colors.Light.highEmphasis,
    borderMediumEmphasis: Colors.Light.mediumEmphasis,
    borderDisabled: Colors.Light.disabled,
    borderPrimary: Colors.Light.primary,
    primaryButtonBackground: Colors.Light.primary,
    primaryButtonOutline: Colors.Light.primaryOutline,
    primaryTextHighEmphasis: Colors.Dark.maxEmphasis,
    primaryTextMediumEmphasis: Colors.Dark.mediumEmphasis,
    primaryTextDisabled: Colors.Dark.disabled,
    primaryIconHighEmphasis: Colors.Dark.maxEmphasis,
    primaryIconMediumEmphasis: Colors.Dark.mediumEmphasis,
    primaryIconDisabled: Colors.Dark.disabled,
    primaryStatePressed: Colors.Dark.statePressed,
    primaryStateHovered: Colors.Dark.stateHovered,
    primaryStateActive: Colors.Dark.stateActive,
    primaryStateDisabled: Colors.Dark.stateDisabled,
    toastIconMediumEmphasis: Colors.Dark.mediumEmphasis,
    toastError: Colors.Dark.error,
    toastValid: Colors.Dark.valid,
    surfaceButtonBackground: Colors.Light.surface,
    surfaceButtonOutline: Colors.Light.surfaceOutline,
  },
}

let darkTheme = {
  dark: true,
  colors: {
    textMaxEmphasis: Colors.Dark.maxEmphasis,
    textHighEmphasis: Colors.Dark.highEmphasis,
    textMediumEmphasis: Colors.Dark.mediumEmphasis,
    textDisabled: Colors.Dark.disabled,
    textPositive: Colors.Dark.positive,
    textNegative: Colors.Dark.negative,
    textPrimary: Colors.Dark.primary,
    iconMaxEmphasis: Colors.Dark.maxEmphasis,
    iconHighEmphasis: Colors.Dark.highEmphasis,
    iconMediumEmphasis: Colors.Dark.mediumEmphasis,
    iconDisabled: Colors.Dark.disabled,
    iconPrimary: Colors.Dark.primary,
    background: Colors.Dark.background,
    barBackground: Colors.Dark.barBackground,
    backgroundMediumEmphasis: Colors.Dark.backgroundMediumEmphasis,
    logBackground: Colors.Light.background,
    elevatedBackground: Colors.Dark.elevatedBackground,
    cardBackground: Colors.Dark.cardBackground,
    scrim: Colors.Dark.scrim,
    error: Colors.Dark.error,
    valid: Colors.Dark.valid,
    statePressed: Colors.Dark.statePressed,
    stateHovered: Colors.Dark.stateHovered,
    stateFocusedOutline: Colors.Dark.stateFocusedOutline,
    stateActive: Colors.Dark.stateActive,
    stateDisabled: Colors.Dark.stateDisabled,
    stateRowHovered: Colors.Dark.stateSmallHovered,
    borderHighEmphasis: Colors.Dark.highEmphasis,
    borderMediumEmphasis: Colors.Dark.mediumEmphasis,
    borderDisabled: Colors.Dark.disabled,
    borderPrimary: Colors.Dark.primary,
    primaryButtonBackground: Colors.Dark.primary,
    primaryButtonOutline: Colors.Dark.primaryOutline,
    primaryTextHighEmphasis: Colors.Light.maxEmphasis,
    primaryTextMediumEmphasis: Colors.Light.mediumEmphasis,
    primaryTextDisabled: Colors.Light.disabled,
    primaryIconHighEmphasis: Colors.Light.maxEmphasis,
    primaryIconMediumEmphasis: Colors.Light.mediumEmphasis,
    primaryIconDisabled: Colors.Light.disabled,
    primaryStatePressed: Colors.Dark.statePressed,
    primaryStateHovered: Colors.Dark.stateHovered,
    primaryStateActive: Colors.Dark.stateActive,
    primaryStateDisabled: Colors.Dark.stateActive,
    toastIconMediumEmphasis: Colors.Light.mediumEmphasis,
    toastError: Colors.Light.error,
    toastValid: Colors.Light.valid,
    surfaceButtonBackground: Colors.Dark.surface,
    surfaceButtonOutline: Colors.Dark.surfaceOutline,
  },
}

type themeMain = [#system | #dark | #light]

type state = {themeSetting: (themeMain, themeMain => unit)}

let initialState = {themeSetting: (#light, _ => ())}
let context = React.createContext(initialState)

module Provider = {
  let makeProps = (~value, ~children, ()) =>
    {
      "value": value,
      "children": children,
    }

  let make = React.Context.provider(context)
}

@react.component
let make = (~children, ~theme, ~setTheme) =>
  <Provider value={themeSetting: (theme, setTheme)}> children </Provider>

// Hooks

@val external window: 'a = "window"
let mediaQueryColorSchemeDark = window["matchMedia"]("(prefers-color-scheme: dark)")

let usePrefersColorSchemeDark: unit => bool = () => {
  let (prefersColorSchemeDark, setPrefersColorSchemeDark) = React.useState(_ =>
    mediaQueryColorSchemeDark["matches"]
  )

  let listener = React.useCallback1(
    event => setPrefersColorSchemeDark(_ => event["matches"]),
    [setPrefersColorSchemeDark],
  )

  React.useEffect1(() => {
    mediaQueryColorSchemeDark["addEventListener"]("change", listener)->ignore
    Some(() => mediaQueryColorSchemeDark["removeEventListener"]("change", listener)->ignore)
  }, [listener])

  prefersColorSchemeDark
}

let useThemeSetting = () => React.useContext(context).themeSetting

let useTheme = () => {
  let (currentTheme, _) = useThemeSetting()
  let systemDarkMode = usePrefersColorSchemeDark()

  switch currentTheme {
  | #dark => darkTheme
  | #light => lightTheme
  | #system => systemDarkMode ? darkTheme : lightTheme
  }
}
