open ReactNative;

module HeaderButtons = {
  module Close = {
    [@react.component]
    let make = (~onPress) => {
      let theme = ThemeContext.useTheme();
      <TouchableOpacity onPress>
        <Icons.Close size=36. color={theme.colors.iconMediumEmphasis} />
      </TouchableOpacity>;
    };
  };

  module Back = {
    [@react.component]
    let make = (~onPress) => {
      let theme = ThemeContext.useTheme();
      <TouchableOpacity onPress>
        <Icons.ArrowLeft size=36. color={theme.colors.iconMediumEmphasis} />
      </TouchableOpacity>;
    };
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "headerLeft":
        style(~position=`absolute, ~left=20.->dp, ~top=20.->dp, ()),
      "headerRight":
        style(~position=`absolute, ~right=20.->dp, ~top=20.->dp, ()),
      "loadingView":
        StyleSheet.flatten([|
          StyleSheet.absoluteFillObject,
          style(~justifyContent=`center, ~alignItems=`center, ()),
        |]),
      "modal":
        style(~alignSelf=`center, ~borderRadius=4., ())
        ->unsafeAddStyle({
            "boxShadow": "0 7px 8px -4px rgba(0, 0, 0, 0.2), 0 5px 22px 4px rgba(0, 0, 0, 0.12), 0 12px 17px 2px rgba(0, 0, 0, 0.14)",
          }),
      "modalForm":
        style(
          ~width=642.->dp,
          ~paddingTop=40.->dp,
          ~paddingBottom=40.->dp,
          ~paddingHorizontal=135.->dp,
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
      (
        ~children,
        ~headerLeft=?,
        ~headerRight=?,
        ~loading as (loadingState, loadingTitle)=(false, None),
        ~style as styleFromProp=?,
      ) => {
    let theme = ThemeContext.useTheme();
    <View
      style=Style.(
        arrayOption([|
          Some(styles##modal),
          Some(style(~backgroundColor=theme.colors.background, ())),
          styleFromProp,
        |])
      )>
      children
      {loadingState
         ? <View
             style=Style.(
               array([|
                 styles##loadingView,
                 style(
                   ~backgroundColor=theme.colors.background,
                   ~opacity=0.87,
                   (),
                 ),
               |])
             )>
             {loadingTitle->ReactUtils.mapOpt(loadingTitle =>
                <Typography.Headline style=FormStyles.header>
                  loadingTitle->React.string
                </Typography.Headline>
              )}
             <ActivityIndicator
               animating=true
               size=ActivityIndicator_Size.large
               color={theme.colors.iconMediumEmphasis}
             />
           </View>
         : React.null}
      {headerLeft->ReactUtils.mapOpt(headerLeft =>
         <View style=styles##headerLeft> headerLeft </View>
       )}
      {headerRight->ReactUtils.mapOpt(headerRight =>
         <View style=styles##headerRight> headerRight </View>
       )}
    </View>;
  };
};

module Form = {
  [@react.component]
  let make = (~headerLeft=?, ~headerRight=?, ~loading=?, ~children) => {
    <Base ?headerLeft ?headerRight ?loading style=styles##modalForm>
      children
    </Base>;
  };
};

module Dialog = {
  [@react.component]
  let make = (~children) => {
    <Base style=styles##modalDialog> children </Base>;
  };
};

include Base;
