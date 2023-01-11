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

module Base = {
  @react.component
  let make = (
    ~style as styleProp=?,
    ~height=18.,
    ~width,
    ~contentStyle=?,
    ~fontSize=9.7,
    ~content: string,
  ) => {
    let theme = ThemeContext.useTheme()
    let borderRadius = height /. 2.
    let height = height->Style.dp

    let tag_style = {
      open Style
      style(
        ~backgroundColor=theme.colors.statePressed,
        ~height,
        ~width,
        ~borderRadius,
        ~alignItems=#center,
        ~justifyContent=#center,
        (),
      )
    }

    <View style={Style.arrayOption([tag_style->Some, styleProp])}>
      <Typography.Body2 fontSize colorStyle=#mediumEmphasis style=?contentStyle>
        {content->React.string}
      </Typography.Body2>
    </View>
  }
}

module Fixed = {
  @react.component
  let make = (
    ~style=?,
    ~height=?,
    ~width=40.->Style.dp,
    ~contentStyle=?,
    ~fontSize=?,
    ~content: string,
  ) => {
    <Base ?style ?height width ?fontSize ?contentStyle content />
  }
}

@react.component
let make = (
  ~style=?,
  ~height=?,
  ~width="fit-content"->StyleUtils.stringToSize,
  ~contentStyle=?,
  ~fontSize=?,
  ~content: string,
) => {
  <Base ?style ?height width ?fontSize ?contentStyle content />
}
