/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        StyleSheet.flatten([|
          FormStyles.square(40.),
          style(
            ~alignItems=`center,
            ~justifyContent=`center,
            ~borderRadius=20.,
            (),
          ),
          ShadowStyles.button,
        |]),
    })
  );

let tooltip = ("refresh_button", I18n.tooltip#refresh);

[@react.component]
let make = (~onRefresh, ~loading, ~style as styleArg=?) => {
  let theme = ThemeContext.useTheme();

  <View
    style=Style.(
      arrayOption([|
        Some(styles##container),
        Some(style(~backgroundColor=theme.colors.elevatedBackground, ())),
        styleArg,
      |])
    )>
    {loading
       ? <ActivityIndicator
           animating=true
           size=ActivityIndicator_Size.small
           color={theme.colors.iconHighEmphasis}
         />
       : <IconButton
           size=40.
           icon=Icons.Refresh.build
           tooltip
           onPress={_ => onRefresh()}
         />}
  </View>;
};
