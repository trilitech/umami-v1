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
        style(
          ~justifyContent=`spaceBetween,
          ~flexDirection=`row,
          ~height=60.->dp,
          ~alignItems=`center,
          ~paddingLeft=NavBar.width->dp,
          (),
        ),
      "nameLogo": style(~flexDirection=`row, ~alignItems=`center, ()),
      "networkHeader":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~marginLeft=16.->dp,
          ~marginBottom=2.->dp,
          (),
        ),
      "tag":
        style(
          ~height=26.->dp,
          ~paddingHorizontal=16.->dp,
          ~paddingVertical=4.->dp,
          ~marginRight=8.->dp,
          ~borderRadius=13.,
          ~alignItems=`center,
          ~justifyContent=`center,
          ~borderWidth=1.,
          (),
        ),
    })
  );

[@react.component]
let make = () => {
  let theme = ThemeContext.useTheme();

  let config = ConfigContext.useContent();

  let (networkText, networkColor) = (
    config.network.name,
    config.network.chain == `Mainnet
      ? Some(`primary) : Some(`mediumEmphasis),
  );

  let tag = {
    config.defaultNetwork
      ? None : config.network.chain->Network.getDisplayedName->Some;
  };

  let tagBorderColor = {
    config.network.chain == `Mainnet
      ? theme.colors.textPrimary : theme.colors.textMediumEmphasis;
  };

  let notices = NoticesContext.useNotices();

  <View
    style=Style.(
      array([|
        styles##container,
        style(~backgroundColor=theme.colors.barBackground, ()),
      |])
    )>
    <View style={styles##nameLogo}>
      <SVGLogoUmami
        width={87.->Style.dp}
        height={20.->Style.dp}
        fill={theme.colors.textHighEmphasis}
      />
      <View style=styles##networkHeader>
        {<View
           style=Style.(
             array([|styles##tag, style(~borderColor=tagBorderColor, ())|])
           )>
           <Typography.Body2
             fontSize=14. colorStyle={Option.getExn(networkColor)}>
             {switch (tag) {
              | Some(content) => "Custom " ++ content
              | None => ""
              }}
             ->React.string
           </Typography.Body2>
         </View>
         ->ReactUtils.onlyWhen(tag != None)}
        <Typography.Overline2 fontWeightStyle=`black colorStyle=?networkColor>
          networkText->React.string
        </Typography.Overline2>
      </View>
    </View>
    {switch (notices) {
     | [] => React.null
     | [notice, ..._] => <NoticeView notice />
     }}
  </View>;
};
