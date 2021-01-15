open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "wordsList": style(~flexDirection=`row, ~flexWrap=`wrap, ()),
      "wordItem":
        style(
          ~marginVertical=2.->dp,
          ~flexGrow=1.,
          ~flexShrink=1.,
          ~flexBasis=40.->pct,
          ~paddingHorizontal=10.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~height=36.->dp,
          ~borderWidth=1.,
          ~borderRadius=4.,
          ~borderStyle=`dashed,
          (),
        ),
      "wordItemInner": style(~flexDirection=`row, ~alignItems=`baseline, ()),
      "wordItemIndex":
        style(~width=17.->dp, ~marginRight=13.->dp, ~textAlign=`right, ()),
      "wordSpacer": style(~width=20.->dp, ()),
    })
  );

[@react.component]
let make = (~mnemonic) => {
  let theme = ThemeContext.useTheme();

  <View style=styles##wordsList>
    {mnemonic
     ->Array.mapWithIndex((index, word) => {
         <React.Fragment key={word ++ index->string_of_int}>
           <View
             style=Style.(
               array([|
                 styles##wordItem,
                 style(~borderColor=theme.colors.borderDisabled, ()),
               |])
             )>
             <View style=styles##wordItemInner>
               <Typography.Subtitle1
                 colorStyle=`disabled style=styles##wordItemIndex>
                 {(index + 1)->string_of_int->React.string}
               </Typography.Subtitle1>
               <Typography.Body1> word->React.string </Typography.Body1>
             </View>
           </View>
           {index mod 2 == 0 ? <View style=styles##wordSpacer /> : React.null}
         </React.Fragment>
       })
     ->React.array}
  </View>;
};
