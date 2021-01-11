open ReactNative;

module LoadingView = {
  let styles =
    Style.(
      StyleSheet.create({
        "loadingView":
          style(
            ~height=400.->dp,
            ~justifyContent=`center,
            ~alignItems=`center,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~title, ~height=?) => {
    let theme = ThemeContext.useTheme();
    <View
      style={Style.arrayOption([|
        Some(styles##loadingView),
        height->Belt.Option.map(height =>
          Style.style(~height=height->string_of_int, ())
        ),
      |])}>
      <Typography.Headline style=FormStyles.header>
        title->React.string
      </Typography.Headline>
      <ActivityIndicator
        animating=true
        size=ActivityIndicator_Size.large
        color={theme.colors.iconMediumEmphasis}
      />
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "headerLeft":
        style(~position=`absolute, ~left=20.->dp, ~top=20.->dp, ()),
      "headerRight":
        style(~position=`absolute, ~right=20.->dp, ~top=20.->dp, ()),
      "modal":
        style(~alignSelf=`center, ~borderRadius=4., ())
        ->unsafeAddStyle({
            "boxShadow": "0 7px 8px -4px rgba(0, 0, 0, 0.2), 0 5px 22px 4px rgba(0, 0, 0, 0.12), 0 12px 17px 2px rgba(0, 0, 0, 0.14)",
          }),
      "modalForm":
        style(
          ~width=642.->dp,
          ~paddingTop=45.->dp,
          ~paddingBottom=40.->dp,
          ~paddingHorizontal=110.->dp,
          (),
        ),
      "modalDialog":
        style(
          ~width=522.->dp,
          ~paddingTop=40.->dp,
          ~paddingBottom=40.->dp,
          ~paddingHorizontal=50.->dp,
          (),
        ),
    })
  );

module Base = {
  [@react.component]
  let make =
      (~children, ~headerLeft=?, ~headerRight=?, ~style as styleFromProp=?) => {
    let theme = ThemeContext.useTheme();
    <View
      style=Style.(
        arrayOption([|
          Some(styles##modal),
          Some(style(~backgroundColor=theme.colors.background, ())),
          styleFromProp,
        |])
      )>
      {headerLeft->ReactUtils.mapOpt(headerLeft =>
         <View style=styles##headerLeft> headerLeft </View>
       )}
      {headerRight->ReactUtils.mapOpt(headerRight =>
         <View style=styles##headerRight> headerRight </View>
       )}
      children
    </View>;
  };
};

module Form = {
  [@react.component]
  let make = (~headerLeft=?, ~headerRight=?, ~children) => {
    <Base ?headerLeft ?headerRight style=styles##modalForm> children </Base>;
  };
};

module Dialog = {
  [@react.component]
  let make = (~children) => {
    <Base style=styles##modalDialog> children </Base>;
  };
};

include Base;
