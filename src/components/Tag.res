open ReactNative

let styles = {
  open Style
  StyleSheet.create({
    "tag": style(~alignItems=#center, ~justifyContent=#center, ()),
    "fixed": style(~width=40.->dp, ~height=18.->dp, ~borderRadius=9., ()),
    "fit": style(
      ~width="fit-content"->StyleUtils.stringToSize,
      ~height="fit-content"->StyleUtils.stringToSize,
      (),
    ),
  })
}

module Fixed = {
  @react.component
  let make = (~style as styleProp=?, ~contentStyle=?, ~content: string) => {
    let theme = ThemeContext.useTheme()

    <View
      style={
        open Style
        arrayOption([
          styles["tag"]->Some,
          styles["fixed"]->Some,
          style(~backgroundColor=theme.colors.statePressed, ())->Some,
          styleProp,
        ])
      }>
      <Typography.Body2 fontSize=9.7 colorStyle=#mediumEmphasis style=?contentStyle>
        {content->React.string}
      </Typography.Body2>
    </View>
  }
}

@react.component
let make = (~style as styleProp=?, ~borderRadius=9., ~contentStyle=?, ~content: string) => {
  let theme = ThemeContext.useTheme()

  <View
    style={
      open Style
      arrayOption([
        styles["tag"]->Some,
        styles["fit"]->Some,
        style(~backgroundColor=theme.colors.statePressed, ~borderRadius, ())->Some,
        styleProp,
      ])
    }>
    <Typography.Body2 fontSize=9.7 colorStyle=#mediumEmphasis style=?contentStyle>
      {content->React.string}
    </Typography.Body2>
  </View>
}
