/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2022 Nomadic Labs, <contact@nomadic-labs.com>          */
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

/* Same as BigButton */
let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~maxWidth=570.->dp,
          ~alignItems=`center,
          ~borderRadius=4.,
          ~flexDirection=`row,
          ~paddingVertical=20.->dp,
          ~paddingHorizontal=24.->dp,
          ~marginTop=16.->dp,
          ~marginBottom=40.->dp,
          (),
        )
        ->unsafeAddStyle({
            "boxShadow": "0 1px 3px 0 rgba(0, 0, 0, 0.2), 0 2px 1px -1px rgba(0, 0, 0, 0.12), 0 1px 1px 0 rgba(0, 0, 0, 0.14)",
          }),
      "iconContainer":
        style(
          ~borderRadius=16.,
          ~width=32.->dp,
          ~height=32.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          (),
        ),
      "buttonText":
        style(
          ~borderRadius=16.,
          ~marginLeft=16.->dp,
          ~fontFamily="SourceSansPro",
          ~fontSize=16.,
          ~fontWeight=`_900,
          (),
        ),
    })
  );

module HelpLink = {
  let onPress = (link, _event) => {
    System.openExternal(link);
  };

  [@react.component]
  let make = (~icon: Icons.builder, ~text, ~link) => {
    let theme = ThemeContext.useTheme();

    <Pressable_ onPress={onPress(link)}>
      {({hovered, pressed, focused}) => {
         <View
           style=Style.(
             array([|
               styles##button,
               style(~backgroundColor=theme.colors.barBackground, ()),
             |])
           )>
           <View
             style=Style.(
               array([|
                 styles##iconContainer,
                 style(
                   ~backgroundColor=theme.colors.primaryButtonBackground,
                   (),
                 ),
               |])
             )>
             {icon(
                ~style=?None,
                ~size=20.,
                ~color=theme.colors.textHighEmphasis,
              )}
           </View>
           <View
             style=Style.(
               array([|
                 styles##buttonText,
                 style(~color=theme.colors.textHighEmphasis, ()),
               |])
             )>
             text->React.string
           </View>
         </View>;
       }}
    </Pressable_>;
  };
};

module HelpItem = {
  [@react.component]
  let make = (~icon, ~text, ~link, ~descr) => {
    <View>
      <Typography.Body1> descr->React.string </Typography.Body1>
      <HelpLink icon text link />
    </View>;
  };
};

[@react.component]
let make = () => {
  <Page>
    <Typography.Headline
      style=Style.(
        array([|
          Styles.title,
          style(~marginBottom=8.->dp, ~paddingBottom=0.->dp, ()),
        |])
      )>
      I18n.Title.help->React.string
    </Typography.Headline>
    <Typography.Overline3 style=Style.(style(~marginBottom=24.->dp, ()))>
      I18n.Help.subtitle->React.string
    </Typography.Overline3>
    <HelpItem
      icon=Icons.OpenExternal.build
      text=I18n.Help.articles_text
      link="https://medium.com/umamiwallet"
      descr=I18n.Help.articles_descr
    />
    <HelpItem
      icon=Icons.OpenExternal.build
      text=I18n.Help.faq_text
      link="https://umamiwallet.com/#faq"
      descr=I18n.Help.faq_descr
    />
    <HelpItem
      icon=Icons.QuestionMarkOutlined.build
      text=I18n.Help.support_text
      link={"mailto:" ++ MailToSupportButton.supportEmail}
      descr=I18n.Help.support_descr
    />
    <HelpItem
      icon=Icons.Tos.build
      text=I18n.Help.tos_text
      link="https://umamiwallet.com/tos.html"
      descr=I18n.Help.tos_descr
    />
  </Page>;
};
